
/*
 Copyright (C) 2003, 2004 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/PricingEngines/Asian/analytic_cont_geom_av_price.hpp>
#include <ql/PricingEngines/blackformula.hpp>

namespace QuantLib {

    void AnalyticContinuousGeometricAveragePriceAsianEngine::calculate() const {

        QL_REQUIRE(arguments_.averageType == Average::Geometric,
                   "not a geometric average option");

        QL_REQUIRE(arguments_.exercise->type() == Exercise::European,
                   "not an European Option");

        Date exercise = arguments_.exercise->lastDate();

        boost::shared_ptr<PlainVanillaPayoff> payoff =
            boost::dynamic_pointer_cast<PlainVanillaPayoff>(arguments_.payoff);
        QL_REQUIRE(payoff, "non-plain payoff given");

        const boost::shared_ptr<BlackScholesProcess>& process =
            arguments_.blackScholesProcess;


        Volatility volatility = 
            process->blackVolatility()->blackVol(exercise, payoff->strike());
        Real variance = 
            process->blackVolatility()->blackVariance(exercise,
                                                      payoff->strike());
        DiscountFactor riskFreeDiscount =
            process->riskFreeRate()->discount(exercise);

        #ifndef QL_DISABLE_DEPRECATED
        DayCounter rfdc  = process->riskFreeRate()->dayCounter();
        DayCounter divdc = process->dividendYield()->dayCounter();
        DayCounter voldc = process->blackVolatility()->dayCounter();
        #else
        DayCounter rfdc = Settings::instance().dayCounter();
        DayCounter divdc = Settings::instance().dayCounter();
        DayCounter voldc = Settings::instance().dayCounter();
        #endif

        Spread dividendYield = 0.5 * (
            // process->riskFreeRate()->zeroYield(exercise) +
            process->riskFreeRate()->zeroRate(exercise, rfdc, Continuous, Annual) +
            // process->dividendYield()->zeroYield(exercise) +
            process->dividendYield()->zeroRate(exercise, divdc, Continuous, Annual) +
            volatility*volatility/6.0);

        Time t_q = divdc.yearFraction(
            process->dividendYield()->referenceDate(), exercise);
        DiscountFactor dividendDiscount = QL_EXP(-dividendYield*t_q);

        Real spot = process->stateVariable()->value();
        Real forward = spot * dividendDiscount / riskFreeDiscount;

        BlackFormula black(forward, riskFreeDiscount, variance/3.0, payoff);

        results_.value = black.value();
        results_.delta = black.delta(spot);
        results_.gamma = black.gamma(spot);

        results_.dividendRho = black.dividendRho(t_q)/2.0;

        Time t_r = rfdc.yearFraction(process->riskFreeRate()->referenceDate(),
                                     arguments_.exercise->lastDate());
        results_.rho = black.rho(t_r) + 0.5 * black.dividendRho(t_q);

        Time t_v = voldc.yearFraction(
            process->blackVolatility()->referenceDate(),
            arguments_.exercise->lastDate());
        results_.vega = black.vega(t_v)/QL_SQRT(3.0) +
                        black.dividendRho(t_q)*volatility/6.0;
        try {
            results_.theta = black.theta(spot, t_v);
        } catch (Error&) {
            results_.theta = Null<Real>();
        }
    }

}

