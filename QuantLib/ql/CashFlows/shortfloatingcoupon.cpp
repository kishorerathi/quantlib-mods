/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

//#include <ql/CashFlows/shortfloatingcoupon.hpp>
//
//namespace QuantLib {
//
//    Short<ParCoupon>::Short(
//        const Date& paymentDate, const Real nominal, 
//        const Date& startDate, const Date& endDate,
//        const Integer fixingDays, const boost::shared_ptr<IborIndex>& index,
//        const Real gearing, const Spread spread,
//        const Date& refPeriodStart, const Date& refPeriodEnd,
//        const DayCounter& dayCounter,
//        bool isInArrears)
//    : ParCoupon(paymentDate, nominal,
//                startDate, endDate,
//                fixingDays, index,
//                gearing, spread,
//                refPeriodStart, refPeriodEnd,
//                dayCounter, isInArrears) {}
//
//    Real Short<ParCoupon>::rate() const {
//        Date today = Settings::instance().evaluationDate();
//        Date fixing_date = fixingDate();
//        QL_REQUIRE(fixing_date >= today,
//                   // must have been fixed
//                   // but we have no way to interpolate the fixing yet
//                   "short/long floating coupons not supported yet"
//                   << " (start = " << accrualStartDate_
//                   << ", end = " << accrualEndDate_ << ")");
//        return ParCoupon::rate();
//    }
//
//}

