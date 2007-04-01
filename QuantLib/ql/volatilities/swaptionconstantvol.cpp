/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
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

#include <ql/volatilities/swaptionconstantvol.hpp>
#include <ql/quotes/simplequote.hpp>

namespace QuantLib {

    SwaptionConstantVolatility::SwaptionConstantVolatility(
                                              const Date& referenceDate,
                                              Volatility volatility,
                                              const DayCounter& dayCounter)
    : SwaptionVolatilityStructure(referenceDate),
      volatility_(boost::shared_ptr<Quote>(new SimpleQuote(volatility))),
      dayCounter_(dayCounter), maxSwapTenor_(100*Years) {}

    SwaptionConstantVolatility::SwaptionConstantVolatility(
                                              const Date& referenceDate,
                                              const Handle<Quote>& volatility,
                                              const DayCounter& dayCounter)
    : SwaptionVolatilityStructure(referenceDate), volatility_(volatility),
      dayCounter_(dayCounter), maxSwapTenor_(100*Years) {
        registerWith(volatility_);
    }

    SwaptionConstantVolatility::SwaptionConstantVolatility(
                                              Natural settlementDays,
                                              const Calendar& calendar,
                                              Volatility volatility,
                                              const DayCounter& dayCounter)
    : SwaptionVolatilityStructure(settlementDays, calendar),
      volatility_(boost::shared_ptr<Quote>(new SimpleQuote(volatility))),
      dayCounter_(dayCounter), maxSwapTenor_(100*Years) {}

    SwaptionConstantVolatility::SwaptionConstantVolatility(
                                              Natural settlementDays,
                                              const Calendar& calendar,
                                              const Handle<Quote>& volatility,
                                              const DayCounter& dayCounter)
    : SwaptionVolatilityStructure(settlementDays, calendar),
      volatility_(volatility), dayCounter_(dayCounter),
      maxSwapTenor_(100*Years) {
        registerWith(volatility_);
    }

    Volatility SwaptionConstantVolatility::volatilityImpl(const Date&,
                                                          const Period&,
                                                          Rate) const {
        return volatility_->value();
    }

    Volatility SwaptionConstantVolatility::volatilityImpl(
                                                     Time, Time, Rate) const {
        return volatility_->value();
    }

    boost::shared_ptr<SmileSection>
    SwaptionConstantVolatility::smileSection(Time optionTime,
                                             Time) const {
        Volatility atmVol = volatility_->value();
        return boost::shared_ptr<SmileSection>(new
            FlatSmileSection(optionTime, atmVol));
    }

    boost::shared_ptr<SmileSection>
    SwaptionConstantVolatility::smileSection(const Date& optionDate,
                                             const Period&) const {
        Volatility atmVol = volatility_->value();
        return boost::shared_ptr<SmileSection>(new
            FlatSmileSection(timeFromReference(optionDate), atmVol));
    }

}
