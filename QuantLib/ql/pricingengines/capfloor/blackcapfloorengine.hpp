/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
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

/*! \file blackcapfloorengine.hpp
    \brief Black-formula cap/floor engine
*/

#ifndef quantlib_pricers_black_capfloor_hpp
#define quantlib_pricers_black_capfloor_hpp

#include <ql/daycounters/actual365fixed.hpp>
#include <ql/instruments/capfloor.hpp>
#include <ql/math/distributions/normaldistribution.hpp>
#include <ql/capvolstructures.hpp>

namespace QuantLib {

    //! Black-formula cap/floor engine
    /*! \ingroup capfloorengines */
    class BlackCapFloorEngine : public CapFloor::engine,
                                public Observer {
      public:
        BlackCapFloorEngine(const Handle<Quote>& volatility,
                            const DayCounter& dc = Actual365Fixed());
        BlackCapFloorEngine(const Handle<CapletVolatilityStructure>&);
        void calculate() const;
        void update();
      private:
        Handle<CapletVolatilityStructure> volatility_;
        CumulativeNormalDistribution N_;
    };

}

#endif
