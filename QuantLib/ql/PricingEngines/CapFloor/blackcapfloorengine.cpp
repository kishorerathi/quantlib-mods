/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
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

#include <ql/PricingEngines/CapFloor/blackcapfloorengine.hpp>
#include <ql/PricingEngines/blackformula.hpp>
#include <ql/Volatilities/capletconstantvol.hpp>
#include <ql/Calendars/nullcalendar.hpp>

namespace QuantLib {

    BlackCapFloorEngine::BlackCapFloorEngine(const Handle<Quote>& volatility,
                                             const DayCounter& dc) {
        volatility_.linkTo(boost::shared_ptr<CapletVolatilityStructure>(new
            CapletConstantVolatility(volatility, dc)));
        registerWith(volatility_);
    }

    BlackCapFloorEngine::BlackCapFloorEngine(
                          const Handle<CapletVolatilityStructure>& volatility)
    : volatility_(volatility) {
        registerWith(volatility_);
    }

    void BlackCapFloorEngine::update()
    {
        notifyObservers();
    }

    void BlackCapFloorEngine::calculate() const
    {
        Real value = 0.0;
        Real vega_ = 0.0;
        CapFloor::Type type = arguments_.type;
        DayCounter volatilityDayCounter = volatility_->dayCounter();
        for (Size i=0; i<arguments_.startTimes.size(); i++) {
            Time end = arguments_.endTimes[i],
                 accrualTime = arguments_.accrualTimes[i],
                 timeToMaturity = arguments_.fixingTimes[i];
            if (end > 0.0) {    // discard expired caplets
                Real nominal = arguments_.nominals[i];
                Real gearing = arguments_.gearings[i];
                DiscountFactor q = arguments_.discounts[i];
                Rate forward = arguments_.forwards[i];

                if ((type == CapFloor::Cap) ||
                    (type == CapFloor::Collar)) {
                    Rate strike = arguments_.capRates[i];
                    Real stdDev = std::sqrt(volatility_->blackVariance(
                        arguments_.fixingDates[i], strike));
                    value += q * accrualTime * nominal * gearing *
                        blackFormula(Option::Call, strike, forward, stdDev);
                    vega_ += nominal * gearing * accrualTime * q 
                            * blackStdDevDerivative(strike, forward, stdDev)
                            * std::sqrt(timeToMaturity);
                }
                if ((type == CapFloor::Floor) ||
                    (type == CapFloor::Collar)) {
                    Rate strike = arguments_.floorRates[i];
                    Real stdDev = std::sqrt(volatility_->blackVariance(
                        arguments_.fixingDates[i], strike));
                    Real temp = q * accrualTime * nominal * gearing *
                        blackFormula(Option::Put, strike, forward, stdDev);
                    if (type == CapFloor::Floor) {
                        value += temp;
                        vega_ += nominal * gearing * accrualTime * q 
                                * blackStdDevDerivative(strike, forward, stdDev)
                                * std::sqrt(timeToMaturity);
                    } else {
                        // a collar is long a cap and short a floor
                        value -= temp;
                        vega_ -= nominal * gearing * accrualTime * q 
                                * blackStdDevDerivative(strike, forward, stdDev)
                                * std::sqrt(timeToMaturity);
                    }
                }
            }
        }
        results_.value = value;
        results_.vega_ = vega_;
    }
    
}
