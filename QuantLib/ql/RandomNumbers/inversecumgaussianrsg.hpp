
/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file inversecumgaussianrsg.hpp
    \brief Inverse cumulative Gaussian random sequence generator

    \fullpath
    ql/RandomNumbers/%inversecumgaussianrsg.hpp
*/

// $Id$

#ifndef quantlib_inversecumulative_gaussian_rsg_h
#define quantlib_inversecumulative_gaussian_rsg_h

#include <ql/array.hpp>
#include <ql/Math/normaldistribution.hpp>
#include <ql/MonteCarlo/sample.hpp>

namespace QuantLib {

    namespace RandomNumbers {

        //! Inverse cumulative Gaussian random sequence generator
        /*! It uses a sequence of uniform deviate in (0, 1) as the
            source of cumulative normal distribution values.
            Then an inverse cumulative normal distribution is used as it is
            approximately a Gaussian deviate with average 0.0 and standard
            deviation 1.0.

            The uniform deviate sequence is supplied by U.

            Class U must implement the following interface:
            \code
                U::sample_type U::next() const;
                Size U::dimension() const;
            \endcode

            The inverse cumulative normal distribution is supplied by I.

            Class I must implement the following interface:
            \code
                I::I();
                double I::operator() const;
            \endcode
        */
        template <class U, class I>
        class ICGaussianRsg {
          public:
            typedef MonteCarlo::Sample<Array> sample_type;
            explicit ICGaussianRsg(const U& uniformSequenceGenerator);
            //! returns next sample from the Gaussian distribution
            const sample_type& nextSequence() const;
            Size dimension() const {return dimension_;}
          private:
            U uniformSequenceGenerator_;
            Size dimension_;
            mutable sample_type x;
            I ICND_;
        };

        template <class U, class I>
        ICGaussianRsg<U, I>::ICGaussianRsg(const U& uniformSequenceGenerator)
        : uniformSequenceGenerator_(uniformSequenceGenerator),
          dimension_(uniformSequenceGenerator_.dimension()),
          x(Array(dimension_), 1.0) {}

        template <class U, class I>
        inline const typename ICGaussianRsg<U, I>::sample_type& 
        ICGaussianRsg<U, I>::nextSequence() const {
            typename U::sample_type sample = uniformSequenceGenerator_.nextSequence();
            x.weight = sample.weight;
            for (Size i = 0; i < dimension_; i++) {
                x.value[i] = ICND_(sample.value[i]);
            }
            return x;
        }

    }

}

#endif
