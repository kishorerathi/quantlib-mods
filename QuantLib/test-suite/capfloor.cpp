/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 RiskMap srl
 Copyright (C) 2004, 2005, 2006, 2007 StatPro Italia srl

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

#include "capfloor.hpp"
#include "utilities.hpp"
#include <ql/instruments/capfloor.hpp>
#include <ql/instruments/vanillaswap.hpp>
#include <ql/cashflows/cashflowvectors.hpp>
#include <ql/yieldtermstructures/flatforward.hpp>
#include <ql/indexes/ibor/euribor.hpp>
#include <ql/pricingengines/capfloor/blackcapfloorengine.hpp>
#include <ql/pricingengines/swap/discountingswapengine.hpp>
#include <ql/models/marketmodels/models/flatvol.hpp>
#include <ql/models/marketmodels/correlations/expcorrelations.hpp>
#include <ql/math/matrix.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/schedule.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/cashflows/cashflows.hpp>
#include <ql/cashflows/couponpricer.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(CapFloorTest)

struct CommonVars {
    // common data
    Date settlement;
    std::vector<Real> nominals;
    BusinessDayConvention convention;
    Frequency frequency;
    boost::shared_ptr<IborIndex> index;
    Calendar calendar;
    Natural fixingDays;
    RelinkableHandle<YieldTermStructure> termStructure;

    // cleanup

    SavedSettings backup;

    // setup
    CommonVars() {
        nominals = std::vector<Real>(1,100);
        frequency = Semiannual;
        index = boost::shared_ptr<IborIndex>(new Euribor6M(termStructure));
        calendar = index->fixingCalendar();
        convention = ModifiedFollowing;
        Date today = calendar.adjust(Date::todaysDate());
        Settings::instance().evaluationDate() = today;
        Natural settlementDays = 2;
        fixingDays = 2;
        settlement = calendar.advance(today,settlementDays,Days);
        termStructure.linkTo(flatRate(settlement,0.05,
                                      ActualActual(ActualActual::ISDA)));
    }

    // utilities
    Leg makeLeg(const Date& startDate, Integer length) {
        Date endDate = calendar.advance(startDate,length*Years,convention);
        Schedule schedule(startDate, endDate, Period(frequency), calendar,
                          convention, convention, false, false);
        return IborLeg(schedule, index)
            .withNotionals(nominals)
            .withPaymentDayCounter(index->dayCounter())
            .withPaymentAdjustment(convention)
            .withFixingDays(fixingDays);
    }

    boost::shared_ptr<PricingEngine> makeEngine(Volatility volatility) {
        Handle<Quote> vol(boost::shared_ptr<Quote>(
                                                new SimpleQuote(volatility)));
        return boost::shared_ptr<PricingEngine>(
                                new BlackCapFloorEngine(termStructure, vol));
    }

    boost::shared_ptr<CapFloor> makeCapFloor(CapFloor::Type type,
                                             const Leg& leg,
                                             Rate strike,
                                             Volatility volatility) {
        boost::shared_ptr<CapFloor> result;
        switch (type) {
          case CapFloor::Cap:
            result = boost::shared_ptr<CapFloor>(
                new Cap(leg, std::vector<Rate>(1, strike),
                        termStructure));
            break;
          case CapFloor::Floor:
            result = boost::shared_ptr<CapFloor>(
                new Floor(leg, std::vector<Rate>(1, strike),
                          termStructure));
            break;
          default:
            QL_FAIL("unknown cap/floor type");
        }
        result->setPricingEngine(makeEngine(volatility));
        return result;
    }

};

bool checkAbsError(Real x1, Real x2, Real tolerance){
    return std::fabs(x1 - x2) < tolerance;
}

std::string typeToString(CapFloor::Type type) {
    switch (type) {
      case CapFloor::Cap:
        return "cap";
      case CapFloor::Floor:
        return "floor";
      case CapFloor::Collar:
        return "collar";
      default:
        QL_FAIL("unknown cap/floor type");
    }
}

QL_END_TEST_LOCALS(CapFloorTest)


void CapFloorTest::testVega() {

    BOOST_MESSAGE("Testing cap/floor vega...");

    CommonVars vars;

    Integer lengths[] = { 1, 2, 3, 4, 5, 6, 7, 10, 15, 20, 30 };
    Volatility vols[] = { 0.01, 0.05, 0.10, 0.15, 0.20 };
    Rate strikes[] = { 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09 };
    CapFloor::Type types[] = { CapFloor::Cap, CapFloor::Floor};

    Date startDate = vars.termStructure->referenceDate();
    static const Real shift = 1e-8;
    static const Real tolerance = 0.005;

    for (Size i=0; i<LENGTH(lengths); i++) {
        for (Size j=0; j<LENGTH(vols); j++) {
            for (Size k=0; k<LENGTH(strikes); k++) {
                for (Size h=0; h<LENGTH(types); h++) {
                    Leg leg = vars.makeLeg(startDate, lengths[i]);
                    boost::shared_ptr<CapFloor> capFloor =
                        vars.makeCapFloor(types[h],leg,
                                          strikes[k],vols[j]);
                    boost::shared_ptr<CapFloor> shiftedCapFloor2 =
                        vars.makeCapFloor(types[h],leg,
                                          strikes[k],vols[j]+shift);
                     boost::shared_ptr<CapFloor> shiftedCapFloor1 =
                        vars.makeCapFloor(types[h],leg,
                                          strikes[k],vols[j]-shift);
                    Real value1 = shiftedCapFloor1->NPV();
                    Real value2 = shiftedCapFloor2->NPV();
                    Real numericalVega = (value2 - value1) / (2*shift);
                    if (numericalVega>1.0e-4) {
                        Real analyticalVega = capFloor->result<Real>("vega");
                        Real discrepancy =
                            std::fabs(numericalVega - analyticalVega);
                        discrepancy /= numericalVega;
                        if (discrepancy > tolerance)
                            BOOST_FAIL(
                                "failed to compute cap/floor vega:" <<
                                "\n   lengths:     " << lengths[j]*Years <<
                                "\n   strike:      " << io::rate(strikes[k]) <<
                                //"\n   types:       " << types[h] <<
                                QL_FIXED << std::setprecision(12) <<
                                "\n   calculated:  " << analyticalVega <<
                                "\n   expected:    " << numericalVega <<
                                "\n   discrepancy: " << io::rate(discrepancy) <<
                                "\n   tolerance:   " << io::rate(tolerance));
                     }
                }
            }
        }
    }
}

void CapFloorTest::testStrikeDependency() {

    BOOST_MESSAGE("Testing cap/floor dependency on strike...");

    CommonVars vars;

    Integer lengths[] = { 1, 2, 3, 5, 7, 10, 15, 20 };
    Volatility vols[] = { 0.01, 0.05, 0.10, 0.15, 0.20 };
    Rate strikes[] = { 0.03, 0.04, 0.05, 0.06, 0.07 };

    Date startDate = vars.termStructure->referenceDate();

    for (Size i=0; i<LENGTH(lengths); i++) {
        for (Size j=0; j<LENGTH(vols); j++) {
            // store the results for different strikes...
            std::vector<Real> cap_values, floor_values;
            for (Size k=0; k<LENGTH(strikes); k++) {
                Leg leg = vars.makeLeg(startDate,lengths[i]);
                boost::shared_ptr<Instrument> cap =
                    vars.makeCapFloor(CapFloor::Cap,leg,
                                      strikes[k],vols[j]);
                cap_values.push_back(cap->NPV());
                boost::shared_ptr<Instrument> floor =
                    vars.makeCapFloor(CapFloor::Floor,leg,
                                      strikes[k],vols[j]);
                floor_values.push_back(floor->NPV());
            }
            // and check that they go the right way
            std::vector<Real>::iterator it =
                std::adjacent_find(cap_values.begin(),cap_values.end(),
                                   std::less<Real>());
            if (it != cap_values.end()) {
                Size n = it - cap_values.begin();
                BOOST_FAIL(
                    "NPV is increasing with the strike in a cap: \n"
                    << std::setprecision(2)
                    << "    length:     " << lengths[i] << " years\n"
                    << "    volatility: " << io::volatility(vols[j]) << "\n"
                    << "    value:      " << cap_values[n]
                    << " at strike: " << io::rate(strikes[n]) << "\n"
                    << "    value:      " << cap_values[n+1]
                    << " at strike: " << io::rate(strikes[n+1]));
            }
            // same for floors
            it = std::adjacent_find(floor_values.begin(),floor_values.end(),
                                    std::greater<Real>());
            if (it != floor_values.end()) {
                Size n = it - floor_values.begin();
                BOOST_FAIL(
                    "NPV is decreasing with the strike in a floor: \n"
                    << std::setprecision(2)
                    << "    length:     " << lengths[i] << " years\n"
                    << "    volatility: " << io::volatility(vols[j]) << "\n"
                    << "    value:      " << floor_values[n]
                    << " at strike: " << io::rate(strikes[n]) << "\n"
                    << "    value:      " << floor_values[n+1]
                    << " at strike: " << io::rate(strikes[n+1]));
            }
        }
    }
}

void CapFloorTest::testConsistency() {

    BOOST_MESSAGE("Testing consistency between cap, floor and collar...");

    CommonVars vars;

    Integer lengths[] = { 1, 2, 3, 5, 7, 10, 15, 20 };
    Rate cap_rates[] = { 0.03, 0.04, 0.05, 0.06, 0.07 };
    Rate floor_rates[] = { 0.03, 0.04, 0.05, 0.06, 0.07 };
    Volatility vols[] = { 0.01, 0.05, 0.10, 0.15, 0.20 };

    Date startDate = vars.termStructure->referenceDate();

    for (Size i=0; i<LENGTH(lengths); i++) {
      for (Size j=0; j<LENGTH(cap_rates); j++) {
        for (Size k=0; k<LENGTH(floor_rates); k++) {
          for (Size l=0; l<LENGTH(vols); l++) {

              Leg leg = vars.makeLeg(startDate,lengths[i]);
              boost::shared_ptr<Instrument> cap =
                  vars.makeCapFloor(CapFloor::Cap,leg,
                                    cap_rates[j],vols[l]);
              boost::shared_ptr<Instrument> floor =
                  vars.makeCapFloor(CapFloor::Floor,leg,
                                    floor_rates[k],vols[l]);
              Collar collar(leg,std::vector<Rate>(1,cap_rates[j]),
                            std::vector<Rate>(1,floor_rates[k]),
                            vars.termStructure);
              collar.setPricingEngine(vars.makeEngine(vols[l]));

              if (std::fabs((cap->NPV()-floor->NPV())-collar.NPV()) > 1e-10) {
                  BOOST_FAIL(
                    "inconsistency between cap, floor and collar:\n"
                    << "    length:       " << lengths[i] << " years\n"
                    << "    volatility:   " << io::volatility(vols[l]) << "\n"
                    << "    cap value:    " << cap->NPV()
                    << " at strike: " << io::rate(cap_rates[j]) << "\n"
                    << "    floor value:  " << floor->NPV()
                    << " at strike: " << io::rate(floor_rates[k]) << "\n"
                    << "    collar value: " << collar.NPV());
              }
          }
        }
      }
    }
}

void CapFloorTest::testParity() {

    BOOST_MESSAGE("Testing cap/floor parity...");

    CommonVars vars;

    Integer lengths[] = { 1, 2, 3, 5, 7, 10, 15, 20 };
    Rate strikes[] = { 0., 0.03, 0.04, 0.05, 0.06, 0.07 };
    Volatility vols[] = { 0.01, 0.05, 0.10, 0.15, 0.20 };

    Date startDate = vars.termStructure->referenceDate();

    for (Size i=0; i<LENGTH(lengths); i++) {
      for (Size j=0; j<LENGTH(strikes); j++) {
        for (Size k=0; k<LENGTH(vols); k++) {

            Leg leg = vars.makeLeg(startDate,lengths[i]);
            boost::shared_ptr<Instrument> cap =
                vars.makeCapFloor(CapFloor::Cap,leg,
                                  strikes[j],vols[k]);
            boost::shared_ptr<Instrument> floor =
                vars.makeCapFloor(CapFloor::Floor,leg,
                                  strikes[j],vols[k]);
            Date maturity = vars.calendar.advance(startDate,lengths[i],Years,
                                              vars.convention);
            Schedule schedule(startDate,maturity,
                              Period(vars.frequency),vars.calendar,
                              vars.convention,vars.convention,false,false);
            VanillaSwap swap(VanillaSwap::Payer, vars.nominals[0],
                             schedule, strikes[j], vars.index->dayCounter(),
                             schedule, vars.index, 0.0,
                             vars.index->dayCounter());
            swap.setPricingEngine(boost::shared_ptr<PricingEngine>(
                              new DiscountingSwapEngine(vars.termStructure)));
            // FLOATING_POINT_EXCEPTION
            if (std::fabs((cap->NPV()-floor->NPV()) - swap.NPV()) > 1.0e-10) {
                BOOST_FAIL(
                    "put/call parity violated:\n"
                    << "    length:      " << lengths[i] << " years\n"
                    << "    volatility:  " << io::volatility(vols[k]) << "\n"
                    << "    strike:      " << io::rate(strikes[j]) << "\n"
                    << "    cap value:   " << cap->NPV() << "\n"
                    << "    floor value: " << floor->NPV() << "\n"
                    << "    swap value:  " << swap.NPV());
            }
        }
      }
    }
}

void CapFloorTest::testATMRate() {

    BOOST_MESSAGE("Testing ATM rate...");

    CommonVars vars;

    Integer lengths[] = { 1, 2, 3, 5, 7, 10, 15, 20 };
    Rate strikes[] = { 0., 0.03, 0.04, 0.05, 0.06, 0.07 };
    Volatility vols[] = { 0.01, 0.05, 0.10, 0.15, 0.20 };

    Date startDate = vars.termStructure->referenceDate();

    for (Size i=0; i<LENGTH(lengths); i++) {
        Leg leg = vars.makeLeg(startDate,lengths[i]);
        Date maturity = vars.calendar.advance(startDate,lengths[i],Years,
                                  vars.convention);
        Schedule schedule(startDate,maturity,
                          Period(vars.frequency),vars.calendar,
                          vars.convention,vars.convention,false,false);

        for (Size j=0; j<LENGTH(strikes); j++) {
            for (Size k=0; k<LENGTH(vols); k++) {
                boost::shared_ptr<CapFloor> cap =
                    vars.makeCapFloor(CapFloor::Cap, leg, strikes[j],vols[k]);
                boost::shared_ptr<CapFloor> floor =
                    vars.makeCapFloor(CapFloor::Floor, leg, strikes[j],vols[k]);
                Rate capATMRate = cap->atmRate();
                Rate floorATMRate = floor->atmRate();
                if (!checkAbsError(floorATMRate, capATMRate, 1.0e-10))
                    BOOST_FAIL(
                      "Cap ATM Rate and floor ATM Rate should be equal :\n"
                      << "   length:        " << lengths[i] << " years\n"
                      << "   volatility:    " << io::volatility(vols[k]) << "\n"
                      << "   strike:        " << io::rate(strikes[j]) << "\n"
                      << "   cap ATM rate:  " << capATMRate << "\n"
                      << "   floor ATM rate:" << floorATMRate << "\n"
                      << "   relative Error:"
                      << relativeError(capATMRate, floorATMRate,
                                       capATMRate)*100 << "%" );
                VanillaSwap swap(VanillaSwap::Payer, vars.nominals[0],
                                 schedule, floorATMRate,
                                 vars.index->dayCounter(),
                                 schedule, vars.index, 0.0,
                                 vars.index->dayCounter());
                swap.setPricingEngine(boost::shared_ptr<PricingEngine>(
                              new DiscountingSwapEngine(vars.termStructure)));
                Real swapNPV = swap.NPV();
                if (!checkAbsError(swapNPV, 0, 1.0e-10))
                    BOOST_FAIL(
                      "the NPV of a Swap struck at ATM rate "
                      "should be equal to 0:\n"
                      << "   length:        " << lengths[i] << " years\n"
                      << "   volatility:    " << io::volatility(vols[k]) << "\n"
                      << "   ATM rate:      " << io::rate(floorATMRate) << "\n"
                      << "   swap NPV:      " << swapNPV);
        }
      }
    }
}


void CapFloorTest::testImpliedVolatility() {

    BOOST_MESSAGE("Testing implied term volatility for cap and floor...");

    CommonVars vars;

    Size maxEvaluations = 100;
    Real tolerance = 1.0e-6;

    CapFloor::Type types[] = { CapFloor::Cap, CapFloor::Floor };
    Rate strikes[] = { 0.02, 0.03, 0.04 };
    Integer lengths[] = { 1, 5, 10 };

    // test data
    Rate rRates[] = { 0.02, 0.03, 0.04 };
    Volatility vols[] = { 0.01, 0.20, 0.30, 0.70, 0.90 };

    for (Size k=0; k<LENGTH(lengths); k++) {
        Leg leg = vars.makeLeg(vars.settlement, lengths[k]);

        for (Size i=0; i<LENGTH(types); i++) {
            for (Size j=0; j<LENGTH(strikes); j++) {

                boost::shared_ptr<CapFloor> capfloor =
                    vars.makeCapFloor(types[i], leg, strikes[j], 0.0);

                for (Size n=0; n<LENGTH(rRates); n++) {
                    for (Size m=0; m<LENGTH(vols); m++) {

                        Rate r = rRates[n];
                        Volatility v = vols[m];
                        vars.termStructure.linkTo(flatRate(vars.settlement,r,
                                                       Actual360()));
                        capfloor->setPricingEngine(vars.makeEngine(v));

                        Real value = capfloor->NPV();
                        Volatility implVol = 0.0;

                        try {
                            implVol =
                                capfloor->impliedVolatility(value,
                                                            tolerance,
                                                            maxEvaluations);
                        } catch (std::exception& e) {
                            BOOST_FAIL(typeToString(types[i]) <<
                                "\n  strike:     " << io::rate(strikes[j]) <<
                                "\n  risk-free:  " << io::rate(r) <<
                                "\n  length:     " << lengths[k] << "Y" <<
                                "\n  volatility: " << io::volatility(v) <<
                                "\n" << e.what());
                        }
                        if (std::fabs(implVol-v) > tolerance) {
                            // the difference might not matter
                            capfloor->setPricingEngine(
                                                    vars.makeEngine(implVol));
                            Real value2 = capfloor->NPV();
                            if (std::fabs(value-value2) > tolerance) {
                                BOOST_FAIL(
                                    typeToString(types[i]) << ":\n"
                                    << "    strike:           "
                                    << strikes[j] << "\n"
                                    << "    risk-free rate:   "
                                    << io::rate(r) << "\n"
                                    << "    length:         "
                                    << lengths[k] << " years\n\n"
                                    << "    original volatility: "
                                    << io::volatility(v) << "\n"
                                    << "    price:               "
                                    << value << "\n"
                                    << "    implied volatility:  "
                                    << io::volatility(implVol) << "\n"
                                    << "    corresponding price: " << value2);
                            }
                        }
                    }
                }
            }
        }
    }
}

void CapFloorTest::testCachedValue() {

    BOOST_MESSAGE("Testing Black cap/floor price against cached values...");

    CommonVars vars;

    Date cachedToday(14,March,2002),
         cachedSettlement(18,March,2002);
    Settings::instance().evaluationDate() = cachedToday;
    vars.termStructure.linkTo(flatRate(cachedSettlement, 0.05, Actual360()));
    Date startDate = vars.termStructure->referenceDate();
    Leg leg = vars.makeLeg(startDate,20);
    boost::shared_ptr<Instrument> cap = vars.makeCapFloor(CapFloor::Cap,leg,
                                                          0.07,0.20);
    boost::shared_ptr<Instrument> floor = vars.makeCapFloor(CapFloor::Floor,leg,
                                                            0.03,0.20);
#ifndef QL_USE_INDEXED_COUPON
    // par coupon price
    Real cachedCapNPV   = 6.87570026732,
         cachedFloorNPV = 2.65812927959;
#else
    // index fixing price
    Real cachedCapNPV   = 6.87630307745,
         cachedFloorNPV = 2.65796764715;
#endif
    // test Black cap price against cached value
    if (std::fabs(cap->NPV()-cachedCapNPV) > 1.0e-11)
        BOOST_ERROR(
            "failed to reproduce cached cap value:\n"
            << std::setprecision(12)
            << "    calculated: " << cap->NPV() << "\n"
            << "    expected:   " << cachedCapNPV);
    // test Black floor price against cached value
    if (std::fabs(floor->NPV()-cachedFloorNPV) > 1.0e-11)
        BOOST_ERROR(
            "failed to reproduce cached floor value:\n"
            << std::setprecision(12)
            << "    calculated: " << floor->NPV() << "\n"
            << "    expected:   " << cachedFloorNPV);
}


test_suite* CapFloorTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Cap/floor tests");
    suite->add(BOOST_TEST_CASE(&CapFloorTest::testStrikeDependency));
    suite->add(BOOST_TEST_CASE(&CapFloorTest::testConsistency));
    // FLOATING_POINT_EXCEPTION
    suite->add(BOOST_TEST_CASE(&CapFloorTest::testParity));
    suite->add(BOOST_TEST_CASE(&CapFloorTest::testVega));
    suite->add(BOOST_TEST_CASE(&CapFloorTest::testATMRate));
    suite->add(BOOST_TEST_CASE(&CapFloorTest::testImpliedVolatility));
    suite->add(BOOST_TEST_CASE(&CapFloorTest::testCachedValue));
    return suite;
}

