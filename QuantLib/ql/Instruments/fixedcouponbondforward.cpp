/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Allen Kuo

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

#include <ql/Instruments/fixedcouponbondforward.hpp>

namespace QuantLib {

    FixedCouponBondForward::FixedCouponBondForward(
                    const Date& valueDate,
                    const Date& maturityDate,
                    Instrument::Position type,
                    Real strike,
                    Integer settlementDays,
                    const DayCounter& dayCount,
                    const Calendar& calendar,
                    BusinessDayConvention businessDayConvention,
                    const boost::shared_ptr<FixedCouponBond>& fixedCouponBond,
                    const Handle<YieldTermStructure>& discountCurve,
                    const Handle<YieldTermStructure>& incomeDiscountCurve)
    : Forward(dayCount, calendar, businessDayConvention, settlementDays,
              boost::shared_ptr<Payoff>(new ForwardTypePayoff(type,strike)),
              valueDate, maturityDate, discountCurve),
      fixedCouponBond_(fixedCouponBond) {

        incomeDiscountCurve_ = incomeDiscountCurve;
        registerWith(incomeDiscountCurve_);
    }


    Real FixedCouponBondForward::cleanForwardPrice() const {
        return forwardValue() -
               fixedCouponBond_->accruedAmount(maturityDate_);
	}


    Real FixedCouponBondForward::forwardPrice() const {
        return forwardValue();
	}


    Real FixedCouponBondForward::spotIncome(const Handle<YieldTermStructure>&
                                                  incomeDiscountCurve) const {

        Real income = 0.0;
        Date settlement = settlementDate();
        std::vector<boost::shared_ptr<CashFlow> > cf =
            fixedCouponBond_->cashflows();

        /*
          the following assumes
          1. cashflows are in ascending order !
          2. considers as income: all coupons paid between settlementDate()
          and contract delivery/maturity date
        */
        for (Size i = 0; i < cf.size(); ++i) {
            if (!cf[i]->hasOccurred(settlement)) {
                if (cf[i]->hasOccurred(maturityDate_)) {
                    income += cf[i]->amount() *
                              incomeDiscountCurve->discount(cf[i]->date()) ;
                } else {
                    break;
                }
            }
        }

        return income;
    }


    Real FixedCouponBondForward::spotValue() const {
        return fixedCouponBond_->dirtyPrice();
    }


    void FixedCouponBondForward::performCalculations() const {

        underlyingSpotValue_ = spotValue();
        underlyingIncome_    = spotIncome(incomeDiscountCurve_);

        Forward::performCalculations();
    }

}

