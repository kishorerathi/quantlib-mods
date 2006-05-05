/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Ferdinando Ametrano

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

#ifndef quantlib_functions_calendar_h
#define quantlib_functions_calendar_h

#include <ql/Functions/qlfunctions.hpp>
#include <ql/calendar.hpp>
#include <ql/daycounter.hpp>
#include <ql/settings.hpp>
#include <vector>

namespace QuantLib {

    /*! get the evaluation date */
    inline Date evaluationDate() {
        return Settings::instance().evaluationDate();
    }

    /*! set the evaluation date */
    inline Date setEvaluationDate(const Date &evalDate) {
        Settings::instance().evaluationDate() = evalDate;
        return evalDate;
    }

    //! Returns the holidays between two dates
    std::vector<Date> holidayList(const Calendar& calendar,
                                  const Date& from,
                                  const Date& to,
                                  bool includeWeekEnds = false);

    /*! return the advanced date over a given calendar
    */
    inline Date advanceCalendar(
            const Date &startDate,
            const long &n,
            const TimeUnit &timeUnits,
            const Calendar &calendar,
            const BusinessDayConvention &convention) {
        return calendar.advance(startDate,
                                n,
                                timeUnits,
                                convention);
    }


}

#endif

