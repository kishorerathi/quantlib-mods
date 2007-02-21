/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 StatPro Italia srl

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

#include "convertiblebonds.hpp"
#include "utilities.hpp"
#include <ql/Instruments/convertiblebond.hpp>
#include <ql/Instruments/zerocouponbond.hpp>
#include <ql/Instruments/fixedcouponbond.hpp>
#include <ql/Instruments/floatingratebond.hpp>
#include <ql/Instruments/vanillaoption.hpp>
#include <ql/PricingEngines/Hybrid/binomialconvertibleengine.hpp>
#include <ql/PricingEngines/Vanilla/binomialengine.hpp>
#include <ql/Calendars/target.hpp>
#include <ql/DayCounters/actual360.hpp>
#include <ql/Indexes/euribor.hpp>
#include <ql/TermStructures/flatforward.hpp>
#include <ql/TermStructures/forwardspreadedtermstructure.hpp>
#include <ql/Volatilities/blackconstantvol.hpp>
#include <ql/Utilities/dataformatters.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(ConvertibleBondTest)

Date today_, issueDate_, maturityDate_;
Calendar calendar_;
DayCounter dayCounter_;
Frequency frequency_;
Integer settlementDays_;

RelinkableHandle<Quote> underlying_;
RelinkableHandle<YieldTermStructure> dividendYield_, riskFreeRate_;
RelinkableHandle<BlackVolTermStructure> volatility_;
boost::shared_ptr<StochasticProcess> process_;

RelinkableHandle<Quote> creditSpread_;

CallabilitySchedule no_callability;
DividendSchedule no_dividends;

Real faceAmount_, redemption_, conversionRatio_;

void setup() {
    calendar_ = TARGET();

    today_ = calendar_.adjust(Date::todaysDate());
    Settings::instance().evaluationDate() = today_;

    dayCounter_ = Actual360();
    frequency_ = Annual;
    settlementDays_ = 3;

    issueDate_ = calendar_.advance(today_,2,Days);
    maturityDate_ = calendar_.advance(issueDate_, 10, Years);
    // reset to avoid inconsistencies as the schedule is backwards
    issueDate_ = calendar_.advance(maturityDate_, -10, Years);

    underlying_.linkTo(boost::shared_ptr<Quote>(new SimpleQuote(50.0)));
    dividendYield_.linkTo(flatRate(today_, 0.02, dayCounter_));
    riskFreeRate_.linkTo(flatRate(today_, 0.05, dayCounter_));
    volatility_.linkTo(flatVol(today_, 0.15, dayCounter_));

    process_ = boost::shared_ptr<StochasticProcess>(
                    new BlackScholesMertonProcess(underlying_, dividendYield_,
                                                  riskFreeRate_, volatility_));

    creditSpread_.linkTo(boost::shared_ptr<Quote>(new SimpleQuote(0.005)));

    // it fails with 1000000
    //faceAmount_ = 1000000.0;
    faceAmount_ = 100.0;
    redemption_ = 100.0;
    conversionRatio_ = redemption_/underlying_->value();
}

void teardown() {
    Settings::instance().evaluationDate() = Date();
}

QL_END_TEST_LOCALS(ConvertibleBondTest)


void ConvertibleBondTest::testBond() {

    /* when deeply out-of-the-money, the value of the convertible bond
       should equal that of the underlying plain-vanilla bond. */

    BOOST_MESSAGE(
       "Testing out-of-the-money convertible bonds against vanilla bonds...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    conversionRatio_ = 1.0e-16;

    boost::shared_ptr<Exercise> euExercise(
                                         new EuropeanExercise(maturityDate_));
    boost::shared_ptr<Exercise> amExercise(
                                         new AmericanExercise(issueDate_,
                                                              maturityDate_));

    Size timeSteps = 1001;
    boost::shared_ptr<PricingEngine> engine(
                 new BinomialConvertibleEngine<CoxRossRubinstein>(timeSteps));

    Handle<YieldTermStructure> discountCurve(
         boost::shared_ptr<YieldTermStructure>(
             new ForwardSpreadedTermStructure(riskFreeRate_, creditSpread_)));

    // zero-coupon

    Schedule schedule = MakeSchedule(issueDate_, maturityDate_,
                                     Period(Once), calendar_,
                                     Following).backwards();

    ConvertibleZeroCouponBond euZero(process_, euExercise, engine,
                                     conversionRatio_, no_dividends,
                                     no_callability, creditSpread_,
                                     issueDate_, settlementDays_,
                                     dayCounter_, schedule, redemption_);

    ConvertibleZeroCouponBond amZero(process_, amExercise, engine,
                                     conversionRatio_, no_dividends,
                                     no_callability, creditSpread_,
                                     issueDate_, settlementDays_,
                                     dayCounter_, schedule, redemption_);

    ZeroCouponBond zero(100.0, issueDate_, maturityDate_, settlementDays_,
                        dayCounter_, calendar_, Following,
                        redemption_, discountCurve);

    Real tolerance = 1.0e-2 * (faceAmount_/100.0);

    Real error = std::fabs(euZero.NPV()-zero.NPV());
    if (error > tolerance) {
        BOOST_ERROR("failed to reproduce zero-coupon bond price:"
                    << "\n    calculated: " << euZero.NPV()
                    << "\n    expected:   " << zero.NPV()
                    << "\n    error:      " << error);
    }

    error = std::fabs(amZero.NPV()-zero.NPV());
    if (error > tolerance) {
        BOOST_ERROR("failed to reproduce zero-coupon bond price:"
                    << "\n    calculated: " << amZero.NPV()
                    << "\n    expected:   " << zero.NPV()
                    << "\n    error:      " << error);
    }

    // coupon

    std::vector<Rate> coupons(1, 0.05);

    schedule = MakeSchedule(issueDate_, maturityDate_,
                            Period(frequency_), calendar_,
                            Following).backwards();

    ConvertibleFixedCouponBond euFixed(process_, euExercise, engine,
                                       conversionRatio_, no_dividends,
                                       no_callability, creditSpread_,
                                       issueDate_, settlementDays_,
                                       coupons, dayCounter_,
                                       schedule, redemption_);

    ConvertibleFixedCouponBond amFixed(process_, amExercise, engine,
                                       conversionRatio_, no_dividends,
                                       no_callability, creditSpread_,
                                       issueDate_, settlementDays_,
                                       coupons, dayCounter_,
                                       schedule, redemption_);

    FixedCouponBond fixed(faceAmount_, issueDate_, issueDate_, maturityDate_,
                          settlementDays_, coupons, frequency_,
                          calendar_, dayCounter_, Following, Following,
                          redemption_, discountCurve);

    tolerance = 2.0e-2 * (faceAmount_/100.0);

    error = std::fabs(euFixed.NPV()-fixed.NPV());
    if (error > tolerance) {
        BOOST_ERROR("failed to reproduce fixed-coupon bond price:"
                    << "\n    calculated: " << euFixed.NPV()
                    << "\n    expected:   " << fixed.NPV()
                    << "\n    error:      " << error);
    }

    error = std::fabs(amFixed.NPV()-fixed.NPV());
    if (error > tolerance) {
        BOOST_ERROR("failed to reproduce fixed-coupon bond price:"
                    << "\n    calculated: " << amFixed.NPV()
                    << "\n    expected:   " << fixed.NPV()
                    << "\n    error:      " << error);
    }

    // floating-rate

    boost::shared_ptr<IborIndex> index(new Euribor1Y(discountCurve));
    Integer fixingDays = 2;
    std::vector<Real> gearings(1, 1.0);
    std::vector<Rate> spreads;

    ConvertibleFloatingRateBond euFloating(process_, euExercise, engine,
                                           conversionRatio_, no_dividends,
                                           no_callability, creditSpread_,
                                           issueDate_, settlementDays_,
                                           index, fixingDays, spreads,
                                           dayCounter_, schedule, redemption_);

    ConvertibleFloatingRateBond amFloating(process_, amExercise, engine,
                                           conversionRatio_, no_dividends,
                                           no_callability, creditSpread_,
                                           issueDate_, settlementDays_,
                                           index, fixingDays, spreads,
                                           dayCounter_, schedule, redemption_);

    FloatingRateBond floating(faceAmount_,issueDate_,issueDate_,maturityDate_,
                              settlementDays_, index, fixingDays, gearings,
                              spreads, frequency_, calendar_, dayCounter_,
                              Following, Following, redemption_,
                              discountCurve);

    tolerance = 2.0e-2 * (faceAmount_/100.0);

    error = std::fabs(euFloating.NPV()-floating.NPV());
    if (error > tolerance) {
        BOOST_ERROR("failed to reproduce floating-rate bond price:"
                    << "\n    calculated: " << euFloating.NPV()
                    << "\n    expected:   " << floating.NPV()
                    << "\n    error:      " << error);
    }

    error = std::fabs(amFloating.NPV()-floating.NPV());
    if (error > tolerance) {
        BOOST_ERROR("failed to reproduce floating-rate bond price:"
                    << "\n    calculated: " << amFloating.NPV()
                    << "\n    expected:   " << floating.NPV()
                    << "\n    error:      " << error);
    }

    QL_TEST_TEARDOWN
}

void ConvertibleBondTest::testOption() {

    /* a zero-coupon convertible bond with no credit spread is
       equivalent to a call option. */

    BOOST_MESSAGE(
       "Testing zero-coupon convertible bonds against vanilla option...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    boost::shared_ptr<Exercise> euExercise(new
                                    EuropeanExercise(maturityDate_));

    settlementDays_ = 0;

    Size timeSteps = 1001;
    boost::shared_ptr<PricingEngine> engine(
                 new BinomialConvertibleEngine<CoxRossRubinstein>(timeSteps));
    boost::shared_ptr<PricingEngine> vanillaEngine(
                 new BinomialVanillaEngine<CoxRossRubinstein>(timeSteps));

    creditSpread_.linkTo(boost::shared_ptr<Quote>(new SimpleQuote(0.0)));

    Real conversionStrike = redemption_/conversionRatio_;
    boost::shared_ptr<StrikedTypePayoff> payoff(
                      new PlainVanillaPayoff(Option::Call, conversionStrike));

    Schedule schedule = MakeSchedule(issueDate_, maturityDate_,
                                     Period(Once), calendar_,
                                     Following).backwards();

    ConvertibleZeroCouponBond euZero(process_, euExercise, engine,
                                     conversionRatio_, no_dividends,
                                     no_callability, creditSpread_,
                                     issueDate_, settlementDays_,
                                     dayCounter_, schedule, redemption_);


    VanillaOption euOption(process_, payoff, euExercise, vanillaEngine);

    Real tolerance = 5.0e-2 * (faceAmount_/100.0);

    Real expected = faceAmount_/100.0 *
        (redemption_ * riskFreeRate_->discount(maturityDate_)
         + conversionRatio_* euOption.NPV());
    Real error = std::fabs(euZero.NPV()-expected);
    if (error > tolerance) {
        BOOST_ERROR("failed to reproduce plain-option price:"
                    << "\n    calculated: " << euZero.NPV()
                    << "\n    expected:   " << expected
                    << "\n    error:      " << error);
    }

    QL_TEST_TEARDOWN
}

test_suite* ConvertibleBondTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Convertible bond tests");

    suite->add(BOOST_TEST_CASE(&ConvertibleBondTest::testBond));
    suite->add(BOOST_TEST_CASE(&ConvertibleBondTest::testOption));

    return suite;
}
