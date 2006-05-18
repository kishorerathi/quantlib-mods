/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003, 2004 Ferdinando Ametrano
 Copyright (C) 2006 Richard Gould

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

/*! \file sobolrsg.hpp
    \brief Sobol low-discrepancy sequence generator
*/

#ifndef quantlib_sobol_ld_rsg_hpp
#define quantlib_sobol_ld_rsg_hpp

#include <ql/Math/array.hpp>
#include <ql/MonteCarlo/sample.hpp>
#include <vector>

namespace QuantLib {

    //! Sobol low-discrepancy sequence generator
    /*! A Gray code counter and bitwise operations are used for very
        fast sequence generation.

        The implementation relies on primitive polynomials modulo two
        from the book "Monte Carlo Methods in Finance" by Peter
        J�ckel.

        21 200 primitive polynomials modulo two are provided by
        default in QuantLib. J�ckel has calculated 8 129 334
        polynomials, also available in a different file that can be
        downloaded from http://quantlib.org. If you need that many
        dimensions you must replace the default version of the
        primitivepolynomials.c file with the extended one.

        The choice of initialization numbers (also know as free direction
        integers) is crucial for the homogeneity properties of the sequence.
        Sobol defines two homogeneity properties: Property A and Property A'.

        The unit initialization numbers suggested in "Numerical
        Recipes in C", 2nd edition, by Press, Teukolsky, Vetterling,
        and Flannery (section 7.7) fail the test for Property A even
        for low dimensions.

        Bratley and Fox published coefficients of the free direction
        integers up to dimension 40, crediting unpublished work of
        Sobol' and Levitan. See Bratley, P., Fox, B.L. (1988)
        "Algorithm 659: Implementing Sobol's quasirandom sequence
        generator," ACM Transactions on Mathematical Software
        14:88-100. These values satisfy Property A for d<=20 and d =
        23, 31, 33, 34, 37; Property A' holds for d<=6.

        J�ckel provides in his book (section 8.3) initialization
        numbers up to dimension 32. Coefficients for d<=8 are the same
        as in Bradley-Fox, so Property A' holds for d<=6 but Property
        A holds for d<=32.

        The implementation of Lemieux, Cieslak, and Luttmer includes
        coefficients of the free direction integers up to dimension
        360.  Coefficients for d<=40 are the same as in Bradley-Fox.
        For dimension 40<d<=360 the coefficients have
        been calculated as optimal values based on the "resolution"
        criterion. See "RandQMC user's guide - A package for
        randomized quasi-Monte Carlo methods in C," by C. Lemieux,
        M. Cieslak, and K. Luttmer, version January 13 2004, and
        references cited there
        (http://www.math.ucalgary.ca/~lemieux/randqmc.html).
        The values up to d<=360 has been provided to the QuantLib team by
        Christiane Lemieux, private communication, September 2004.

        For more info on Sobol' sequences see also "Monte Carlo
        Methods in Financial Engineering," by P. Glasserman, 2004,
        Springer, section 5.2.3

        \test
        - the correctness of the returned values is tested by
          reproducing known good values.
        - the correctness of the returned values is tested by checking
          their discrepancy against known good values.
    */
    class SobolRsg {
      public:
        typedef Sample<Array> sample_type;
        enum DirectionIntegers {
            Unit, Jaeckel, SobolLevitan, SobolLevitanLemieux };
        /*! \pre dimensionality must be <= PPMT_MAX_DIM */
        SobolRsg(Size dimensionality,
                 unsigned long seed = 0,
                 DirectionIntegers directionIntegers = Jaeckel);
        /*! skip to the n-th sample in the low-discrepancy sequence */
        void skipTo(unsigned long n);
        const std::vector<unsigned long>& nextInt32Sequence() const;
        const SobolRsg::sample_type& nextSequence() const {
            const std::vector<unsigned long>& v = nextInt32Sequence();
            // normalize to get a double in (0,1)
            for (Size k=0; k<dimensionality_; k++)
                sequence_.value[k] = v[k] * normalizationFactor_;
            return sequence_;
        }
        const sample_type& lastSequence() const { return sequence_; }
        Size dimension() const { return dimensionality_; }
      private:
        static const int bits_;
        static const double normalizationFactor_;
        Size dimensionality_;
        mutable unsigned long sequenceCounter_;
        mutable bool firstDraw_;
        mutable sample_type sequence_;
        mutable std::vector<unsigned long> integerSequence_;
        std::vector<std::vector<unsigned long> > directionIntegers_;
    };

}

#endif
