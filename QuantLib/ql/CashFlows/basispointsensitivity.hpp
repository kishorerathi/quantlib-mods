/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000-2005 StatPro Italia srl
 Copyright (C) 2003 Decillion Pty(Ltd)

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

/*! \file basispointsensitivity.hpp
    \brief basis point sensitivity calculator
*/

#ifndef quantlib_bps_calculator_hpp
#define quantlib_bps_calculator_hpp

#include <ql/yieldtermstructure.hpp>
#include <ql/CashFlows/fixedratecoupon.hpp>
#include <ql/CashFlows/timebasket.hpp>

namespace QuantLib {

    #ifndef QL_DISABLE_DEPRECATED
    //! Collective basis-point sensitivity of a cash-flow sequence
    /*! \deprecated use Cashflows::bps instead */
    Real BasisPointSensitivity(
                             const std::vector<boost::shared_ptr<CashFlow> >&,
                             const Handle<YieldTermStructure>&);
    #endif


    #ifndef QL_DISABLE_DEPRECATED
    /*! \bug this class must still be checked. It is not guaranteed
             to yield the right results.
    */
    class BPSBasketCalculator : public AcyclicVisitor,
                                public Visitor<CashFlow>,
                                public Visitor<Coupon>,
                                public Visitor<FixedRateCoupon> {
      public:
        BPSBasketCalculator(const Handle<YieldTermStructure>& ts,
                            Integer basis)
        : termStructure_(ts), basis_(basis) {}
        //! \name Visitor interface
        //@{
        virtual void visit(Coupon&);
        virtual void visit(FixedRateCoupon&);
        virtual void visit(CashFlow&);
        //@}
        const TimeBasket& result() const { return result_; }
      private:
        Real sensfactor(const Date&, const DayCounter&) const;
        Handle<YieldTermStructure> termStructure_;
        Integer basis_;
        TimeBasket result_;
    };

    /*! \bug This function must still be checked. It is not guaranteed
             to yield the right results.
    */
    TimeBasket BasisPointSensitivityBasket(
                             const std::vector<boost::shared_ptr<CashFlow> >&,
                             const Handle<YieldTermStructure>&,
                             Integer basis);
    #endif

}


#endif
