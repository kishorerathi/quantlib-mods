/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Chris Kenyon
 Copyright (C) 2007, 2008 StatPro Italia srl

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

/*! \file piecewisezeroinflationcurve.hpp
    \brief Piecewise zero-inflation term structure
*/

#ifndef quantlib_piecewise_zero_inflation_curve_hpp
#define quantlib_piecewise_zero_inflation_curve_hpp

#include <ql/termstructures/inflation/interpolatedzeroinflationcurve.hpp>
#include <ql/termstructures/iterativebootstrap.hpp>
#include <ql/patterns/lazyobject.hpp>

namespace QuantLib {

    //! Bootstrap traits to use for PiecewiseZeroInflationCurve
    class ZeroInflationTraits {
      public:
        typedef BootstrapHelper<ZeroInflationTermStructure> helper;

        static Size maxIterations() { return 5; } // calibration is trivial,
                                                  // should be immediate
        static Date initialDate(const ZeroInflationTermStructure* t) {
            return t->referenceDate() - t->lag();
        }
        static bool dummyInitialValue() { return false; }
        static Rate initialValue(const ZeroInflationTermStructure* t) {
            return t->baseRate();
        }
        static Rate initialGuess() { return 0.02; }
        // further guesses
        static Rate guess(const ZeroInflationTermStructure*, const Date&) {
            return 0.02;    // initial guess at flat inflation
        }
        // possible constraints based on previous values
        static Real minValueAfter(Size, const std::vector<Rate>&) {
            return -0.1 + QL_EPSILON;
        }
        static Real maxValueAfter(Size, const std::vector<Rate>&) {
            return 0.3 - QL_EPSILON;
        }
        // update with new guess
        static void updateGuess(std::vector<Rate>& data,
                                Rate level,
                                Size i) {
            data[i] = level;
        }
    };


    //! Piecewise zero-inflation term structure
    template <class Interpolator,
              template <class> class Bootstrap = IterativeBootstrap,
              class Traits = ZeroInflationTraits>
    class PiecewiseZeroInflationCurve
        : public InterpolatedZeroInflationCurve<Interpolator>,
          public LazyObject {
      private:
        typedef InterpolatedZeroInflationCurve<Interpolator> base_curve;
        typedef PiecewiseZeroInflationCurve<Interpolator,Bootstrap,Traits>
                                                                   this_curve;
      public:
        typedef Traits traits_type;
        typedef Interpolator interpolator_type;
        //! \name Constructors
        //@{
        PiecewiseZeroInflationCurve(
               const Date& referenceDate,
               const Calendar& calendar,
               const DayCounter& dayCounter,
               const Period& lag,
               Frequency frequency,
               Rate baseZeroRate,
               const Handle<YieldTermStructure>& nominalTS,
               const std::vector<boost::shared_ptr<typename Traits::helper> >&
                                                                  instruments,
               Real accuracy = 1.0e-12,
               const Interpolator& i = Interpolator())
        : base_curve(referenceDate, calendar, dayCounter,
                     lag, frequency, baseZeroRate,
                     nominalTS, i),
          instruments_(instruments), accuracy_(accuracy) {
            bootstrap_.setup(this);
        }
        //@}
        //! \name Inflation interface
        //@{
        Date baseDate() const;
        Date maxDate() const;
        //@
        //! \name Inspectors
        //@{
        const std::vector<Time>& times() const;
        const std::vector<Date>& dates() const;
        const std::vector<Real>& data() const;
        std::vector<std::pair<Date, Real> > nodes() const;
        //@}
        //! \name Observer interface
        //@{
        void update();
        //@}
      private:
        // methods
        void performCalculations() const;
        // data members
        std::vector<boost::shared_ptr<typename Traits::helper> > instruments_;
        Real accuracy_;

        friend class Bootstrap<this_curve>;
        friend class BootstrapError<this_curve>;
        Bootstrap<this_curve> bootstrap_;
    };


    // inline and template definitions

    template <class I, template <class> class B, class T>
    inline Date PiecewiseZeroInflationCurve<I,B,T>::baseDate() const {
        this->calculate();
        return base_curve::baseDate();
    }

    template <class I, template <class> class B, class T>
    inline Date PiecewiseZeroInflationCurve<I,B,T>::maxDate() const {
        this->calculate();
        return base_curve::maxDate();
    }

    template <class I, template <class> class B, class T>
    const std::vector<Time>& PiecewiseZeroInflationCurve<I,B,T>::times() const {
        calculate();
        return base_curve::times();
    }

    template <class I, template <class> class B, class T>
    const std::vector<Date>& PiecewiseZeroInflationCurve<I,B,T>::dates() const {
        calculate();
        return base_curve::dates();
    }

    template <class I, template <class> class B, class T>
    const std::vector<Real>& PiecewiseZeroInflationCurve<I,B,T>::data() const {
        calculate();
        return base_curve::data();
    }

    template <class I, template <class> class B, class T>
    std::vector<std::pair<Date, Real> >
    PiecewiseZeroInflationCurve<I,B,T>::nodes() const {
        calculate();
        return base_curve::nodes();
    }

    template <class I, template <class> class B, class T>
    void PiecewiseZeroInflationCurve<I,B,T>::performCalculations() const {
        bootstrap_.calculate();
    }

    template <class I, template<class> class B, class T>
    void PiecewiseZeroInflationCurve<I,B,T>::update() {
        base_curve::update();
        LazyObject::update();
    }

}

#endif

