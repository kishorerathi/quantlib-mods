/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

#include <ql/ShortRateModels/CalibrationHelpers/caphelper.hpp>
#include <ql/PricingEngines/CapFloor/blackcapfloorengine.hpp>
#include <ql/PricingEngines/CapFloor/discretizedcapfloor.hpp>
#include <ql/CashFlows/cashflowvectors.hpp>
#include <ql/Instruments/swap.hpp>

namespace QuantLib {

    CapHelper::CapHelper(const Period& length,
                         const Handle<Quote>& volatility,
                         const boost::shared_ptr<Xibor>& index,
                         Frequency fixedLegFrequency,
                         const DayCounter& fixedLegDayCounter,
                         bool includeFirstSwaplet,
                         const Handle<YieldTermStructure>& termStructure,
                         bool calibrateVolatility)
    : CalibrationHelper(volatility,termStructure,calibrateVolatility) {

        Period indexTenor = index->tenor();
        Rate fixedRate = 0.04; // dummy value
        Date startDate, maturity;
        if (includeFirstSwaplet) {
            startDate = termStructure->referenceDate();
            maturity = termStructure->referenceDate() + length;
        } else {
            startDate = termStructure->referenceDate() + indexTenor;
            maturity = termStructure->referenceDate() + length;
        }
        boost::shared_ptr<Xibor> dummyIndex(
                                     new Xibor("dummy",
                                               Period(indexTenor.length(),
                                                      indexTenor.units()),
                                               index->settlementDays(),
                                               index->currency(),
                                               index->calendar(),
                                               index->businessDayConvention(),
                                               index->endOfMonth(),
                                               termStructure->dayCounter(),
                                               termStructure));

        std::vector<Real> nominals(1,1.0);

        Schedule floatSchedule(startDate, maturity, index->tenor(), index->calendar(),
                               index->businessDayConvention(),
                               index->businessDayConvention(), false, false); 
        std::vector<boost::shared_ptr<CashFlow> > floatingLeg =
            FloatingRateCouponVector(floatSchedule,
                                     index->businessDayConvention(),
                                     nominals,
                                     0, index, std::vector<Real>(),
                                     std::vector<Spread>());
        Schedule fixedSchedule(startDate, maturity, Period(fixedLegFrequency),
                               index->calendar(), Unadjusted, Unadjusted,
                               false, false); 
        std::vector<boost::shared_ptr<CashFlow> > fixedLeg =
            FixedRateCouponVector(fixedSchedule,
                                  index->businessDayConvention(),
                                  nominals,
                                  std::vector<Rate>(1, fixedRate),
                                  fixedLegDayCounter);

        boost::shared_ptr<Swap> swap(
            new Swap(termStructure, floatingLeg, fixedLeg));
        Rate fairRate = fixedRate - swap->NPV()/(swap->legBPS(1)/1.0e-4);
        engine_  = boost::shared_ptr<PricingEngine>();
        cap_ = boost::shared_ptr<Cap>(new Cap(floatingLeg,
                                              std::vector<Rate>(1, fairRate),
                                              termStructure, engine_));
        marketValue_ = blackPrice(volatility_->value());
    }

    void CapHelper::addTimesTo(std::list<Time>& times) const {
        CapFloor::arguments args;
        cap_->setupArguments(&args);
        std::vector<Time> capTimes =
            DiscretizedCapFloor(args).mandatoryTimes();
        std::copy(capTimes.begin(), capTimes.end(),
                  std::back_inserter(times));
    }

    Real CapHelper::modelValue() const {
        cap_->setPricingEngine(engine_);
        return cap_->NPV();
    }

    Real CapHelper::blackPrice(Volatility sigma) const {
        boost::shared_ptr<Quote> vol(new SimpleQuote(sigma));
        boost::shared_ptr<PricingEngine> black(
                                 new BlackCapFloorEngine(Handle<Quote>(vol)));
        cap_->setPricingEngine(black);
        Real value = cap_->NPV();
        cap_->setPricingEngine(engine_);
        return value;
    }

}
