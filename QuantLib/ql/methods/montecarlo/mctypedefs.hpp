/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

/*! \file mctypedefs.hpp
    \brief Default choices for template instantiations
*/

#ifndef quantlib_mc_typedefs_h
#define quantlib_mc_typedefs_h

#include <ql/methods/montecarlo/montecarlomodel.hpp>

namespace QuantLib {

    //! default choice for one-factor Monte Carlo model.
    typedef MonteCarloModel<SingleVariate,PseudoRandom>
                                                  OneFactorMonteCarloOption;

    //! default choice for multi-factor Monte Carlo model.
    typedef MonteCarloModel<MultiVariate,PseudoRandom>
                                                  MultiFactorMonteCarloOption;

}


#endif
