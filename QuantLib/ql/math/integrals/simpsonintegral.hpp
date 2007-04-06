/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Roman Gitlin
 Copyright (C) 2003 StatPro Italia srl

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

/*! \file simpsonintegral.hpp
    \brief integral of a one-dimensional function
*/

#ifndef quantlib_simpson_integral_hpp
#define quantlib_simpson_integral_hpp

#include <ql/math/integrals/trapezoidintegral.hpp>

namespace QuantLib {

    //! Integral of a one-dimensional function
    /*! \test the correctness of the result is tested by checking it
              against known good values.
    */
    class SimpsonIntegral : public TrapezoidIntegral {
      public:
        SimpsonIntegral(Real accuracy,
                        Size maxIterations = Null<Size>())
        : TrapezoidIntegral(accuracy,Default,maxIterations) {}
        template <class F>
        Real operator()(const F& f, Real a, Real b) const {

            if (a == b)
                return 0.0;
            if (a > b)
                return -(*this)(f,b,a);

            // start from the coarsest trapezoid...
            Size N = 1;
            Real I = (f(a)+f(b))*(b-a)/2.0, newI;
            Real adjI = I, newAdjI;
            // ...and refine it
            Size i = 1;
            do {
                newI = defaultIteration(f,a,b,I,N);
                N *= 2;
                newAdjI = (4.0*newI-I)/3.0;
                // good enough? Also, don't run away immediately
                if (std::fabs(adjI-newAdjI) <= accuracy_ && i > 5)
                    // ok, exit
                    return newAdjI;
                // oh well. Another step.
                I = newI;
                adjI = newAdjI;
                i++;
            } while (i < maxIterations_);
            QL_FAIL("max number of iterations reached");
        }
      private:
        // inhibited
        Method method() const { return method_; }
        Method& method() { return method_; }
    };

}


#endif
