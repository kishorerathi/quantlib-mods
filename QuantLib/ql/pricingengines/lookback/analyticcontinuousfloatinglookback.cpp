/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Warren Chou
 Copyright (C) 2007 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/pricingengines/lookback/analyticcontinuousfloatinglookback.hpp>
#include <ql/exercise.hpp>

namespace QuantLib {

    AnalyticContinuousFloatingLookbackEngine::
    AnalyticContinuousFloatingLookbackEngine(
             const boost::shared_ptr<GeneralizedBlackScholesProcess>& process)
    : process_(process) {
        registerWith(process_);
    }

    void AnalyticContinuousFloatingLookbackEngine::calculate() const {

        boost::shared_ptr<FloatingTypePayoff> payoff =
            boost::dynamic_pointer_cast<FloatingTypePayoff>(arguments_.payoff);
        QL_REQUIRE(payoff, "Non-floating payoff given");

        QL_REQUIRE(process_->x0() > 0.0, "negative or null underlying");

        switch (payoff->optionType()) {
          case Option::Call:
            results_.value = A(1);
            break;
          case Option::Put:
            results_.value = A(-1);
            break;
          default:
            QL_FAIL("Unknown type");
        }
    }

    Real AnalyticContinuousFloatingLookbackEngine::underlying() const {
        return process_->x0();
    }

    Time AnalyticContinuousFloatingLookbackEngine::residualTime() const {
        return process_->time(arguments_.exercise->lastDate());
    }

    Volatility AnalyticContinuousFloatingLookbackEngine::volatility() const {
        return process_->blackVolatility()->blackVol(residualTime(), minmax());
    }

    Real AnalyticContinuousFloatingLookbackEngine::stdDeviation() const {
        return volatility() * std::sqrt(residualTime());
    }

    Rate AnalyticContinuousFloatingLookbackEngine::riskFreeRate() const {
        return process_->riskFreeRate()->zeroRate(residualTime(), Continuous,
                                                  NoFrequency);
    }

    DiscountFactor AnalyticContinuousFloatingLookbackEngine::riskFreeDiscount()
                                 const {
        return process_->riskFreeRate()->discount(residualTime());
    }

    Rate AnalyticContinuousFloatingLookbackEngine::dividendYield() const {
        return process_->dividendYield()->zeroRate(residualTime(),
                                                   Continuous, NoFrequency);
    }

    DiscountFactor AnalyticContinuousFloatingLookbackEngine::dividendDiscount()
                                 const {
        return process_->dividendYield()->discount(residualTime());
    }

    Real AnalyticContinuousFloatingLookbackEngine::minmax() const {
        return arguments_.minmax;
    }

    Real AnalyticContinuousFloatingLookbackEngine::A(Real eta) const {
        Volatility vol = volatility();
        Real lambda = 2.0*(riskFreeRate() - dividendYield())/(vol*vol);
        Real SS = underlying()/minmax();
        Real d1 =
            std::log(SS)/stdDeviation() + 0.5*(lambda+1.0)*stdDeviation();
        Real N1 = f_(eta*d1);
        Real N2 = f_(eta*(d1-stdDeviation()));
        Real N3 = f_(eta*(-d1+lambda*stdDeviation()));
        Real N4 = f_(eta*-d1);
        Real powSS = std::pow(SS, -lambda);
        return eta*((underlying() * dividendDiscount() * N1 -
                    minmax() * riskFreeDiscount() * N2) +
                    (underlying() * riskFreeDiscount() *
                    (powSS * N3 - dividendDiscount()* N4/riskFreeDiscount())/
            lambda));
    }

}

