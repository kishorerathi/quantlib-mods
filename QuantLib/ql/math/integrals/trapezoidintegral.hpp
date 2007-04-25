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

/*! \file trapezoidintegral.hpp
    \brief integral of a one-dimensional function
*/

#ifndef quantlib_trapezoid_integral_hpp
#define quantlib_trapezoid_integral_hpp

#include <ql/math/integrals/integral.hpp>
#include <ql/utilities/null.hpp>
#include <ql/errors.hpp>

namespace QuantLib {

    //! Integral of a one-dimensional function
    /*! Given a target accuracy \f$ \epsilon \f$, the integral of
        a function \f$ f \f$ between \f$ a \f$ and \f$ b \f$ is
        calculated by means of the trapezoid formula
        \f[
        \int_{a}^{b} f \mathrm{d}x =
        \frac{1}{2} f(x_{0}) + f(x_{1}) + f(x_{2}) + \dots
        + f(x_{N-1}) + \frac{1}{2} f(x_{N})
        \f]
        where \f$ x_0 = a \f$, \f$ x_N = b \f$, and
        \f$ x_i = a+i \Delta x \f$ with
        \f$ \Delta x = (b-a)/N \f$. The number \f$ N \f$ of intervals
        is repeatedly increased until the target accuracy is reached.

        \test the correctness of the result is tested by checking it
              against known good values.
    */
    class TrapezoidIntegral : public Integrator {
      public:
        enum Method { Default, MidPoint };
        TrapezoidIntegral(Real accuracy,
                          Method method = Default,
                          Size maxIterations = Null<Size>())
        : Integrator(accuracy, maxIterations), method_(method){}
      protected:
        // calculation parameters
        Method method() const { return method_; }
        Method& method() { return method_; }
      
    protected:
        Real integrate (const boost::function<Real (Real)>& f, 
                                            Real a, Real b) const {

            // start from the coarsest trapezoid...
            Size N = 1;
            Real I = (f(a)+f(b))*(b-a)/2.0, newI;
            // ...and refine it
            Size i = 1;
            do {
                switch (method_) {
                  case MidPoint:
                    newI = midPointIteration(f,a,b,I,N);
                    N *= 3;
                    break;
                  default:
                    newI = defaultIteration(f,a,b,I,N);
                    N *= 2;
                    break;
                }
                // good enough? Also, don't run away immediately
                if (std::fabs(I-newI) <= absoluteAccuracy() && i > 5)
                    // ok, exit
                    return newI;
                // oh well. Another step.
                I = newI;
                i++;
            } while (i < maxEvaluations());
            QL_FAIL("max number of iterations reached");
        }
        Method method_;

        Real defaultIteration(const boost::function<Real (Real)>& f, 
                    Real a, Real b, Real I, Size N) const {
            Real sum = 0.0;
            Real dx = (b-a)/N;
            Real x = a + dx/2.0;
            for (Size i=0; i<N; x += dx, ++i)
                sum += f(x);
            return (I + dx*sum)/2.0;
        }

        Real midPointIteration(const boost::function<Real (Real)>& f, Real a,
                                              Real b, Real I, Size N) const {
            Real sum = 0.0;
            Real dx = (b-a)/N;
            Real x = a + dx/6.0;
            Real D = 2.0*dx/3.0;
            for (Size i=0; i<N; x += dx, ++i)
                sum += f(x) + f(x+D);
            return (I + dx*sum)/3.0;
        }
    };

}


#endif
