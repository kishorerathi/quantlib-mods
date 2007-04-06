
/*
 Copyright (C) 2005, 2006 Eric Ehlers

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

#ifndef qla_volatilities_hpp
#define qla_volatilities_hpp

#include <oh/objhandler.hpp>
#include <ql/termstructures/volatilities/blackconstantvol.hpp>
#include <ql/termstructures/volatilities/blackvariancesurface.hpp>

namespace QuantLibAddin {

    class BlackVolTermStructure : public ObjHandler::LibraryObject<QuantLib::BlackVolTermStructure> {
    };

    class BlackConstantVol : public BlackVolTermStructure {
    public:
        BlackConstantVol(
            const QuantLib::Date &settlementDate,
            const double &volatility,
            const QuantLib::DayCounter &dayCounter);
    };

    class BlackVarianceSurface : public BlackVolTermStructure {
    public:
        BlackVarianceSurface(
            const QuantLib::Date &settlementDate,
            const std::vector < QuantLib::Date > &dates,
            const std::vector < double > &strikes,
            const QuantLib::Matrix &vols,
            const QuantLib::DayCounter &dayCounter);
    };

}

#endif

