/*
 Copyright (C) 2004 Jeff Yu
 Copyright (C) 2004 M-Dimension Consulting Inc.

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

#include <ql/Instruments/fixedcouponbond.hpp>
#include <ql/CashFlows/cashflowvectors.hpp>

namespace QuantLib {

    FixedCouponBond::FixedCouponBond(const Date& issueDate,
                                     const Date& datedDate,
                                     const Date& maturityDate,
                                     Integer settlementDays,
                                     Rate coupon,
                                     Frequency couponFrequency,
                                     const DayCounter& dayCounter,
                                     const Calendar& calendar,
                                     BusinessDayConvention convention,
                                     Real redemption)
    : Bond(dayCounter, calendar, settlementDays) {

        issueDate_ = issueDate;
        datedDate_ = datedDate;
        maturityDate_ = maturityDate;

        redemption_ = redemption;

        Schedule schedule(calendar, datedDate, maturityDate,
                          couponFrequency, convention,
                          Date(), true);

        cashFlows_ = FixedRateCouponVector(schedule, convention,
                                           std::vector<Real>(1, 100.0),
                                           std::vector<Rate>(1, coupon),
                                           dayCounter);
    }

};

