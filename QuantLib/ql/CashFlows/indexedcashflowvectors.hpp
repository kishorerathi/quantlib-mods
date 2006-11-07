/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Nicolas Di C�sar�
 Copyright (C) 2004, 2005 StatPro Italia srl

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

/*! \file indexedcashflowvectors.hpp
    \brief Indexed cash-flow vector builders
*/

#ifndef quantlib_indexed_cash_flow_vectors_hpp
#define quantlib_indexed_cash_flow_vectors_hpp

#include <ql/CashFlows/shortindexedcoupon.hpp>
#include <ql/schedule.hpp>

namespace QuantLib {

    //! helper function building a leg of floating coupons
    /*! Either ParCoupon, UpFrontIndexedCoupon, InArrearIndexedCoupon,
        or any other coupon can be used whose constructor takes the
        same arguments.

        \warning The last argument is used due to a known VC6 bug
                 regarding function template instantiation. It must be
                 passed explicitly when using the function with that
                 compiler; the simplest choice for the value to be
                 passed is <tt>(const Type*) 0</tt> where
                 <tt>Type</tt> is the desired coupon type.
    */
    template <class IndexedCouponType, class IndexType>
    std::vector<boost::shared_ptr<CashFlow> >
    IndexedCouponVector(const Schedule& schedule,
                        const BusinessDayConvention paymentAdjustment,
                        const std::vector<Real>& nominals,
                        const Integer fixingDays,
                        const boost::shared_ptr<IndexType>& index,
                        const std::vector<Real>& gearings,
                        const std::vector<Spread>& spreads,
                        const DayCounter& dayCounter = DayCounter()
                        #ifdef QL_PATCH_MSVC6
                        , const IndexedCouponType* msvc6_bug = 0
                        #endif
                        )
    {
        QL_REQUIRE(!nominals.empty(), "nominals not specified");

        std::vector<boost::shared_ptr<CashFlow> > leg;
        // first period might be short or long
        Date start = schedule.date(0), end = schedule.date(1);
        Calendar calendar = schedule.calendar();
        Date paymentDate = calendar.adjust(end,paymentAdjustment);

        Real gearing;
        if (gearings.size() > 0) gearing = gearings[0];
        else                     gearing = 1.0;

        Spread spread;
        if (spreads.size() > 0) spread = spreads[0];
        else                    spread = 0.0;

        Real nominal = nominals[0];
        if (schedule.isRegular(1)) {
            leg.push_back(boost::shared_ptr<CashFlow>(
                new IndexedCouponType(paymentDate, nominal,
                                      start, end,
                                      fixingDays, index,
                                      gearing, spread,
                                      start, end, dayCounter)));
        } else {
            Date reference = end - schedule.tenor();
            reference = calendar.adjust(reference,
                                        schedule.businessDayConvention());
            typedef Short<IndexedCouponType> ShortIndexedCouponType;
            leg.push_back(boost::shared_ptr<CashFlow>(
                new ShortIndexedCouponType(paymentDate, nominal,
                                           start, end,
                                           fixingDays, index,
                                           gearing, spread,
                                           reference, end, dayCounter)));
        }
        // regular periods
        for (Size i=2; i<schedule.size()-1; i++) {
            start = end; end = schedule.date(i);
            paymentDate = calendar.adjust(end,paymentAdjustment);

            if ((i-1) < gearings.size())  gearing = gearings[i-1];
            else if (gearings.size() > 0) gearing = gearings.back();
            else                          gearing = 1.0;

            if ((i-1) < spreads.size())  spread = spreads[i-1];
            else if (spreads.size() > 0) spread = spreads.back();
            else                         spread = 0.0;

            if ((i-1) < nominals.size()) nominal = nominals[i-1];
            else                         nominal = nominals.back();

            leg.push_back(boost::shared_ptr<CashFlow>(
                new IndexedCouponType(paymentDate, nominal,
                                      start, end,
                                      fixingDays, index,
                                      gearing, spread,
                                      start, end, dayCounter)));
        }
        if (schedule.size() > 2) {
            // last period might be short or long
            Size N = schedule.size();
            start = end; end = schedule.date(N-1);
            paymentDate = calendar.adjust(end,paymentAdjustment);

            if ((N-2) < gearings.size())  gearing = gearings[N-2];
            else if (gearings.size() > 0) gearing = gearings.back();
            else                          gearing = 1.0;

            if ((N-2) < spreads.size())  spread = spreads[N-2];
            else if (spreads.size() > 0) spread = spreads.back();
            else                         spread = 0.0;

            if ((N-2) < nominals.size()) nominal = nominals[N-2];
            else                         nominal = nominals.back();

            if (schedule.isRegular(N-1)) {
                leg.push_back(boost::shared_ptr<CashFlow>(
                    new IndexedCouponType(paymentDate, nominal,
                                          start, end,
                                          fixingDays, index,
                                          gearing, spread,
                                          start, end, dayCounter)));
            } else {
                Date reference = start + schedule.tenor();
                reference = calendar.adjust(reference,
                                            schedule.businessDayConvention());
                typedef Short<IndexedCouponType> ShortIndexedCouponType;
                leg.push_back(boost::shared_ptr<CashFlow>(
                    new ShortIndexedCouponType(paymentDate, nominal,
                                               start, end,
                                               fixingDays, index,
                                               gearing, spread,
                                               start, reference, dayCounter)));
            }
        }
        return leg;
    }

}

#endif
