/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file onefactoroperator.hpp
    \brief general differential operator for one-factor interest rate models
*/

#ifndef quantlib_finite_differences_one_factor_operator_hpp
#define quantlib_finite_differences_one_factor_operator_hpp

#include <ql/qldefines.hpp>

#if !defined(QL_PATCH_BORLAND)

#include <ql/FiniteDifferences/tridiagonaloperator.hpp>
#include <ql/ShortRateModels/onefactormodel.hpp>

namespace QuantLib {

    //! Interest-rate single factor model differential operator
    /*! \ingroup findiff */
    class OneFactorOperator : public TridiagonalOperator {
      public:
        OneFactorOperator() {}
        OneFactorOperator(
            const Array& grid,
            const boost::shared_ptr<OneFactorModel::ShortRateDynamics>&);
        virtual ~OneFactorOperator() {}

        class SpecificTimeSetter;
    };

    class OneFactorOperator::SpecificTimeSetter
        : public TridiagonalOperator::TimeSetter{
      public:
        SpecificTimeSetter(
            Real x0, Real dx,
            const boost::shared_ptr<OneFactorModel::ShortRateDynamics>&);
        virtual ~SpecificTimeSetter() {}
        virtual void setTime(Time t, TridiagonalOperator& L) const;
      private:
        Real x0_, dx_;
        boost::shared_ptr<OneFactorModel::ShortRateDynamics> dynamics_;
    };

}

#endif

#endif
