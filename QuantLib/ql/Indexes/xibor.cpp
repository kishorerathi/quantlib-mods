/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl

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

#include <ql/Indexes/xibor.hpp>
#include <ql/Indexes/indexmanager.hpp>
#include <sstream>

namespace QuantLib {

    #ifndef QL_DISABLE_DEPRECATED
    Xibor::Xibor(const std::string& familyName,
                 const Period& tenor,
                 Integer settlementDays,
                 const Currency& currency,
                 const Calendar& calendar,
                 BusinessDayConvention convention,
                 const DayCounter& dayCounter,
                 const Handle<YieldTermStructure>& h)
    : InterestRateIndex(familyName, tenor, settlementDays, currency,
                        calendar, dayCounter),
      convention_(convention), termStructure_(h), endOfMonth_(true) {
        registerWith(termStructure_);
    }
    #endif
    Xibor::Xibor(const std::string& familyName,
                 const Period& tenor,
                 Integer settlementDays,
                 const Currency& currency,
                 const Calendar& calendar,
                 BusinessDayConvention convention,
                 bool endOfMonth,
                 const DayCounter& dayCounter,
                 const Handle<YieldTermStructure>& h)
    : InterestRateIndex(familyName, tenor, settlementDays, currency,
                        calendar, dayCounter),
      convention_(convention), termStructure_(h), endOfMonth_(endOfMonth) {
        registerWith(termStructure_);
      }

    Rate Xibor::forecastFixing(const Date& fixingDate) const
    {
        QL_REQUIRE(!termStructure_.empty(),
                   "no forecasting term structure set to " << name());
        Date fixingValueDate = valueDate(fixingDate);
        Date endValueDate = maturityDate(fixingValueDate);
        DiscountFactor fixingDiscount =
            termStructure_->discount(fixingValueDate);
        DiscountFactor endDiscount =
            termStructure_->discount(endValueDate);
        Time fixingPeriod =
            dayCounter_.yearFraction(fixingValueDate, endValueDate);
        return (fixingDiscount/endDiscount-1.0) / fixingPeriod;
    }

    Date Xibor::maturityDate(const Date& valueDate) const {
        return calendar_.advance(valueDate, tenor_, convention_,endOfMonth_);
    }

}

