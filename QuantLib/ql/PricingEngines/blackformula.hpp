/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003, 2004, 2005, 2006 Ferdinando Ametrano
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2006 StatPro Italia srl

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

/*! \file blackformula.hpp
    \brief Black formula
*/

#ifndef quantlib_blackformula_hpp
#define quantlib_blackformula_hpp

#include <ql/Instruments/payoffs.hpp>

namespace QuantLib {

    /*! Black 1976 formula
        \warning instead of volatility it uses standard deviation,
                 i.e. volatility*sqrt(timeToMaturity)
    */
    Real blackFormula(Option::Type optionType,
                      Real strike,
                      Real forward,
                      Real stdDev);

    /*! Approximated Black 1976 implied standard deviation,
        i.e. volatility*sqrt(timeToMaturity).
        
        It is calculated using Brenner and Subrahmanyan (1988) and Feinstein
        (1988) approximation for at-the-money forward option, with the
        extended moneyness approximation by Corrado and Miller (1996)
    */
    Real blackImpliedStdDevApproximation(Option::Type optionType,
                                         Real strike,
                                         Real forward,
                                         Real blackPrice);

    /*! Black 1976 implied  standard deviation,
        i.e. volatility*sqrt(timeToMaturity)
    */
    Real blackImpliedStdDev(Option::Type optionType,
                            Real strike,
                            Real forward,
                            Real blackPrice,
                            Real guess = Null<Real>(),
                            Real accuracy = 1.0e-6);

    /*! Black 1976 "int the money probability" formula
        \warning instead of volatility it uses standard deviation,
                 i.e. volatility*sqrt(timeToMaturity)
    */
    Real blackItmProbability(Option::Type optionType,
                             Real strike,
                             Real forward,
                             Real stdDev);

    /*! Black 1976 formula for standard deviation derivative
        \warning instead of volatility it uses standard deviation, i.e.
                 volatility*sqrt(timeToMaturity), and it returns the
                 derivative with respect to the standard deviation.
                 If T is the time to maturity Black vega would be
                 blackStdDevDerivative(strike, forward, stdDev)*sqrt(T)
    */
    Real blackStdDevDerivative(Rate strike,
                               Rate forward,
                               Real stdDev);


    //! Black-formula calculator class
    /*! \bug When the variance is null, division by zero occur during
             the calculation of delta, delta forward, gamma, gamma
             forward, rho, dividend rho, vega, and strike sensitivity.
    */
    class BlackFormula {
      private:
        class Calculator;
        friend class Calculator;
      public:
        BlackFormula(Real forward,
                     DiscountFactor discount,
                     Real variance,
                     const boost::shared_ptr<StrikedTypePayoff>& payoff);
        Real value() const;
        Real delta(Real spot) const;
        //! Sensitivity in percent to a percent movement in the underlying.
        Real elasticity(Real spot) const;
        Real gamma(Real spot) const;
        Real deltaForward() const;
        //! Sensitivity in percent to a percent movement in the forward price.
        Real elasticityForward() const;
        Real gammaForward() const;
        Real theta(Real spot, Time maturity) const;
        Real thetaPerDay(Real spot, Time maturity) const;
        Real vega(Time maturity) const;
        Real rho(Time maturity) const;
        Real dividendRho(Time maturity) const;
        /*! Probability of being in the money in the bond martingale measure.
            It is a risk-neutral probability, not the real world probability.
        */
        Real itmCashProbability() const;
        /*! Probability of being in the money in the asset martingale measure.
            It is a risk-neutral probability, not the real world probability.
        */
        Real itmAssetProbability() const;
        Real strikeSensitivity() const;
        Real alpha() const;
        Real beta() const;
      private:
        Real forward_, discount_, variance_;
        Real stdDev_, strike_;
        Real D1_, D2_, alpha_, beta_, DalphaDd1_, DbetaDd2_;
        Real n_d1_, cum_d1_, n_d2_, cum_d2_;
        Real X_, DXDs_, DXDstrike_;
    };

    /*! Black style formulas when forward is normal rather than log-normal, 
	this is essentially the model of Bachelier 
    */
	
Real bachelierBlackPut( Real strike, 
					  Real forward,  
					  Real absoluteVolatility,
					  Real maturity, 
					  Real annuity);

Real bachelierBlackCall(Real strike, 
					  Real forward,  
					  Real absoluteVolatility,
					  Real maturity, 
					  Real annuity);

}


#endif
