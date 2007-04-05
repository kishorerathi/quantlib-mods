/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Giorgio Facchinetti
 Copyright (C) 2006, 2007 Cristina Duminuco
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004 StatPro Italia srl
 Copyright (C) 2003 Nicolas Di C�sar�

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

#include <ql/cashflows/floatingratecoupon.hpp>
#include <ql/indexes/interestrateindex.hpp>
#include <ql/cashflows/couponpricer.hpp>

namespace QuantLib {

//===========================================================================//
//                            FloatingRateCoupon                             //
//===========================================================================//

    FloatingRateCoupon::FloatingRateCoupon(
                         const Date& paymentDate, const Real nominal,
                         const Date& startDate, const Date& endDate,
                         const Natural fixingDays,
                         const boost::shared_ptr<InterestRateIndex>& index,
                         const Real gearing, const Spread spread,
                         const Date& refPeriodStart, const Date& refPeriodEnd,
                         const DayCounter& dayCounter,
                         bool isInArrears)
    : Coupon(nominal, paymentDate,
             startDate, endDate, refPeriodStart, refPeriodEnd),
      index_(index), dayCounter_(dayCounter),
      fixingDays_(fixingDays==Null<Size>() ? index->fixingDays() : fixingDays),
      gearing_(gearing), spread_(spread),
      isInArrears_(isInArrears)
    {
        QL_REQUIRE(gearing_!=0, "Null gearing: degenerate Floating Rate Coupon not admitted");

        if (dayCounter_.empty())
            dayCounter_ = index_->dayCounter();

        registerWith(index_);
        registerWith(Settings::instance().evaluationDate());
    }


    void FloatingRateCoupon::setPricer(
                const boost::shared_ptr<FloatingRateCouponPricer>& pricer) {
            if(pricer_)
                unregisterWith(pricer_);
            pricer_ = pricer;
            QL_REQUIRE(pricer_, "no adequate pricer given");
            registerWith(pricer_);
            update();
    }

    Real FloatingRateCoupon::amount() const {
        return rate() * accrualPeriod() * nominal();
    }
    Real FloatingRateCoupon::accruedAmount(const Date& d) const {
        if (d <= accrualStartDate_ || d > paymentDate_) {
            return 0.0;
        } else {
            return nominal() * rate() *
                dayCounter().yearFraction(accrualStartDate_,
                                          std::min(d,accrualEndDate_),
                                          refPeriodStart_,
                                          refPeriodEnd_);
        }
    }


    Real FloatingRateCoupon::price(const Handle<YieldTermStructure>& discountingCurve) const {
        return amount()*discountingCurve->discount(date());
    }

    DayCounter FloatingRateCoupon::dayCounter() const {
        return dayCounter_;
    }

    const boost::shared_ptr<InterestRateIndex>& FloatingRateCoupon::index() const {
        return index_;
    }

    Natural FloatingRateCoupon::fixingDays() const {
        return fixingDays_;
    }

    Date FloatingRateCoupon::fixingDate() const {
        // if isInArrears_ fix at the end of period
        Date refDate = isInArrears_ ? accrualEndDate_ : accrualStartDate_;
        return index_->calendar().advance(refDate,
                                          -static_cast<Integer>(fixingDays_),
                                          Days, Preceding);
    }

    Real FloatingRateCoupon::gearing() const {
        return gearing_;
    }

    Spread FloatingRateCoupon::spread() const {
        return spread_;
    }


    Rate FloatingRateCoupon::indexFixing() const {
        return index_->fixing(fixingDate());
    }
    Rate FloatingRateCoupon::rate() const {
        QL_REQUIRE(pricer_, "pricer not set");
        pricer_->initialize(*this);
        return pricer_->swapletRate();
    }

    Rate FloatingRateCoupon::adjustedFixing() const{
        return (rate()-spread())/gearing();
    }
    Rate FloatingRateCoupon::convexityAdjustmentImpl(Rate f) const {
       return (gearing() == 0.0 ? 0.0 : adjustedFixing()-f);
    }
    Rate FloatingRateCoupon::convexityAdjustment() const {
        return convexityAdjustmentImpl(indexFixing());
    }

    void FloatingRateCoupon::update() {
        notifyObservers();
    }

    void FloatingRateCoupon::accept(AcyclicVisitor& v) {
        Visitor<FloatingRateCoupon>* v1 =
            dynamic_cast<Visitor<FloatingRateCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Coupon::accept(v);
    }


}
