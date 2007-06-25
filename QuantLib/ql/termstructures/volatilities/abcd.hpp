/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2006 Cristina Duminuco
 Copyright (C) 2007 Giorgio Facchinetti

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

#ifndef quantlib_abcd_hpp
#define quantlib_abcd_hpp

#include <ql/math/optimization/costfunction.hpp>

namespace QuantLib {
    

    //! %Abcd functional form for instantaneous volatility
    /*! \f[ f(T-t) = [ a + b(T-t) ] e^{-c(T-t)} + d \f] following Rebonato's notation. */
    inline void validateAbcdParameters(Real a, Real b, Real c, Real d) {
        QL_REQUIRE(a+d>=0,
                   "a+d (" << a << ", " << d <<") must be non negative");
        QL_REQUIRE(c>=0,
                   "c (" << c << ") must be non negative");
        QL_REQUIRE(d>=0,
                   "d (" << d << ") must be non negative");
    }



    //! %Abcd
    class Abcd : public std::unary_function<Real, Real> {

      public:
        Abcd(Real a=-0.06, Real b=0.17, Real c=0.54, Real d=0.17);

        //! volatility function value at time u: \f[ f(u) \f]
        Real operator()(Time u) const;

        //! time at which the volatility function reaches maximum (if any)
        Real maximumLocation() const;

        //! maximum value of the volatility function
        Real maximumVolatility() const;

        //! volatility function value at time 0: \f[ f(0) \f]
        Real shortTermVolatility() const { return a_+d_; }

        //! volatility function value at time +inf: \f[ f(\inf) \f]
        Real longTermVolatility() const { return d_; }

        /*! instantaneous covariance function at time t between T-fixing and
            S-fixing rates \f[ f(T-t)f(S-t) \f] */
        Real covariance(Time t, Time T, Time S) const;

        /*! integral of the instantaneous covariance function between
            time t1 and t2 for T-fixing and S-fixing rates
            \f[ \int_{t1}^{t2} f(T-t)f(S-t)dt \f] */

        Real covariance(Time t1, Time t2, Time T, Time S) const;

         /*! volatility in [tMin,tMax] of T-fixing rate:
            \f[ \sqrt{ \int_{tMin}^{tMax} f^2(T-u)du }\f] */
        Real volatility(Time T, Time tMax, Time tMin) const;

        /*! variance in [tMin,tMax] of T-fixing rate:
            \f[ \int_{tMin}^{tMax} f^2(T-u)du \f] */
        Real variance(Time T, Time tMax, Time tMin) const;
        

        
        // INSTANTANEOUS
        /*! instantaneous volatility at time t of the T-fixing rate:
            \f[ f(T-t) \f] */
        Real instantaneousVolatility(Time t, Time T) const;

        /*! instantaneous variance at time t of T-fixing rate:
            \f[ f(T-t)f(T-t) \f] */
        Real instantaneousVariance(Time t, Time T) const;

        /*! instantaneous covariance at time t between T and S fixing rates:
            \f[ f(T-u)f(S-u) \f] */
        Real instantaneousCovariance(Time u, Time T, Time S) const;

        // PRIMITIVE
        /*! indefinite integral of the instantaneous covariance function at
            time t between T-fixing and S-fixing rates
            \f[ \int f(T-t)f(S-t)dt \f] */
        Real primitive(Time t, Time T, Time S) const;
        
        /*! Inspectors*/
        Real a() const { return a_; }
        Real b() const { return b_; }
        Real c() const { return c_; }
        Real d() const { return d_; }

      private:
        Real a_, b_, c_, d_;
    };

    
    // Helper class used by unit tests
    class AbcdSquared : public std::unary_function<Real,Real> {
      
      public:
        AbcdSquared(Real a, Real b, Real c, Real d, Time T, Time S);
        Real operator()(Time t) const;
      
      private:
        boost::shared_ptr<Abcd> abcd_;
        Time T_, S_;
    };

    inline Real abcdBlackVolatility(Time u, Real a, Real b, Real c, Real d) {
        Abcd model(a,b,c,d);
        return model.volatility(0.,u,u);
    }
}

#endif
