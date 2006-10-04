/*
 Copyright (C) 2006 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.


 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details. */

#include <ql/Indexes/swapindex.hpp>
#include <ql/Instruments/all.hpp>

#include <sstream>


namespace QuantLib {

    SwapIndex::SwapIndex(const std::string& familyName,
                         Integer years,
                         Integer settlementDays,
                         Currency currency,
                         const Calendar& calendar,
                         Frequency fixedLegFrequency,
                         BusinessDayConvention fixedLegConvention,
                         const DayCounter& fixedLegDayCounter,
                         const boost::shared_ptr<Xibor>& iborIndex)
    : InterestRateIndex(familyName, Period(years, Years), settlementDays,
                        currency, calendar, fixedLegDayCounter),
      years_(years), iborIndex_(iborIndex),
      fixedLegFrequency_(fixedLegFrequency),
      fixedLegConvention_(fixedLegConvention)
    {
        registerWith(iborIndex_);
    }

    Rate SwapIndex::forecastFixing(const Date& fixingDate) const {
        QL_REQUIRE(iborIndex_, "no index set");
        QL_REQUIRE(iborIndex_->termStructure(), "no term structure set");
		boost::shared_ptr<VanillaSwap> swap(underlyingSwap(fixingDate));
         return swap->fairRate();
    }

	boost::shared_ptr<VanillaSwap> SwapIndex::underlyingSwap(
        const Date& fixingDate) const
    {
        bool old = true;
        if (old) {
		    Date start = calendar_.advance(fixingDate, settlementDays_,Days);
            Date end = NullCalendar().advance(start,years_,Years);
            
            Schedule fixedLegSchedule(start, end,
                                      Period(fixedLegFrequency_), calendar_,
                                      fixedLegConvention_,
                                      fixedLegConvention_,
                                      true, false);
            Schedule floatingLegSchedule(start, end,
                                         iborIndex_->tenor(), calendar_,
                                         iborIndex_->businessDayConvention(),
                                         iborIndex_->businessDayConvention(),
                                         true, false);
		    return boost::shared_ptr<VanillaSwap>(new
                VanillaSwap(true, 1.0,
                            fixedLegSchedule, 0.0, dayCounter_,
                            floatingLegSchedule, iborIndex_, 0.0,dayCounter_,
                            Handle<YieldTermStructure>(
                                    iborIndex_->termStructure())));
        } else {
            return MakeVanillaSwap(fixingDate, Period(fixedLegFrequency_),
                calendar_, 0.0, iborIndex_, iborIndex_->termStructure());
        }
	}

	boost::shared_ptr<Schedule> SwapIndex::fixedRateSchedule(const Date& fixingDate) const {
	
		Date start = calendar_.advance(fixingDate, settlementDays_,Days);
        Date end = calendar_.advance(start,years_,Years);

        return boost::shared_ptr<Schedule>(new Schedule(start, end, 
                                  Period(fixedLegFrequency_),calendar_,
                                  fixedLegConvention_, fixedLegConvention_,
                                  false, false));
	}
}
