/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004 StatPro Italia srl

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

#ifndef quantlib_hpp
#define quantlib_hpp

#include <ql/qldefines.hpp>

/* Use BOOST_MSVC instead of _MSC_VER since some other vendors
   (Metrowerks, for example) also #define _MSC_VER
*/
#ifdef BOOST_MSVC
#  include <ql/auto_link.hpp>
#endif

#include <ql/core.hpp>

#include <ql/Calendars/all.hpp>
#include <ql/CashFlows/all.hpp>
#include <ql/Currencies/all.hpp>
#include <ql/DayCounters/all.hpp>
#include <ql/FiniteDifferences/all.hpp>
#include <ql/Indexes/all.hpp>
#include <ql/Instruments/all.hpp>
#include <ql/Lattices/all.hpp>
#include <ql/Math/all.hpp>
#include <ql/MonteCarlo/all.hpp>
#include <ql/Optimization/all.hpp>
#include <ql/Patterns/all.hpp>
#include <ql/Pricers/all.hpp>
#include <ql/PricingEngines/all.hpp>
#include <ql/Processes/all.hpp>
#include <ql/RandomNumbers/all.hpp>
#include <ql/ShortRateModels/all.hpp>
#include <ql/Solvers1D/all.hpp>
#include <ql/TermStructures/all.hpp>
#include <ql/Utilities/all.hpp>
#include <ql/Volatilities/all.hpp>
#include <ql/VolatilityModels/all.hpp>


#endif
