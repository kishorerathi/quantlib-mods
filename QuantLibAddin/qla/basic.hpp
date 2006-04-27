/*
 Copyright (C) 2006 Eric Ehlers
 Copyright (C) 2006 Marco Marchioro--StatPro Italia
 Copyright (C) 2006 Katiuscia Manzoni

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
    \brief basic utility functions defined in basic.xml

    The functions implemented in this file are invoked in the Addins from code
    autogenerated by srcgen.py.
*/

#ifndef qla_basic_hpp
#define qla_basic_hpp

#include <oh/objhandler.hpp>
#include <ql/calendar.hpp>
#include <ql/Functions/prices.hpp>

namespace QuantLibAddin {

/*! \group basic
    basic utility functions for QuantLibAddin
*/

    /*! return the mid equivalent price, i.e. the mid if available,
        or a suitable substitute if the proper mid is not available
    */
    inline double qlMidEquivalent(const double bid,
                                  const double ask,
                                  const double last,
                                  const double close) {
        return QuantLib::midEquivalent(bid, ask, last, close);
    }    


    /*! return the advanced date over a given calendar
    */
    QuantLib::Date qlAdvanceCalendar(
            const long &lStartDate,
            const long &n,
            const std::string &timeUnitsID,
            const std::string &calendarID,
            const std::string &conventionID);    

    /*! return the year fraction with respect to a given day-counter convention
    */
    double qlYearFraction(
        const std::string &bDayConvID,
        const long &lStartDate,
        const long &lEndDate,
        const long &lRefStartDate,
        const long &lRefEndDate);    

    /*! return the evaluation date
    */
    QuantLib::Date qlEvaluationDate();

    /*! set the evaluation date
    */
    long qlSetEvaluationDate(const long &evalDate);

}

#endif

