
/*
 Copyright (C) 2005 Eric Ehlers

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

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <qla/config.hpp>
#endif
#include <oh/objhandler.hpp>
#include <qla/generalutils.hpp>
#include <ql/basicdataformatters.hpp>
#include <ql/Calendars/all.hpp>

namespace QuantLibAddin {

    QuantLib::DayCounter IDtoDayCounter(const std::string &dayCounterID) {
        std::string idUpper = QuantLib::StringFormatter::toUppercase(dayCounterID);
        if (idUpper.compare("ACTUAL365FIXED") ==0)
            return QuantLib::Actual365Fixed();
        else if (idUpper.compare("ACTUAL360") == 0)
            return QuantLib::Actual360();
        else if (idUpper.compare("ACTUALACTUAL") == 0)
            return QuantLib::ActualActual();
        else if (idUpper.compare("THIRTY360") == 0)
            return QuantLib::Thirty360();
        else
            QL_FAIL("IDtoDayCounter: unrecognized dayCounterID: " + dayCounterID);
    }

    QuantLib::Calendar IDtoCalendar(const std::string &calendarID) {
        std::string idUpper = QuantLib::StringFormatter::toUppercase(calendarID);
        if (idUpper.compare("GERMANY") ==0)
            return QuantLib::Germany();
        else if (idUpper.compare("ITALY") == 0)
            return QuantLib::Italy();
        else if (idUpper.compare("TARGET") == 0)
            return QuantLib::TARGET();
        else if (idUpper.compare("TOKYO") == 0)
            return QuantLib::Tokyo();
        else if (idUpper.compare("UNITEDKINGDOM") == 0)
            return QuantLib::UnitedKingdom();
        else if (idUpper.compare("UNITEDSTATES") == 0)
            return QuantLib::UnitedStates();
        else
            QL_FAIL("IDtoCalendar: unrecognized calendarID: " + calendarID);
    }
    
    QuantLib::TimeUnit IDtoTimeUnit(const std::string &timeUnitID) {
        std::string idUpper = QuantLib::StringFormatter::toUppercase(timeUnitID);
        if (idUpper.compare("DAYS") == 0)
            return QuantLib::Days;
        else if (idUpper.compare("WEEKS") == 0)
            return QuantLib::Weeks;
        else if (idUpper.compare("MONTHS") == 0)
            return QuantLib::Months;
        else if (idUpper.compare("YEARS") == 0)
            return QuantLib::Years;
        else
            QL_FAIL("IDtoTimeUnit: unrecognized timeUnitID: " + timeUnitID);
    }

    QuantLib::BusinessDayConvention IDtoConvention(const std::string &conventionID) {
        std::string idUpper = QuantLib::StringFormatter::toUppercase(conventionID);
        if (idUpper.compare("FOLLOWING") ==0)
            return QuantLib::Following;
        else if (idUpper.compare("MODIFIEDFOLLOWING") == 0)
            return QuantLib::ModifiedFollowing;
        else if (idUpper.compare("MODIFIEDPRECEDING") == 0)
            return QuantLib::ModifiedPreceding;
        else if (idUpper.compare("MONTHENDREFERENCE") == 0)
            return QuantLib::MonthEndReference;
        else if (idUpper.compare("PRECEDING") == 0)
            return QuantLib::Preceding;
        else if (idUpper.compare("UNADJUSTED") == 0)
            return QuantLib::Unadjusted;
        else
            QL_FAIL("IDtoConvention: unrecognized conventionID: " + conventionID);
    }

    QuantLib::Frequency IDtoFrequency(const std::string &frequencyID) {
        std::string idUpper = QuantLib::StringFormatter::toUppercase(frequencyID);
        if (idUpper.compare("NOFREQUENCY") ==0)
            return QuantLib::NoFrequency;
        else if (idUpper.compare("ONCE") == 0)
            return QuantLib::Once;
        else if (idUpper.compare("ANNUAL") == 0)
            return QuantLib::Annual;
        else if (idUpper.compare("SEMIANNUAL") == 0)
            return QuantLib::Semiannual;
        else if (idUpper.compare("EVERY4MONTH") == 0)
            return QuantLib::EveryFourthMonth;
        else if (idUpper.compare("QUARTERLY") == 0)
            return QuantLib::Quarterly;
        else if (idUpper.compare("BIMONTHLY") == 0)
            return QuantLib::Bimonthly;
        else if (idUpper.compare("MONTHLY") == 0)
            return QuantLib::Monthly;
        else
            QL_FAIL("IDtoFrequency: unrecognized frequencyID: " + frequencyID);
    }

    std::vector<QuantLib::Date> longVectorToDateVector(
            const std::vector < long > &v) {
        std::vector<QuantLib::Date> ret;
        std::vector<long>::const_iterator i;
        for (i=v.begin(); i!=v.end(); i++)
            ret.push_back(QuantLib::Date(*i));
        return ret;
    }

    QuantLib::Matrix vectorVectorToMatrix(
        const std::vector < std::vector < double > > &vv) {
        if (vv.size() == 0) {
            QuantLib::Matrix m(0, 0);
            return m;
        } else {
            const std::vector < double > v = vv[0];
            if (v.size() == 0) {
                QuantLib::Matrix m(0, 0);
                return m;
            }
            QuantLib::Matrix m(vv.size(), v.size());
            for (unsigned int i = 0; i < vv.size(); i++) {
                const std::vector < double > v = vv[i];
                for (unsigned int j = 0; j < v.size(); j++)
                    m[i][j] = v[j];
            }
            return m;
        }
    }

}
