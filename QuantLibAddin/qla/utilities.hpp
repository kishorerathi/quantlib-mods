
/*
 Copyright (C) 2004, 2005 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file
    \brief diagnostic and information functions for QuantLibAddin
*/

#ifndef qla_utilities_hpp
#define qla_utilities_hpp

#include <oh/objhandler.hpp>

// we want a QuantLibAddin function called QL_VERSION
// but QuantLib has already #defined that symbol
#undef QL_VERSION

namespace QuantLibAddin {

/*! \group utilities
    diagnostic and information functions for QuantLibAddin
*/

    /*! return the version of QuantLib
    */
    std::string QL_VERSION();

    /*! return the version of ObjectHandler
    */
    std::string QL_OH_VERSION();

    /*! list all available Enumerations
    */
	const std::vector<std::string>& getRegisteredEnums();

    /*! list the members of a given Enumeration
    */
	const std::vector<std::string>& getEnumMembers(const std::string&);

}

#endif

