/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Nicolas Di C�sar�

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

/*! \file method.hpp
    \brief Abstract optimization method class
*/

#ifndef quantlib_optimization_method_h
#define quantlib_optimization_method_h

#include <ql/Utilities/null.hpp>
#include <ql/Optimization/constraint.hpp>
#include <ql/Optimization/costfunction.hpp>
#include <ql/Optimization/criteria.hpp>

namespace QuantLib {

    class Problem;

    //! Abstract class for constrained optimization method
    class OptimizationMethod {
      public:
        OptimizationMethod()
        : iterationNumber_(0), functionEvaluation_(0), gradientEvaluation_(0),
          functionValue_(Null<Real>()), squaredNorm_(Null<Real>()),
          //endCriteria_(none),
          initialValue_(Array()) {}
        OptimizationMethod(const EndCriteria& endCriteria,
                           const Array& initialValue)
        : iterationNumber_(0), functionEvaluation_(0), gradientEvaluation_(0),
          functionValue_(Null<Real>()), squaredNorm_(Null<Real>()) {
              setEndCriteria(endCriteria);
              setInitialValue(initialValue);
          }
        virtual ~OptimizationMethod() {}

        //! Set initial value
        void setInitialValue(const Array& initialValue);

        //! Set optimization end criteria
        void setEndCriteria(const EndCriteria& endCriteria);

        //! current iteration number
        Integer& iterationNumber() const { return iterationNumber_; }

        //! optimization end criteria
        EndCriteria& endCriteria() const { return endCriteria_; }

        //! number of evaluation of cost function
        Integer& functionEvaluation() const { return functionEvaluation_; }

        //! number of evaluation of cost function gradient
        Integer& gradientEvaluation() const { return gradientEvaluation_; }

        //! value of cost function
        Real& functionValue() const { return functionValue_; }

        //! value of cost function gradient norm
        Real& gradientNormValue() const { return squaredNorm_; }

        //! current value of the local minimum
        Array& x() const { return x_; }

        //! current value of the search direction
        Array& searchDirection() const { return searchDirection_; }

        //! minimize the optimization problem P
        virtual void minimize(const Problem& P) const = 0;
      protected:
        //! current iteration step in the Optimization process
        mutable Integer iterationNumber_;
        //! number of evaluation of cost function and its gradient
        mutable Integer functionEvaluation_, gradientEvaluation_;
        //! function and gradient norm values of the last step
        mutable Real functionValue_, squaredNorm_;
        //! optimization end criteria
        mutable EndCriteria endCriteria_;
        //! initial value of unknowns
        Array initialValue_;
        //! current values of the local minimum and the search direction
        mutable Array x_, searchDirection_;
    };

    // inline definitions

    inline void OptimizationMethod::setEndCriteria(
                                        const EndCriteria& endCriteria) {
        endCriteria_ = endCriteria;
    }

    inline void OptimizationMethod::setInitialValue(
                                        const Array& initialValue) {
        iterationNumber_ = 0;
        initialValue_ = initialValue;
        x_ = initialValue;
        searchDirection_ = Array(x_.size ());
    }

}


#endif
