/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 Joseph Wang

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

/*! \file fddividendengine.hpp
    \brief base engine for option with dividends
*/

#ifndef quantlib_fd_dividend_engine_hpp
#define quantlib_fd_dividend_engine_hpp

#include <ql/PricingEngines/Vanilla/fdmultiperiodengine.hpp>

namespace QuantLib {

    //! Base finite-differences pricing engine for dividend options
    /*! \ingroup vanillaengines */
    class FDDividendEngine : public FDMultiPeriodEngine {
      public:
        FDDividendEngine(const OneAssetOption::arguments* option_args,
                         const DividendSchedule* schedule,
                         Size timeSteps = 100,
                         Size gridPoints = 100,
                         bool timeDependent = false)
        : FDMultiPeriodEngine(option_args,
                              schedule,
                              timeSteps, gridPoints,
                              timeDependent) {}
      private:
        void executeIntermediateStep(Size step) const;
        void movePricesBeforeExDiv(Array& prices,
                                   const Array& newGrid,
                                   const Array& oldGrid) const;
        Real addElements(const std::vector<Real>& A) const{
            return std::accumulate(A.begin(), A.end(), 0.0);
        }
        Real getDividend(int i) const {
            return schedule_->dividends[i];
        }
    };

}


#endif
