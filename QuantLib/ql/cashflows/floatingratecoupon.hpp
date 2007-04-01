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

/*! \file floatingratecoupon.hpp
    \brief Coupon paying a variable index-based rate
*/

#ifndef quantlib_floating_rate_coupon_hpp
#define quantlib_floating_rate_coupon_hpp

#include <ql/cashflows/coupon.hpp>
#include <ql/cashflows/couponpricer.hpp>
#include <ql/utilities/null.hpp>
#include <ql/indexes/interestrateindex.hpp>
#include <ql/capvolstructures.hpp>

namespace QuantLib {
    
    class FloatingRateCoupon : public Coupon,
                               public Observer {
      public:
        FloatingRateCoupon(const Date& paymentDate,
                           const Real nominal,
                           const Date& startDate,
                           const Date& endDate,
                           const Natural fixingDays,
                           const boost::shared_ptr<InterestRateIndex>& index,
                           const Real gearing = 1.0,
                           const Spread spread = 0.0,
                           const Date& refPeriodStart = Date(),
                           const Date& refPeriodEnd = Date(),
                           const DayCounter& dayCounter = DayCounter(),
                           bool isInArrears = false);
        virtual ~FloatingRateCoupon() {}
        //! \name CashFlow interface
        //@{
        Real amount() const;
        //@}
        //! \name Coupon interface
        //@{
        Rate rate() const;
        Real price(const Handle<YieldTermStructure>& discountingCurve) const;
        DayCounter dayCounter() const;
        Real accruedAmount(const Date&) const; //Attenzione
        //@}
        //! \name Inspectors
        //@{
        //! floating index
        const boost::shared_ptr<InterestRateIndex>& index() const;
        //! fixing days
        Natural fixingDays() const;
        //! fixing date
        virtual Date fixingDate() const;
        //! index gearing, i.e. multiplicative coefficient for the index
        Real gearing() const;
        //! spread paid over the fixing of the underlying index
        Spread spread() const;
        //! fixing of the underlying index
        Rate indexFixing() const;
        //! convexity adjustment
        Rate convexityAdjustment() const;
        //! convexity-adjusted fixing
        Rate adjustedFixing() const;

        bool isInArrears() const {return isInArrears_;};
        //@}
        //! \name Observer interface
        //@{
        void update();
        //@}
        //! \name Visitability
        //@{
        virtual void accept(AcyclicVisitor&);
        //@}
        //@}
        void setPricer(const boost::shared_ptr<FloatingRateCouponPricer>& pricer){
            if(pricer_)
                unregisterWith(pricer_);
            pricer_ = pricer;
            QL_REQUIRE(pricer_, "no adequate pricer given");
            registerWith(pricer_);
            update();
		}
        boost::shared_ptr<FloatingRateCouponPricer> pricer() const{
			return pricer_;
	    }
      protected:
        //! convexity adjustment for the given index fixing
        Rate convexityAdjustmentImpl(Rate fixing) const;
        boost::shared_ptr<InterestRateIndex> index_;
        DayCounter dayCounter_;
        Natural fixingDays_;
        Real gearing_;
        Spread spread_;
        bool isInArrears_; 
        boost::shared_ptr<FloatingRateCouponPricer> pricer_;
    };


  

}

#endif
