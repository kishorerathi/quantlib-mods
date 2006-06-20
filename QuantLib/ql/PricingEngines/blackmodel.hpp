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

/*! \file blackmodel.hpp
    \brief Abstract class for Black-type models (market models)
*/

#ifndef quantlib_black_model_hpp
#define quantlib_black_model_hpp

#include <ql/yieldtermstructure.hpp>
#include <ql/quote.hpp>
#include <ql/Math/normaldistribution.hpp>

namespace QuantLib {

    //! Black-model for vanilla interest-rate derivatives
    class BlackModel : public Observable, public Observer {

      #ifndef QL_DISABLE_DEPRECATED
      public:

        //! \deprecated use the termStructure-less constructor
        BlackModel(const Handle<Quote>& volatility,
                   const Handle<YieldTermStructure>& termStructure)
        : volatility_(volatility), termStructure_(termStructure)
        {
            registerWith(volatility_);
            registerWith(termStructure_);
        }

        //! \deprecated don't ask BlackModel for the termStructure
        const Handle<YieldTermStructure>& termStructure() const
        {
            return termStructure_;
        }
      private:
        Handle<YieldTermStructure> termStructure_;
      #endif

      public:
        BlackModel(const Handle<Quote>& volatility)
        : volatility_(volatility)
        {
            registerWith(volatility_);
        }

        void update() { notifyObservers(); }
        //Returns the Black volatility
        Volatility volatility() const { return volatility_->value(); }
        //! General Black formula
        static Real formula(Real f, Real k, Real v, Real w);
        //! In-the-money cash probability
        static Real itmProbability(Real f, Real k, Real v, Real w);
      private:
        Handle<Quote> volatility_;
    };

    // inline definitions

     /*! Returns
        \f[
            Black(f,k,v,w) = fw\Phi(wd_1(f,k,v)) - kw\Phi(wd_2(f,k,v)),
        \f]
        where
        \f[
            d_1(f,k,v) = \frac{\ln(f/k)+v^2/2}{v}
        \f]
        and
        \f[
            d_2(f,k,v) = d_1(f,k,v) - v.
        \f]
    */
    inline Real BlackModel::formula(Real f, Real k, Real v, Real w) {
        if (std::fabs(v) < QL_EPSILON)
            return std::max(f*w - k*w, Real(0.0));
        Real d1 = std::log(f/k)/v + 0.5*v;
        Real d2 = d1 - v;
        CumulativeNormalDistribution phi;
        Real result = w*(f*phi(w*d1) - k*phi(w*d2));
        // numerical inaccuracies can yield a negative answer
        return std::max(Real(0.0), result);
    }

    /*! Returns
        \f[
            P(f,k,v,w) = \Phi(wd_2(f,k,v)),
        \f]
        where
        \f[
            d_1(f,k,v) = \frac{\ln(f/k)+v^2/2}{v}
        \f]
        and
        \f[
            d_2(f,k,v) = d_1(f,k,v) - v.
        \f]
    */
    inline Real BlackModel::itmProbability(Real f, Real k, Real v, Real w) {
        if (std::fabs(v) < QL_EPSILON)
            return (f*w > k*w ? 1.0 : 0.0);
        Real d1 = std::log(f/k)/v + 0.5*v;
        Real d2 = d1 - v;
        CumulativeNormalDistribution phi;
        return phi(w*d2);
    }

}

#endif
