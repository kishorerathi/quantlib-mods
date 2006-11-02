/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Mark Joshi

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


#ifndef quantlib_market_model_evolver_hpp
#define quantlib_market_model_evolver_hpp

#include <ql/MarketModels/marketmodelevolver.hpp>

namespace QuantLib {

    /*! Abstract base class. Requires extra methods above that of
        marketmodelevolver to let you fix rates via importance sampling.

        The evolver does the actual gritty work of evolving the forward rates
        from one time to the next.

        This is intended to be used for the Fries-Joshi proxy simulation
        approach to Greeks
    */
    class MarketModelConstrainedEvolver {
      public:
        virtual ~MarketModelConstrainedEvolver() {}
        //! call once
        virtual void SetConstraintType(
            const std::vector<Size>& startIndexOfSwapRate,
            const std::vector<Size>& EndIndexOfSwapRate) = 0;
        //! call before each path
        virtual void SetThisConstraint(
            const std::vector<Rate>& rateConstraints,
            const std::vector<bool>& isConstraintActive) = 0;
    };

}

#endif
