
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, contact ferdinando@ametrano.net
 * The license is also available at http://quantlib.sourceforge.net/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.sourceforge.net/Authors.txt
*/
/*! \file standardpathgenerator.hpp
    \brief Generates path from random points

    $Source$
    $Name$
    $Log$
    Revision 1.2  2001/05/24 11:34:07  nando
    smoothing #include xx.hpp

    Revision 1.1  2001/04/09 14:05:48  nando
    all the *.hpp moved below the Include/ql level

    Revision 1.2  2001/04/06 18:46:20  nando
    changed Authors, Contributors, Licence and copyright header

    Revision 1.1  2001/04/04 11:07:22  nando
    Headers policy part 1:
    Headers should have a .hpp (lowercase) filename extension
    All *.h renamed to *.hpp

    Revision 1.4  2001/02/02 10:21:00  marmar
    PathGenerator replaced by RandomArrayGenerator

*/


#ifndef quantlib_montecarlo_standard_path_generator_h
#define quantlib_montecarlo_standard_path_generator_h

#include "ql/MonteCarlo/gaussianrandomgenerator.hpp"
#include "ql/MonteCarlo/randomarraygenerator.hpp"

namespace QuantLib {

    namespace MonteCarlo {

    /*! \typedef StandardPathGenerator
        The following is the default choice for the standard path generator
        See the corresponding class for documentation.
    */

        typedef RandomArrayGenerator<GaussianRandomGenerator >
                                               StandardPathGenerator;

    }
}

#endif
