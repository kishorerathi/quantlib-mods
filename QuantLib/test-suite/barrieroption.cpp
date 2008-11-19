/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Neil Firth
 Copyright (C) 2003, 2004, 2005, 2007, 2008 StatPro Italia srl
 Copyright (C) 2004 Ferdinando Ametrano

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

#include "barrieroption.hpp"
#include "utilities.hpp"
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/instruments/barrieroption.hpp>
#include <ql/pricingengines/barrier/analyticbarrierengine.hpp>
#include <ql/experimental/finitedifferences/fdblackscholesbarrierengine.hpp>
#include <ql/experimental/barrieroption/perturbativebarrieroptionengine.hpp>
#include <ql/pricingengines/barrier/mcbarrierengine.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>
#include <ql/utilities/dataformatters.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

#define REPORT_FAILURE(greekName, barrierType, barrier, rebate, payoff, \
                       exercise, s, q, r, today, v, expected, calculated, \
                       error, tolerance) \
    BOOST_ERROR("\n" << barrierTypeToString(barrierType) << " " \
               << exerciseTypeToString(exercise) << " " \
               << payoff->optionType() << " option with " \
               << payoffTypeToString(payoff) << " payoff:\n" \
               << "    underlying value: " << s << "\n" \
               << "    strike:           " << payoff->strike() << "\n" \
               << "    barrier:          " << barrier << "\n" \
               << "    rebate:           " << rebate << "\n" \
               << "    dividend yield:   " << io::rate(q) << "\n" \
               << "    risk-free rate:   " << io::rate(r) << "\n" \
               << "    reference date:   " << today << "\n" \
               << "    maturity:         " << exercise->lastDate() << "\n" \
               << "    volatility:       " << io::volatility(v) << "\n\n" \
               << "    expected   " << greekName << ": " << expected << "\n" \
               << "    calculated " << greekName << ": " << calculated << "\n"\
               << "    error:            " << error << "\n" \
               << "    tolerance:        " << tolerance);

namespace {

    std::string barrierTypeToString(Barrier::Type type) {
        switch(type){
          case Barrier::DownIn:
            return std::string("Down-and-in");
          case Barrier::UpIn:
            return std::string("Up-and-in");
          case Barrier::DownOut:
            return std::string("Down-and-out");
          case Barrier::UpOut:
            return std::string("Up-and-out");
          default:
            QL_FAIL("unknown exercise type");
        }
    }

    struct BarrierOptionData {
        Barrier::Type type;
        Volatility volatility;
        Real strike;
        Real barrier;
        Real callValue;
        Real putValue;
    };

    struct NewBarrierOptionData {
        Barrier::Type barrierType;
        Real barrier;
        Real rebate;
        Option::Type type;
        Real strike;
        Real s;        // spot
        Rate q;        // dividend
        Rate r;        // risk-free rate
        Time t;        // time to maturity
        Volatility v;  // volatility
        Real result;   // result
        Real tol;      // tolerance
    };

}


void BarrierOptionTest::testHaugValues() {

    BOOST_MESSAGE("Testing barrier options against Haug's values...");

    NewBarrierOptionData values[] = {
        /* The data below are from
          "Option pricing formulas", E.G. Haug, McGraw-Hill 1998 pag. 72
        */
        //     barrierType, barrier, rebate,         type, strike,     s,    q,    r,    t,    v,  result, tol
        { Barrier::DownOut,    95.0,    3.0, Option::Call,     90, 100.0, 0.04, 0.08, 0.50, 0.25,  9.0246, 1.0e-4},
        { Barrier::DownOut,    95.0,    3.0, Option::Call,    100, 100.0, 0.04, 0.08, 0.50, 0.25,  6.7924, 1.0e-4},
        { Barrier::DownOut,    95.0,    3.0, Option::Call,    110, 100.0, 0.04, 0.08, 0.50, 0.25,  4.8759, 1.0e-4},
        { Barrier::DownOut,   100.0,    3.0, Option::Call,     90, 100.0, 0.04, 0.08, 0.50, 0.25,  3.0000, 1.0e-4},
        { Barrier::DownOut,   100.0,    3.0, Option::Call,    100, 100.0, 0.04, 0.08, 0.50, 0.25,  3.0000, 1.0e-4},
        { Barrier::DownOut,   100.0,    3.0, Option::Call,    110, 100.0, 0.04, 0.08, 0.50, 0.25,  3.0000, 1.0e-4},
        { Barrier::UpOut,     105.0,    3.0, Option::Call,     90, 100.0, 0.04, 0.08, 0.50, 0.25,  2.6789, 1.0e-4},
        { Barrier::UpOut,     105.0,    3.0, Option::Call,    100, 100.0, 0.04, 0.08, 0.50, 0.25,  2.3580, 1.0e-4},
        { Barrier::UpOut,     105.0,    3.0, Option::Call,    110, 100.0, 0.04, 0.08, 0.50, 0.25,  2.3453, 1.0e-4},

        { Barrier::DownIn,     95.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.25,  7.7627, 1.0e-4},
        { Barrier::DownIn,     95.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  4.0109, 1.0e-4},
        { Barrier::DownIn,     95.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.25,  2.0576, 1.0e-4},
        { Barrier::DownIn,    100.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.25, 13.8333, 1.0e-4},
        { Barrier::DownIn,    100.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  7.8494, 1.0e-4},
        { Barrier::DownIn,    100.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.25,  3.9795, 1.0e-4},
        { Barrier::UpIn,      105.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.25, 14.1112, 1.0e-4},
        { Barrier::UpIn,      105.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  8.4482, 1.0e-4},
        { Barrier::UpIn,      105.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.25,  4.5910, 1.0e-4},

        { Barrier::DownOut,    95.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  8.8334, 1.0e-4},
        { Barrier::DownOut,    95.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  7.0285, 1.0e-4},
        { Barrier::DownOut,    95.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  5.4137, 1.0e-4},
        { Barrier::DownOut,   100.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  3.0000, 1.0e-4},
        { Barrier::DownOut,   100.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  3.0000, 1.0e-4},
        { Barrier::DownOut,   100.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  3.0000, 1.0e-4},
        { Barrier::UpOut,     105.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  2.6341, 1.0e-4},
        { Barrier::UpOut,     105.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  2.4389, 1.0e-4},
        { Barrier::UpOut,     105.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  2.4315, 1.0e-4},

        { Barrier::DownIn,     95.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  9.0093, 1.0e-4},
        { Barrier::DownIn,     95.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  5.1370, 1.0e-4},
        { Barrier::DownIn,     95.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  2.8517, 1.0e-4},
        { Barrier::DownIn,    100.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.30, 14.8816, 1.0e-4},
        { Barrier::DownIn,    100.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  9.2045, 1.0e-4},
        { Barrier::DownIn,    100.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  5.3043, 1.0e-4},
        { Barrier::UpIn,      105.0,    3.0, Option::Call,    90, 100.0, 0.04, 0.08, 0.50, 0.30, 15.2098, 1.0e-4},
        { Barrier::UpIn,      105.0,    3.0, Option::Call,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  9.7278, 1.0e-4},
        { Barrier::UpIn,      105.0,    3.0, Option::Call,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  5.8350, 1.0e-4},



        { Barrier::DownOut,    95.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.25,  2.2798, 1.0e-4 },
        { Barrier::DownOut,    95.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  2.2947, 1.0e-4 },
        { Barrier::DownOut,    95.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.25,  2.6252, 1.0e-4 },
        { Barrier::DownOut,   100.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.25,  3.0000, 1.0e-4 },
        { Barrier::DownOut,   100.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  3.0000, 1.0e-4 },
        { Barrier::DownOut,   100.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.25,  3.0000, 1.0e-4 },
        { Barrier::UpOut,     105.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.25,  3.7760, 1.0e-4 },
        { Barrier::UpOut,     105.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  5.4932, 1.0e-4 },
        { Barrier::UpOut,     105.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.25,  7.5187, 1.0e-4 },

        { Barrier::DownIn,     95.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.25,  2.9586, 1.0e-4 },
        { Barrier::DownIn,     95.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  6.5677, 1.0e-4 },
        { Barrier::DownIn,     95.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.25, 11.9752, 1.0e-4 },
        { Barrier::DownIn,    100.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.25,  2.2845, 1.0e-4 },
        { Barrier::DownIn,    100.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  5.9085, 1.0e-4 },
        { Barrier::DownIn,    100.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.25, 11.6465, 1.0e-4 },
        { Barrier::UpIn,      105.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.25,  1.4653, 1.0e-4 },
        { Barrier::UpIn,      105.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.25,  3.3721, 1.0e-4 },
        { Barrier::UpIn,      105.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.25,  7.0846, 1.0e-4 },

        { Barrier::DownOut,    95.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  2.4170, 1.0e-4 },
        { Barrier::DownOut,    95.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  2.4258, 1.0e-4 },
        { Barrier::DownOut,    95.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  2.6246, 1.0e-4 },
        { Barrier::DownOut,   100.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  3.0000, 1.0e-4 },
        { Barrier::DownOut,   100.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  3.0000, 1.0e-4 },
        { Barrier::DownOut,   100.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  3.0000, 1.0e-4 },
        { Barrier::UpOut,     105.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  4.2293, 1.0e-4 },
        { Barrier::UpOut,     105.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  5.8032, 1.0e-4 },
        { Barrier::UpOut,     105.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  7.5649, 1.0e-4 },

        { Barrier::DownIn,     95.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  3.8769, 1.0e-4 },
        { Barrier::DownIn,     95.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  7.7989, 1.0e-4 },
        { Barrier::DownIn,     95.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.30, 13.3078, 1.0e-4 },
        { Barrier::DownIn,    100.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  3.3328, 1.0e-4 },
        { Barrier::DownIn,    100.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  7.2636, 1.0e-4 },
        { Barrier::DownIn,    100.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.30, 12.9713, 1.0e-4 },
        { Barrier::UpIn,      105.0,    3.0,  Option::Put,    90, 100.0, 0.04, 0.08, 0.50, 0.30,  2.0658, 1.0e-4 },
        { Barrier::UpIn,      105.0,    3.0,  Option::Put,   100, 100.0, 0.04, 0.08, 0.50, 0.30,  4.4226, 1.0e-4 },
        { Barrier::UpIn,      105.0,    3.0,  Option::Put,   110, 100.0, 0.04, 0.08, 0.50, 0.30,  8.3686, 1.0e-4 }

        /*
            Data from "Going to Extreme: Correcting Simulation Bias in Exotic
            Option Valuation"
            D.R. Beaglehole, P.H. Dybvig and G. Zhou
            Financial Analysts Journal; Jan / Feb 1997; 53, 1
        */
        //    barrierType, barrier, rebate,         type, strike,     s,    q,    r,    t,    v,  result, tol
        // { Barrier::DownOut,    45.0,    0.0,  Option::Put,     50,  50.0,-0.05, 0.10, 0.25, 0.50,   4.032, 1.0e-3 },
        // { Barrier::DownOut,    45.0,    0.0,  Option::Put,     50,  50.0,-0.05, 0.10, 1.00, 0.50,   5.477, 1.0e-3 }

    };


    DayCounter dc = Actual360();
    Date today = Date::todaysDate();

    boost::shared_ptr<SimpleQuote> spot(new SimpleQuote(0.0));
    boost::shared_ptr<SimpleQuote> qRate(new SimpleQuote(0.0));
    boost::shared_ptr<YieldTermStructure> qTS = flatRate(today, qRate, dc);
    boost::shared_ptr<SimpleQuote> rRate(new SimpleQuote(0.0));
    boost::shared_ptr<YieldTermStructure> rTS = flatRate(today, rRate, dc);
    boost::shared_ptr<SimpleQuote> vol(new SimpleQuote(0.0));
    boost::shared_ptr<BlackVolTermStructure> volTS = flatVol(today, vol, dc);

    for (Size i=0; i<LENGTH(values); i++) {
        Date exDate = today + Integer(values[i].t*360+0.5);
        boost::shared_ptr<Exercise> exercise(new EuropeanExercise(exDate));

        spot ->setValue(values[i].s);
        qRate->setValue(values[i].q);
        rRate->setValue(values[i].r);
        vol  ->setValue(values[i].v);

        boost::shared_ptr<StrikedTypePayoff> payoff(new
            PlainVanillaPayoff(values[i].type, values[i].strike));

        boost::shared_ptr<BlackScholesMertonProcess> stochProcess(new
            BlackScholesMertonProcess(Handle<Quote>(spot),
                                      Handle<YieldTermStructure>(qTS),
                                      Handle<YieldTermStructure>(rTS),
                                      Handle<BlackVolTermStructure>(volTS)));

        boost::shared_ptr<PricingEngine> engine(
                                     new AnalyticBarrierEngine(stochProcess));

        BarrierOption barrierOption(
                values[i].barrierType,
                values[i].barrier,
                values[i].rebate,
                payoff,
                exercise);
        barrierOption.setPricingEngine(engine);

        Real calculated = barrierOption.NPV();
        Real expected = values[i].result;
        Real error = std::fabs(calculated-expected);
        if (error>values[i].tol) {
            REPORT_FAILURE("value", values[i].barrierType, values[i].barrier,
                           values[i].rebate, payoff, exercise, values[i].s,
                           values[i].q, values[i].r, today, values[i].v,
                           expected, calculated, error, values[i].tol);
        }

        engine = boost::shared_ptr<PricingEngine>(
                new FdBlackScholesBarrierEngine(stochProcess, 200, 400));
        barrierOption.setPricingEngine(engine);

        calculated = barrierOption.NPV();
        expected = values[i].result;
        error = std::fabs(calculated-expected);
        if (error>5.0e-3) {
            REPORT_FAILURE("fd value", values[i].barrierType, values[i].barrier,
                           values[i].rebate, payoff, exercise, values[i].s,
                           values[i].q, values[i].r, today, values[i].v,
                           expected, calculated, error, values[i].tol);
        }

    }
}

void BarrierOptionTest::testBabsiriValues() {

    BOOST_MESSAGE("Testing barrier options against Babsiri's values...");

    /*
        Data from
        "Simulating Path-Dependent Options: A New Approach"
          - M. El Babsiri and G. Noel
            Journal of Derivatives; Winter 1998; 6, 2
    */
    BarrierOptionData values[] = {
        { Barrier::DownIn, 0.10,   100,       90,   0.07187,  0.0 },
        { Barrier::DownIn, 0.15,   100,       90,   0.60638,  0.0 },
        { Barrier::DownIn, 0.20,   100,       90,   1.64005,  0.0 },
        { Barrier::DownIn, 0.25,   100,       90,   2.98495,  0.0 },
        { Barrier::DownIn, 0.30,   100,       90,   4.50952,  0.0 },
        { Barrier::UpIn,   0.10,   100,      110,   4.79148,  0.0 },
        { Barrier::UpIn,   0.15,   100,      110,   7.08268,  0.0 },
        { Barrier::UpIn,   0.20,   100,      110,   9.11008,  0.0 },
        { Barrier::UpIn,   0.25,   100,      110,  11.06148,  0.0 },
        { Barrier::UpIn,   0.30,   100,      110,  12.98351,  0.0 }
    };

    Real underlyingPrice = 100.0;
    Real rebate = 0.0;
    Rate r = 0.05;
    Rate q = 0.02;

    DayCounter dc = Actual360();
    Date today = Date::todaysDate();
    boost::shared_ptr<SimpleQuote> underlying(
                                            new SimpleQuote(underlyingPrice));

    boost::shared_ptr<SimpleQuote> qH_SME(new SimpleQuote(q));
    boost::shared_ptr<YieldTermStructure> qTS = flatRate(today, qH_SME, dc);

    boost::shared_ptr<SimpleQuote> rH_SME(new SimpleQuote(r));
    boost::shared_ptr<YieldTermStructure> rTS = flatRate(today, rH_SME, dc);

    boost::shared_ptr<SimpleQuote> volatility(new SimpleQuote(0.10));
    boost::shared_ptr<BlackVolTermStructure> volTS =
        flatVol(today, volatility, dc);

    Date exDate = today+360;
    boost::shared_ptr<Exercise> exercise(new EuropeanExercise(exDate));

    for (Size i=0; i<LENGTH(values); i++) {
        volatility->setValue(values[i].volatility);

        boost::shared_ptr<StrikedTypePayoff> callPayoff(new
            PlainVanillaPayoff(Option::Call, values[i].strike));

        boost::shared_ptr<BlackScholesMertonProcess> stochProcess(new
            BlackScholesMertonProcess(Handle<Quote>(underlying),
                                      Handle<YieldTermStructure>(qTS),
                                      Handle<YieldTermStructure>(rTS),
                                      Handle<BlackVolTermStructure>(volTS)));


        boost::shared_ptr<PricingEngine> engine(
                                     new AnalyticBarrierEngine(stochProcess));

        // analytic
        BarrierOption barrierCallOption(
                values[i].type,
                values[i].barrier,
                rebate,
                callPayoff,
                exercise);
        barrierCallOption.setPricingEngine(engine);
        Real calculated = barrierCallOption.NPV();
        Real expected = values[i].callValue;
        Real error = std::fabs(calculated-expected);
        Real maxErrorAllowed = 1.0e-5;
        if (error>maxErrorAllowed) {
            REPORT_FAILURE("value", values[i].type, values[i].barrier,
                           rebate, callPayoff, exercise, underlyingPrice,
                           q, r, today, values[i].volatility,
                           expected, calculated, error, maxErrorAllowed);
        }

        Real maxMcRelativeErrorAllowed = 2.0e-2;
        Size timeSteps = 1;
        bool antitheticVariate = false;
        bool brownianBridge = true;
        Size requiredSamples = 131071; // 2^17-1
        Real requiredTolerance = Null<Real>();
        Size maxSamples = 1048575; // 2^20-1
        bool isBiased = false;
        long seed = 5;

        boost::shared_ptr<PricingEngine> mcEngine(new
            MCBarrierEngine<LowDiscrepancy>(stochProcess,
                                            timeSteps, brownianBridge,
                                            antitheticVariate,
                                            requiredSamples, requiredTolerance,
                                            maxSamples, isBiased, seed));

        barrierCallOption.setPricingEngine(mcEngine);
        calculated = barrierCallOption.NPV();
        error = std::fabs(calculated-expected)/expected;
        if (error>maxMcRelativeErrorAllowed) {
            REPORT_FAILURE("value", values[i].type, values[i].barrier,
                           rebate, callPayoff, exercise, underlyingPrice,
                           q, r, today, values[i].volatility,
                           expected, calculated, error,
                           maxMcRelativeErrorAllowed);
        }

    }
}

void BarrierOptionTest::testBeagleholeValues() {

    BOOST_MESSAGE("Testing barrier options against Beaglehole's values...");


    /*
        Data from
        "Going to Extreme: Correcting Simulation Bias in Exotic
         Option Valuation"
          - D.R. Beaglehole, P.H. Dybvig and G. Zhou
            Financial Analysts Journal; Jan / Feb 1997; 53, 1
    */
    BarrierOptionData values[] = {
        { Barrier::DownOut, 0.50,   50,      45,  5.477,  0.0 }
    };

    Real underlyingPrice = 50.0;
    Real rebate = 0.0;
    Rate r = std::log(1.1);
    Rate q = 0.00;

    DayCounter dc = Actual360();
    Date today = Date::todaysDate();

    boost::shared_ptr<SimpleQuote> underlying(
                                            new SimpleQuote(underlyingPrice));

    boost::shared_ptr<SimpleQuote> qH_SME(new SimpleQuote(q));
    boost::shared_ptr<YieldTermStructure> qTS = flatRate(today, qH_SME, dc);

    boost::shared_ptr<SimpleQuote> rH_SME(new SimpleQuote(r));
    boost::shared_ptr<YieldTermStructure> rTS = flatRate(today, rH_SME, dc);

    boost::shared_ptr<SimpleQuote> volatility(new SimpleQuote(0.10));
    boost::shared_ptr<BlackVolTermStructure> volTS =
        flatVol(today, volatility, dc);


    Date exDate = today+360;
    boost::shared_ptr<Exercise> exercise(new EuropeanExercise(exDate));

    for (Size i=0; i<LENGTH(values); i++) {
        volatility->setValue(values[i].volatility);

        boost::shared_ptr<StrikedTypePayoff> callPayoff(new
            PlainVanillaPayoff(Option::Call, values[i].strike));

        boost::shared_ptr<BlackScholesMertonProcess> stochProcess(new
            BlackScholesMertonProcess(Handle<Quote>(underlying),
                                      Handle<YieldTermStructure>(qTS),
                                      Handle<YieldTermStructure>(rTS),
                                      Handle<BlackVolTermStructure>(volTS)));


        boost::shared_ptr<PricingEngine> engine(
                                     new AnalyticBarrierEngine(stochProcess));

        BarrierOption barrierCallOption(
                values[i].type,
                values[i].barrier,
                rebate,
                callPayoff,
                exercise);
        barrierCallOption.setPricingEngine(engine);
        Real calculated = barrierCallOption.NPV();
        Real expected = values[i].callValue;
        Real maxErrorAllowed = 1.0e-3;
        Real error = std::fabs(calculated-expected);
        if (error > maxErrorAllowed) {
            REPORT_FAILURE("value", values[i].type, values[i].barrier,
                           rebate, callPayoff, exercise, underlyingPrice,
                           q, r, today, values[i].volatility,
                           expected, calculated, error, maxErrorAllowed);
        }

        Real maxMcRelativeErrorAllowed = 0.01;
        Size timeSteps = 1;
        bool brownianBridge = true;
        bool antitheticVariate = false;
        Size requiredSamples = 131071; // 2^17-1
        Real requiredTolerance = Null<Real>();
        Size maxSamples = 1048575; // 2^20-1
        bool isBiased = false;
        long seed = 10;
        boost::shared_ptr<PricingEngine> mcEngine(
            new MCBarrierEngine<LowDiscrepancy>(stochProcess,
                                                timeSteps, brownianBridge,
                                                antitheticVariate,
                                                requiredSamples,
                                                requiredTolerance,
                                                maxSamples, isBiased, seed));

        barrierCallOption.setPricingEngine(mcEngine);
        calculated = barrierCallOption.NPV();
        error = std::fabs(calculated-expected)/expected;
        if (error>maxMcRelativeErrorAllowed) {
            REPORT_FAILURE("value", values[i].type, values[i].barrier,
                           rebate, callPayoff, exercise, underlyingPrice,
                           q, r, today, values[i].volatility,
                           expected, calculated, error,
                           maxMcRelativeErrorAllowed);
        }
    }
}

void BarrierOptionTest::testPerturbative() {
    BOOST_MESSAGE("Testing perturbative engine for barrier options...");

    Real S = 100.0;
    Real rebate = 0.0;
    Rate r = 0.03;
    Rate q = 0.02;

    DayCounter dc = Actual360();
    Date today = Date::todaysDate();

    boost::shared_ptr<SimpleQuote> underlying(new SimpleQuote(S));
    boost::shared_ptr<YieldTermStructure> qTS = flatRate(today, q, dc);
    boost::shared_ptr<YieldTermStructure> rTS = flatRate(today, r, dc);

    std::vector<Date> dates(2);
    std::vector<Volatility> vols(2);

    dates[0] = today + 90;  vols[0] = 0.105;
    dates[1] = today + 180; vols[1] = 0.11;

    boost::shared_ptr<BlackVolTermStructure> volTS(
                              new BlackVarianceCurve(today, dates, vols, dc));

    boost::shared_ptr<BlackScholesMertonProcess> stochProcess(
        new BlackScholesMertonProcess(Handle<Quote>(underlying),
                                      Handle<YieldTermStructure>(qTS),
                                      Handle<YieldTermStructure>(rTS),
                                      Handle<BlackVolTermStructure>(volTS)));

    Real strike = 101.0;
    Real barrier = 101.0;
    Date exDate = today+180;

    boost::shared_ptr<Exercise> exercise(new EuropeanExercise(exDate));
    boost::shared_ptr<StrikedTypePayoff> payoff(
                                 new PlainVanillaPayoff(Option::Put, strike));

    BarrierOption option(Barrier::UpOut, barrier, rebate, payoff, exercise);

    Natural order = 0;
    bool zeroGamma = false;
    boost::shared_ptr<PricingEngine> engine(
         new PerturbativeBarrierOptionEngine(stochProcess, order, zeroGamma));

    option.setPricingEngine(engine);

    Real calculated = option.NPV();
    Real expected = 0.897365;
    Real tolerance = 1.0e-6;
    if (std::fabs(calculated-expected) > tolerance) {
        BOOST_ERROR("Failed to reproduce expected value"
                    << "\n  calculated: " << std::setprecision(5) << calculated
                    << "\n  expected:   " << std::setprecision(5) << expected);
    }

    order = 1;
    engine = boost::shared_ptr<PricingEngine>(
         new PerturbativeBarrierOptionEngine(stochProcess, order, zeroGamma));

    option.setPricingEngine(engine);

    calculated = option.NPV();
    expected = 0.894374;
    if (std::fabs(calculated-expected) > tolerance) {
        BOOST_ERROR("Failed to reproduce expected value"
                    << "\n  calculated: " << std::setprecision(5) << calculated
                    << "\n  expected:   " << std::setprecision(5) << expected);
    }

    order = 2;
    engine = boost::shared_ptr<PricingEngine>(
         new PerturbativeBarrierOptionEngine(stochProcess, order, zeroGamma));

    option.setPricingEngine(engine);

    calculated = option.NPV();
    expected = 0.894375;
    if (std::fabs(calculated-expected) > tolerance) {
        BOOST_ERROR("Failed to reproduce expected value"
                    << "\n  calculated: " << std::setprecision(5) << calculated
                    << "\n  expected:   " << std::setprecision(5) << expected);
    }
}

test_suite* BarrierOptionTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Barrier option tests");
    suite->add(QUANTLIB_TEST_CASE(&BarrierOptionTest::testHaugValues));
    suite->add(QUANTLIB_TEST_CASE(&BarrierOptionTest::testBabsiriValues));
    suite->add(QUANTLIB_TEST_CASE(&BarrierOptionTest::testBeagleholeValues));
    suite->add(QUANTLIB_TEST_CASE(&BarrierOptionTest::testPerturbative));
    return suite;
}

