/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2006 Cristina Duminuco
 Copyright (C) 2006, 2007 StatPro Italia srl

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

#include <ql/pricingengines/swaption/blackswaptionengine.hpp>
#include <ql/pricingengines/blackformula.hpp>
#include <ql/termstructures/volatilities/swaptionconstantvol.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/calendars/nullcalendar.hpp>

namespace QuantLib {

    BlackSwaptionEngine::BlackSwaptionEngine(const Handle<Quote>& volatility)
    : volatility_(boost::shared_ptr<SwaptionVolatilityStructure>(
              new SwaptionConstantVolatility(0, NullCalendar(),
                                             volatility, Actual365Fixed()))) {
        registerWith(volatility_);
    }

    BlackSwaptionEngine::BlackSwaptionEngine(
                        const Handle<SwaptionVolatilityStructure>& volatility)
    : volatility_(volatility) {
        registerWith(volatility_);
    }

    void BlackSwaptionEngine::update()
    {
        notifyObservers();
    }


    void BlackSwaptionEngine::calculate() const
    {
        static const Spread basisPoint = 1.0e-4;
        Time exercise = arguments_.stoppingTimes[0];
        Time maturity = arguments_.floatingPayTimes.back();
        Real annuity;
        switch(arguments_.settlementType) {
          case Settlement::Physical :
            annuity = arguments_.fixedBPS/basisPoint;
            break;
          case Settlement::Cash :
            annuity = arguments_.fixedCashBPS/basisPoint;
            break;
          default:
            QL_FAIL("unknown settlement type");
        }
        Volatility vol = volatility_->volatility(exercise,
                                                 maturity-exercise,
                                                 arguments_.fixedRate);
        Option::Type w = arguments_.type==VanillaSwap::Payer ?
                                                Option::Call : Option::Put;
        results_.value = annuity * blackFormula(w, arguments_.fixedRate,
                                                arguments_.fairRate,
                                                vol*std::sqrt(exercise));
        Real variance = volatility_->blackVariance(exercise,
                                                 maturity-exercise,
                                                 arguments_.fixedRate);
        Real stdDev = std::sqrt(variance);
        Rate forward = arguments_.fairRate;
        Rate strike = arguments_.fixedRate;
        results_.additionalResults["vega"] =  std::sqrt(exercise) *
            blackStdDevDerivative(strike, forward, stdDev, annuity);
    }
}
