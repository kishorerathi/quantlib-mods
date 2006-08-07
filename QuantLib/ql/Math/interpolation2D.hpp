/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003, 2006 Ferdinando Ametrano
 Copyright (C) 2004, 2005, 2006 StatPro Italia srl

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

/*! \file interpolation2D.hpp
    \brief abstract base classes for 2-D interpolations
*/

#ifndef quantlib_interpolation2D_hpp
#define quantlib_interpolation2D_hpp

#include <ql/Patterns/bridge.hpp>
#include <ql/Math/extrapolation.hpp>
#include <ql/Math/comparison.hpp>
#include <ql/Math/matrix.hpp>
#include <ql/errors.hpp>
#include <ql/types.hpp>
#include <vector>

namespace QuantLib {

    //! abstract base class for 2-D interpolation implementations
    class Interpolation2DImpl {
      public:
        virtual ~Interpolation2DImpl() {}
        virtual void calculate() = 0;
        virtual Real xMin() const = 0;
        virtual Real xMax() const = 0;
        virtual std::vector<Real> xValues() const = 0;
        virtual Size locateX(Real x) const = 0;
        virtual Real yMin() const = 0;
        virtual Real yMax() const = 0;
        virtual std::vector<Real> yValues() const = 0;
        virtual Size locateY(Real y) const = 0;
        virtual const Matrix& zData() const = 0;
        virtual bool isInRange(Real x, Real y) const = 0;
        virtual Real value(Real x, Real y) const = 0;
    };

    //! base class for 2-D interpolations.
    /*! Classes derived from this class will provide interpolated
        values from two sequences of length \f$ N \f$ and \f$ M \f$,
        representing the discretized values of the \f$ x \f$ and \f$ y
        \f$ variables, and a \f$ N \times M \f$ matrix representing
        the tabulated function values.
    */
    class Interpolation2D
        : public Bridge<Interpolation2D,Interpolation2DImpl>,
          public Extrapolator {
      public:
        typedef Real first_argument_type;
        typedef Real second_argument_type;
        typedef Real result_type;
        //! basic template implementation
        template <class I1, class I2, class M>
        class templateImpl : public Interpolation2DImpl {
          public:
            templateImpl(const I1& xBegin, const I1& xEnd,
                         const I2& yBegin, const I2& yEnd,
                         const M& zData)
            : xBegin_(xBegin), xEnd_(xEnd), yBegin_(yBegin), yEnd_(yEnd),
              zData_(zData) {
                QL_REQUIRE(xEnd_-xBegin_ >= 2 && yEnd_-yBegin_ >= 2,
                           "not enough points to interpolate");
                #if defined(QL_EXTRA_SAFETY_CHECKS)
                for (I1 i=xBegin_, j=xBegin_+1; j!=xEnd_; i++, j++)
                    QL_REQUIRE(*j > *i, "unsorted x values");
                for (I2 k=yBegin_, l=yBegin_+1; l!=yEnd_; k++, l++)
                    QL_REQUIRE(*l > *k, "unsorted y values");
                #endif
            }
            Real xMin() const {
                return *xBegin_;
            }
            Real xMax() const {
                return *(xEnd_-1);
            }
            std::vector<Real> xValues() const {
                std::vector<Real> result;
                for (I1 i=xBegin_; i!=xEnd_; i++)
                    result.push_back(*i);
                return result;
            }
            Real yMin() const {
                return *yBegin_;
            }
            Real yMax() const {
                return *(yEnd_-1);
            }
            std::vector<Real> yValues() const {
                std::vector<Real> result;
                for (I2 i=yBegin_; i!=yEnd_; i++)
                    result.push_back(*i);
                return result;
            }
            const Matrix& zData() const {
                return zData_;
            }
            bool isInRange(Real x, Real y) const {
                Real x1 = xMin(), x2 = xMax();
                bool xIsInrange = (x >= x1 && x <= x2) ||
                                  close(x,x1) ||
                                  close(x,x2);
                if (!xIsInrange) return false;

                Real y1 = yMin(), y2 = yMax();
                return (y >= y1 && y <= y2) || close(y,y1) || close(y,y2);
            }
          protected:
            Size locateX(Real x) const {
                if (x < *xBegin_)
                    return 0;
                else if (x > *(xEnd_-1))
                    return xEnd_-xBegin_-2;
                else
                    return std::upper_bound(xBegin_,xEnd_-1,x)-xBegin_-1;
            }
            Size locateY(Real y) const {
                if (y < *yBegin_)
                    return 0;
                else if (y > *(yEnd_-1))
                    return yEnd_-yBegin_-2;
                else
                    return std::upper_bound(yBegin_,yEnd_-1,y)-yBegin_-1;
            }
            I1 xBegin_, xEnd_;
            I2 yBegin_, yEnd_;
            const M& zData_;
        };
      public:
        Interpolation2D() {}
        Real operator()(Real x, Real y,
                        bool allowExtrapolation = false) const {
            checkRange(x,y,allowExtrapolation);
            return impl_->value(x,y);
        }
        Real xMin() const {
            return impl_->xMin();
        }
        Real xMax() const {
            return impl_->xMax();
        }
        std::vector<Real> xValues() const {
            return impl_->xValues();
        }
        Size locateX(Real x) const {
            return impl_->locateX(x);
        }
        Real yMin() const {
            return impl_->yMin();
        }
        Real yMax() const {
            return impl_->yMax();
        }
        std::vector<Real> yValues() const {
            return impl_->yValues();
        }
        Size locateY(Real y) const {
            return impl_->locateY(y);
        }
        const Matrix& zData() const {
            return impl_->zData();
        }
        bool isInRange(Real x, Real y) const {
            return impl_->isInRange(x,y);
        }
        void update() {
            impl_->calculate();
        }
      protected:
        void checkRange(Real x, Real y, bool extrapolate) const {
            QL_REQUIRE(extrapolate || allowsExtrapolation() ||
                       impl_->isInRange(x,y),
                       "interpolation range is ["
                       << impl_->xMin() << ", " << impl_->xMax()
                       << "] x ["
                       << impl_->yMin() << ", " << impl_->yMax()
                       << "]: extrapolation at ("
                       << x << ", " << y << ") not allowed");
        }
    };

}


#endif
