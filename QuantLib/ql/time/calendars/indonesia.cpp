/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2007 StatPro Italia srl

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

#include <ql/time/calendars/indonesia.hpp>
#include <ql/errors.hpp>

namespace QuantLib {

    Indonesia::Indonesia(Market market) {
        // all calendar instances share the same implementation instance
        static boost::shared_ptr<Calendar::Impl> bejImpl(
                                                      new Indonesia::BejImpl);
        switch (market) {
          case BEJ:
          case JSX:
            impl_ = bejImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }

    bool Indonesia::BejImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        Day d = date.dayOfMonth();
        Month m = date.month();
        Year y = date.year();
        Day dd = date.dayOfYear();
        Day em = easterMonday(y);

        if (isWeekend(w)
            // New Year's Day
            || (d == 1 && m == January)
            // Good Friday
            || (dd == em-3)
            // Ascension Thursday
            || (dd == em+38)
            // Independence Day
            || (d == 17 && m == August)
            // Christmas
            || (d == 25 && m == December)
            )
            return false;
        if (y == 2005) {
            if (// Idul Adha
                (d == 21 && m == January)
                // Imlek
                || (d == 9 && m == February)
                // Moslem's New Year Day
                || (d == 10 && m == February)
                // Nyepi
                || (d == 11 && m == March)
                // Birthday of Prophet Muhammad SAW
                || (d == 22 && m == April)
                // Waisak
                || (d == 24 && m == May)
                // Ascension of Prophet Muhammad SAW
                || (d == 2 && m == September)
                // Idul Fitri
                || ((d == 3 || d == 4) && m == November)
                // National leaves
                || ((d == 2 || d == 7 || d == 8) && m == November)
                || (d == 26 && m == December)
                )
                return false;
        }
        if (y == 2006) {
            if (// Idul Adha
                (d == 10 && m == January)
                // Moslem's New Year Day
                || (d == 31 && m == January)
                // Nyepi
                || (d == 30 && m == March)
                // Birthday of Prophet Muhammad SAW
                || (d == 10 && m == April)
                // Ascension of Prophet Muhammad SAW
                || (d == 21 && m == August)
                // Idul Fitri
                || ((d == 24 || d == 25) && m == October)
                // National leaves
                || ((d == 23 || d == 26 || d == 27) && m == October)
                )
                return false;
        }
        if (y == 2007) {
            if (// Nyepi
                (d == 19 && m == March)
                // Waisak
                || (d == 1 && m == June)
                // Ied Adha
                || (d == 20 && m == December)
                // National leaves
                || (d == 18 && m == May)
                || ((d == 12 || d == 15 || d == 16) && m == October)
                || ((d == 21 || d == 24) && m == October)
                )
                return false;
        }
        return true;
    }

}

