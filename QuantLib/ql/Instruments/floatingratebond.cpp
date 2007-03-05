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
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/Instruments/floatingratebond.hpp>
#include <ql/CashFlows/cashflowvectors.hpp>
#include <ql/CashFlows/simplecashflow.hpp>

namespace QuantLib {

    FloatingRateBond::FloatingRateBond(
                           Size settlementDays,
                           Real faceAmount,
                           const Schedule& schedule,
                           const boost::shared_ptr<IborIndex>& index,
                           const DayCounter& paymentDayCounter,
                           BusinessDayConvention paymentConvention,
                           Size fixingDays,
                           const std::vector<Real>& gearings,
                           const std::vector<Spread>& spreads,
                           const std::vector<Rate>& caps,
                           const std::vector<Rate>& floors,
                           bool inArrears,
                           Real redemption,
                           const Date& issueDate,
                           const Handle<YieldTermStructure>& discountCurve)
    : Bond(settlementDays, faceAmount, schedule.calendar(),
           paymentDayCounter, paymentConvention, discountCurve) {

        datedDate_    = schedule.startDate();
        maturityDate_ = schedule.endDate();
        frequency_    = schedule.tenor().frequency();
        issueDate_    = (issueDate==Date() ? datedDate_ : issueDate);

        cashflows_ = IborLeg(std::vector<Real>(1, faceAmount_),
                             schedule,
                             index,
                             paymentDayCounter,
                             paymentConvention,
                             fixingDays,
                             gearings, spreads,
                             caps, floors,
                             inArrears);

        Date redemptionDate = calendar_.adjust(maturityDate_,
                                               paymentConvention);
        cashflows_.push_back(boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(faceAmount_*redemption/100.0, redemptionDate)));

        QL_ENSURE(!cashflows().empty(), "bond with no cashflows!");

        registerWith(index);
    }

}
