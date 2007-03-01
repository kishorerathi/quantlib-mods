/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006, 2007 StatPro Italia srl

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

#include <ql/Instruments/floatingratebond.hpp>

namespace QuantLib {

    FloatingRateBond::FloatingRateBond(
                Real faceAmount,
                const Date& issueDate,
                const Date& datedDate,
                const Date& maturityDate,
                Integer settlementDays,
                const boost::shared_ptr<IborIndex>& index,
                Integer fixingDays,
                const std::vector<Real>& gearings,
                const std::vector<Spread>& spreads,
                Frequency couponFrequency,
                const Calendar& calendar,
                const DayCounter& dayCounter,
                BusinessDayConvention accrualConvention,
                BusinessDayConvention paymentConvention,
                Real redemption,
                const Handle<YieldTermStructure>& discountCurve,
                const Date& stub, bool fromEnd)
    : CappedFlooredCouponBond(settlementDays, issueDate, calendar, datedDate,
                              couponFrequency, maturityDate, accrualConvention,
                              faceAmount, index, dayCounter,
                              Handle<CapletVolatilityStructure>(),
                              fixingDays, paymentConvention, gearings, spreads,
                              std::vector<Rate>(), std::vector<Rate>(),
                              discountCurve, redemption, stub, fromEnd) {}

}

