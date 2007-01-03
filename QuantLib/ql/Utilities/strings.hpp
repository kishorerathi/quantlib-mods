/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 StatPro Italia srl

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

/*! \file strings.hpp
    \brief string utilities
*/

#ifndef quantlib_strings_hpp
#define quantlib_strings_hpp

#include <ql/qldefines.hpp>

#ifndef QL_DISABLE_DEPRECATED

#include <string>
#include <boost/algorithm/string/case_conv.hpp>

namespace QuantLib {

    //! \deprecated use boost::algorithm::to_lower_copy instead
    inline std::string lowercase(const std::string& s) {
        return boost::algorithm::to_lower_copy(s);
    }

    //! \deprecated use boost::algorithm::to_upper_copy instead
    inline std::string uppercase(const std::string& s) {
        return boost::algorithm::to_upper_copy(s);
    }

}

#endif

#endif
