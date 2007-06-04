/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Mark Joshi

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/models/marketmodels/models/alphafinder.hpp>

namespace QuantLib {

namespace
{

    template<class T, Real (T::*Value)(Real) >
    Real Bisection(Real target,
                   Real low,
                   Real high,
                   Real tolerance,
                   T& theObject) {

        Real x=0.5*(low+high);
        Real y=(theObject.*Value)(x);

        do {
            if (y < target) low = x;
            else if (y > target) high = x;

            x = 0.5*(low+high);
            y = (theObject.*Value)(x);
        } while ((fabs(y-target) > tolerance));

        return x;
    }

    template<class T, bool (T::*Value)(Real) >
    Real FindHighestOK(Real low,
                       Real high,
                       Real tolerance,
                       T& theObject) {

        Real x=0.5*(low+high);
        bool ok=(theObject.*Value)(x);

        do {
            if (ok) low = x;
            else    high = x;

            x = 0.5*(low+high);
            ok = (theObject.*Value)(x);
        } while ((fabs(high-low) > tolerance));

        return x;
    }

    template<class T, bool (T::*Value)(Real) >
    Real FindLowestOK(Real low,
                      Real high,
                      Real tolerance,
                      T& theObject) {

        Real x=0.5*(low+high);
        bool ok=(theObject.*Value)(x);

        do {
            if (ok) high = x;
            else    low = x;

            x = 0.5*(low+high);
            ok = (theObject.*Value)(x);
        } while ( (fabs(high-low) > tolerance) );

        return x;
    }
  

    template<class T, Real (T::*Value)(Real),  bool (T::*Condition)(Real) >
    Real Minimize(Real low,
                  Real high,
                  Real tolerance,
                  T& theObject,
                  bool& failed) {

        Real leftValue = (theObject.*Value)(low);
        Real rightValue = (theObject.*Value)(high);
		Real W = 0.5*(3.0-sqrt(5.0));
        Real x=W*low+(1-W)*high;

		Real midValue =  (theObject.*Value)(x);
 
        failed = true;
      
        while(high - low > tolerance) {
			Real tentativeNewMid = W*low+(1-W)*high;
			Real tentativeNewMidValue =  (theObject.*Value)(tentativeNewMid);
			bool conditioner = (theObject.*Condition)(tentativeNewMidValue);
			 
			if (!conditioner) {
                if  ((theObject.*Condition)(x))
					return x;
				else
					if (leftValue < rightValue)
						return low;
					else 
						return high;
            }

			if (tentativeNewMid <= midValue) // go right
			{
				leftValue = midValue;
				low = x;
				x = tentativeNewMid;
				midValue = tentativeNewMidValue;
			}
			else //go left
			{
				rightValue = tentativeNewMidValue;
				high = tentativeNewMid;
			}
            
			
		}
        failed = false;
        return x;
    }
}

    class quadratic {
      public:
    	quadratic (Real a, Real b, Real c);
    	Real turningPoint() const;
    	Real valueAtTurningPoint() const;
    	Real operator()(Real x) const;
    	Real discriminant() const;
        // return false if roots not real, and give turning point instead
        bool roots(Real& x, Real& y) const;
      private:
    	Real a_, b_, c_;

    };

    quadratic::quadratic(Real a, Real b, Real c) : a_(a), b_(b), c_(c) {}

    Real quadratic::turningPoint() const {
    	return -b_/(2.0*a_);
    }

    Real quadratic::valueAtTurningPoint() const {
        return (*this)(turningPoint());
    }

    Real quadratic::operator()(Real x) const {
        return x*(x*a_+b_)+c_;
    }

    Real quadratic::discriminant() const {
        return b_*b_-4*a_*c_;
    }

    // return false if roots not real, and give turning point instead
    bool quadratic::roots(Real& x, Real& y) const {
        Real d = discriminant();
        if (d<0) {
            x = y = turningPoint();
            return false;
        }
        d = sqrt(d);
        x = (-b_ -  d)/(2*a_);
        y = (-b_ + d)/(2*a_);
        return true;

    }

    alphafinder::alphafinder(boost::shared_ptr<alphaform> parametricform)
    : parametricform_(parametricform) {}


	Real alphafinder::computeLinearPart(Real alpha) {
    	Real cov =0.0;
    	parametricform_->setAlpha(alpha);

    	for (Integer i=0; i < stepindex_+1; ++i) {
        	Real vol1 = ratetwohomogeneousvols_[i]*(*parametricform_)(i);
        	cov += vol1*rateonevols_[i]*correlations_[i];
        }

    	cov *= 2*w0_*w1_;
    	return cov;
    }


	Real alphafinder::computeQuadraticPart(Real alpha) {
    	Real var =0.0;
    	parametricform_->setAlpha(alpha);

    	for (Integer i=0; i < stepindex_+1; ++i) {
        	Real vol = ratetwohomogeneousvols_[i]*(*parametricform_)(i);
        	var+= vol*vol;
        }

    	var *= w1_*w1_;
    	return var;
    }

    Real alphafinder::homogeneityfailure(Real alpha) {
        Real dum1, dum2, dum3;
        finalPart(alpha,
                  stepindex_,
                  ratetwohomogeneousvols_,
                  computeQuadraticPart(alpha),
                  computeLinearPart(alpha),
                  constantPart_,
                  dum1,
                  dum2,
                  dum3,
                  putativevols_);

	Real result=0.0;
	for (Size i=0; i<=static_cast<Size>(stepindex_)+1; ++i) {
    	Real val =  putativevols_[i]-ratetwohomogeneousvols_[i];
    	result +=val*val;
    }

	return result;
}

    bool alphafinder::finalPart(Real alphaFound,
                                Integer stepindex,
                                const std::vector<Volatility>& ratetwohomogeneousvols,
                                Real quadraticPart,
                                Real linearPart,
                                Real constantPart,
                                Real& alpha,
                                Real& a,
                                Real& b,
                                std::vector<Volatility>& ratetwovols) {
            alpha = alphaFound;
        	quadratic q2(quadraticPart, linearPart, constantPart-targetVariance_ );
        	parametricform_->setAlpha(alpha);
        	Real y; // dummy
        	q2.roots(a,y);

        	Real totalVar=0.0;
        	Real varSoFar=0.0;
        	for (Integer i =0; i < stepindex+1; ++i) {
            	ratetwovols[i] = ratetwohomogeneousvols[i] *
                	                        (*parametricform_)(i) * a;
            	varSoFar += ratetwovols[i]* ratetwovols[i];
            }

        	Real VarToFind = totalVar_-varSoFar;
        	if (VarToFind < 0)
            	return false;
        	Real requiredSd = sqrt(VarToFind);
        	b = requiredSd / (ratetwohomogeneousvols[stepindex+1] *
        	                                (*parametricform_)(stepindex));
        	ratetwovols[stepindex+1] = requiredSd;
        	return true;
    }

    Real alphafinder::valueAtTurningPoint(Real alpha) {

        linearPart_ = computeLinearPart(alpha);
        quadraticPart_ = computeQuadraticPart(alpha);

        quadratic q(quadraticPart_, linearPart_, constantPart_);
        Real valueAtTP =q.valueAtTurningPoint();
        return valueAtTP;
    }

    Real alphafinder::minusValueAtTurningPoint(Real alpha) {
        return -valueAtTurningPoint(alpha);
    }

    bool alphafinder::testIfSolutionExists(Real alpha) {
        bool aExists =  valueAtTurningPoint(alpha)<targetVariance_;
        if (!aExists)
            return false;

        Real dum1, dum2, dum3;
        return finalPart(alpha,
                         stepindex_,
                         ratetwohomogeneousvols_,
                         computeQuadraticPart(alpha),
                         computeLinearPart(alpha),
                         constantPart_,
                         dum1,
                         dum2,
                         dum3,
                         putativevols_);
    }

    bool alphafinder::solve(Real alpha0,
                            Integer stepindex, // caplet index
                            const std::vector<Volatility>& rateonevols,
                            const std::vector<Volatility>& ratetwohomogeneousvols,
                            const std::vector<Real>& correlations,
                            Real w0,
                            Real w1,
                            Real targetVariance,
                            Real tolerance,
                            Real alphaMax,
                            Real alphaMin,
                            Integer steps,
                            Real& alpha,
                            Real& a,
                            Real& b,
                            std::vector<Volatility>& ratetwovols) {
        stepindex_=stepindex;
        rateonevols_=rateonevols;
        ratetwohomogeneousvols_=ratetwohomogeneousvols;
        correlations_=correlations;
        w0_=w0;
        w1_=w1;
        totalVar_=0;
        for (Size i=0; i <=static_cast<Size>(stepindex)+1; ++i)
            totalVar_+=ratetwohomogeneousvols[i]*ratetwohomogeneousvols[i];
        targetVariance_ = targetVariance;

        // constant part will not depend on alpha

        constantPart_ =0.0;
        for (Integer i=0; i < stepindex+1; ++i)
                constantPart_+=rateonevols[i]*rateonevols[i];

        constantPart_ *= w0*w0;

        // compute linear part with initial alpha
        Real valueAtTP = valueAtTurningPoint(alpha0);

        if (valueAtTP <= targetVariance) {
            finalPart(alpha0,
                      stepindex,
                      ratetwohomogeneousvols,
                      quadraticPart_,
                      linearPart_,
                      constantPart_,
                      alpha,
                      a,
                      b,
                      ratetwovols);
            return true;
        }

        // we now have to solve
        Real bottomValue = valueAtTurningPoint(alphaMin);
        Real bottomAlpha = alphaMin;
        Real topValue = valueAtTurningPoint(alphaMax);
        Real topAlpha = alphaMax;
        Real bilimit = alpha0;

        if (bottomValue > targetVariance && topValue > targetVariance) {
            // see if if ok at some intermediate point by stepping through
            Integer i=1;
            while ( i < steps && topValue> targetVariance) {
                topAlpha = alpha0 + (alphaMax-alpha0)*(i+0.0)/(steps+0.0);
                topValue=valueAtTurningPoint(topAlpha);
                ++i;
            }

            if (topValue <= targetVariance)
                bilimit = alpha0 + (topAlpha-alpha0)*(i-2.0)/(steps+0.0);
        }

        if (bottomValue > targetVariance && topValue > targetVariance) {
            // see if if ok at some intermediate point by stepping through
            Integer i=1;
            while ( i < steps && topValue> targetVariance) {
                bottomAlpha = alpha0 + (alphaMin-alpha0)*(i+0.0)/(steps+0.0);
                bottomValue=valueAtTurningPoint(bottomAlpha);
                ++i;
            }

            if (bottomValue <= targetVariance)
                bilimit = alpha0 +(bottomAlpha-alpha0)*(i-2.0)/(steps+0.0);
        }

        if (bottomValue > targetVariance && topValue > targetVariance)
            return false;

        if (bottomValue <= targetVariance) {
            // then find root of increasing function
            // (or as if increasing function)
            alpha = Bisection<alphafinder, &alphafinder::valueAtTurningPoint>(
                                                            targetVariance,
                                                            bottomAlpha,
                                                            bilimit,
                                                            tolerance,
                                                            *this);
        } else {
            // find root of decreasing function (or as if decreasing function)
            alpha=Bisection<alphafinder, &alphafinder::minusValueAtTurningPoint>(
                                                            targetVariance,
                                                            bilimit,
                                                            topAlpha,
                                                            tolerance,
                                                            *this);
        }
        finalPart(alpha,
                  stepindex,
                  ratetwohomogeneousvols,
                  quadraticPart_,
                  linearPart_,
                  constantPart_,
                  alpha,
                  a,
                  b,
                  ratetwovols);
        return true;
    }

    bool alphafinder::solveWithMaxHomogeneity(
                        Real alpha0,
                        Integer stepindex, // caplet index
                        const std::vector<Volatility>& rateonevols,
                        const std::vector<Volatility>& ratetwohomogeneousvols,
                        const std::vector<Real>& correlations,
                        Real w0,
                        Real w1,
                        Real targetVariance,
                        Real tolerance,
                        Real alphaMax,
                        Real alphaMin,
                        Integer steps,
                        Real& alpha,
                        Real& a,
                        Real& b,
                        std::vector<Volatility>& ratetwovols) {

        stepindex_=stepindex;
        rateonevols_=rateonevols;
        ratetwohomogeneousvols_=ratetwohomogeneousvols;
        putativevols_.resize(ratetwohomogeneousvols_.size());
        correlations_=correlations;
        w0_=w0;
        w1_=w1;
        totalVar_=0;
        for (Size i=0; i <=static_cast<Size>(stepindex)+1; ++i)
            totalVar_+=ratetwohomogeneousvols[i]*ratetwohomogeneousvols[i];
        targetVariance_=targetVariance;

        // constant part will not depend on alpha

        constantPart_ =0.0;
        for (Integer i=0; i < stepindex+1; ++i)
            constantPart_+=rateonevols[i]*rateonevols[i];

        constantPart_ *= w0*w0;

        Real alpha1 = alphaMin;
        Real alpha2 = alphaMax;

        // compute linear part with initial alpha
        bool alpha0OK = testIfSolutionExists(alpha0);
        bool alphaMaxOK = testIfSolutionExists(alphaMax);
        bool alphaMinOK = testIfSolutionExists(alphaMin);

        bool foundOKPoint = alpha0OK || alphaMaxOK || alphaMinOK;

        if (foundOKPoint) {
            if (!alphaMinOK) {
                // lower alpha is bad
                if (alpha0OK) {
                    // must die somewhere in between
                    alpha1 = FindLowestOK<alphafinder, &alphafinder::testIfSolutionExists>(
                         alphaMin,
                         alpha0,
                         tolerance,
                        *this);
                } else {
                    // alphaMaxOK must be true to get here
                    alpha1 = FindLowestOK<alphafinder, &alphafinder::testIfSolutionExists>(
                         alpha0,
                         alphaMax,
                         tolerance,
                        *this);
                }
            }


            if (!alphaMaxOK) {
                // higher alpha is bad
                alpha2 = FindHighestOK<alphafinder, &alphafinder::testIfSolutionExists>(
                     alpha1,
                     alphaMax,
                     tolerance,
                     *this);
            } else
                alpha2= alphaMax;
            }
        else {
            // ok let's see if we can find a value of alpha that works
            bool foundUpOK = false;
            bool foundDownOK = false;
            Real alphaUp,alphaDown;
            Real stepSize = (alphaMax-alpha0)/steps;

            for (Size j=0;
                 j<static_cast<Size>(steps) && !foundUpOK && !foundDownOK;
                 ++j) {
                alphaUp = alpha0+j*stepSize;
                foundUpOK=testIfSolutionExists(alphaUp);
                alphaDown = alpha0-j*stepSize;
                foundDownOK=testIfSolutionExists(alphaDown);
            }
            foundOKPoint = foundUpOK || foundDownOK;
            if (!foundOKPoint)
                return false;

            if (foundUpOK) {
                alpha1 = alphaUp;
                alpha2 = FindHighestOK<alphafinder, &alphafinder::testIfSolutionExists>(
                     alpha1,
                     alphaMax,
                     tolerance,
                     *this);
            } else {
                alpha2 = alphaDown;
                alpha1 = FindLowestOK<alphafinder, &alphafinder::testIfSolutionExists>(
                     alphaMin,
                     alpha2,
                     tolerance,
                    *this);
            }
        }

        // we have now found alpha1, alpha2 such that solution exists
        // at endpoints. we now want to minimize within that interval
        bool failed;
        alpha =  Minimize<alphafinder, &alphafinder::homogeneityfailure, &alphafinder::testIfSolutionExists>(
                                        alpha1,
                                        alpha2,
                                        tolerance,
                                        *this,
                                        failed) ;

        finalPart(alpha,
                  stepindex,
                  ratetwohomogeneousvols,
                  computeQuadraticPart(alpha),
                  computeLinearPart(alpha),
                  constantPart_,
                  alpha,
                  a,
                  b,
                  ratetwovols);

        return true;;
    }
}
