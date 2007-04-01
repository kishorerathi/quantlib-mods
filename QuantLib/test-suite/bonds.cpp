/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2004, 2005 StatPro Italia srl

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

#include "bonds.hpp"
#include "utilities.hpp"
#include <ql/instruments/fixedratebond.hpp>
#include <ql/instruments/floatingratebond.hpp>
#include <ql/instruments/zerocouponbond.hpp>
#include <ql/calendars/target.hpp>
#include <ql/calendars/unitedstates.hpp>
#include <ql/calendars/nullcalendar.hpp>
#include <ql/daycounters/thirty360.hpp>
#include <ql/daycounters/actual360.hpp>
#include <ql/daycounters/actualactual.hpp>
#include <ql/indexes/usdlibor.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/schedule.hpp>
#include <ql/cashflows/couponpricer.hpp>
#include <ql/cashflows/analysis.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(BondTest)

Calendar calendar;
Date today;
Real faceAmount = 1000000.0;

void setup() {
    calendar = TARGET();
    today = calendar.adjust(Date::todaysDate());
    Settings::instance().evaluationDate() = today;
}

void teardown() {
    Settings::instance().evaluationDate() = Date();
}

QL_END_TEST_LOCALS(BondTest)


void BondTest::testYield() {

    BOOST_MESSAGE("Testing consistency of bond price/yield calculation...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Real tolerance = 1.0e-7;
    Size maxEvaluations = 100;

    Integer issueMonths[] = { -24, -18, -12, -6, 0, 6, 12, 18, 24 };
    Integer lengths[] = { 3, 5, 10, 15, 20 };
    Natural settlementDays = 3;
    Real coupons[] = { 0.02, 0.05, 0.08 };
    Frequency frequencies[] = { Semiannual, Annual };
    DayCounter bondDayCount = Thirty360();
    BusinessDayConvention accrualConvention = Unadjusted;
    BusinessDayConvention paymentConvention = ModifiedFollowing;
    Real redemption = 100.0;

    Rate yields[] = { 0.03, 0.04, 0.05, 0.06, 0.07 };
    Compounding compounding[] = { Compounded, Continuous };

    for (Size i=0; i<LENGTH(issueMonths); i++) {
      for (Size j=0; j<LENGTH(lengths); j++) {
        for (Size k=0; k<LENGTH(coupons); k++) {
          for (Size l=0; l<LENGTH(frequencies); l++) {
            for (Size n=0; n<LENGTH(compounding); n++) {

              Date dated = calendar.advance(today, issueMonths[i], Months);
              Date issue = dated;
              Date maturity = calendar.advance(issue, lengths[j], Years);

              Schedule sch(dated, maturity, Period(frequencies[l]), calendar,
                           accrualConvention, accrualConvention, true,false);

              FixedRateBond bond(settlementDays, faceAmount, sch,
                                 std::vector<Rate>(1, coupons[k]),
                                 bondDayCount, paymentConvention,
                                 redemption, issue);

              for (Size m=0; m<LENGTH(yields); m++) {

                Real price = bond.cleanPrice(yields[m],compounding[n]);
                Rate calculated = bond.yield(price, compounding[n], Date(),
                                             tolerance, maxEvaluations);

                if (std::fabs(yields[m]-calculated) > tolerance) {
                  // the difference might not matter
                  Real price2 = bond.cleanPrice(calculated,compounding[n]);
                  if (std::fabs(price-price2)/price > tolerance) {
                      BOOST_ERROR(
                          "yield recalculation failed:\n"
                          << "    issue:     " << issue << "\n"
                          << "    maturity:  " << maturity << "\n"
                          << "    coupon:    " << io::rate(coupons[k]) << "\n"
                          << "    frequency: " << frequencies[l] << "\n\n"
                          << "    yield:  " << io::rate(yields[m]) << " "
                          << (compounding[n] == Compounded ?
                              "compounded" : "continuous") << "\n"
                          << std::setprecision(7)
                          << "    price:  " << price << "\n"
                          << "    yield': " << io::rate(calculated) << "\n"
                          << "    price': " << price2);
                  }
                }
              }
            }
          }
        }
      }
    }

    QL_TEST_TEARDOWN
}



void BondTest::testTheoretical() {

    BOOST_MESSAGE("Testing theoretical bond price/yield calculation...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Real tolerance = 1.0e-7;
    Size maxEvaluations = 100;

    Size lengths[] = { 3, 5, 10, 15, 20 };
    Natural settlementDays = 3;
    Real coupons[] = { 0.02, 0.05, 0.08 };
    Frequency frequencies[] = { Semiannual, Annual };
    DayCounter bondDayCount = Actual360();
    BusinessDayConvention accrualConvention = Unadjusted;
    BusinessDayConvention paymentConvention = ModifiedFollowing;
    Real redemption = 100.0;

    Rate yields[] = { 0.03, 0.04, 0.05, 0.06, 0.07 };

    for (Size j=0; j<LENGTH(lengths); j++) {
      for (Size k=0; k<LENGTH(coupons); k++) {
        for (Size l=0; l<LENGTH(frequencies); l++) {

            Date dated = today;
            Date issue = dated;
            Date maturity = calendar.advance(issue, lengths[j], Years);

            boost::shared_ptr<SimpleQuote> rate(new SimpleQuote(0.0));
            Handle<YieldTermStructure> discountCurve(flatRate(today,rate,
                                                              bondDayCount));

            Schedule sch(dated, maturity, Period(frequencies[l]), calendar,
                         accrualConvention, accrualConvention, true,false);

            FixedRateBond bond(settlementDays, faceAmount, sch,
                               std::vector<Rate>(1, coupons[k]),
                               bondDayCount, paymentConvention,
                               redemption, issue, discountCurve);

            for (Size m=0; m<LENGTH(yields); m++) {

                rate->setValue(yields[m]);

                Real price = bond.cleanPrice(yields[m],Continuous);
                Real calculatedPrice = bond.cleanPrice();
                Rate calculatedYield = bond.yield(Continuous, tolerance,
                                                  maxEvaluations);

                if (std::fabs(price-calculatedPrice) > tolerance) {
                    BOOST_ERROR(
                        "price calculation failed:"
                        << "\n    issue:     " << issue
                        << "\n    maturity:  " << maturity
                        << "\n    coupon:    " << io::rate(coupons[k])
                        << "\n    frequency: " << frequencies[l] << "\n"
                        << "\n    yield:  " << io::rate(yields[m])
                        << std::setprecision(7)
                        << "\n    expected:    " << price
                        << "\n    calculated': " << calculatedPrice
                        << "\n    error':      " << price-calculatedPrice);
                }

                if (std::fabs(yields[m]-calculatedYield) > tolerance) {
                    BOOST_ERROR(
                        "yield calculation failed:"
                        << "\n    issue:     " << issue
                        << "\n    maturity:  " << maturity
                        << "\n    coupon:    " << io::rate(coupons[k])
                        << "\n    frequency: " << frequencies[l] << "\n"
                        << "\n    yield:  " << io::rate(yields[m])
                        << std::setprecision(7)
                        << "\n    price:  " << price
                        << "\n    yield': " << io::rate(calculatedYield));
                }
            }
        }
      }
    }

    QL_TEST_TEARDOWN
}


void BondTest::testCached() {

    BOOST_MESSAGE(
        "Testing bond price/yield calculation against cached values...");

    QL_TEST_BEGIN

    // with implicit settlement calculation:

    Date today(22,November,2004);
    Settings::instance().evaluationDate() = today;

    Calendar bondCalendar = NullCalendar();
    DayCounter bondDayCount = ActualActual(ActualActual::ISMA);
    Natural settlementDays = 1;

    Handle<YieldTermStructure> discountCurve(flatRate(today,0.03,Actual360()));

    // actual market values from the evaluation date

    Schedule sch1(Date(31,October,2004),
                  Date(31,October,2006), Period(Semiannual), bondCalendar,
                  Unadjusted, Unadjusted, true,false);

    FixedRateBond bond1(settlementDays, faceAmount, sch1,
                        std::vector<Rate>(1, 0.025),
                        bondDayCount, ModifiedFollowing,
                        100.0, Date(1,November,2004), discountCurve);

    Real marketPrice1 = 99.203125;
    Rate marketYield1 = 0.02925;

    Schedule sch2(Date(15,November,2004),
                  Date(15,November,2009), Period(Semiannual), bondCalendar,
                  Unadjusted, Unadjusted, true,false);

    FixedRateBond bond2(settlementDays, faceAmount, sch2,
                        std::vector<Rate>(1, 0.035),
                        bondDayCount, ModifiedFollowing,
                        100.0, Date(15,November,2004), discountCurve);

    Real marketPrice2 = 99.6875;
    Rate marketYield2 = 0.03569;

    // calculated values

    Real cachedPrice1a = 99.204505, cachedPrice2a = 99.687192;
    Real cachedPrice1b = 98.943393, cachedPrice2b = 101.986794;
    Rate cachedYield1a = 0.029257, cachedYield2a = 0.035689;
    Rate cachedYield1b = 0.029045, cachedYield2b = 0.035375;
    Rate cachedYield1c = 0.030423, cachedYield2c = 0.030432;

    // check
    Real tolerance = 1.0e-6;
    Real price, yield;

    price = bond1.cleanPrice(marketYield1, Compounded);
    if (std::fabs(price-cachedPrice1a) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice1a << "\n"
                    << "    error:      " << price-cachedPrice1a);
    }

    price = bond1.cleanPrice();
    if (std::fabs(price-cachedPrice1b) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice1b << "\n"
                    << "    error:      " << price-cachedPrice1b);
    }

    yield = bond1.yield(marketPrice1, Compounded);
    if (std::fabs(yield-cachedYield1a) > tolerance) {
        BOOST_ERROR("failed to reproduce cached compounded yield:\n"
                    << std::setprecision(4)
                    << "    calculated: " << io::rate(yield) << "\n"
                    << "    expected:   " << io::rate(cachedYield1a) << "\n"
                    << "    error:      " << io::rate(yield-cachedYield1a));
    }

    yield = bond1.yield(marketPrice1, Continuous);
    if (std::fabs(yield-cachedYield1b) > tolerance) {
        BOOST_ERROR("failed to reproduce cached continuous yield:\n"
                    << std::setprecision(4)
                    << "    calculated: " << io::rate(yield) << "\n"
                    << "    expected:   " << io::rate(cachedYield1b) << "\n"
                    << "    error:      " << io::rate(yield-cachedYield1b));
    }

    yield = bond1.yield(Continuous);
    if (std::fabs(yield-cachedYield1c) > tolerance) {
        BOOST_ERROR("failed to reproduce cached continuous yield:\n"
                    << std::setprecision(4)
                    << "    calculated: " << io::rate(yield) << "\n"
                    << "    expected:   " << io::rate(cachedYield1c) << "\n"
                    << "    error:      " << io::rate(yield-cachedYield1c));
    }


    price = bond2.cleanPrice(marketYield2, Compounded);
    if (std::fabs(price-cachedPrice2a) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice2a << "\n"
                    << "    error:      " << price-cachedPrice2a);
    }

    price = bond2.cleanPrice();
    if (std::fabs(price-cachedPrice2b) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice2b << "\n"
                    << "    error:      " << price-cachedPrice2b);
    }

    yield = bond2.yield(marketPrice2, Compounded);
    if (std::fabs(yield-cachedYield2a) > tolerance) {
        BOOST_ERROR("failed to reproduce cached compounded yield:\n"
                    << std::setprecision(4)
                    << "    calculated: " << io::rate(yield) << "\n"
                    << "    expected:   " << io::rate(cachedYield2a) << "\n"
                    << "    error:      " << io::rate(yield-cachedYield2a));
    }

    yield = bond2.yield(marketPrice2, Continuous);
    if (std::fabs(yield-cachedYield2b) > tolerance) {
        BOOST_ERROR("failed to reproduce cached continuous yield:\n"
                    << std::setprecision(4)
                    << "    calculated: " << io::rate(yield) << "\n"
                    << "    expected:   " << io::rate(cachedYield2b) << "\n"
                    << "    error:      " << io::rate(yield-cachedYield2b));
    }

    yield = bond2.yield(Continuous);
    if (std::fabs(yield-cachedYield2c) > tolerance) {
        BOOST_ERROR("failed to reproduce cached continuous yield:\n"
                    << std::setprecision(4)
                    << "    calculated: " << io::rate(yield) << "\n"
                    << "    expected:   " << io::rate(cachedYield2c) << "\n"
                    << "    error:      " << io::rate(yield-cachedYield2c));
    }

    // with explicit settlement date:


    Schedule sch3(Date(30,November,2004),
                  Date(30,November,2006), Period(Semiannual),
                  UnitedStates(UnitedStates::GovernmentBond),
                  Unadjusted, Unadjusted, true,false);

    FixedRateBond bond3(settlementDays, faceAmount, sch3,
                        std::vector<Rate>(1, 0.02875),
                        ActualActual(ActualActual::ISMA),
                        ModifiedFollowing,
                        100.0, Date(30,November,2004), discountCurve);

    Rate marketYield3 = 0.02997;

    Date settlementDate = Date(30,November,2004);
    Real cachedPrice3 = 99.764874;

    price = bond3.cleanPrice(marketYield3, Compounded, settlementDate);
    if (std::fabs(price-cachedPrice3) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice3 << "\n"
                    << "    error:      " << price-cachedPrice3);
    }

    // this should give the same result since the issue date is the
    // earliest possible settlement date

    Settings::instance().evaluationDate() = Date(22,November,2004);

    price = bond3.cleanPrice(marketYield3, Compounded);
    if (std::fabs(price-cachedPrice3) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice3 << "\n"
                    << "    error:      " << price-cachedPrice3);
    }

    QL_TEST_TEARDOWN
}



void BondTest::testCachedZero() {

    BOOST_MESSAGE(
        "Testing zero-coupon bond prices against cached values...");

    QL_TEST_BEGIN

    Date today(22,November,2004);
    Settings::instance().evaluationDate() = today;

    Natural settlementDays = 1;

    Handle<YieldTermStructure> discountCurve(flatRate(today,0.03,Actual360()));

    Real tolerance = 1.0e-6;

    // plain

    ZeroCouponBond bond1(settlementDays, faceAmount,
                         UnitedStates(UnitedStates::GovernmentBond),
                         Date(30,November,2008),
                         ActualActual(ActualActual::ISMA),
                         ModifiedFollowing,
                         100.0, Date(30,November,2004), discountCurve);

    Real cachedPrice1 = 88.551726;

    Real price = bond1.cleanPrice();
    if (std::fabs(price-cachedPrice1) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice1 << "\n"
                    << "    error:      " << price-cachedPrice1);
    }

    ZeroCouponBond bond2(settlementDays, faceAmount,
                         UnitedStates(UnitedStates::GovernmentBond),
                         Date(30,November,2007),
                         ActualActual(ActualActual::ISMA),
                         ModifiedFollowing,
                         100.0, Date(30,November,2004), discountCurve);

    Real cachedPrice2 = 91.278949;

    price = bond2.cleanPrice();
    if (std::fabs(price-cachedPrice2) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice2 << "\n"
                    << "    error:      " << price-cachedPrice2);
    }

    ZeroCouponBond bond3(settlementDays, faceAmount,
                         UnitedStates(UnitedStates::GovernmentBond),
                         Date(30,November,2006),
                         ActualActual(ActualActual::ISMA),
                         ModifiedFollowing,
                         100.0, Date(30,November,2004), discountCurve);

    Real cachedPrice3 = 94.098006;

    price = bond3.cleanPrice();
    if (std::fabs(price-cachedPrice3) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice3 << "\n"
                    << "    error:      " << price-cachedPrice3);
    }

    QL_TEST_TEARDOWN
}


void BondTest::testCachedFixed() {

    BOOST_MESSAGE(
        "Testing fixed-coupon bond prices against cached values...");

    QL_TEST_BEGIN

    Date today(22,November,2004);
    Settings::instance().evaluationDate() = today;

    Natural settlementDays = 1;

    Handle<YieldTermStructure> discountCurve(flatRate(today,0.03,Actual360()));

    Real tolerance = 1.0e-6;

    // plain

    Schedule sch(Date(30,November,2004),
                 Date(30,November,2008), Period(Semiannual),
                 UnitedStates(UnitedStates::GovernmentBond),
                 Unadjusted, Unadjusted, true,false);

    FixedRateBond bond1(settlementDays, faceAmount, sch,
                          std::vector<Rate>(1, 0.02875),
                          ActualActual(ActualActual::ISMA),
                          ModifiedFollowing,
                          100.0, Date(30,November,2004), discountCurve);

    Real cachedPrice1 = 99.298100;

    Real price = bond1.cleanPrice();
    if (std::fabs(price-cachedPrice1) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice1 << "\n"
                    << "    error:      " << price-cachedPrice1);
    }

    // varying coupons

    std::vector<Rate> couponRates(4);
    couponRates[0] = 0.02875;
    couponRates[1] = 0.03;
    couponRates[2] = 0.03125;
    couponRates[3] = 0.0325;

    FixedRateBond bond2(settlementDays, faceAmount, sch,
                          couponRates,
                          ActualActual(ActualActual::ISMA),
                          ModifiedFollowing,
                          100.0, Date(30,November,2004), discountCurve);

    Real cachedPrice2 = 100.334149;

    price = bond2.cleanPrice();
    if (std::fabs(price-cachedPrice2) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice2 << "\n"
                    << "    error:      " << price-cachedPrice2);
    }

    // stub date

    Schedule sch3(Date(30,November,2004),
                  Date(30,March,2009), Period(Semiannual),
                  UnitedStates(UnitedStates::GovernmentBond),
                  Unadjusted, Unadjusted, true, false,
                  Date(), Date(30,November,2008));

    FixedRateBond bond3(settlementDays, faceAmount, sch3,
                          couponRates, ActualActual(ActualActual::ISMA),
                          ModifiedFollowing,
                          100.0, Date(30,November,2004), discountCurve);

    Real cachedPrice3 = 100.382794;

    price = bond3.cleanPrice();
    if (std::fabs(price-cachedPrice3) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice3 << "\n"
                    << "    error:      " << price-cachedPrice3);
    }

    QL_TEST_TEARDOWN
}


void BondTest::testCachedFloating() {

    BOOST_MESSAGE(
        "Testing floating-rate bond prices against cached values...");

    QL_TEST_BEGIN

    Date today(22,November,2004);
    Settings::instance().evaluationDate() = today;

    Natural settlementDays = 1;

    Handle<YieldTermStructure> riskFreeRate(flatRate(today,0.025,Actual360()));
    Handle<YieldTermStructure> discountCurve(flatRate(today,0.03,Actual360()));

    boost::shared_ptr<IborIndex> index(new USDLibor(6*Months, riskFreeRate));
    Natural fixingDays = 1;

    Real tolerance = 1.0e-6;

    boost::shared_ptr<IborCouponPricer> pricer(new
        BlackIborCouponPricer(Handle<CapletVolatilityStructure>()));

    // plain

    Schedule sch(Date(30,November,2004),
                 Date(30,November,2008),
                 Period(Semiannual),
                 UnitedStates(UnitedStates::GovernmentBond),
                 ModifiedFollowing, ModifiedFollowing,
                 true, false);

    FloatingRateBond bond1(settlementDays, faceAmount, sch,
                             index, ActualActual(ActualActual::ISMA),
                             ModifiedFollowing, fixingDays,
                             std::vector<Real>(), std::vector<Spread>(),
                             std::vector<Rate>(), std::vector<Rate>(),
                             false,
                             100.0, Date(30,November,2004), riskFreeRate);
    CashFlows::setPricer(bond1.cashflows(),pricer);

    #if defined(QL_USE_INDEXED_COUPON)
    Real cachedPrice1 = 99.874645;
    #else
    Real cachedPrice1 = 100.013038;
    #endif


    Real price = bond1.cleanPrice();
    if (std::fabs(price-cachedPrice1) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice1 << "\n"
                    << "    error:      " << price-cachedPrice1);
    }

    // different risk-free and discount curve

    FloatingRateBond bond2(settlementDays, faceAmount, sch,
                           index, ActualActual(ActualActual::ISMA),
                           ModifiedFollowing, fixingDays,
                           std::vector<Rate>(), std::vector<Spread>(),
                           std::vector<Rate>(), std::vector<Rate>(),
                           false,
                           100.0, Date(30,November,2004), discountCurve);
    CashFlows::setPricer(bond2.cashflows(),pricer);

    #if defined(QL_USE_INDEXED_COUPON)
    Real cachedPrice2 = 97.955904;
    #else
    Real cachedPrice2 = 98.092692;
    #endif

    price = bond2.cleanPrice();
    if (std::fabs(price-cachedPrice2) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice2 << "\n"
                    << "    error:      " << price-cachedPrice2);
    }

    // varying spread

    std::vector<Rate> spreads(4);
    spreads[0] = 0.001;
    spreads[1] = 0.0012;
    spreads[2] = 0.0014;
    spreads[3] = 0.0016;

    FloatingRateBond bond3(settlementDays, faceAmount, sch,
                           index, ActualActual(ActualActual::ISMA),
                           ModifiedFollowing, fixingDays,
                           std::vector<Real>(), spreads,
                           std::vector<Rate>(), std::vector<Rate>(),
                           false,
                           100.0, Date(30,November,2004), discountCurve);
    CashFlows::setPricer(bond3.cashflows(),pricer);

    #if defined(QL_USE_INDEXED_COUPON)
    Real cachedPrice3 = 98.495458;
    #else
    Real cachedPrice3 = 98.632247;
    #endif

    price = bond3.cleanPrice();
    if (std::fabs(price-cachedPrice3) > tolerance) {
        BOOST_ERROR("failed to reproduce cached price:\n"
                    << QL_FIXED
                    << "    calculated: " << price << "\n"
                    << "    expected:   " << cachedPrice3 << "\n"
                    << "    error:      " << price-cachedPrice3);
    }

    QL_TEST_TEARDOWN
}


test_suite* BondTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Bond tests");

    suite->add(BOOST_TEST_CASE(&BondTest::testYield));
    suite->add(BOOST_TEST_CASE(&BondTest::testTheoretical));
    suite->add(BOOST_TEST_CASE(&BondTest::testCached));
    suite->add(BOOST_TEST_CASE(&BondTest::testCachedZero));
    suite->add(BOOST_TEST_CASE(&BondTest::testCachedFixed));
    suite->add(BOOST_TEST_CASE(&BondTest::testCachedFloating));
    return suite;
}

