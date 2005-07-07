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

#include "swaption.hpp"
#include "utilities.hpp"
#include <ql/Instruments/swaption.hpp>
#include <ql/TermStructures/flatforward.hpp>
#include <ql/Indexes/euribor.hpp>
#include <ql/DayCounters/actual365fixed.hpp>
#include <ql/DayCounters/thirty360.hpp>
#include <ql/PricingEngines/Swaption/blackswaptionengine.hpp>
#include <ql/Utilities/dataformatters.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(SwaptionTest)

// global data

Integer exercises[] = { 1, 2, 3, 5, 7, 10 };
Integer lengths[] = { 1, 2, 3, 5, 7, 10, 15, 20 };
bool payFixed[] = { false, true };

Date today_, settlement_;
Real nominal_;
Calendar calendar_;
BusinessDayConvention fixedConvention_, floatingConvention_;
Frequency fixedFrequency_, floatingFrequency_;
DayCounter fixedDayCount_;
boost::shared_ptr<Xibor> index_;
Integer settlementDays_, fixingDays_;
Handle<YieldTermStructure> termStructure_;

// utilities

boost::shared_ptr<SimpleSwap> makeSwap(const Date& start, Integer length,
                                       Rate fixedRate,
                                       Spread floatingSpread,
                                       bool payFixed) {
    Date maturity = calendar_.advance(start,length,Years,
                                      floatingConvention_);
    Schedule fixedSchedule(calendar_,start,maturity,
                           fixedFrequency_,fixedConvention_);
    Schedule floatSchedule(calendar_,start,maturity,
                           floatingFrequency_,floatingConvention_);
    return boost::shared_ptr<SimpleSwap>(
            new SimpleSwap(payFixed,nominal_,
                           fixedSchedule,fixedRate,fixedDayCount_,
                           floatSchedule,index_,fixingDays_,floatingSpread,
                           termStructure_));
}

boost::shared_ptr<Swaption> makeSwaption(
                                    const boost::shared_ptr<SimpleSwap>& swap,
                                    const Date& exercise,
                                    Volatility volatility) {
    boost::shared_ptr<Quote> vol_me(new SimpleQuote(volatility));
    Handle<Quote> vol_rh(vol_me);
    boost::shared_ptr<BlackModel> model(new BlackModel(vol_rh,termStructure_));
    boost::shared_ptr<PricingEngine> engine(new BlackSwaptionEngine(model));
    return boost::shared_ptr<Swaption>(new Swaption(
                  swap,
                  boost::shared_ptr<Exercise>(new EuropeanExercise(exercise)),
                  termStructure_,
                  engine));
}

void setup() {
    settlementDays_ = 2;
    fixingDays_ = 2;
    nominal_ = 100.0;
    fixedConvention_ = Unadjusted;
    floatingConvention_ = ModifiedFollowing;
    fixedFrequency_ = Annual;
    floatingFrequency_ = Semiannual;
    fixedDayCount_ = Thirty360();
    index_ = boost::shared_ptr<Xibor>(new Euribor(12/floatingFrequency_,
                                                  Months, termStructure_));
    calendar_ = index_->calendar();
    today_ = calendar_.adjust(Date::todaysDate());
    Settings::instance().evaluationDate() = today_;
    settlement_ = calendar_.advance(today_,settlementDays_,Days);
    termStructure_.linkTo(flatRate(settlement_,0.05,Actual365Fixed()));
}

void teardown() {
    Settings::instance().evaluationDate() = Date();
}

QL_END_TEST_LOCALS(SwaptionTest)


void SwaptionTest::testStrikeDependency() {

    BOOST_MESSAGE("Testing swaption dependency on strike...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Rate strikes[] = { 0.03, 0.04, 0.05, 0.06, 0.07 };

    for (Size i=0; i<LENGTH(exercises); i++) {
        for (Size j=0; j<LENGTH(lengths); j++) {
            for (Size k=0; k<LENGTH(payFixed); k++) {
                Date exerciseDate = calendar_.advance(today_,
                                                      exercises[i],Years);
                Date startDate = calendar_.advance(exerciseDate,
                                                   settlementDays_,Days);
                // store the results for different rates...
                std::vector<Real> values;
                for (Size l=0; l<LENGTH(strikes); l++) {
                    boost::shared_ptr<SimpleSwap> swap =
                        makeSwap(startDate,lengths[j],strikes[l],
                                 0.0,payFixed[k]);
                    boost::shared_ptr<Swaption> swaption =
                        makeSwaption(swap,exerciseDate,0.20);
                    values.push_back(swaption->NPV());
                }
                // and check that they go the right way
                if (payFixed[k]) {
                    std::vector<Real>::iterator it =
                        std::adjacent_find(values.begin(), values.end(),
                                           std::less<Real>());
                    if (it != values.end()) {
                        Size n = it - values.begin();
                        BOOST_ERROR(
                            "NPV is increasing with the strike "
                            << "in a payer swaption: \n"
                            << "    exercise date: " << exerciseDate << "\n"
                            << "    length: " << lengths[j] << " years\n"
                            << "    value:  " << values[n]
                            << " at strike: " << io::rate(strikes[n]) << "\n"
                            << "    value:  " << values[n+1]
                            << " at strike: " << io::rate(strikes[n+1]));
                    }
                } else {
                    std::vector<Real>::iterator it =
                        std::adjacent_find(values.begin(), values.end(),
                                           std::greater<Real>());
                    if (it != values.end()) {
                        Size n = it - values.begin();
                        BOOST_ERROR(
                            "NPV is decreasing with the strike "
                            << "in a receiver swaption: \n"
                            << "    exercise date: " << exerciseDate << "\n"
                            << "    length: " << lengths[j] << " years\n"
                            << "    value:  " << values[n]
                            << " at strike: " << io::rate(strikes[n]) << "\n"
                            << "    value:  " << values[n+1]
                            << " at strike: " << io::rate(strikes[n+1]));
                    }
                }
            }
        }
    }

    QL_TEST_TEARDOWN
}

void SwaptionTest::testSpreadDependency() {

    BOOST_MESSAGE("Testing swaption dependency on spread...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Spread spreads[] = { -0.002, -0.001, 0.0, 0.001, 0.002 };

    for (Size i=0; i<LENGTH(exercises); i++) {
        for (Size j=0; j<LENGTH(lengths); j++) {
            for (Size k=0; k<LENGTH(payFixed); k++) {
                Date exerciseDate = calendar_.advance(today_,
                                                      exercises[i],Years);
                Date startDate = calendar_.advance(exerciseDate,
                                                   settlementDays_,Days);
                // store the results for different rates...
                std::vector<Real> values;
                for (Size l=0; l<LENGTH(spreads); l++) {
                    boost::shared_ptr<SimpleSwap> swap =
                        makeSwap(startDate,lengths[j],0.06,
                                 spreads[l],payFixed[k]);
                    boost::shared_ptr<Swaption> swaption =
                        makeSwaption(swap,exerciseDate,0.20);
                    values.push_back(swaption->NPV());
                }
                // and check that they go the right way
                if (payFixed[k]) {
                    std::vector<Real>::iterator it =
                        std::adjacent_find(values.begin(), values.end(),
                                           std::greater<Real>());
                    if (it != values.end()) {
                        Size n = it - values.begin();
                        BOOST_ERROR(
                            "NPV is decreasing with the spread "
                            << "in a payer swaption: \n"
                            << "    exercise date: " << exerciseDate << "\n"
                            << "    length: " << lengths[j] << " years\n"
                            << "    value:  " << values[n]
                            << " for spread: " << io::rate(spreads[n]) << "\n"
                            << "    value:  " << values[n+1]
                            << " for spread: " << io::rate(spreads[n+1]));
                    }
                } else {
                    std::vector<Real>::iterator it =
                        std::adjacent_find(values.begin(), values.end(),
                                           std::less<Real>());
                    if (it != values.end()) {
                        Size n = it - values.begin();
                        BOOST_ERROR(
                            "NPV is increasing with the spread "
                            << "in a receiver swaption: \n"
                            << "    exercise date: " << exerciseDate << "\n"
                            << "    length: " << lengths[j] << " years\n"
                            << "    value:  " << values[n]
                            << " for spread: " << io::rate(spreads[n]) << "\n"
                            << "    value:  " << values[n+1]
                            << " for spread: " << io::rate(spreads[n+1]));
                    }
                }
            }
        }
    }

    QL_TEST_TEARDOWN
}

void SwaptionTest::testSpreadTreatment() {

    BOOST_MESSAGE("Testing swaption treatment of spread...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Spread spreads[] = { -0.002, -0.001, 0.0, 0.001, 0.002 };

    for (Size i=0; i<LENGTH(exercises); i++) {
        for (Size j=0; j<LENGTH(lengths); j++) {
            for (Size k=0; k<LENGTH(payFixed); k++) {
                Date exerciseDate = calendar_.advance(today_,
                                                      exercises[i],Years);
                Date startDate = calendar_.advance(exerciseDate,
                                                   settlementDays_,Days);
                for (Size l=0; l<LENGTH(spreads); l++) {
                    boost::shared_ptr<SimpleSwap> swap =
                        makeSwap(startDate,lengths[j],0.06,
                                 spreads[l],payFixed[k]);
                    Spread correction = spreads[l] *
                                        swap->floatingLegBPS() /
                                        swap->fixedLegBPS();
                    boost::shared_ptr<SimpleSwap> equivalentSwap =
                        makeSwap(startDate,lengths[j],0.06+correction,
                                 0.0,payFixed[k]);
                    boost::shared_ptr<Swaption> swaption1 =
                        makeSwaption(swap,exerciseDate,0.20);
                    boost::shared_ptr<Swaption> swaption2 =
                        makeSwaption(equivalentSwap,exerciseDate,0.20);
                    if (std::fabs(swaption1->NPV()-swaption2->NPV()) > 1.0e-10)
                        BOOST_ERROR(
                            "wrong spread treatment: \n"
                            << "    exercise date: " << exerciseDate << "\n"
                            << "    length: " << lengths[j] << " years\n"
                            << "    pay " << (payFixed[k] ? "fixed\n"
                                                          : "floating\n")
                            << "    spread: " << io::rate(spreads[l]) << "\n"
                            << "    value of original swaption:   "
                            << swaption1->NPV() << "\n"
                            << "    value of equivalent swaption: "
                            << swaption2->NPV());
                }
            }
        }
    }

    QL_TEST_TEARDOWN
}

void SwaptionTest::testCachedValue() {

    BOOST_MESSAGE("Testing swaption value against cached value...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    today_ = Date(13,March,2002);
    settlement_ = Date(15,March,2002);
    Settings::instance().evaluationDate() = today_;
    termStructure_.linkTo(flatRate(settlement_,0.05,Actual365Fixed()));
    Date exerciseDate = calendar_.advance(settlement_,5,Years);
    Date startDate = calendar_.advance(exerciseDate,settlementDays_,Days);
    boost::shared_ptr<SimpleSwap> swap = makeSwap(startDate,10,0.06,0.0,true);
    boost::shared_ptr<Swaption> swaption =
        makeSwaption(swap,exerciseDate,0.20);
#ifndef QL_USE_INDEXED_COUPON
    Real cachedNPV = 3.639365179345;
#else
	Real cachedNPV = 3.639692232647;
#endif

    if (std::fabs(swaption->NPV()-cachedNPV) > 1.0e-11)
        BOOST_ERROR("failed to reproduce cached swaption value:\n"
                    << QL_FIXED << std::setprecision(12)
                    << "    calculated: " << swaption->NPV() << "\n"
                    << "    expected:   " << cachedNPV);

    QL_TEST_TEARDOWN
}


test_suite* SwaptionTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Swaption tests");
    suite->add(BOOST_TEST_CASE(&SwaptionTest::testStrikeDependency));
    suite->add(BOOST_TEST_CASE(&SwaptionTest::testSpreadDependency));
    suite->add(BOOST_TEST_CASE(&SwaptionTest::testSpreadTreatment));
    suite->add(BOOST_TEST_CASE(&SwaptionTest::testCachedValue));
    return suite;
}

