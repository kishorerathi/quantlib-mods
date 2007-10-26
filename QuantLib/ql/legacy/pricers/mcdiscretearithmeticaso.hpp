/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

/*! \file mcdiscretearithmeticaso.hpp
    \brief Discrete arithmetic average-strike Asian option
*/

#ifndef quantlib_pricers_mc_discrete_arithmetic_average_strike_mcpricer_h
#define quantlib_pricers_mc_discrete_arithmetic_average_strike_mcpricer_h

#include <ql/option.hpp>
#include <ql/types.hpp>
#include <ql/legacy/pricers/mcpricer.hpp>
#include <ql/yieldtermstructure.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>

namespace QuantLib {

    //! Discrete arithmetic average-strike Asian option
    class McDiscreteArithmeticASO
        : public McPricer<SingleVariate,PseudoRandom>{
      public:
        McDiscreteArithmeticASO(
                            Option::Type type,
                            Real underlying,
                            const Handle<YieldTermStructure>& dividendYield,
                            const Handle<YieldTermStructure>& riskFreeRate,
                            const Handle<BlackVolTermStructure>& volatility,
                            const std::vector<Time>& times,
                            bool controlVariate,
                            BigNatural seed = 0);
    };

}


#endif
