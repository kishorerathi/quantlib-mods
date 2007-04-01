/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Ferdinando Ametrano

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

/*! \file quantotermstructure.hpp
    \brief Quanto term structure
*/

#ifndef quantlib_quanto_term_structure_hpp
#define quantlib_quanto_term_structure_hpp

#include <ql/termstructures/zeroyieldstructure.hpp>
#include <ql/voltermstructure.hpp>

namespace QuantLib {

    //! Quanto term structure
    /*! Quanto term structure for modelling quanto effect in
        option pricing.

        \note This term structure will remain linked to the original
              structures, i.e., any changes in the latters will be
              reflected in this structure as well.
    */
    class QuantoTermStructure : public ZeroYieldStructure {
      public:
        QuantoTermStructure(
                    const Handle<YieldTermStructure>& underlyingDividendTS,
                    const Handle<YieldTermStructure>& riskFreeTS,
                    const Handle<YieldTermStructure>& foreignRiskFreeTS,
                    const Handle<BlackVolTermStructure>& underlyingBlackVolTS,
                    Real strike,
                    const Handle<BlackVolTermStructure>& exchRateBlackVolTS,
                    Real exchRateATMlevel,
                    Real underlyingExchRateCorrelation);
        //! \name YieldTermStructure interface
        //@{
        DayCounter dayCounter() const {
            return underlyingDividendTS_->dayCounter();
        }
        Calendar calendar() const;
        const Date& referenceDate() const;
        Date maxDate() const { return maxDate_; }
        //@}
      protected:
        //! returns the zero yield as seen from the evaluation date
        Rate zeroYieldImpl(Time) const;
      private:
        Handle<YieldTermStructure> underlyingDividendTS_, riskFreeTS_,
                                   foreignRiskFreeTS_;
        Handle<BlackVolTermStructure> underlyingBlackVolTS_,
                                      exchRateBlackVolTS_;
        Real underlyingExchRateCorrelation_, strike_, exchRateATMlevel_;
        Date maxDate_;
    };


    // inline definitions

    inline QuantoTermStructure::QuantoTermStructure(
                    const Handle<YieldTermStructure>& underlyingDividendTS,
                    const Handle<YieldTermStructure>& riskFreeTS,
                    const Handle<YieldTermStructure>& foreignRiskFreeTS,
                    const Handle<BlackVolTermStructure>& underlyingBlackVolTS,
                    Real strike,
                    const Handle<BlackVolTermStructure>& exchRateBlackVolTS,
                    Real exchRateATMlevel,
                    Real underlyingExchRateCorrelation)
    : ZeroYieldStructure(underlyingDividendTS->dayCounter()),
      underlyingDividendTS_(underlyingDividendTS),
      riskFreeTS_(riskFreeTS), foreignRiskFreeTS_(foreignRiskFreeTS),
      underlyingBlackVolTS_(underlyingBlackVolTS),
      exchRateBlackVolTS_(exchRateBlackVolTS),
      underlyingExchRateCorrelation_(underlyingExchRateCorrelation),
      strike_(strike), exchRateATMlevel_(exchRateATMlevel) {
        registerWith(underlyingDividendTS_);
        registerWith(riskFreeTS_);
        registerWith(foreignRiskFreeTS_);
        registerWith(underlyingBlackVolTS_);
        registerWith(exchRateBlackVolTS_);

        maxDate_ = std::min(underlyingDividendTS_->maxDate(),
                            riskFreeTS_->maxDate());
        maxDate_ = std::min(maxDate_, foreignRiskFreeTS_->maxDate());
        maxDate_ = std::min(maxDate_, underlyingBlackVolTS_->maxDate());
        maxDate_ = std::min(maxDate_, exchRateBlackVolTS_->maxDate());

    }

    inline Calendar QuantoTermStructure::calendar() const {
        return underlyingDividendTS_->calendar();
    }

    inline const Date& QuantoTermStructure::referenceDate() const {
        return underlyingDividendTS_->referenceDate();
    }

    inline Rate QuantoTermStructure::zeroYieldImpl(Time t) const {
        // warning: here it is assumed that all TS have the same daycount.
        //          It should be QL_REQUIREd
        return underlyingDividendTS_->zeroRate(t, Continuous, NoFrequency, true)
            +            riskFreeTS_->zeroRate(t, Continuous, NoFrequency, true)
            -     foreignRiskFreeTS_->zeroRate(t, Continuous, NoFrequency, true)
            + underlyingExchRateCorrelation_
            * underlyingBlackVolTS_->blackVol(t, strike_, true)
            *   exchRateBlackVolTS_->blackVol(t, exchRateATMlevel_, true);
    }

}


#endif
