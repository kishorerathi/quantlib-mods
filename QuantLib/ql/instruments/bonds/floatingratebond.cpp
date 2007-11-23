/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2007 Chiara Fornarola

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

#include <ql/instruments/bonds/floatingratebond.hpp>
#include <ql/cashflows/iborcoupon.hpp>
#include <ql/cashflows/simplecashflow.hpp>
#include <ql/time/schedule.hpp>
#include <ql/indexes/swapindex.hpp>
#include <ql/indexes/iborindex.hpp>

namespace QuantLib {

    FloatingRateBond::FloatingRateBond(
                           Natural settlementDays,
                           Real faceAmount,
                           const Schedule& schedule,
                           const boost::shared_ptr<IborIndex>& index,
                           const DayCounter& paymentDayCounter,
                           BusinessDayConvention paymentConvention,
                           Natural fixingDays,
                           const std::vector<Real>& gearings,
                           const std::vector<Spread>& spreads,
                           const std::vector<Rate>& caps,
                           const std::vector<Rate>& floors,
                           bool inArrears,
                           Real redemption,
                           const Date& issueDate)
    : Bond(settlementDays, schedule.calendar(), faceAmount, schedule.endDate(), issueDate) {

        cashflows_ = IborLeg(schedule, index)
            .withNotionals(faceAmount_)
            .withPaymentDayCounter(paymentDayCounter)
            .withPaymentAdjustment(paymentConvention)
            .withFixingDays(fixingDays)
            .withGearings(gearings)
            .withSpreads(spreads)
            .withCaps(caps)
            .withFloors(floors)
            .inArrears(inArrears);

        Date redemptionDate = calendar_.adjust(maturityDate_,
                                               paymentConvention);
        cashflows_.push_back(boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(faceAmount_*redemption/100.0, redemptionDate)));

        QL_ENSURE(!cashflows().empty(), "bond with no cashflows!");

        registerWith(index);
    }

    FloatingRateBond::FloatingRateBond(
                           Natural settlementDays,
                           Real faceAmount,
                           const Date& startDate,
                           const Date& maturityDate,
                           Frequency couponFrequency,
                           const Calendar& calendar,
                           const boost::shared_ptr<IborIndex>& index,
                           const DayCounter& accrualDayCounter,
                           BusinessDayConvention accrualConvention,
                           BusinessDayConvention paymentConvention,
                           Natural fixingDays,
                           const std::vector<Real>& gearings,
                           const std::vector<Spread>& spreads,
                           const std::vector<Rate>& caps,
                           const std::vector<Rate>& floors,
                           bool inArrears,
                           Real redemption,
                           const Date& issueDate,
                           const Date& stubDate,
                           DateGeneration::Rule rule,
                           bool endOfMonth)
    : Bond(settlementDays, calendar, faceAmount, maturityDate, issueDate) {


        Date firstDate, nextToLastDate;
        switch (rule) {
          case DateGeneration::Backward:
            firstDate = Date();
            nextToLastDate = stubDate;
            break;
          case DateGeneration::Forward:
            firstDate = stubDate;
            nextToLastDate = Date();
            break;
          case DateGeneration::Zero:
          case DateGeneration::ThirdWednesday:
            QL_FAIL("stub date (" << stubDate << ") not allowed with " <<
                    rule << " DateGeneration::Rule");
          default:
            QL_FAIL("unknown DateGeneration::Rule (" << Integer(rule) << ")");
        }

        Schedule schedule(startDate, maturityDate_, Period(couponFrequency),
                          calendar_, accrualConvention, accrualConvention,
                          rule, endOfMonth,
                          firstDate, nextToLastDate);

        cashflows_ = IborLeg(schedule, index)
            .withNotionals(faceAmount_)
            .withPaymentDayCounter(accrualDayCounter)
            .withPaymentAdjustment(paymentConvention)
            .withFixingDays(fixingDays)
            .withGearings(gearings)
            .withSpreads(spreads)
            .withCaps(caps)
            .withFloors(floors)
            .inArrears(inArrears);

        Date redemptionDate = calendar_.adjust(maturityDate_,
                                               paymentConvention);
        cashflows_.push_back(boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(faceAmount_*redemption/100.0, redemptionDate)));

        QL_ENSURE(!cashflows().empty(), "bond with no cashflows!");

        registerWith(index);
    }

}
