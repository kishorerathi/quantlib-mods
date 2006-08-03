/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
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

/*! \file inarrearindexedcoupon.hpp
    \brief in-arrear floating-rate coupon
*/

#ifndef quantlib_in_arrear_indexed_coupon_hpp
#define quantlib_in_arrear_indexed_coupon_hpp

#include <ql/CashFlows/indexedcoupon.hpp>
#include <ql/Indexes/xibor.hpp>
#include <ql/capvolstructures.hpp>

namespace QuantLib {

    //! In-arrear floating-rate coupon
    /*! \warning This class does not perform any date adjustment,
                 i.e., the start and end date passed upon construction
                 should be already rolled to a business day.

        \test The class is tested by comparing the value of an in-arrear
              swap against a known good value.
    */
    class InArrearIndexedCoupon : public FloatingRateCoupon {
      public:
        InArrearIndexedCoupon(const Date& paymentDate,
                              const Real nominal,
                              const Date& startDate,
                              const Date& endDate,
                              const Integer fixingDays,
                              const boost::shared_ptr<Xibor>& index,
                              const Real gearing = 1.0,
                              const Spread spread = 0.0,
                              const Date& refPeriodStart = Date(),
                              const Date& refPeriodEnd = Date(),
                              const DayCounter& dayCounter = DayCounter())
        : FloatingRateCoupon(paymentDate, nominal, startDate, endDate, fixingDays,
                        index, gearing, spread, refPeriodStart, refPeriodEnd,
                        dayCounter) {}
        //! \name FloatingRateCoupon interface
        //@{
        Date fixingDate() const;
        //@}
        //! \name Modifiers
        //@{
        void setCapletVolatility(const Handle<CapletVolatilityStructure>&);
        //@}
        //! \name Visitability
        //@{
        virtual void accept(AcyclicVisitor&);
        //@}
      protected:
        Rate convexityAdjustment(Rate fixing) const;
        Handle<CapletVolatilityStructure> capletVolatility_;
    };


    // inline definitions

    inline Date InArrearIndexedCoupon::fixingDate() const {
        // fix at the end of period
        return index_->calendar().advance(accrualEndDate_,
                                          -fixingDays_, Days,
                                          Preceding);
    }

    inline void InArrearIndexedCoupon::accept(AcyclicVisitor& v) {
        Visitor<InArrearIndexedCoupon>* v1 =
            dynamic_cast<Visitor<InArrearIndexedCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            FloatingRateCoupon::accept(v);
    }

}

#endif
