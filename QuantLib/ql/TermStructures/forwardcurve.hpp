/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006 StatPro Italia srl

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

/*! \file forwardcurve.hpp
    \brief interpolated forward-rate structure
*/

#ifndef quantlib_forward_curve_hpp
#define quantlib_forward_curve_hpp

#include <ql/TermStructures/forwardstructure.hpp>
#include <ql/Math/backwardflatinterpolation.hpp>
#include <vector>
#include <utility>

namespace QuantLib {

    //! Term structure based on interpolation of forward rates
    /*! \ingroup yieldtermstructures */
    template <class Interpolator>
    class InterpolatedForwardCurve : public ForwardRateStructure {
      public:
        // constructor
        InterpolatedForwardCurve(const std::vector<Date>& dates,
                                 const std::vector<Rate>& forwards,
                                 const DayCounter& dayCounter,
                                 const Interpolator& interpolator
                                                            = Interpolator());
        //! \name Inspectors
        //@{
        Date maxDate() const;
        const std::vector<Time>& times() const;
        const std::vector<Date>& dates() const;
        const std::vector<Rate>& forwards() const;
        std::vector<std::pair<Date,Rate> > nodes() const;
      protected:
        InterpolatedForwardCurve(const DayCounter&,
                                 const Interpolator& interpolator
                                                            = Interpolator());
        InterpolatedForwardCurve(const Date& referenceDate,
                                 const DayCounter&,
                                 const Interpolator& interpolator
                                                            = Interpolator());
        InterpolatedForwardCurve(Natural settlementDays,
                                 const Calendar&,
                                 const DayCounter&,
                                 const Interpolator& interpolator
                                                            = Interpolator());
        Rate forwardImpl(Time t) const;
        Rate zeroYieldImpl(Time t) const;
        mutable std::vector<Date> dates_;
        mutable std::vector<Time> times_;
        mutable std::vector<Rate> data_;
        mutable Interpolation interpolation_;
        Interpolator interpolator_;
    };

    //! Term structure based on flat interpolation of forward rates
    /*! \ingroup yieldtermstructures */
    typedef InterpolatedForwardCurve<BackwardFlat> ForwardCurve;


    // inline definitions

    template <class T>
    inline Date InterpolatedForwardCurve<T>::maxDate() const {
        return dates_.back();
    }

    template <class T>
    inline const std::vector<Time>& InterpolatedForwardCurve<T>::times()
                                                                       const {
        return times_;
    }

    template <class T>
    inline const std::vector<Date>&
    InterpolatedForwardCurve<T>::dates() const {
        return dates_;
    }

    template <class T>
    inline const std::vector<Rate>&
    InterpolatedForwardCurve<T>::forwards() const {
        return data_;
    }

    template <class T>
    inline std::vector<std::pair<Date,Rate> >
    InterpolatedForwardCurve<T>::nodes() const {
        std::vector<std::pair<Date,Rate> > results(dates_.size());
        for (Size i=0; i<dates_.size(); ++i)
            results[i] = std::make_pair(dates_[i],data_[i]);
        return results;
    }

    template <class T>
    inline InterpolatedForwardCurve<T>::InterpolatedForwardCurve(
                                                 const DayCounter& dayCounter,
                                                 const T& interpolator)
    : interpolator_(interpolator) {}

    template <class T>
    inline InterpolatedForwardCurve<T>::InterpolatedForwardCurve(
                                                 const Date& referenceDate,
                                                 const DayCounter& dayCounter,
                                                 const T& interpolator)
    : ForwardRateStructure(referenceDate, Calendar(), dayCounter),
      interpolator_(interpolator) {}

    template <class T>
    inline InterpolatedForwardCurve<T>::InterpolatedForwardCurve(
                                                 Natural settlementDays,
                                                 const Calendar& calendar,
                                                 const DayCounter& dayCounter,
                                                 const T& interpolator)
    : ForwardRateStructure(settlementDays, calendar, dayCounter),
      interpolator_(interpolator) {}

    template <class T>
    Rate InterpolatedForwardCurve<T>::forwardImpl(Time t) const {
        return interpolation_(t, true);
    }

    template <class T>
    Rate InterpolatedForwardCurve<T>::zeroYieldImpl(Time t) const {
        if (t == 0.0)
            return forwardImpl(0.0);
        else
            return interpolation_.primitive(t, true)/t;
    }


    // template definitions

    template <class T>
    InterpolatedForwardCurve<T>::InterpolatedForwardCurve(
                                            const std::vector<Date>& dates,
                                            const std::vector<Rate>& forwards,
                                            const DayCounter& dayCounter,
                                            const T& interpolator)
    : ForwardRateStructure(dates[0], Calendar(), dayCounter),
      dates_(dates), data_(forwards), interpolator_(interpolator) {

        QL_REQUIRE(dates_.size()>1, "too few dates");
        QL_REQUIRE(data_.size()==dates_.size(),
                   "dates/yields count mismatch");

        times_.resize(dates_.size());
        times_[0]=0.0;
        for (Size i = 1; i < dates_.size(); i++) {
            QL_REQUIRE(dates_[i] > dates_[i-1],
                       "invalid date (" << dates_[i] << ", vs "
                       << dates_[i-1] << ")");
            #if !defined(QL_NEGATIVE_RATES)
            QL_REQUIRE(data_[i] >= 0.0, "negative forward");
            #endif
            times_[i] = dayCounter.yearFraction(dates_[0], dates_[i]);
        }

        interpolation_ = interpolator_.interpolate(times_.begin(),
                                                   times_.end(),
                                                   data_.begin());
    }

}


#endif
