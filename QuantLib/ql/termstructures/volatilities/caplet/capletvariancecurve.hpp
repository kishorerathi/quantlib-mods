/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 Klaus Spanderen

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

/*! \file capletvariancecurve.hpp
    \brief caplet variance curve
*/

#ifndef quantlib_caplet_variance_curve_hpp
#define quantlib_caplet_variance_curve_hpp

#include <ql/termstructures/capvolstructures.hpp>
#include <ql/termstructures/volatilities/equity/blackvariancecurve.hpp>

namespace QuantLib {

    class CapletVarianceCurve : public CapletVolatilityStructure {
      public:
        CapletVarianceCurve(const Date& referenceDate,
                            const std::vector<Date>& dates,
                            const std::vector<Volatility>& capletVolCurve,
                            const DayCounter& dayCounter);
        //! \name TermStructure interface
        //@{
        DayCounter dayCounter() const;
        Date maxDate() const;
        //@}
        Real minStrike() const;
        Real maxStrike() const;
        template <class Interpolator>
        void setInterpolation(const Interpolator& i = Interpolator()) {
            blackCurve_.setInterpolation(i);
            notifyObservers();
        }
      protected:
        Volatility volatilityImpl(Time t, Rate) const;
      private:
        BlackVarianceCurve blackCurve_;
    };

    inline CapletVarianceCurve::CapletVarianceCurve(
                                const Date& referenceDate,
                                const std::vector<Date>& dates,
                                const std::vector<Volatility>& capletVolCurve,
                                const DayCounter& dayCounter)
    : CapletVolatilityStructure(referenceDate),
      blackCurve_(referenceDate, dates, capletVolCurve, dayCounter, false) {}

    inline DayCounter CapletVarianceCurve::dayCounter() const {
        return blackCurve_.dayCounter();
    }

    inline Date CapletVarianceCurve::maxDate() const {
        return blackCurve_.maxDate();
    }

    inline Real CapletVarianceCurve::minStrike() const {
        return blackCurve_.minStrike();
    }

    inline Real CapletVarianceCurve::maxStrike() const {
        return blackCurve_.maxStrike();
    }

    inline
    Volatility CapletVarianceCurve::volatilityImpl(Time t, Rate r) const {
        return blackCurve_.blackVol(t, r, true);
    }

}


#endif
