/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl

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

/*! \file interpolation.hpp
    \brief base class for 1-D interpolations
*/

#ifndef quantlib_interpolation_hpp
#define quantlib_interpolation_hpp

#include <ql/Patterns/bridge.hpp>
#include <ql/Math/extrapolation.hpp>
#include <ql/Math/comparison.hpp>
#include <ql/errors.hpp>
#include <ql/types.hpp>
#include <vector>

namespace QuantLib {

    //! abstract base class for interpolation implementations
    class InterpolationImpl {
      public:
        virtual ~InterpolationImpl() {}
        virtual void calculate() = 0;
        virtual Real xMin() const = 0;
        virtual Real xMax() const = 0;
        virtual bool isInRange(Real) const = 0;
        virtual Real value(Real) const = 0;
        virtual Real primitive(Real) const = 0;
        virtual Real derivative(Real) const = 0;
        virtual Real secondDerivative(Real) const = 0;
    };

    //! base class for 1-D interpolations.
    /*! Classes derived from this class will provide interpolated
        values from two sequences of equal length, representing
        discretized values of a variable and a function of the former,
        respectively.
    */
    class Interpolation : public Bridge<Interpolation,InterpolationImpl>,
                          public Extrapolator {
      public:
        typedef Real argument_type;
        typedef Real result_type;
        //! basic template implementation
        template <class I1, class I2>
        class templateImpl : public InterpolationImpl {
          public:
            templateImpl(const I1& xBegin, const I1& xEnd, const I2& yBegin)
                : x_(xEnd-xBegin), y_(xEnd-xBegin),
                  xBegin_(x_.begin()), xEnd_(x_.end()),
                  yBegin_(y_.begin()) {
                QL_REQUIRE(xEnd_-xBegin_ >= 2,
                           "not enough points to interpolate");
                #if defined(QL_EXTRA_SAFETY_CHECKS)
                for (I1 i=xBegin, j=xBegin+1; j!=xEnd; i++, j++)
                    QL_REQUIRE(*j > *i, "unsorted x values");
                #endif
                I1 xx = xBegin;
                I2 yy = yBegin;
                for (Size i=0; xx!=xEnd; ++i, ++xx, ++yy) {
                    x_[i]=*xx;
                    y_[i]=*yy;
                }
            }
            Real xMin() const {
                return *xBegin_;
            }
            Real xMax() const {
                return *(xEnd_-1);
            }
            bool isInRange(Real x) const {
                Real x1 = xMin(), x2 = xMax();
                return (x >= x1 && x <= x2) || close(x,x1) || close(x,x2);
            }
          protected:
            Size locate(Real x) const {
                if (x < *xBegin_)
                    return 0;
                else if (x > *(xEnd_-1))
                    return xEnd_-xBegin_-2;
                else
                    return std::upper_bound(xBegin_,xEnd_-1,x)-xBegin_-1;
            }
            std::vector<Real> x_, y_;
            std::vector<Real>::const_iterator xBegin_, xEnd_, yBegin_;
        };
      public:
        Interpolation() {}
        virtual ~Interpolation() {}
        Real operator()(Real x, bool allowExtrapolation = false) const {
            checkRange(x,allowExtrapolation);
            return impl_->value(x);
        }
        Real primitive(Real x, bool allowExtrapolation = false) const {
            checkRange(x,allowExtrapolation);
            return impl_->primitive(x);
        }
        Real derivative(Real x, bool allowExtrapolation = false) const {
            checkRange(x,allowExtrapolation);
            return impl_->derivative(x);
        }
        Real secondDerivative(Real x, bool allowExtrapolation = false) const {
            checkRange(x,allowExtrapolation);
            return impl_->secondDerivative(x);
        }
        Real xMin() const {
            return impl_->xMin();
        }
        Real xMax() const {
            return impl_->xMax();
        }
        bool isInRange(Real x) const {
            return impl_->isInRange(x);
        }
        void update() {
            impl_->calculate();
        }
      protected:
        void checkRange(Real x, bool extrapolate) const {
            QL_REQUIRE(extrapolate || allowsExtrapolation() ||
                       impl_->isInRange(x),
                       "interpolation range is ["
                       << impl_->xMin() << ", " << impl_->xMax()
                       << "]: extrapolation at " << x << " not allowed");
        }
    };

}


#endif
