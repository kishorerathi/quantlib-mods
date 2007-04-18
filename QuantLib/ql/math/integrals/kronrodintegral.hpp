/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Francois du Vignaud
 Copyright (C) 2003 Niels Elken S�nderby
 
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

/*! \file kronrodintegral.hpp
    \brief Integral of a 1-dimensional function using the Gauss-Kronrod method

*/

#ifndef quantlib_kronrod_integral_hpp
#define quantlib_kronrod_integral_hpp

#include <ql/errors.hpp>
#include <ql/types.hpp>
#include <ql/utilities/null.hpp>
#include <boost/function.hpp>
#include <ql/math/integrals/integral.hpp>

namespace QuantLib {

    //! Integral of a 1-dimensional function using the Gauss-Kronrod method
    /*! References:

        Gauss-Kronrod Integration
        <http://mathcssun1.emporia.edu/~oneilcat/ExperimentApplet3/ExperimentApplet3.html>

        NMS - Numerical Analysis Library
        <http://www.math.iastate.edu/burkardt/f_src/nms/nms.html>

        \test the correctness of the result is tested by checking it
              against known good values.
    */

    //typedef boost::function<Real (Real)> Integrand;

    /*! 
        The gaussKronrodNonAdaptative free function is a non-adaptive 
        integration procedure which uses fixed Gauss-Kronrod abscissae 
        to sample the integrand at a maximum of 87 points. 
        It is provided for fast integration of smooth functions.
        This function applies the Gauss-Kronrod 10-point, 21-point, 43-point 
        and 87-point integration rules in succession until an estimate of the 
        integral of f over (a, b) is achieved within the desired absolute and 
        relative error limits, epsabs and epsrel. The function returns the 
        final approximation, result, an estimate of the absolute error, 
        abserr and the number of function evaluations used, neval. The 
        Gauss-Kronrod rules are designed in such a way that each rule uses 
        all the results of its predecessors, in order to minimize the total 
        number of function evaluations. 
        
        f: integrand
        a:lower limit of integration
        b: upper limit of integration
        epsAbs: absolute accuracy requested
        epsRel: epsRel relative accuracy requested
        result: approximation to the integral i
                    result is obtained by applying the 21-point
                    gauss-kronrod rule (res21) obtained by optimal
                    addition of abscissae to the 10-point gauss rule
                    (res10), or by applying the 43-point rule (res43)
                    obtained by optimal addition of abscissae to the
                    21-point gauss-kronrod rule, or by applying the
                    87-point rule (res87) obtained by optimal addition
                    of abscissae to the 43-point rule.
        abserr: estimate of the modulus of the absolute error,
                    which should equal or exceed abs(i-result)
        neval: number of integrand evaluations
        This function returns true if the requested accuracy has been achieved.
    */

	class GaussKronrodNonAdaptive : public Integrator {
		public:
			GaussKronrodNonAdaptive(Real absoluteAccuracy, Size maxEvaluations,
								Real relativeAccuracy);
		Real operator()(const boost::function<Real (Real)>& f,
                        Real a, Real b) const;
		void setRelativeAccuracy(Real);	
		Real relativeAccuracy() const;

		private:
			Real relativeAccuracy_;
	};


    //bool gaussKronrodNonAdaptative(const Integrand& f,
    //                               Real a,
    //                               Real b, 
    //                               Real epsAbs,
    //                               Real epsRel,
    //                               Real& result,
    //                               Real& absErr, 
    //                               Integer& nEval);
    class KronrodIntegral {
      public:
        
        KronrodIntegral(Real tolerance,
                        Size maxFunctionEvaluations = Null<Size>());

        template <class F>
        Real operator()(const F& f, Real a, Real b) const {

            if (a == b)
                return 0.0;
            if (a > b)
                return -(*this)(f,b,a);

            functionEvaluations_ = 0;
            Real result = GaussKronrod(f, a, b, tolerance_);
            return result;
        }

        Size functionEvaluations() { return functionEvaluations_; }

      private:
        template <class F>
        Real GaussKronrod(const F& f,
                          Real a, Real b,
                          Real tolerance) const {
            // weights for 7-point Gauss-Legendre integration
            // (only 4 values out of 7 are given as they are symmetric)
            static const Real g7w[] = { 0.417959183673469,
                                        0.381830050505119,
                                        0.279705391489277,
                                        0.129484966168870 };
            // weights for 15-point Gauss-Kronrod integration
            static const Real k15w[] = { 0.209482141084728,
                                         0.204432940075298,
                                         0.190350578064785,
                                         0.169004726639267,
                                         0.140653259715525,
                                         0.104790010322250,
                                         0.063092092629979,
                                         0.022935322010529 };
            // abscissae (evaluation points)
            // for 15-point Gauss-Kronrod integration
            static const Real k15t[] = { 0.000000000000000,
                                         0.207784955007898,
                                         0.405845151377397,
                                         0.586087235467691,
                                         0.741531185599394,
                                         0.864864423359769,
                                         0.949107912342758,
                                         0.991455371120813 };

            Real halflength = (b - a) / 2;
            Real center = (a + b) / 2;

            Real g7; // will be result of G7 integral
            Real k15; // will be result of K15 integral

            Real t, fsum; // t (abscissa) and f(t)
            Real fc = f(center);
            g7 = fc * g7w[0];
            k15 = fc * k15w[0];

            // calculate g7 and half of k15
            Integer j, j2;
            for (j = 1, j2 = 2; j < 4; j++, j2 += 2) {
                t = halflength * k15t[j2];
                fsum = f(center - t) + f(center + t);
                g7  += fsum * g7w[j];
                k15 += fsum * k15w[j2];
            }

            // calculate other half of k15
            for (j2 = 1; j2 < 8; j2 += 2) {
                t = halflength * k15t[j2];
                fsum = f(center - t) + f(center + t);
                k15 += fsum * k15w[j2];
            }

            // multiply by (a - b) / 2
            g7 = halflength * g7;
            k15 = halflength * k15;

            // 15 more function evaluations have been used
            functionEvaluations_ += 15;

            // error is <= k15 - g7
            // if error is larger than tolerance then split the interval
            // in two and integrate recursively
            if (std::fabs(k15 - g7) < tolerance) {
                return k15;
            } else {
                QL_REQUIRE(functionEvaluations_+30 <=
                           maxFunctionEvaluations_,
                           "maximum number of function evaluations "
                           "exceeded");
                return GaussKronrod(f, a, center, tolerance/2)
                    + GaussKronrod(f, center, b, tolerance/2);
            }
        }

        Real tolerance_;
        mutable Size functionEvaluations_;
        Size maxFunctionEvaluations_;
    };

    inline KronrodIntegral::KronrodIntegral(Real tolerance,
                                            Size maxFunctionEvaluations)
    : tolerance_(tolerance),
      maxFunctionEvaluations_(maxFunctionEvaluations) {
        QL_REQUIRE(tolerance > QL_EPSILON,
                   std::scientific
                   << "required tolerance (" << tolerance
                   << ") not allowed. It must be > " << QL_EPSILON);
        QL_REQUIRE(maxFunctionEvaluations >= 15,
                   "required maxFunctionEvaluations ("
                   << maxFunctionEvaluations
                   << ") not allowed. It must be >= 15");
    }

}


#endif
