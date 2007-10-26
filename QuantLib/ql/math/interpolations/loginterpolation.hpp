/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano
 Copyright (C) 2004 StatPro Italia srl

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

/*! \file loginterpolation.hpp
    \brief log-linear and log-cubic interpolation between discrete points
*/

#ifndef quantlib_log_interpolation_hpp
#define quantlib_log_interpolation_hpp

#include <ql/math/interpolations/linearinterpolation.hpp>
#include <ql/math/interpolations/cubicspline.hpp>
#include <ql/utilities/dataformatters.hpp>

namespace QuantLib {

    namespace detail {

        template <class I1, class I2>
        class LogLinearInterpolationImpl
            : public Interpolation::templateImpl<I1,I2> {
          public:
            LogLinearInterpolationImpl(const I1& xBegin, const I1& xEnd,
                                       const I2& yBegin)
            : Interpolation::templateImpl<I1,I2>(xBegin, xEnd, yBegin),
              logY_(xEnd-xBegin) {}
            void update() {
                for (Size i=0; i<logY_.size(); ++i) {
                    QL_REQUIRE(this->yBegin_[i]>0.0,
                               "negative or null value (" << this->yBegin_[i]
                               << ") at " << io::ordinal(i) << " position");
                    logY_[i] = std::log(this->yBegin_[i]);
                }
                interpolation_ = LinearInterpolation(this->xBegin_,
                                                     this->xEnd_,
                                                     logY_.begin());
                interpolation_.update();
            }
            Real value(Real x) const {
                return std::exp(interpolation_(x, true));
            }
            Real primitive(Real) const {
                QL_FAIL("LogLinear primitive not implemented");
            }
            Real derivative(Real) const {
                QL_FAIL("LogLinear derivative not implemented");
            }
            Real secondDerivative(Real) const {
                QL_FAIL("LogLinear secondDerivative not implemented");
            }
          private:
            std::vector<Real> logY_;
            Interpolation interpolation_;
        };

        template <class I1, class I2>
        class LogCubicInterpolationImpl
            : public Interpolation::templateImpl<I1,I2> {
          public:
            LogCubicInterpolationImpl(const I1& xBegin, const I1& xEnd,
                                      const I2& yBegin)
            : Interpolation::templateImpl<I1,I2>(xBegin, xEnd, yBegin),
              logY_(xEnd-xBegin) {}
            void update() {
                for (Size i=0; i<logY_.size(); ++i) {
                    QL_REQUIRE(this->yBegin_[i]>0.0,
                               "negative or null value (" << this->yBegin_[i]
                               << ") at " << io::ordinal(i) << " position");
                    logY_[i] = std::log(this->yBegin_[i]);
                }
                interpolation_ = CubicSpline(
                    this->xBegin_, this->xEnd_,
                    logY_.begin(),
                    CubicSpline::NotAKnot, 0.0,
                    CubicSpline::SecondDerivative, 0.0,
                    true);
                interpolation_.update();
            }
            Real value(Real x) const {
                return std::exp(interpolation_(x, true));
            }
            Real primitive(Real) const {
                QL_FAIL("LogCubic primitive not implemented");
            }
            Real derivative(Real) const {
                QL_FAIL("LogCubic derivative not implemented");
            }
            Real secondDerivative(Real) const {
                QL_FAIL("LogCubic secondDerivative not implemented");
            }
          private:
            std::vector<Real> logY_;
            Interpolation interpolation_;
        };

    }

    //! %log-linear interpolation between discrete points
    /*! \todo - merge with LogCubicInterpolation
              - implement primitive, derivative, and secondDerivative functions.
    */
    class LogLinearInterpolation : public Interpolation {
      public:
        /*! \pre the \f$ x \f$ values must be sorted. */
        template <class I1, class I2>
        LogLinearInterpolation(const I1& xBegin, const I1& xEnd,
                               const I2& yBegin) {
            impl_ = boost::shared_ptr<Interpolation::Impl>(new
                detail::LogLinearInterpolationImpl<I1,I2>(xBegin, xEnd,
                                                          yBegin));
            impl_->update();
        }
    };

    //! %log-cubic interpolation between discrete points
    /*! \todo - merge with LogLinearInterpolation
              - implement primitive, derivative, and secondDerivative functions.
    */
    class LogCubicInterpolation : public Interpolation {
      public:
        /*! \pre the \f$ x \f$ values must be sorted. */
        template <class I1, class I2>
        LogCubicInterpolation(const I1& xBegin, const I1& xEnd,
                              const I2& yBegin) {
            impl_ = boost::shared_ptr<Interpolation::Impl>(new
                detail::LogCubicInterpolationImpl<I1,I2>(xBegin, xEnd,
                                                         yBegin));
            impl_->update();
        }
    };

    //! log-linear interpolation factory and traits
    class LogLinear {
      public:
        template <class I1, class I2>
        Interpolation interpolate(const I1& xBegin, const I1& xEnd,
                                  const I2& yBegin) const {
            return LogLinearInterpolation(xBegin, xEnd, yBegin);
        }
        enum { global = 0 };
    };

    //! log-cubic interpolation factory and traits
    class LogCubic {
      public:
        template <class I1, class I2>
        Interpolation interpolate(const I1& xBegin, const I1& xEnd,
                                  const I2& yBegin) const {
            return LogCubicInterpolation(xBegin, xEnd, yBegin);
        }
        enum { global = 1 };
    };

}


#endif
