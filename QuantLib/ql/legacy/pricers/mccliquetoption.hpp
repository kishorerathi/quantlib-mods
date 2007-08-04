/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file mccliquetoption.hpp
    \brief Cliquet option priced with Monte Carlo simulation
*/

#ifndef quantlib_montecarlo_cliquetoption_pricer_hpp
#define quantlib_montecarlo_cliquetoption_pricer_hpp

#include <ql/option.hpp>
#include <ql/types.hpp>
#include <ql/legacy/pricers/mcpricer.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/blackvoltermstructure.hpp>

namespace QuantLib {

    //! simple example of Monte Carlo pricer
    class McCliquetOption
        : public McPricer<SingleVariate,PseudoRandom>{
      public:
        McCliquetOption(Option::Type type,
                        Real underlying,
                        Real moneyness,
                        const Handle<YieldTermStructure>& dividendYield,
                        const Handle<YieldTermStructure>& riskFreeRate,
                        const Handle<BlackVolTermStructure>& volatility,
                        const std::vector<Time>& times,
                        Real accruedCoupon,
                        Real lastFixing,
                        Real localCap,
                        Real localFloor,
                        Real globalCap,
                        Real globalFloor,
                        bool redemptionOnly,
                        BigNatural seed = 0);
    };

}


#endif
