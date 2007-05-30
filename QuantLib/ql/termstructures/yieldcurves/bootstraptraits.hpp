/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 StatPro Italia srl

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

/*! \file bootstraptraits.hpp
    \brief bootstrap traits
*/

#ifndef ql_bootstrap_traits_hpp
#define ql_bootstrap_traits_hpp

#include <ql/termstructures/yieldcurves/discountcurve.hpp>
#include <ql/termstructures/yieldcurves/zerocurve.hpp>
#include <ql/termstructures/yieldcurves/forwardcurve.hpp>

namespace QuantLib {

    //! Discount-curve traits
    struct Discount {
        // interpolated curve type
        template <class Interpolator>
        struct curve {
            typedef InterpolatedDiscountCurve<Interpolator> type;
        };
        // value at reference
        static DiscountFactor initialValue() { return 1.0; }
        // initial guess
        static DiscountFactor initialGuess() { return 0.9; }
        // further guesses
        static DiscountFactor guess(const YieldTermStructure* c,
                                    const Date& d) {
            return c->discount(d,true);
        }
        // possible constraints based on previous values
        static DiscountFactor minValueAfter(Size,
                                            const std::vector<Real>&) {
            return QL_EPSILON;
        }
        static DiscountFactor maxValueAfter(Size i,
                                            const std::vector<Real>& data) {
            #if defined(QL_NEGATIVE_RATES)
            // discount are not required to be decreasing--all bets are off.
            // We choose as max a value very unlikely to be exceeded.
            return 3.0;
            #else
            // discounts cannot increase
            return data[i-1];
            #endif
        }
        // update with new guess
        static void updateGuess(std::vector<DiscountFactor>& data,
                                DiscountFactor discount,
                                Size i) {
            data[i] = discount;
        }
    };


    //! Zero-curve traits
    struct ZeroYield {
        // interpolated curve type
        template <class Interpolator>
        struct curve {
            typedef InterpolatedZeroCurve<Interpolator> type;
        };
        // (dummy) value at reference
        static Rate initialValue() { return 0.02; }
        // initial guess
        static Rate initialGuess() { return 0.02; }
        // further guesses
        static Rate guess(const YieldTermStructure* c,
                          const Date& d) {
            return c->zeroRate(d, c->dayCounter(),
                               Continuous, Annual, true);
        }
        // possible constraints based on previous values
        static Rate minValueAfter(Size, const std::vector<Real>&) {
            #if defined(QL_NEGATIVE_RATES)
            // no constraints.
            // We choose as min a value very unlikely to be exceeded.
            return -3.0;
            #else
            return QL_EPSILON;
            #endif
        }
        static Rate maxValueAfter(Size, const std::vector<Real>&) {
            // no constraints.
            // We choose as max a value very unlikely to be exceeded.
            return 3.0;
        }
        // update with new guess
        static void updateGuess(std::vector<Rate>& data,
                                Rate rate,
                                Size i) {
            data[i] = rate;
            if (i == 1)
                data[0] = rate; // first point is updated as well
        }
    };


    //! Forward-curve traits
    struct ForwardRate {
        // interpolated curve type
        template <class Interpolator>
        struct curve {
            typedef InterpolatedForwardCurve<Interpolator> type;
        };
        // (dummy) value at reference
        static Rate initialValue() { return 0.02; }
        // initial guess
        static Rate initialGuess() { return 0.02; }
        // further guesses
        static Rate guess(const YieldTermStructure* c,
                          const Date& d) {
            return c->forwardRate(d, d, c->dayCounter(),
                                  Continuous, Annual, true);
        }
        // possible constraints based on previous values
        static Rate minValueAfter(Size, const std::vector<Real>&) {
            #if defined(QL_NEGATIVE_RATES)
            // no constraints.
            // We choose as min a value very unlikely to be exceeded.
            return -3.0;
            #else
            return QL_EPSILON;
            #endif
        }
        static Rate maxValueAfter(Size, const std::vector<Real>&) {
            // no constraints.
            // We choose as max a value very unlikely to be exceeded.
            return 3.0;
        }
        // update with new guess
        static void updateGuess(std::vector<Rate>& data,
                                Rate forward,
                                Size i) {
            data[i] = forward;
            if (i == 1)
                data[0] = forward; // first point is updated as well
        }
    };

}


#endif
