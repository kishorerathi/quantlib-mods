/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 RiskMap srl
 Copyright (C) 2006 StatPro Italia srl

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

/*! \file parcoupon.hpp
    \brief Coupon at par on a term structure
*/

#ifndef quantlib_par_coupon_hpp
#define quantlib_par_coupon_hpp

#include <ql/CashFlows/floatingratecoupon.hpp>
#include <ql/Indexes/iborindex.hpp>

namespace QuantLib {

    //! %coupon at par on a term structure
    /*! \warning This class does not perform any date adjustment,
                 i.e., the start and end date passed upon construction
                 should be already rolled to a business day.
    */
    class ParCoupon : public FloatingRateCoupon {
      public:
        ParCoupon(const Date& paymentDate,
                  const Real nominal,
                  const Date& startDate,
                  const Date& endDate,
                  const Integer fixingDays,
                  const boost::shared_ptr<IborIndex>& index,
                  const Real gearing = 1.0,
                  const Spread spread = 0.0,
                  const Date& refPeriodStart = Date(),
                  const Date& refPeriodEnd = Date(),
                  const DayCounter& dayCounter = DayCounter());
        //! \name Coupon interface
        //@{
        Rate rate() const;
        //@}
        //! \name Visitability
        //@{
        virtual void accept(AcyclicVisitor&);
        //@}
      private:
        Rate convexityAdjustmentImpl(Rate f) const;
    };


    // inline definitions

    inline void ParCoupon::accept(AcyclicVisitor& v) {
        Visitor<ParCoupon>* v1 = dynamic_cast<Visitor<ParCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            FloatingRateCoupon::accept(v);
    }

    inline Rate ParCoupon::convexityAdjustmentImpl(Rate f) const {
        return (gearing() == 0.0 ? 0.0 : (rate()-spread())/gearing() - f);
    }

}

#endif
