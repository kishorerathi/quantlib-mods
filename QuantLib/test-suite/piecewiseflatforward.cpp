/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 RiskMap srl

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

#include "piecewiseflatforward.hpp"
#include "utilities.hpp"
#include <ql/termstructures/piecewiseflatforward.hpp>
#include <ql/termstructures/ratehelpers.hpp>
#include <ql/calendars/target.hpp>
#include <ql/daycounters/actual360.hpp>
#include <ql/daycounters/thirty360.hpp>
#include <ql/indexes/euribor.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/quotes/simplequote.hpp>
#include <iomanip>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(PiecewiseFlatForwardTest)

struct Datum {
    Integer n;
    TimeUnit units;
    Rate rate;
};

Datum depositData[] = {
    { 1, Weeks,  4.559 },
    { 1, Months, 4.581 },
    { 2, Months, 4.573 },
    { 3, Months, 4.557 },
    { 6, Months, 4.496 },
    { 9, Months, 4.490 }
};

Datum swapData[] = {
    {  1, Years, 4.54 },
    {  2, Years, 4.63 },
    {  3, Years, 4.75 },
    {  4, Years, 4.86 },
    {  5, Years, 4.99 },
    {  6, Years, 5.11 },
    {  7, Years, 5.23 },
    {  8, Years, 5.33 },
    {  9, Years, 5.41 },
    { 10, Years, 5.47 },
    { 12, Years, 5.60 },
    { 15, Years, 5.75 },
    { 20, Years, 5.89 },
    { 25, Years, 5.95 },
    { 30, Years, 5.96 }
};

// test-global variables

Calendar calendar;
Integer settlementDays;
Date today, settlement;
BusinessDayConvention depoConvention;
DayCounter depoDayCounter;
BusinessDayConvention fixedLegConvention, floatingLegConvention;
Frequency fixedLegFrequency;
DayCounter fixedLegDayCounter;
Frequency floatingLegFrequency;

Size deposits, swaps;
std::vector<boost::shared_ptr<SimpleQuote> > rates;
std::vector<boost::shared_ptr<RateHelper> > instruments;
boost::shared_ptr<YieldTermStructure> termStructure;

void setup() {

    // data
    calendar = TARGET();
    settlementDays = 2;
    today = calendar.adjust(Date::todaysDate());
    Settings::instance().evaluationDate() = today;
    settlement = calendar.advance(today,settlementDays,Days);
    depoConvention = ModifiedFollowing;
    depoDayCounter = Actual360();
    fixedLegConvention = Unadjusted;
    floatingLegConvention = ModifiedFollowing;
    fixedLegFrequency = Annual;
    fixedLegDayCounter = Thirty360();
    floatingLegFrequency = Semiannual;

    deposits = LENGTH(depositData);
    swaps = LENGTH(swapData);

    // market elements
    rates = std::vector<boost::shared_ptr<SimpleQuote> >(deposits+swaps);
    Size i;
    for (i=0; i<deposits; i++) {
        rates[i] = boost::shared_ptr<SimpleQuote>(
                                    new SimpleQuote(depositData[i].rate/100));
    }
    for (i=0; i<swaps; i++) {
        rates[i+deposits] = boost::shared_ptr<SimpleQuote>(
                                       new SimpleQuote(swapData[i].rate/100));
    }

    // rate helpers
    instruments =
        std::vector<boost::shared_ptr<RateHelper> >(deposits+swaps);
    for (i=0; i<deposits; i++) {
        Handle<Quote> r(rates[i]);
        instruments[i] = boost::shared_ptr<RateHelper>(
              new DepositRateHelper(r, depositData[i].n*depositData[i].units,
                                    settlementDays, calendar,
                                    depoConvention, true, 2, depoDayCounter));
    }
    boost::shared_ptr<IborIndex> index(new IborIndex("dummy",
                                             Period(floatingLegFrequency),
                                             settlementDays,
                                             Currency(),
                                             calendar,
                                             floatingLegConvention,
                                             false,
                                             Actual360()));
    for (i=0; i<swaps; i++) {
        Handle<Quote> r(rates[i+deposits]);
        instruments[i+deposits] = boost::shared_ptr<RateHelper>(
                new SwapRateHelper(r, swapData[i].n*swapData[i].units,
                                   settlementDays, calendar,
                                   fixedLegFrequency, fixedLegConvention,
                                   fixedLegDayCounter, index));
    }

    // instantiate curve
    termStructure = boost::shared_ptr<YieldTermStructure>(
                new PiecewiseFlatForward(settlement,instruments,Actual360()));

}

void teardown() {
    Settings::instance().evaluationDate() = Date();
}

QL_END_TEST_LOCALS(PiecewiseFlatForwardTest)


void PiecewiseFlatForwardTest::testConsistency() {

    BOOST_MESSAGE("Testing consistency of piecewise flat forward curve...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Handle<YieldTermStructure> euriborHandle;
    euriborHandle.linkTo(termStructure);

    Size i;
    // check deposits
    for (i=0; i<deposits; i++) {
        Euribor index(depositData[i].n*depositData[i].units,euriborHandle);
        Rate expectedRate  = depositData[i].rate/100,
             estimatedRate = index.fixing(today);
        if (std::fabs(expectedRate-estimatedRate) > 1.0e-9) {
            BOOST_FAIL(
                depositData[i].n << " "
                << (depositData[i].units == Weeks ? "week(s)" : "month(s)")
                << " deposit: \n"
                << std::setprecision(8)
                << "    estimated rate: " << io::rate(estimatedRate) << "\n"
                << "    expected rate:  " << io::rate(expectedRate));
        }
    }

    // check swaps
    boost::shared_ptr<IborIndex> index(new
        Euribor(Period(floatingLegFrequency), euriborHandle));
    for (i=0; i<swaps; i++) {
        Date maturity = settlement + swapData[i].n*swapData[i].units;
        Schedule fixedSchedule(settlement,maturity,Period(fixedLegFrequency),
                               calendar,fixedLegConvention,fixedLegConvention,
                               false,false);
        Schedule floatSchedule(settlement,maturity,Period(floatingLegFrequency),
                               calendar,floatingLegConvention,floatingLegConvention,
                               false,false);
        VanillaSwap swap(VanillaSwap::Payer, 100.0,
                         fixedSchedule, 0.0, fixedLegDayCounter,
                         floatSchedule, index, 0.0, Actual360(),
                         euriborHandle);
        Rate expectedRate = swapData[i].rate/100,
             estimatedRate = swap.fairRate();
        if (std::fabs(expectedRate-estimatedRate) > 1.0e-9) {
            BOOST_FAIL(swapData[i].n << " year(s) swap:\n"
                       << std::setprecision(8)
                       << "    estimated rate: "
                       << io::rate(estimatedRate) << "\n"
                       << "    expected rate:  "
                       << io::rate(expectedRate));
        }
    }

    QL_TEST_TEARDOWN
}

void PiecewiseFlatForwardTest::testObservability() {

    BOOST_MESSAGE("Testing observability of piecewise flat forward curve...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    termStructure = boost::shared_ptr<YieldTermStructure>(new
        PiecewiseFlatForward(settlementDays, calendar,
                             instruments, Actual360()));

    Flag f;
    f.registerWith(termStructure);

    for (Size i=0; i<deposits+swaps; i++) {
        f.lower();
        rates[i]->setValue(rates[i]->value()*1.01);
        if (!f.isUp())
            BOOST_FAIL("Observer was not notified of underlying rate change");
    }

    f.lower();
    Settings::instance().evaluationDate() = calendar.advance(today,1,Months);
    if (!f.isUp())
            BOOST_FAIL("Observer was not notified of date change");

    QL_TEST_TEARDOWN
}


test_suite* PiecewiseFlatForwardTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Piecewise flat forward tests");
    #if defined(QL_PATCH_MSVC6) || defined(QL_PATCH_MSVC70)
    suite->add(BOOST_TEST_CASE(&PiecewiseFlatForwardTest::testConsistency));
    suite->add(BOOST_TEST_CASE(&PiecewiseFlatForwardTest::testObservability));
    #endif
    return suite;
}

