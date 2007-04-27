/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2006, 2007 Cristina Duminuco
 Copyright (C) 2006, 2007 Giorgio Facchinetti
 Copyright (C) 2006 Mario Pucci
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004 StatPro Italia srl

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

/*! \file cashflowvectors.hpp
    \brief Cash flow vector builders
*/

#ifndef quantlib_cash_flow_vectors_hpp
#define quantlib_cash_flow_vectors_hpp

#include <ql/cashflow.hpp>
#include <ql/time/schedule.hpp>
#include <ql/cashflows/iborcoupon.hpp>
#include <ql/cashflows/cmscoupon.hpp>
#include <ql/indexes/swapindex.hpp>

namespace QuantLib {

    //! helper function building a sequence of fixed rate coupons
    Leg FixedRateLeg(const std::vector<Real>& nominals,
                     const Schedule& schedule,
                     const std::vector<Rate>& couponRates,
                     const DayCounter& paymentDayCounter,
                     BusinessDayConvention paymentAdjustment = Following,
                     const DayCounter& firstPeriodDayCounter = DayCounter());

    //! helper function building a sequence of par coupons
    /*! Either UpFrontIndexedCoupons or ParCoupons are used depending
        on the library configuration.

        \todo A suitable algorithm should be implemented for the
              calculation of the interpolated index fixing for a
              short/long first coupon.
    */

    //! helper function building a sequence of capped/floored ibor rate coupons
    Leg IborLeg(const std::vector<Real>& nominals,
                const Schedule& schedule,
                const boost::shared_ptr<IborIndex>& index,
                const DayCounter& paymentDayCounter = DayCounter(),
                const BusinessDayConvention paymentConvention = Following,
                Natural fixingDays = Null<Size>(),
                const std::vector<Real>& gearings = std::vector<Real>(),
                const std::vector<Spread>& spreads = std::vector<Spread>(),
                const std::vector<Rate>& caps = std::vector<Rate>(),
                const std::vector<Rate>& floors = std::vector<Rate>(),
                bool isInArrears = false);

    //! helper function building a sequence of capped/floored cms rate coupons
    Leg CmsLeg(const std::vector<Real>& nominals,
               const Schedule& schedule,
               const boost::shared_ptr<SwapIndex>& index,
               const DayCounter& paymentDayCounter = DayCounter(),
               BusinessDayConvention paymentConvention = Following,
               Natural fixingDays = Null<Size>(),
               const std::vector<Real>& gearings = std::vector<Real>(),
               const std::vector<Spread>& spreads = std::vector<Spread>(),
               const std::vector<Rate>& caps = std::vector<Rate>(),
               const std::vector<Rate>& floors = std::vector<Rate>(),
               bool isInArrears = false);
    
    //! helper function building a sequence of capped/floored cms zero rate coupons
    Leg CmsZeroLeg(const std::vector<Real>& nominals,
                   const Schedule& schedule,
                   const boost::shared_ptr<SwapIndex>& index,
                   const DayCounter& paymentDayCounter = DayCounter(),
                   BusinessDayConvention paymentConvention = Following,
                   Natural fixingDays = Null<Natural>(),
                   const std::vector<Real>& gearings = std::vector<Real>(),
                   const std::vector<Spread>& spreads =std::vector<Spread>(),
                   const std::vector<Rate>& caps = std::vector<Rate>(),
                   const std::vector<Rate>& floors = std::vector<Rate>());
        
    //! helper function building a sequence of range accrual floaters coupons
    Leg RangeAccrualLeg(const std::vector<Real>& nominals,
                       const Schedule& schedule,
                       const boost::shared_ptr<IborIndex>& index,
                       const DayCounter& paymentDayCounter = DayCounter(),
                       BusinessDayConvention paymentConvention = Following,
                       Natural fixingDays = Null<Natural>(),
                       const std::vector<Real>& gearings = std::vector<Real>(),
                       const std::vector<Spread>& spreads =std::vector<Spread>(),
                       const std::vector<Rate>& lowerTriggers = std::vector<Rate>(),
                       const std::vector<Rate>& upperTriggers = std::vector<Rate>(),
                       const Period& observationTenor = 1*Days,
                       BusinessDayConvention observationConvention = ModifiedFollowing);
}

#endif
