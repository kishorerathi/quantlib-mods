/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Chiara Fornarola

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

#include "assetswap.hpp"
#include "utilities.hpp"
#include <ql/time/schedule.hpp>
#include <ql/instruments/assetswap.hpp>
#include <ql/instruments/bond.hpp>
#include <ql/instruments/bonds/fixedratebond.hpp>
#include <ql/instruments/bonds/floatingratebond.hpp>
#include <ql/instruments/bonds/cmsratebond.hpp>
#include <ql/instruments/bonds/zerocouponbond.hpp>
#include <ql/index.hpp>
#include <ql/termstructures/yieldcurves/flatforward.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/time/daycounters/thirty360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/simpledaycounter.hpp>
#include <ql/indexes/ibor/euribor.hpp>
#include <ql/indexes/swapindex.hpp>
#include <ql/cashflows/iborcoupon.hpp>
#include <ql/cashflows/cashflowvectors.hpp>
#include <ql/cashflows/couponpricer.hpp>
#include <ql/cashflows/conundrumpricer.hpp>
#include <ql/termstructures/volatilities/interestrate/caplet/capletconstantvol.hpp>
#include <ql/termstructures/volatilities/interestrate/swaption/swaptionconstantvol.hpp>
#include <ql/termstructures/volatilities/interestrate/swaption/swaptionvolmatrix.hpp>
#include <ql/termstructures/volatilities/interestrate/swaption/swaptionvolcube2.hpp>
#include <ql/termstructures/volatilities/interestrate/swaption/swaptionvolcube1.hpp>
#include <ql/termstructures/volatilities/interestrate/swaption/swaptionvolcube.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/cashflows/cashflows.hpp>
#include <ql/cashflows/simplecashflow.hpp>
#include <ql/pricingengines/bond/discountingbondengine.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(AssetSwapTest)

// global data

Date today_, settlement_;
boost::shared_ptr<IborIndex> iborIndex_;
boost::shared_ptr<SwapIndex> swapIndex_;
boost::shared_ptr<IborCouponPricer> pricer_;
boost::shared_ptr<CmsCouponPricer> cmspricer_;
GFunctionFactory::ModelOfYieldCurve modelOfYieldCurve_;
Spread spread_;
Spread nonnullspread_;
Real faceAmount_;
Calendar calendar_;
Compounding compounding_ ;
BusinessDayConvention fixedConvention_; //, floatingConvention_;
Frequency floatingFrequency_ , fixedFrequency_;
Natural settlementDays_ , swapSettlementDays_;
RelinkableHandle<YieldTermStructure> termStructure_;

void setup() {
    settlementDays_ = 3;
    swapSettlementDays_ = 2;
    faceAmount_ = 100.0;
    fixedConvention_ = Unadjusted;
    //floatingConvention_ = ModifiedFollowing;
    compounding_ = Continuous;
    fixedFrequency_ = Annual;
    floatingFrequency_ = Semiannual;
    iborIndex_ = boost::shared_ptr<IborIndex>(new
        Euribor(Period(floatingFrequency_), termStructure_));
    calendar_ = iborIndex_->fixingCalendar();
    swapIndex_= boost::shared_ptr<SwapIndex>(new
           SwapIndex("EuriborSwapFixA", 10*Years,swapSettlementDays_,
                iborIndex_->currency(), calendar_,
                Period(fixedFrequency_), fixedConvention_,
                iborIndex_->dayCounter(), iborIndex_));
    spread_=0.0;
    nonnullspread_=0.003;
    //today_ = calendar_.adjust(Date::todaysDate());
    Date today_(24,April,2007);
    Settings::instance().evaluationDate() = today_;
    //settlement_ = calendar_.advance(today_,settlementDays_,Days);
    termStructure_.linkTo(flatRate(today_,0.05,Actual365Fixed()));
    pricer_= boost::shared_ptr<IborCouponPricer> (new
        BlackIborCouponPricer(Handle<CapletVolatilityStructure>()));
    Handle<SwaptionVolatilityStructure> swaptionVolatilityStructure(
           boost::shared_ptr<SwaptionVolatilityStructure>(new
           SwaptionConstantVolatility(Settings::instance().evaluationDate(),0.2,
                                      Actual365Fixed())));
    Handle<Quote> meanReversionQuote(boost::shared_ptr<Quote>(new SimpleQuote(0.01)));
    modelOfYieldCurve_= GFunctionFactory::Standard;
    cmspricer_= boost::shared_ptr<CmsCouponPricer> (new
                      ConundrumPricerByBlack(swaptionVolatilityStructure,
                      modelOfYieldCurve_, meanReversionQuote));
}

QL_END_TEST_LOCALS(AssetSwapTest)


void AssetSwapTest::testImpliedValue() {

    BOOST_MESSAGE("Testing bond implied value against asset-swap fair price"
                  " with spread=0...");

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool payFixedRate = true;
    bool parAssetSwap = true;
    bool inArrears = false;

    // Fixed Underlying bond (Isin: DE0001135275 DBR 4 01/04/37)
    // maturity doesn't occur on a business day

    Schedule fixedBondSchedule1(Date(4,January,2005),
                                Date(4,January,2037),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    boost::shared_ptr<Bond> fixedBond1(new
        FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule1,
                      std::vector<Rate>(1, 0.04),
                      ActualActual(ActualActual::ISDA), Following,
                      100.0, Date(4,January,2005) ));

    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    fixedBond1->setPricingEngine(bondEngine);

    Real fixedBondPrice1 = fixedBond1->cleanPrice();
    AssetSwap fixedBondAssetSwap1(payFixedRate,
                                  fixedBond1, fixedBondPrice1,
                                  iborIndex_, spread_, termStructure_,
                                  Schedule(),
                                  iborIndex_->dayCounter(),
                                  parAssetSwap);
    Real fixedBondAssetSwapPrice1 = fixedBondAssetSwap1.fairPrice();
    Real tolerance = 1.0e-13;
    Real error1 = std::fabs(fixedBondAssetSwapPrice1-fixedBondPrice1);

    if (error1>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << fixedBondPrice1
                    << "\n  asset swap fair price: " << fixedBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error1
                    << "\n  tolerance:             " << tolerance);
    }

    // Fixed Underlying bond (Isin: IT0006527060 IBRD 5 02/05/19)
    // maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    boost::shared_ptr<Bond> fixedBond2(new
        FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule2,
                      std::vector<Rate>(1, 0.05),
                      Thirty360(Thirty360::BondBasis), Following,
                      100.0, Date(5,February,2005)));

    fixedBond2->setPricingEngine(bondEngine);

    Real fixedBondPrice2 = fixedBond2->cleanPrice();
    AssetSwap fixedBondAssetSwap2(payFixedRate,
                                  fixedBond2, fixedBondPrice2,
                                  iborIndex_, spread_, termStructure_,
                                  Schedule(),
                                  iborIndex_->dayCounter(),
                                  parAssetSwap);
    Real fixedBondAssetSwapPrice2 = fixedBondAssetSwap2.fairPrice();
    Real error2 = std::fabs(fixedBondAssetSwapPrice2-fixedBondPrice2);

    if (error2>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << fixedBondPrice2
                    << "\n  asset swap fair price: " << fixedBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: IT0003543847 ISPIM 0 09/29/13)
    // maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);

    boost::shared_ptr<Bond> floatingBond1(new
        FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule1,
                         iborIndex_, Actual360(),
                         Following, fixingDays,
                         std::vector<Real>(1,1), std::vector<Spread>(1,0.0056),
                         std::vector<Rate>(), std::vector<Rate>(),
                         inArrears,
                         100.0, Date(29,September,2003)));

    floatingBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondPrice1 = floatingBond1->cleanPrice();
    AssetSwap floatingBondAssetSwap1(payFixedRate,
                                     floatingBond1, floatingBondPrice1,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real floatingBondAssetSwapPrice1 = floatingBondAssetSwap1.fairPrice();
    Real error3 = std::fabs(floatingBondAssetSwapPrice1-floatingBondPrice1);

    if (error3>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for floater:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << floatingBondPrice1
                    << "\n  asset swap fair price: " << floatingBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: XS0090566539 COE 0 09/24/18)
    // maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    boost::shared_ptr<Bond> floatingBond2(new
        FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule2,
                         iborIndex_, Actual360(),
                         ModifiedFollowing, fixingDays,
                         std::vector<Real>(1,1), std::vector<Spread>(1,0.0025),
                         std::vector<Rate>(), std::vector<Rate>(),
                         inArrears,
                         100.0, Date(24,September,2004)));

    floatingBond2->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond2->cashflows(),pricer_);
    iborIndex_->addFixing(Date(22,March,2007), 0.04013);
    Real currentCoupon=0.04013+0.0025;
    Real floatingCurrentCoupon= floatingBond2->currentCoupon();
    Real error4= std::fabs(floatingCurrentCoupon-currentCoupon);
    if (error4>tolerance) {
        BOOST_ERROR("wrong current coupon is returned for floater bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's calculated current coupon:      " << currentCoupon
                    << "\n  current coupon asked to the bond: " << floatingCurrentCoupon
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

    Real floatingBondPrice2 = floatingBond2->cleanPrice();
    AssetSwap floatingBondAssetSwap2(payFixedRate,
                                     floatingBond2, floatingBondPrice2,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real floatingBondAssetSwapPrice2 = floatingBondAssetSwap2.fairPrice();
    Real error5 = std::fabs(floatingBondAssetSwapPrice2-floatingBondPrice2);

    if (error5>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for floater:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << floatingBondPrice2
                    << "\n  asset swap fair price: " << floatingBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error5
                    << "\n  tolerance:             " << tolerance);
    }

    // CMS Underlying bond (Isin: XS0228052402 CRDIT 0 8/22/20)
    // maturity doesn't occur on a business day

    Schedule cmsBondSchedule1(Date(22,August,2005),
                              Date(22,August,2020),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    boost::shared_ptr<Bond> cmsBond1(new
        CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule1,
                    swapIndex_, Thirty360(),
                    Following, fixingDays,
                    std::vector<Real>(1,1.0), std::vector<Spread>(1,0.0),
                    std::vector<Rate>(1,0.055), std::vector<Rate>(1,0.025),
                    inArrears,
                    100.0, Date(22,August,2005)));

    cmsBond1->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond1->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(18,August,2006), 0.04158);
    Real cmsBondPrice1 = cmsBond1->cleanPrice();
    AssetSwap cmsBondAssetSwap1(payFixedRate,
                                cmsBond1, cmsBondPrice1,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real cmsBondAssetSwapPrice1 = cmsBondAssetSwap1.fairPrice();
    Real error6 = std::fabs(cmsBondAssetSwapPrice1-cmsBondPrice1);

    if (error6>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << cmsBondPrice1
                    << "\n  asset swap fair price: " << cmsBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

     // CMS Underlying bond (Isin: XS0218766664 ISPIM 0 5/6/15)
     // maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    boost::shared_ptr<Bond> cmsBond2(new
        CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule2,
                    swapIndex_, Thirty360(),
                    Following, fixingDays,
                    std::vector<Real>(1,0.84), std::vector<Spread>(1,0.0),
                    std::vector<Rate>(), std::vector<Rate>(),
                    inArrears,
                    100.0, Date(06,May,2005)));

    cmsBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondPrice2 = cmsBond2->cleanPrice();
    AssetSwap cmsBondAssetSwap2(payFixedRate,
                                cmsBond2, cmsBondPrice2,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real cmsBondAssetSwapPrice2 = cmsBondAssetSwap2.fairPrice();
    Real error7 = std::fabs(cmsBondAssetSwapPrice2-cmsBondPrice2);

    if (error7>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << cmsBondPrice2
                    << "\n  asset swap fair price: " << cmsBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error7
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
    // maturity doesn't occur on a business day

    boost::shared_ptr<Bond> zeroCpnBond1(new
        ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                       Date(20,December,2015),
                       Following,
                       100.0, Date(19,December,1985)));

    zeroCpnBond1->setPricingEngine(bondEngine);

    Real zeroCpnBondPrice1 = zeroCpnBond1->cleanPrice();
    AssetSwap zeroCpnAssetSwap1(payFixedRate,
                                zeroCpnBond1, zeroCpnBondPrice1,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real zeroCpnBondAssetSwapPrice1 = zeroCpnAssetSwap1.fairPrice();
    Real error8 = std::fabs(cmsBondAssetSwapPrice1-cmsBondPrice1);

    if (error8>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << zeroCpnBondPrice1
                    << "\n  asset swap fair price: " << zeroCpnBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error8
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
    // maturity occurs on a business day

    boost::shared_ptr<Bond> zeroCpnBond2(new
        ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                       Date(17,February,2028),
                       Following,
                       100.0, Date(17,February,1998)));

    zeroCpnBond2->setPricingEngine(bondEngine);

    Real zeroCpnBondPrice2 = zeroCpnBond2->cleanPrice();
    AssetSwap zeroCpnAssetSwap2(payFixedRate,
                                zeroCpnBond2, zeroCpnBondPrice2,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real zeroCpnBondAssetSwapPrice2 = zeroCpnAssetSwap2.fairPrice();
    Real error9 = std::fabs(cmsBondAssetSwapPrice2-cmsBondPrice2);

    if (error9>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << zeroCpnBondPrice2
                    << "\n  asset swap fair price: " << zeroCpnBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error9
                    << "\n  tolerance:             " << tolerance);
    }
}


void AssetSwapTest::testMarketASWSpread() {

    BOOST_MESSAGE("Testing relationship between market asset swap"
                  " and par asset swap...");

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool payFixedRate = true;
    bool parAssetSwap = true;
    bool mktAssetSwap = false;
    bool inArrears = false;

    // Fixed Underlying bond (Isin: DE0001135275 DBR 4 01/04/37)
    // maturity doesn't occur on a business day

    Schedule fixedBondSchedule1(Date(4,January,2005),
                                Date(4,January,2037),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    boost::shared_ptr<Bond> fixedBond1(new
        FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule1,
                      std::vector<Rate>(1, 0.04),
                      ActualActual(ActualActual::ISDA), Following,
                      100.0, Date(4,January,2005)));

    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    fixedBond1->setPricingEngine(bondEngine);

    Real fixedBondMktPrice1 = 89.22 ; // market price observed on 7th June 2007
    Real fixedBondMktFullPrice1=fixedBondMktPrice1+fixedBond1->accruedAmount();
    AssetSwap fixedBondParAssetSwap1(payFixedRate,
                                     fixedBond1, fixedBondMktPrice1,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real fixedBondParAssetSwapSpread1 = fixedBondParAssetSwap1.fairSpread();
    AssetSwap fixedBondMktAssetSwap1(payFixedRate,
                                     fixedBond1, fixedBondMktPrice1,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     mktAssetSwap);
    Real fixedBondMktAssetSwapSpread1 = fixedBondMktAssetSwap1.fairSpread();

    Real tolerance = 1.0e-13;
    Real error1 = std::fabs(fixedBondMktAssetSwapSpread1-
                            100*fixedBondParAssetSwapSpread1/fixedBondMktFullPrice1);

    if (error1>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << fixedBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error1
                    << "\n  tolerance:             " << tolerance);
    }

    // Fixed Underlying bond (Isin: IT0006527060 IBRD 5 02/05/19)
    // maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    boost::shared_ptr<Bond> fixedBond2(new
        FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule2,
                      std::vector<Rate>(1, 0.05),
                      Thirty360(Thirty360::BondBasis), Following,
                      100.0, Date(5,February,2005)));

    fixedBond2->setPricingEngine(bondEngine);

    Real fixedBondMktPrice2 = 99.98 ; // market price observed on 7th June 2007
    Real fixedBondMktFullPrice2=fixedBondMktPrice2+fixedBond2->accruedAmount();
    AssetSwap fixedBondParAssetSwap2(payFixedRate,
                                     fixedBond2, fixedBondMktPrice2,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real fixedBondParAssetSwapSpread2 = fixedBondParAssetSwap2.fairSpread();
    AssetSwap fixedBondMktAssetSwap2(payFixedRate,
                                     fixedBond2, fixedBondMktPrice2,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     mktAssetSwap);
    Real fixedBondMktAssetSwapSpread2 = fixedBondMktAssetSwap2.fairSpread();
    Real error2 = std::fabs(fixedBondMktAssetSwapSpread2-
                            100*fixedBondParAssetSwapSpread2/fixedBondMktFullPrice2);

    if (error2>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << fixedBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: IT0003543847 ISPIM 0 09/29/13)
    // maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);

    boost::shared_ptr<Bond> floatingBond1(new
        FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule1,
                         iborIndex_, Actual360(),
                         Following, fixingDays,
                         std::vector<Real>(1,1), std::vector<Spread>(1,0.0056),
                         std::vector<Rate>(), std::vector<Rate>(),
                         inArrears,
                         100.0, Date(29,September,2003)));

    floatingBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondMktPrice1 = 101.64 ; // market price observed on 7th June 2007
    Real floatingBondMktFullPrice1 = floatingBondMktPrice1+floatingBond1->accruedAmount();
    AssetSwap floatingBondParAssetSwap1(payFixedRate,
                                        floatingBond1, floatingBondMktPrice1,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        parAssetSwap);
    Real floatingBondParAssetSwapSpread1 = floatingBondParAssetSwap1.fairSpread();
    AssetSwap floatingBondMktAssetSwap1(payFixedRate,
                                        floatingBond1, floatingBondMktPrice1,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        mktAssetSwap);
    Real floatingBondMktAssetSwapSpread1 = floatingBondMktAssetSwap1.fairSpread();
    Real error3 = std::fabs(floatingBondMktAssetSwapSpread1-
                            100*floatingBondParAssetSwapSpread1/floatingBondMktFullPrice1);

    if (error3>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for floating bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << floatingBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: XS0090566539 COE 0 09/24/18)
    // maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    boost::shared_ptr<Bond> floatingBond2(new
        FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule2,
                         iborIndex_, Actual360(),
                         ModifiedFollowing, fixingDays,
                         std::vector<Real>(1,1), std::vector<Spread>(1,0.0025),
                         std::vector<Rate>(), std::vector<Rate>(),
                         inArrears,
                         100.0, Date(24,September,2004)));

    floatingBond2->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond2->cashflows(),pricer_);
    iborIndex_->addFixing(Date(22,March,2007), 0.04013);
    Real floatingBondMktPrice2 = 101.248 ; // market price observed on 7th June 2007
    Real floatingBondMktFullPrice2 = floatingBondMktPrice2+floatingBond2->accruedAmount();
    AssetSwap floatingBondParAssetSwap2(payFixedRate,
                                        floatingBond2, floatingBondMktPrice2,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        parAssetSwap);
    Spread floatingBondParAssetSwapSpread2 = floatingBondParAssetSwap2.fairSpread();
    AssetSwap floatingBondMktAssetSwap2(payFixedRate,
                                        floatingBond2, floatingBondMktPrice2,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        mktAssetSwap);
    Real floatingBondMktAssetSwapSpread2 = floatingBondMktAssetSwap2.fairSpread();
    Real error4 = std::fabs(floatingBondMktAssetSwapSpread2-
                            100*floatingBondParAssetSwapSpread2/floatingBondMktFullPrice2);

    if (error4>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for floating bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << floatingBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

    // CMS Underlying bond (Isin: XS0228052402 CRDIT 0 8/22/20)
    // maturity doesn't occur on a business day

    Schedule cmsBondSchedule1(Date(22,August,2005),
                              Date(22,August,2020),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    boost::shared_ptr<Bond> cmsBond1(new
        CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule1,
                    swapIndex_, Thirty360(),
                    Following, fixingDays,
                    std::vector<Real>(1,1.0), std::vector<Spread>(1,0.0),
                    std::vector<Rate>(1,0.055), std::vector<Rate>(1,0.025),
                    inArrears,
                    100.0, Date(22,August,2005)));

    cmsBond1->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond1->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(18,August,2006), 0.04158);
    Real cmsBondMktPrice1 = 88.45 ; // market price observed on 7th June 2007
    Real cmsBondMktFullPrice1 = cmsBondMktPrice1+cmsBond1->accruedAmount();
    AssetSwap cmsBondParAssetSwap1(payFixedRate,
                                   cmsBond1, cmsBondMktPrice1,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   parAssetSwap);
    Real cmsBondParAssetSwapSpread1 = cmsBondParAssetSwap1.fairSpread();
    AssetSwap cmsBondMktAssetSwap1(payFixedRate,
                                   cmsBond1, cmsBondMktPrice1,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   mktAssetSwap);
    Real cmsBondMktAssetSwapSpread1 = cmsBondMktAssetSwap1.fairSpread();
    Real error5 = std::fabs(cmsBondMktAssetSwapSpread1-
                            100*cmsBondParAssetSwapSpread1/cmsBondMktFullPrice1);

    if (error5>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << cmsBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error5
                    << "\n  tolerance:             " << tolerance);
    }

     // CMS Underlying bond (Isin: XS0218766664 ISPIM 0 5/6/15)
     // maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    boost::shared_ptr<Bond> cmsBond2(new
        CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule2,
                    swapIndex_, Thirty360(),
                    Following, fixingDays,
                    std::vector<Real>(1,0.84), std::vector<Spread>(1,0.0),
                    std::vector<Rate>(), std::vector<Rate>(),
                    inArrears,
                    100.0, Date(06,May,2005)));

    cmsBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondMktPrice2 = 94.08 ; // market price observed on 7th June 2007
    Real cmsBondMktFullPrice2 = cmsBondMktPrice2+cmsBond2->accruedAmount();
    AssetSwap cmsBondParAssetSwap2(payFixedRate,
                                   cmsBond2, cmsBondMktPrice2,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   parAssetSwap);
    Spread cmsBondParAssetSwapSpread2 = cmsBondParAssetSwap2.fairSpread();
    AssetSwap cmsBondMktAssetSwap2(payFixedRate,
                                   cmsBond2, cmsBondMktPrice2,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   mktAssetSwap);
    Real cmsBondMktAssetSwapSpread2 = cmsBondMktAssetSwap2.fairSpread();
    Real error6 = std::fabs(cmsBondMktAssetSwapSpread2-
                            100*cmsBondParAssetSwapSpread2/cmsBondMktFullPrice2);

    if (error6>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << cmsBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
    // maturity doesn't occur on a business day

    boost::shared_ptr<Bond> zeroCpnBond1(new
        ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                       Date(20,December,2015),
                       Following,
                       100.0, Date(19,December,1985)));

    zeroCpnBond1->setPricingEngine(bondEngine);

    Real zeroCpnBondMktPrice1 = 70.436 ; // market price observed on 12th June 2007
    Real zeroCpnBondMktFullPrice1 = zeroCpnBondMktPrice1+zeroCpnBond1->accruedAmount();
    AssetSwap zeroCpnBondParAssetSwap1(payFixedRate,zeroCpnBond1,
                                       zeroCpnBondMktPrice1, iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(), parAssetSwap);
    Real zeroCpnBondParAssetSwapSpread1 = zeroCpnBondParAssetSwap1.fairSpread();
    AssetSwap zeroCpnBondMktAssetSwap1(payFixedRate,zeroCpnBond1,
                                       zeroCpnBondMktPrice1,iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(),mktAssetSwap);
    Real zeroCpnBondMktAssetSwapSpread1 = zeroCpnBondMktAssetSwap1.fairSpread();
    Real error7 = std::fabs(zeroCpnBondMktAssetSwapSpread1-
                            100*zeroCpnBondParAssetSwapSpread1/zeroCpnBondMktFullPrice1);

    if (error7>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << zeroCpnBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << zeroCpnBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error7
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
    // maturity occurs on a business day

    boost::shared_ptr<Bond> zeroCpnBond2(new
        ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                       Date(17,February,2028),
                       Following,
                       100.0, Date(17,February,1998)));

    zeroCpnBond2->setPricingEngine(bondEngine);

    // Real zeroCpnBondPrice2 = zeroCpnBond2->cleanPrice();
    Real zeroCpnBondMktPrice2 = 35.160 ; // market price observed on 12th June 2007
    Real zeroCpnBondMktFullPrice2 = zeroCpnBondMktPrice2+zeroCpnBond2->accruedAmount();
    AssetSwap zeroCpnBondParAssetSwap2(payFixedRate,zeroCpnBond2,
                                       zeroCpnBondMktPrice2, iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(), parAssetSwap);
    Real zeroCpnBondParAssetSwapSpread2 = zeroCpnBondParAssetSwap2.fairSpread();
    AssetSwap zeroCpnBondMktAssetSwap2(payFixedRate,zeroCpnBond2,
                                       zeroCpnBondMktPrice2,iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(),mktAssetSwap);
    Real zeroCpnBondMktAssetSwapSpread2 = zeroCpnBondMktAssetSwap2.fairSpread();
    Real error8 = std::fabs(zeroCpnBondMktAssetSwapSpread2-
                            100*zeroCpnBondParAssetSwapSpread2/zeroCpnBondMktFullPrice2);

    if (error8>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << zeroCpnBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << zeroCpnBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error8
                    << "\n  tolerance:             " << tolerance);
    }
}


void AssetSwapTest::testZSpread() {

    BOOST_MESSAGE(
    "Testing clean and dirty price with Zspread=0 return bond's theo prices..."
    );

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool inArrears = false;

    // Fixed bond (Isin: DE0001135275 DBR 4 01/04/37)
    // maturity doesn't occur on a business day

    Schedule fixedBondSchedule1(Date(4,January,2005),
                                Date(4,January,2037),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    boost::shared_ptr<Bond> fixedBond1(new
        FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule1,
                      std::vector<Rate>(1, 0.04),
                      ActualActual(ActualActual::ISDA), Following,
                      100.0, Date(4,January,2005)));

    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    fixedBond1->setPricingEngine(bondEngine);

    Real fixedBondImpliedValue1 = fixedBond1->cleanPrice();
    Date fixedBondSettlementDate1= fixedBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YC...
    Real fixedBondCleanPrice1= fixedBond1->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate1);
    Real fixedBondDirtyPrice1= fixedBond1->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate1);
    Real tolerance = 1.0e-13;
    Real error1 = std::fabs(fixedBondImpliedValue1-fixedBondCleanPrice1);
    if (error1>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedValue1
                    << "\n  par asset swap spread: " << fixedBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error1
                    << "\n  tolerance:             " << tolerance);
    }
    Real fixedBondImpliedDirty1 = fixedBondImpliedValue1+fixedBond1->accruedAmount();
    Real error2 = std::fabs(fixedBondImpliedDirty1-fixedBondDirtyPrice1);
    if (error2>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedDirty1
                    << "\n  par asset swap spread: " << fixedBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

    // Fixed bond (Isin: IT0006527060 IBRD 5 02/05/19)
    // maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    boost::shared_ptr<Bond> fixedBond2(new
        FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule2,
                      std::vector<Rate>(1, 0.05),
                      Thirty360(Thirty360::BondBasis), Following,
                      100.0, Date(5,February,2005)));

    fixedBond2->setPricingEngine(bondEngine);

    Real fixedBondImpliedValue2 = fixedBond2->cleanPrice();
    Date fixedBondSettlementDate2= fixedBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real fixedBondCleanPrice2= fixedBond2->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate2);
    Real fixedBondDirtyPrice2= fixedBond2->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual, //FIXME ??
         fixedBondSettlementDate2);
    Real error3 = std::fabs(fixedBondImpliedValue2-fixedBondCleanPrice2);
    if (error3>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedValue2
                    << "\n  par asset swap spread: " << fixedBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }
    Real fixedBondImpliedDirty2 = fixedBondImpliedValue2+fixedBond2->accruedAmount();
    Real error4 = std::fabs(fixedBondImpliedDirty2-fixedBondDirtyPrice2);
    if (error4>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedDirty2
                    << "\n  par asset swap spread: " << fixedBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN bond (Isin: IT0003543847 ISPIM 0 09/29/13)
    // maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);

    boost::shared_ptr<Bond> floatingBond1(new
        FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule1,
                         iborIndex_, Actual360(),
                         Following, fixingDays,
                         std::vector<Real>(1,1), std::vector<Spread>(1,0.0056),
                         std::vector<Rate>(), std::vector<Rate>(),
                         inArrears,
                         100.0, Date(29,September,2003)));

    floatingBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondImpliedValue1 = floatingBond1->cleanPrice();
    Date floatingBondSettlementDate1= floatingBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real floatingBondCleanPrice1= floatingBond1->cleanPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        fixedBondSettlementDate1);
    Real floatingBondDirtyPrice1= floatingBond1->dirtyPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        floatingBondSettlementDate1);
    Real error5 = std::fabs(floatingBondImpliedValue1-floatingBondCleanPrice1);
    if (error5>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedValue1
                    << "\n  par asset swap spread: " << floatingBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error5
                    << "\n  tolerance:             " << tolerance);
    }
    Real floatingBondImpliedDirty1 = floatingBondImpliedValue1+
                                     floatingBond1->accruedAmount();
    Real error6 = std::fabs(floatingBondImpliedDirty1-floatingBondDirtyPrice1);
    if (error6>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedDirty1
                    << "\n  par asset swap spread: " << floatingBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN bond (Isin: XS0090566539 COE 0 09/24/18)
    // maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    boost::shared_ptr<Bond> floatingBond2(new
        FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule2,
                         iborIndex_, Actual360(),
                         ModifiedFollowing, fixingDays,
                         std::vector<Real>(1,1), std::vector<Spread>(1,0.0025),
                         std::vector<Rate>(), std::vector<Rate>(),
                         inArrears,
                         100.0, Date(24,September,2004)));

    floatingBond2->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond2->cashflows(),pricer_);
    iborIndex_->addFixing(Date(22,March,2007), 0.04013);
    Real floatingBondImpliedValue2 = floatingBond2->cleanPrice();
    Date floatingBondSettlementDate2= floatingBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real floatingBondCleanPrice2= floatingBond2->cleanPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        fixedBondSettlementDate1);
    Real floatingBondDirtyPrice2= floatingBond2->dirtyPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        floatingBondSettlementDate2);
    Real error7 = std::fabs(floatingBondImpliedValue2-floatingBondCleanPrice2);
    if (error7>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedValue2
                    << "\n  par asset swap spread: " << floatingBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error7
                    << "\n  tolerance:             " << tolerance);
    }
    Real floatingBondImpliedDirty2 = floatingBondImpliedValue2+
                                     floatingBond2->accruedAmount();
    Real error8 = std::fabs(floatingBondImpliedDirty2-floatingBondDirtyPrice2);
    if (error8>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedDirty2
                    << "\n  par asset swap spread: " << floatingBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error8
                    << "\n  tolerance:             " << tolerance);
    }


    //// CMS bond (Isin: XS0228052402 CRDIT 0 8/22/20)
    //// maturity doesn't occur on a business day

    Schedule cmsBondSchedule1(Date(22,August,2005),
                              Date(22,August,2020),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    boost::shared_ptr<Bond> cmsBond1(new
        CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule1,
                    swapIndex_, Thirty360(),
                    Following, fixingDays,
                    std::vector<Real>(1,1.0), std::vector<Spread>(1,0.0),
                    std::vector<Rate>(1,0.055), std::vector<Rate>(1,0.025),
                    inArrears,
                    100.0, Date(22,August,2005)));

    cmsBond1->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond1->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(18,August,2006), 0.04158);
    Real cmsBondImpliedValue1 = cmsBond1->cleanPrice();
    Date cmsBondSettlementDate1= cmsBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real cmsBondCleanPrice1= cmsBond1->cleanPriceFromZSpread(spread_,
        Actual365Fixed(), compounding_, Annual,
        cmsBondSettlementDate1);
    Real cmsBondDirtyPrice1= cmsBond1->dirtyPriceFromZSpread(spread_,
        Actual365Fixed(), compounding_, Annual,
        fixedBondSettlementDate1);
    Real error9 = std::fabs(cmsBondImpliedValue1-cmsBondCleanPrice1);
    if (error9>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedValue1
                    << "\n  par asset swap spread: " << cmsBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error9
                    << "\n  tolerance:             " << tolerance);
    }
    Real cmsBondImpliedDirty1 = cmsBondImpliedValue1+cmsBond1->accruedAmount();
    Real error10 = std::fabs(cmsBondImpliedDirty1-cmsBondDirtyPrice1);
    if (error10>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedDirty1
                    << "\n  par asset swap spread: " << cmsBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error10
                    << "\n  tolerance:             " << tolerance);
    }

     // CMS bond (Isin: XS0218766664 ISPIM 0 5/6/15)
     // maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    boost::shared_ptr<Bond> cmsBond2(new
        CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule2,
                    swapIndex_, Thirty360(),
                    Following, fixingDays,
                    std::vector<Real>(1,0.84), std::vector<Spread>(1,0.0),
                    std::vector<Rate>(), std::vector<Rate>(),
                    inArrears,
                    100.0, Date(06,May,2005)));

    cmsBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondImpliedValue2 = cmsBond2->cleanPrice();
    Date cmsBondSettlementDate2= cmsBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real cmsBondCleanPrice2= cmsBond2->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         cmsBondSettlementDate2);
    Real cmsBondDirtyPrice2= cmsBond2->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate2);
    Real error11 = std::fabs(cmsBondImpliedValue2-cmsBondCleanPrice2);
    if (error11>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedValue2
                    << "\n  par asset swap spread: " << cmsBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error11
                    << "\n  tolerance:             " << tolerance);
    }
    Real cmsBondImpliedDirty2 = cmsBondImpliedValue2+cmsBond2->accruedAmount();
    Real error12 = std::fabs(cmsBondImpliedDirty2-cmsBondDirtyPrice2);
    if (error12>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedDirty2
                    << "\n  par asset swap spread: " << cmsBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error12
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero-Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
    // maturity doesn't occur on a business day

    boost::shared_ptr<Bond> zeroCpnBond1(new
        ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                       Date(20,December,2015),
                       Following,
                       100.0, Date(19,December,1985)));

    zeroCpnBond1->setPricingEngine(bondEngine);

    Real zeroCpnBondImpliedValue1 = zeroCpnBond1->cleanPrice();
    Date zeroCpnBondSettlementDate1= zeroCpnBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real zeroCpnBondCleanPrice1= zeroCpnBond1->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate1);
    Real zeroCpnBondDirtyPrice1= zeroCpnBond1->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate1);
    Real error13 = std::fabs(zeroCpnBondImpliedValue1-zeroCpnBondCleanPrice1);
    if (error13>tolerance) {
        BOOST_ERROR("wrong clean price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  zero cpn implied value: " << zeroCpnBondImpliedValue1
                    << "\n  zero cpn price: " << zeroCpnBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error13
                    << "\n  tolerance:             " << tolerance);
    }
    Real zeroCpnBondImpliedDirty1 = zeroCpnBondImpliedValue1+
                                    zeroCpnBond1->accruedAmount();
    Real error14 = std::fabs(zeroCpnBondImpliedDirty1-zeroCpnBondDirtyPrice1);
    if (error14>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's implied dirty price: " << zeroCpnBondImpliedDirty1
                    << "\n  bond's full price: " << zeroCpnBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error14
                    << "\n  tolerance:             " << tolerance);
    }
    // Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
    // maturity doesn't occur on a business day

    boost::shared_ptr<Bond> zeroCpnBond2(new
        ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                       Date(17,February,2028),
                       Following,
                       100.0, Date(17,February,1998)));

    zeroCpnBond2->setPricingEngine(bondEngine);

    Real zeroCpnBondImpliedValue2 = zeroCpnBond2->cleanPrice();
    Date zeroCpnBondSettlementDate2= zeroCpnBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real zeroCpnBondCleanPrice2= zeroCpnBond2->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate2);
    Real zeroCpnBondDirtyPrice2= zeroCpnBond2->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate2);
    Real error15 = std::fabs(zeroCpnBondImpliedValue2-zeroCpnBondCleanPrice2);
    if (error15>tolerance) {
        BOOST_ERROR("wrong clean price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  zero cpn implied value: " << zeroCpnBondImpliedValue2
                    << "\n  zero cpn price: " << zeroCpnBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error15
                    << "\n  tolerance:             " << tolerance);
    }
    Real zeroCpnBondImpliedDirty2 = zeroCpnBondImpliedValue2+
                                    zeroCpnBond2->accruedAmount();
    Real error16 = std::fabs(zeroCpnBondImpliedDirty2-zeroCpnBondDirtyPrice2);
    if (error16>tolerance) {
        BOOST_ERROR("wrong dirty price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's implied dirty price: " << zeroCpnBondImpliedDirty2
                    << "\n  bond's full price: " << zeroCpnBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error16
                    << "\n  tolerance:             " << tolerance);
    }
}


void AssetSwapTest::testGenericBondImplied() {

    BOOST_MESSAGE("Testing generic bond implied value against" 
                  " assetswap fair price with spread=0...");

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool payFixedRate = true;
    bool parAssetSwap = true;
    bool inArrears = false;

    // Fixed Underlying bond (Isin: DE0001135275 DBR 4 01/04/37)
    // maturity doesn't occur on a business day

    Schedule fixedBondSchedule1(Date(4,January,2005),
                                Date(4,January,2037),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg1 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule1,
                                     std::vector<Rate>(1, 0.04),
                                     ActualActual(ActualActual::ISDA),
                                     Following);
    Date fixedbondRedemption1 = bondCalendar.adjust(Date(4,January,2037),
                                                    Following);
    fixedBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption1)));
    boost::shared_ptr<Bond> fixedBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,Date(4,January,2005),
                            fixedBondLeg1));
    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    fixedBond1->setPricingEngine(bondEngine);
    Real fixedBondPrice1 = fixedBond1->cleanPrice();
    AssetSwap fixedBondAssetSwap1(payFixedRate,
                                  fixedBond1, fixedBondPrice1,
                                  iborIndex_, spread_, termStructure_,
                                  Schedule(),
                                  iborIndex_->dayCounter(),
                                  parAssetSwap);
    Real fixedBondAssetSwapPrice1 = fixedBondAssetSwap1.fairPrice();
    Real tolerance = 1.0e-13;
    Real error1 = std::fabs(fixedBondAssetSwapPrice1-fixedBondPrice1);

    if (error1>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << fixedBondPrice1
                    << "\n  asset swap fair price: " << fixedBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error1
                    << "\n  tolerance:             " << tolerance);
    }

    // Fixed Underlying bond (Isin: IT0006527060 IBRD 5 02/05/19)
    // maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg2 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule2,
                                     std::vector<Rate>(1, 0.05),
                                     Thirty360(Thirty360::BondBasis),
                                     Following);
    Date fixedbondRedemption2 = bondCalendar.adjust(Date(5,February,2019),
                                                    Following);
    fixedBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption2)));
    boost::shared_ptr<Bond> fixedBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(5,February,2005),fixedBondLeg2));
    fixedBond2->setPricingEngine(bondEngine);

    Real fixedBondPrice2 = fixedBond2->cleanPrice();
    AssetSwap fixedBondAssetSwap2(payFixedRate,
                                  fixedBond2, fixedBondPrice2,
                                  iborIndex_, spread_, termStructure_,
                                  Schedule(),
                                  iborIndex_->dayCounter(),
                                  parAssetSwap);
    Real fixedBondAssetSwapPrice2 = fixedBondAssetSwap2.fairPrice();
    Real error2 = std::fabs(fixedBondAssetSwapPrice2-fixedBondPrice2);

    if (error2>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << fixedBondPrice2
                    << "\n  asset swap fair price: " << fixedBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: IT0003543847 ISPIM 0 09/29/13)
    // maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);
    Leg floatingBondLeg1 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule1,
                                   iborIndex_,
                                   Actual360(),
                                   Following,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0056),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption1 = bondCalendar.adjust(Date(29,September,2013),
                                                       Following);
    floatingBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption1)));
    boost::shared_ptr<Bond> floatingBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(29,September,2003),floatingBondLeg1));
    floatingBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondPrice1 = floatingBond1->cleanPrice();
    AssetSwap floatingBondAssetSwap1(payFixedRate,
                                     floatingBond1, floatingBondPrice1,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real floatingBondAssetSwapPrice1 = floatingBondAssetSwap1.fairPrice();
    Real error3 = std::fabs(floatingBondAssetSwapPrice1-floatingBondPrice1);

    if (error3>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for floater:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << floatingBondPrice1
                    << "\n  asset swap fair price: " << floatingBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: XS0090566539 COE 0 09/24/18)
    // maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    Leg floatingBondLeg2 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule2,
                                   iborIndex_,
                                   Actual360(),
                                   ModifiedFollowing,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0025),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption2 = bondCalendar.adjust(Date(24,September,2018),
                                                       ModifiedFollowing);
    floatingBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption2)));
    boost::shared_ptr<Bond> floatingBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(24,September,2004),floatingBondLeg2));
    floatingBond2->setPricingEngine(bondEngine);
    setCouponPricer(floatingBond2->cashflows(),pricer_);
    iborIndex_->addFixing(Date(22,March,2007), 0.04013);
    Real currentCoupon=0.04013+0.0025;
    Real floatingCurrentCoupon= floatingBond2->currentCoupon();
    Real error4= std::fabs(floatingCurrentCoupon-currentCoupon);
    if (error4>tolerance) {
        BOOST_ERROR("wrong current coupon is returned for floater bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's calculated current coupon:      " << currentCoupon
                    << "\n  current coupon asked to the bond: " << floatingCurrentCoupon
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

    Real floatingBondPrice2 = floatingBond2->cleanPrice();
    AssetSwap floatingBondAssetSwap2(payFixedRate,
                                     floatingBond2, floatingBondPrice2,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real floatingBondAssetSwapPrice2 = floatingBondAssetSwap2.fairPrice();
    Real error5 = std::fabs(floatingBondAssetSwapPrice2-floatingBondPrice2);

    if (error5>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for floater:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << floatingBondPrice2
                    << "\n  asset swap fair price: " << floatingBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error5
                    << "\n  tolerance:             " << tolerance);
    }

    // CMS Underlying bond (Isin: XS0228052402 CRDIT 0 8/22/20)
    // maturity doesn't occur on a business day

    Schedule cmsBondSchedule1(Date(22,August,2005),
                              Date(22,August,2020),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg1 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule1,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,1.0), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(1,0.055), 
                             std::vector<Rate>(1,0.025),
                             inArrears);
    Date cmsbondRedemption1 = bondCalendar.adjust(Date(22,August,2020),
                                                  Following);
    cmsBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption1)));
    boost::shared_ptr<Bond> cmsBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(22,August,2005),cmsBondLeg1));
    cmsBond1->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond1->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(18,August,2006), 0.04158);
    Real cmsBondPrice1 = cmsBond1->cleanPrice();
    AssetSwap cmsBondAssetSwap1(payFixedRate,
                                cmsBond1, cmsBondPrice1,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real cmsBondAssetSwapPrice1 = cmsBondAssetSwap1.fairPrice();
    Real error6 = std::fabs(cmsBondAssetSwapPrice1-cmsBondPrice1);

    if (error6>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << cmsBondPrice1
                    << "\n  asset swap fair price: " << cmsBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

     // CMS Underlying bond (Isin: XS0218766664 ISPIM 0 5/6/15)
     // maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg2 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule2,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,0.84), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(), 
                             std::vector<Rate>(),
                             inArrears);
    Date cmsbondRedemption2 = bondCalendar.adjust(Date(06,May,2015),
                                                  Following);
    cmsBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption2)));
    boost::shared_ptr<Bond> cmsBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(06,May,2005),cmsBondLeg2));
    cmsBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondPrice2 = cmsBond2->cleanPrice();
    AssetSwap cmsBondAssetSwap2(payFixedRate,
                                cmsBond2, cmsBondPrice2,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real cmsBondAssetSwapPrice2 = cmsBondAssetSwap2.fairPrice();
    Real error7 = std::fabs(cmsBondAssetSwapPrice2-cmsBondPrice2);

    if (error7>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << cmsBondPrice2
                    << "\n  asset swap fair price: " << cmsBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error7
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
    // maturity doesn't occur on a business day

    Date zerocpbondRedemption1 = bondCalendar.adjust(Date(20,December,2015),
                                                      Following);
    Leg zeroCpnBondLeg1 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption1)));
    boost::shared_ptr<Bond> zeroCpnBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(19,December,1985),zeroCpnBondLeg1));

    zeroCpnBond1->setPricingEngine(bondEngine);

    Real zeroCpnBondPrice1 = zeroCpnBond1->cleanPrice();
    AssetSwap zeroCpnAssetSwap1(payFixedRate,
                                zeroCpnBond1, zeroCpnBondPrice1,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real zeroCpnBondAssetSwapPrice1 = zeroCpnAssetSwap1.fairPrice();
    Real error8 = std::fabs(zeroCpnBondAssetSwapPrice1-zeroCpnBondPrice1);

    if (error8>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << zeroCpnBondPrice1
                    << "\n  asset swap fair price: " << zeroCpnBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error8
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
    // maturity occurs on a business day

    Date zerocpbondRedemption2 = bondCalendar.adjust(Date(17,February,2028),
                                                      Following);
    Leg zeroCpnBondLeg2 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption2)));
    boost::shared_ptr<Bond> zeroCpnBond2(new
           Bond(settlementDays, bondCalendar, faceAmount_,
                Date(17,February,1998),zeroCpnBondLeg2));

    zeroCpnBond2->setPricingEngine(bondEngine);

    Real zeroCpnBondPrice2 = zeroCpnBond2->cleanPrice();
    AssetSwap zeroCpnAssetSwap2(payFixedRate,
                                zeroCpnBond2, zeroCpnBondPrice2,
                                iborIndex_, spread_, termStructure_,
                                Schedule(),
                                iborIndex_->dayCounter(),
                                parAssetSwap);
    Real zeroCpnBondAssetSwapPrice2 = zeroCpnAssetSwap2.fairPrice();
    Real error9 = std::fabs(cmsBondAssetSwapPrice2-cmsBondPrice2);

    if (error9>tolerance) {
        BOOST_ERROR("wrong zero spread asset swap price for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's clean price:      " << zeroCpnBondPrice2
                    << "\n  asset swap fair price: " << zeroCpnBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error9
                    << "\n  tolerance:             " << tolerance);
    }
}


void AssetSwapTest::testMASWWithGenericBond() {

    BOOST_MESSAGE("Testing market asset swap vs par asset swap " 
        "with generic bond...");

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool payFixedRate = true;
    bool parAssetSwap = true;
    bool mktAssetSwap = false;
    bool inArrears = false;

    // Fixed Underlying bond (Isin: DE0001135275 DBR 4 01/04/37)
    // maturity doesn't occur on a business day

    Schedule fixedBondSchedule1(Date(4,January,2005),
                                Date(4,January,2037),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg1 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule1,
                                     std::vector<Rate>(1, 0.04),
                                     ActualActual(ActualActual::ISDA),
                                     Following);
    Date fixedbondRedemption1 = bondCalendar.adjust(Date(4,January,2037),
                                                    Following);
    fixedBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption1)));
    boost::shared_ptr<Bond> fixedBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(4,January,2005), fixedBondLeg1));
    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    fixedBond1->setPricingEngine(bondEngine);

    Real fixedBondMktPrice1 = 89.22 ; // market price observed on 7th June 2007
    Real fixedBondMktFullPrice1=fixedBondMktPrice1+fixedBond1->accruedAmount();
    AssetSwap fixedBondParAssetSwap1(payFixedRate,
                                     fixedBond1, fixedBondMktPrice1,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real fixedBondParAssetSwapSpread1 = fixedBondParAssetSwap1.fairSpread();
    AssetSwap fixedBondMktAssetSwap1(payFixedRate,
                                     fixedBond1, fixedBondMktPrice1,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     mktAssetSwap);
    Real fixedBondMktAssetSwapSpread1 = fixedBondMktAssetSwap1.fairSpread();

    Real tolerance = 1.0e-13;
    Real error1 = std::fabs(fixedBondMktAssetSwapSpread1-
                            100*fixedBondParAssetSwapSpread1/fixedBondMktFullPrice1);

    if (error1>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << fixedBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error1
                    << "\n  tolerance:             " << tolerance);
    }

    // Fixed Underlying bond (Isin: IT0006527060 IBRD 5 02/05/19)
    // maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg2 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule2,
                                     std::vector<Rate>(1, 0.05),
                                     Thirty360(Thirty360::BondBasis),
                                     Following);
    Date fixedbondRedemption2 = bondCalendar.adjust(Date(5,February,2019),
                                                    Following);
    fixedBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption2)));
    boost::shared_ptr<Bond> fixedBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(5,February,2005),fixedBondLeg2));
    fixedBond2->setPricingEngine(bondEngine);

    Real fixedBondMktPrice2 = 99.98 ; // market price observed on 7th June 2007
    Real fixedBondMktFullPrice2=fixedBondMktPrice2+fixedBond2->accruedAmount();
    AssetSwap fixedBondParAssetSwap2(payFixedRate,
                                     fixedBond2, fixedBondMktPrice2,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    Real fixedBondParAssetSwapSpread2 = fixedBondParAssetSwap2.fairSpread();
    AssetSwap fixedBondMktAssetSwap2(payFixedRate,
                                     fixedBond2, fixedBondMktPrice2,
                                     iborIndex_, spread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     mktAssetSwap);
    Real fixedBondMktAssetSwapSpread2 = fixedBondMktAssetSwap2.fairSpread();
    Real error2 = std::fabs(fixedBondMktAssetSwapSpread2-
                            100*fixedBondParAssetSwapSpread2/fixedBondMktFullPrice2);

    if (error2>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << fixedBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: IT0003543847 ISPIM 0 09/29/13)
    // maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);
    Leg floatingBondLeg1 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule1,
                                   iborIndex_,
                                   Actual360(),
                                   Following,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0056),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption1 = bondCalendar.adjust(Date(29,September,2013),
                                                       Following);
    floatingBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption1)));
    boost::shared_ptr<Bond> floatingBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(29,September,2003),floatingBondLeg1));

    floatingBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondMktPrice1 = 101.64 ; // market price observed on 7th June 2007
    Real floatingBondMktFullPrice1 = floatingBondMktPrice1+floatingBond1->accruedAmount();
    AssetSwap floatingBondParAssetSwap1(payFixedRate,
                                        floatingBond1, floatingBondMktPrice1,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        parAssetSwap);
    Real floatingBondParAssetSwapSpread1 = floatingBondParAssetSwap1.fairSpread();
    AssetSwap floatingBondMktAssetSwap1(payFixedRate,
                                        floatingBond1, floatingBondMktPrice1,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        mktAssetSwap);
    Real floatingBondMktAssetSwapSpread1 = floatingBondMktAssetSwap1.fairSpread();
    Real error3 = std::fabs(floatingBondMktAssetSwapSpread1-
                            100*floatingBondParAssetSwapSpread1/floatingBondMktFullPrice1);

    if (error3>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for floating bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << floatingBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN Underlying bond (Isin: XS0090566539 COE 0 09/24/18)
    // maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    Leg floatingBondLeg2 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule2,
                                   iborIndex_,
                                   Actual360(),
                                   ModifiedFollowing,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0025),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption2 = bondCalendar.adjust(Date(24,September,2018),
                                                       ModifiedFollowing);
    floatingBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption2)));
    boost::shared_ptr<Bond> floatingBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(24,September,2004),floatingBondLeg2));
    floatingBond2->setPricingEngine(bondEngine);
    setCouponPricer(floatingBond2->cashflows(),pricer_);
    iborIndex_->addFixing(Date(22,March,2007), 0.04013);
    Real floatingBondMktPrice2 = 101.248 ; // market price observed on 7th June 2007
    Real floatingBondMktFullPrice2 = floatingBondMktPrice2+floatingBond2->accruedAmount();
    AssetSwap floatingBondParAssetSwap2(payFixedRate,
                                        floatingBond2, floatingBondMktPrice2,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        parAssetSwap);
    Spread floatingBondParAssetSwapSpread2 = floatingBondParAssetSwap2.fairSpread();
    AssetSwap floatingBondMktAssetSwap2(payFixedRate,
                                        floatingBond2, floatingBondMktPrice2,
                                        iborIndex_, spread_, termStructure_,
                                        Schedule(),
                                        iborIndex_->dayCounter(),
                                        mktAssetSwap);
    Real floatingBondMktAssetSwapSpread2 = floatingBondMktAssetSwap2.fairSpread();
    Real error4 = std::fabs(floatingBondMktAssetSwapSpread2-
                            100*floatingBondParAssetSwapSpread2/floatingBondMktFullPrice2);

    if (error4>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for floating bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << floatingBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

    // CMS Underlying bond (Isin: XS0228052402 CRDIT 0 8/22/20)
    // maturity doesn't occur on a business day

    Schedule cmsBondSchedule1(Date(22,August,2005),
                              Date(22,August,2020),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg1 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule1,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,1.0), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(1,0.055), 
                             std::vector<Rate>(1,0.025),
                             inArrears);
    Date cmsbondRedemption1 = bondCalendar.adjust(Date(22,August,2020),
                                                  Following);
    cmsBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption1)));
    boost::shared_ptr<Bond> cmsBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(22,August,2005),cmsBondLeg1));
    cmsBond1->setPricingEngine(bondEngine);

    cmsBond1->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond1->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(18,August,2006), 0.04158);
    Real cmsBondMktPrice1 = 88.45 ; // market price observed on 7th June 2007
    Real cmsBondMktFullPrice1 = cmsBondMktPrice1+cmsBond1->accruedAmount();
    AssetSwap cmsBondParAssetSwap1(payFixedRate,
                                   cmsBond1, cmsBondMktPrice1,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   parAssetSwap);
    Real cmsBondParAssetSwapSpread1 = cmsBondParAssetSwap1.fairSpread();
    AssetSwap cmsBondMktAssetSwap1(payFixedRate,
                                   cmsBond1, cmsBondMktPrice1,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   mktAssetSwap);
    Real cmsBondMktAssetSwapSpread1 = cmsBondMktAssetSwap1.fairSpread();
    Real error5 = std::fabs(cmsBondMktAssetSwapSpread1-
                            100*cmsBondParAssetSwapSpread1/cmsBondMktFullPrice1);

    if (error5>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << cmsBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error5
                    << "\n  tolerance:             " << tolerance);
    }

     // CMS Underlying bond (Isin: XS0218766664 ISPIM 0 5/6/15)
     // maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg2 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule2,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,0.84), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(), 
                             std::vector<Rate>(),
                             inArrears);
    Date cmsbondRedemption2 = bondCalendar.adjust(Date(06,May,2015),
                                                  Following);
    cmsBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption2)));
    boost::shared_ptr<Bond> cmsBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(06,May,2005),cmsBondLeg2));

    cmsBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondMktPrice2 = 94.08 ; // market price observed on 7th June 2007
    Real cmsBondMktFullPrice2 = cmsBondMktPrice2+cmsBond2->accruedAmount();
    AssetSwap cmsBondParAssetSwap2(payFixedRate,
                                   cmsBond2, cmsBondMktPrice2,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   parAssetSwap);
    Spread cmsBondParAssetSwapSpread2 = cmsBondParAssetSwap2.fairSpread();
    AssetSwap cmsBondMktAssetSwap2(payFixedRate,
                                   cmsBond2, cmsBondMktPrice2,
                                   iborIndex_, spread_, termStructure_,
                                   Schedule(),
                                   iborIndex_->dayCounter(),
                                   mktAssetSwap);
    Real cmsBondMktAssetSwapSpread2 = cmsBondMktAssetSwap2.fairSpread();
    Real error6 = std::fabs(cmsBondMktAssetSwapSpread2-
                            100*cmsBondParAssetSwapSpread2/cmsBondMktFullPrice2);

    if (error6>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << cmsBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
    // maturity doesn't occur on a business day

    Date zerocpbondRedemption1 = bondCalendar.adjust(Date(20,December,2015),
                                                      Following);
    Leg zeroCpnBondLeg1 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption1)));
    boost::shared_ptr<Bond> zeroCpnBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(19,December,1985),zeroCpnBondLeg1));

    zeroCpnBond1->setPricingEngine(bondEngine);

    Real zeroCpnBondMktPrice1 = 70.436 ; // market price observed on 12th June 2007
    Real zeroCpnBondMktFullPrice1 = zeroCpnBondMktPrice1+zeroCpnBond1->accruedAmount();
    AssetSwap zeroCpnBondParAssetSwap1(payFixedRate,zeroCpnBond1,
                                       zeroCpnBondMktPrice1, iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(), parAssetSwap);
    Real zeroCpnBondParAssetSwapSpread1 = zeroCpnBondParAssetSwap1.fairSpread();
    AssetSwap zeroCpnBondMktAssetSwap1(payFixedRate,zeroCpnBond1,
                                       zeroCpnBondMktPrice1,iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(),mktAssetSwap);
    Real zeroCpnBondMktAssetSwapSpread1 = zeroCpnBondMktAssetSwap1.fairSpread();
    Real error7 = std::fabs(zeroCpnBondMktAssetSwapSpread1-
                            100*zeroCpnBondParAssetSwapSpread1/zeroCpnBondMktFullPrice1);

    if (error7>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << zeroCpnBondMktAssetSwapSpread1
                    << "\n  par asset swap spread: " << zeroCpnBondParAssetSwapSpread1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error7
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
    // maturity occurs on a business day

    Date zerocpbondRedemption2 = bondCalendar.adjust(Date(17,February,2028),
                                                      Following);
    Leg zeroCpnBondLeg2 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption2)));
    boost::shared_ptr<Bond> zeroCpnBond2(new
           Bond(settlementDays, bondCalendar, faceAmount_,
                Date(17,February,1998),zeroCpnBondLeg2));

    zeroCpnBond2->setPricingEngine(bondEngine);

    // Real zeroCpnBondPrice2 = zeroCpnBond2->cleanPrice();
    Real zeroCpnBondMktPrice2 = 35.160 ; // market price observed on 12th June 2007
    Real zeroCpnBondMktFullPrice2 = zeroCpnBondMktPrice2+zeroCpnBond2->accruedAmount();
    AssetSwap zeroCpnBondParAssetSwap2(payFixedRate,zeroCpnBond2,
                                       zeroCpnBondMktPrice2, iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(), parAssetSwap);
    Real zeroCpnBondParAssetSwapSpread2 = zeroCpnBondParAssetSwap2.fairSpread();
    AssetSwap zeroCpnBondMktAssetSwap2(payFixedRate,zeroCpnBond2,
                                       zeroCpnBondMktPrice2,iborIndex_,
                                       spread_, termStructure_,
                                       Schedule(),
                                       iborIndex_->dayCounter(),mktAssetSwap);
    Real zeroCpnBondMktAssetSwapSpread2 = zeroCpnBondMktAssetSwap2.fairSpread();
    Real error8 = std::fabs(zeroCpnBondMktAssetSwapSpread2-
                            100*zeroCpnBondParAssetSwapSpread2/zeroCpnBondMktFullPrice2);

    if (error8>tolerance) {
        BOOST_ERROR("wrong asset swap spreads for zero cpn bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << zeroCpnBondMktAssetSwapSpread2
                    << "\n  par asset swap spread: " << zeroCpnBondParAssetSwapSpread2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error8
                    << "\n  tolerance:             " << tolerance);
    }
}


void AssetSwapTest::testZSpreadWithGenericBond() {

    BOOST_MESSAGE(
    "Testing clean and dirty price with Zspread=0 return "
    "generic bond's theo prices..."
        );

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool inArrears = false;

    // Fixed bond (Isin: DE0001135275 DBR 4 01/04/37)
    // maturity doesn't occur on a business day

    Schedule fixedBondSchedule1(Date(4,January,2005),
                                Date(4,January,2037),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg1 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule1,
                                     std::vector<Rate>(1, 0.04),
                                     ActualActual(ActualActual::ISDA),
                                     Following);
    Date fixedbondRedemption1 = bondCalendar.adjust(Date(4,January,2037),
                                                    Following);
    fixedBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption1)));
    boost::shared_ptr<Bond> fixedBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,Date(4,January,2005),
                            fixedBondLeg1));


    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    fixedBond1->setPricingEngine(bondEngine);

    Real fixedBondImpliedValue1 = fixedBond1->cleanPrice();
    Date fixedBondSettlementDate1= fixedBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real fixedBondCleanPrice1= fixedBond1->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate1);
    Real fixedBondDirtyPrice1= fixedBond1->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate1);
    Real tolerance = 1.0e-13;
    Real error1 = std::fabs(fixedBondImpliedValue1-fixedBondCleanPrice1);
    if (error1>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedValue1
                    << "\n  par asset swap spread: " << fixedBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error1
                    << "\n  tolerance:             " << tolerance);
    }
    Real fixedBondImpliedDirty1 = fixedBondImpliedValue1+fixedBond1->accruedAmount();
    Real error2 = std::fabs(fixedBondImpliedDirty1-fixedBondDirtyPrice1);
    if (error2>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedDirty1
                    << "\n  par asset swap spread: " << fixedBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

    // Fixed bond (Isin: IT0006527060 IBRD 5 02/05/19)
    // maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg2 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule2,
                                     std::vector<Rate>(1, 0.05),
                                     Thirty360(Thirty360::BondBasis),
                                     Following);
    Date fixedbondRedemption2 = bondCalendar.adjust(Date(5,February,2019),
                                                    Following);
    fixedBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption2)));
    boost::shared_ptr<Bond> fixedBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(5,February,2005),fixedBondLeg2));

    fixedBond2->setPricingEngine(bondEngine);

    Real fixedBondImpliedValue2 = fixedBond2->cleanPrice();
    Date fixedBondSettlementDate2= fixedBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve

    Real fixedBondCleanPrice2= fixedBond2->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate2);
    Real fixedBondDirtyPrice2= fixedBond2->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual, //FIXME ??
         fixedBondSettlementDate2);
    Real error3 = std::fabs(fixedBondImpliedValue2-fixedBondCleanPrice2);
    if (error3>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedValue2
                    << "\n  par asset swap spread: " << fixedBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }
    Real fixedBondImpliedDirty2 = fixedBondImpliedValue2+fixedBond2->accruedAmount();
    Real error4 = std::fabs(fixedBondImpliedDirty2-fixedBondDirtyPrice2);
    if (error4>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << fixedBondImpliedDirty2
                    << "\n  par asset swap spread: " << fixedBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN bond (Isin: IT0003543847 ISPIM 0 09/29/13)
    // maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);

    Leg floatingBondLeg1 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule1,
                                   iborIndex_,
                                   Actual360(),
                                   Following,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0056),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption1 = bondCalendar.adjust(Date(29,September,2013),
                                                       Following);
    floatingBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption1)));
    boost::shared_ptr<Bond> floatingBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(29,September,2003),floatingBondLeg1));

    floatingBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondImpliedValue1 = floatingBond1->cleanPrice();
    Date floatingBondSettlementDate1= floatingBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real floatingBondCleanPrice1= floatingBond1->cleanPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        fixedBondSettlementDate1);
    Real floatingBondDirtyPrice1= floatingBond1->dirtyPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        floatingBondSettlementDate1);
    Real error5 = std::fabs(floatingBondImpliedValue1-floatingBondCleanPrice1);
    if (error5>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedValue1
                    << "\n  par asset swap spread: " << floatingBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error5
                    << "\n  tolerance:             " << tolerance);
    }
    Real floatingBondImpliedDirty1 = floatingBondImpliedValue1+
                                     floatingBond1->accruedAmount();
    Real error6 = std::fabs(floatingBondImpliedDirty1-floatingBondDirtyPrice1);
    if (error6>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedDirty1
                    << "\n  par asset swap spread: " << floatingBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN bond (Isin: XS0090566539 COE 0 09/24/18)
    // maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    Leg floatingBondLeg2 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule2,
                                   iborIndex_,
                                   Actual360(),
                                   ModifiedFollowing,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0025),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption2 = bondCalendar.adjust(Date(24,September,2018),
                                                       ModifiedFollowing);
    floatingBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption2)));
    boost::shared_ptr<Bond> floatingBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(24,September,2004),floatingBondLeg2));

    floatingBond2->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond2->cashflows(),pricer_);
    iborIndex_->addFixing(Date(22,March,2007), 0.04013);
    Real floatingBondImpliedValue2 = floatingBond2->cleanPrice();
    Date floatingBondSettlementDate2= floatingBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real floatingBondCleanPrice2= floatingBond2->cleanPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        fixedBondSettlementDate1);
    Real floatingBondDirtyPrice2= floatingBond2->dirtyPriceFromZSpread(
        spread_, Actual365Fixed(), compounding_, Semiannual,
        floatingBondSettlementDate2);
    Real error7 = std::fabs(floatingBondImpliedValue2-floatingBondCleanPrice2);
    if (error7>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedValue2
                    << "\n  par asset swap spread: " << floatingBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error7
                    << "\n  tolerance:             " << tolerance);
    }
    Real floatingBondImpliedDirty2 = floatingBondImpliedValue2+
                                     floatingBond2->accruedAmount();
    Real error8 = std::fabs(floatingBondImpliedDirty2-floatingBondDirtyPrice2);
    if (error8>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << floatingBondImpliedDirty2
                    << "\n  par asset swap spread: " << floatingBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error8
                    << "\n  tolerance:             " << tolerance);
    }


    //// CMS bond (Isin: XS0228052402 CRDIT 0 8/22/20)
    //// maturity doesn't occur on a business day

    Schedule cmsBondSchedule1(Date(22,August,2005),
                              Date(22,August,2020),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg1 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule1,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,1.0), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(1,0.055), 
                             std::vector<Rate>(1,0.025),
                             inArrears);
    Date cmsbondRedemption1 = bondCalendar.adjust(Date(22,August,2020),
                                                  Following);
    cmsBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption1)));
    boost::shared_ptr<Bond> cmsBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(22,August,2005),cmsBondLeg1));

    cmsBond1->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond1->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(18,August,2006), 0.04158);
    Real cmsBondImpliedValue1 = cmsBond1->cleanPrice();
    Date cmsBondSettlementDate1= cmsBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real cmsBondCleanPrice1= cmsBond1->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         cmsBondSettlementDate1);
    Real cmsBondDirtyPrice1= cmsBond1->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate1);
    Real error9 = std::fabs(cmsBondImpliedValue1-cmsBondCleanPrice1);
    if (error9>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedValue1
                    << "\n  par asset swap spread: " << cmsBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error9
                    << "\n  tolerance:             " << tolerance);
    }
    Real cmsBondImpliedDirty1 = cmsBondImpliedValue1+cmsBond1->accruedAmount();
    Real error10 = std::fabs(cmsBondImpliedDirty1-cmsBondDirtyPrice1);
    if (error10>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedDirty1
                    << "\n  par asset swap spread: " << cmsBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error10
                    << "\n  tolerance:             " << tolerance);
    }

     // CMS bond (Isin: XS0218766664 ISPIM 0 5/6/15)
     // maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg2 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule2,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,0.84), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(), 
                             std::vector<Rate>(),
                             inArrears);
    Date cmsbondRedemption2 = bondCalendar.adjust(Date(06,May,2015),
                                                  Following);
    cmsBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption2)));
    boost::shared_ptr<Bond> cmsBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(06,May,2005),cmsBondLeg2));

    cmsBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondImpliedValue2 = cmsBond2->cleanPrice();
    Date cmsBondSettlementDate2= cmsBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real cmsBondCleanPrice2= cmsBond2->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         cmsBondSettlementDate2);
    Real cmsBondDirtyPrice2= cmsBond2->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         fixedBondSettlementDate2);
    Real error11 = std::fabs(cmsBondImpliedValue2-cmsBondCleanPrice2);
    if (error11>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedValue2
                    << "\n  par asset swap spread: " << cmsBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error11
                    << "\n  tolerance:             " << tolerance);
    }
    Real cmsBondImpliedDirty2 = cmsBondImpliedValue2+cmsBond2->accruedAmount();
    Real error12 = std::fabs(cmsBondImpliedDirty2-cmsBondDirtyPrice2);
    if (error12>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  market asset swap spread: " << cmsBondImpliedDirty2
                    << "\n  par asset swap spread: " << cmsBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error12
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero-Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
    // maturity doesn't occur on a business day

    Date zerocpbondRedemption1 = bondCalendar.adjust(Date(20,December,2015),
                                                      Following);
    Leg zeroCpnBondLeg1 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption1)));
    boost::shared_ptr<Bond> zeroCpnBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(19,December,1985),zeroCpnBondLeg1));

    zeroCpnBond1->setPricingEngine(bondEngine);

    Real zeroCpnBondImpliedValue1 = zeroCpnBond1->cleanPrice();
    Date zeroCpnBondSettlementDate1= zeroCpnBond1->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real zeroCpnBondCleanPrice1= zeroCpnBond1->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate1);
    Real zeroCpnBondDirtyPrice1= zeroCpnBond1->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate1);
    Real error13 = std::fabs(zeroCpnBondImpliedValue1-zeroCpnBondCleanPrice1);
    if (error13>tolerance) {
        BOOST_ERROR("wrong clean price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  zero cpn implied value: " << zeroCpnBondImpliedValue1
                    << "\n  zero cpn price: " << zeroCpnBondCleanPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error13
                    << "\n  tolerance:             " << tolerance);
    }
    Real zeroCpnBondImpliedDirty1 = zeroCpnBondImpliedValue1+
                                    zeroCpnBond1->accruedAmount();
    Real error14 = std::fabs(zeroCpnBondImpliedDirty1-zeroCpnBondDirtyPrice1);
    if (error14>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's implied dirty price: " << zeroCpnBondImpliedDirty1
                    << "\n  bond's full price: " << zeroCpnBondDirtyPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error14
                    << "\n  tolerance:             " << tolerance);
    }
    // Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
    // maturity doesn't occur on a business day

    Date zerocpbondRedemption2 = bondCalendar.adjust(Date(17,February,2028),
                                                      Following);
    Leg zeroCpnBondLeg2 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption2)));
    boost::shared_ptr<Bond> zeroCpnBond2(new
           Bond(settlementDays, bondCalendar, faceAmount_,
                Date(17,February,1998),zeroCpnBondLeg2));

    zeroCpnBond2->setPricingEngine(bondEngine);

    Real zeroCpnBondImpliedValue2 = zeroCpnBond2->cleanPrice();
    Date zeroCpnBondSettlementDate2= zeroCpnBond2->settlementDate();
    // standard market conventions: bond's frequency + coumpounding and daycounter of the YC...
    // bond's frequency + coumpounding and daycounter of the YieldCurve
    Real zeroCpnBondCleanPrice2= zeroCpnBond2->cleanPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate2);
    Real zeroCpnBondDirtyPrice2= zeroCpnBond2->dirtyPriceFromZSpread(spread_,
         Actual365Fixed(), compounding_, Annual,
         zeroCpnBondSettlementDate2);
    Real error15 = std::fabs(zeroCpnBondImpliedValue2-zeroCpnBondCleanPrice2);
    if (error15>tolerance) {
        BOOST_ERROR("wrong clean price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  zero cpn implied value: " << zeroCpnBondImpliedValue2
                    << "\n  zero cpn price: " << zeroCpnBondCleanPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error15
                    << "\n  tolerance:             " << tolerance);
    }
    Real zeroCpnBondImpliedDirty2 = zeroCpnBondImpliedValue2+
                                    zeroCpnBond2->accruedAmount();
    Real error16 = std::fabs(zeroCpnBondImpliedDirty2-zeroCpnBondDirtyPrice2);
    if (error16>tolerance) {
        BOOST_ERROR("wrong dirty price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  bond's implied dirty price: " << zeroCpnBondImpliedDirty2
                    << "\n  bond's full price: " << zeroCpnBondDirtyPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error16
                    << "\n  tolerance:             " << tolerance);
    }
}


void AssetSwapTest::testSpecializedBondVsGenericBond() {

    BOOST_MESSAGE(
    "Testing theo clean and dirty prices for specialized bond are equal to"
    " theo clean and dirty prices for equivalent generic bond..."
        );

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool inArrears = false;

    // Fixed bond (Isin: DE0001135275 DBR 4 01/04/37)
    // maturity doesn't occur on a business day

    Schedule fixedBondSchedule1(Date(4,January,2005),
                                Date(4,January,2037),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg1 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule1,
                                     std::vector<Rate>(1, 0.04),
                                     ActualActual(ActualActual::ISDA),
                                     Following);
    Date fixedbondRedemption1 = bondCalendar.adjust(Date(4,January,2037),
                                                    Following);
    fixedBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption1)));

    // specialized fixed rate bond 
    boost::shared_ptr<Bond> fixedBond1(new
        FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule1,
                      std::vector<Rate>(1, 0.04),
                      ActualActual(ActualActual::ISDA), Following,
                      100.0, Date(4,January,2005) ));
    // equivalent generic bond given fixed leg
    boost::shared_ptr<Bond> fixedGenericBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,Date(4,January,2005),
                            fixedBondLeg1));

    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    fixedBond1->setPricingEngine(bondEngine);
    fixedGenericBond1->setPricingEngine(bondEngine);

    Real fixedBondTheoValue1 = fixedBond1->cleanPrice();
    Real fixedGenericBondTheoValue1 = fixedGenericBond1->cleanPrice();
    Real tolerance = 1.0e-13;
    Real error1 = std::fabs(fixedBondTheoValue1-fixedGenericBondTheoValue1);
    if (error1>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized fixed rate bond's theo clean price: " 
                    << fixedBondTheoValue1
                    << "\n  generic equivalent bond's theo clean price: " 
                    << fixedGenericBondTheoValue1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error1
                    << "\n  tolerance:             " << tolerance);
    }
    Real fixedBondTheoDirty1 = fixedBondTheoValue1+fixedBond1->accruedAmount();
    Real fixedGenericTheoDirty1 = fixedGenericBondTheoValue1+
                                  fixedGenericBond1->accruedAmount();
    Real error2 = std::fabs(fixedBondTheoDirty1-fixedGenericTheoDirty1);
    if (error2>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized fixed rate bond's theo dirty price: " 
                    << fixedBondTheoDirty1
                    << "\n  generic equivalent bond's theo dirty price: " 
                    << fixedGenericTheoDirty1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

    // Fixed bond (Isin: IT0006527060 IBRD 5 02/05/19)
    // maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg2 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule2,
                                     std::vector<Rate>(1, 0.05),
                                     Thirty360(Thirty360::BondBasis),
                                     Following);
    Date fixedbondRedemption2 = bondCalendar.adjust(Date(5,February,2019),
                                                    Following);
    fixedBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption2)));
    // specialized fixed rate bond 
    boost::shared_ptr<Bond> fixedBond2(new
         FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule2,
                      std::vector<Rate>(1, 0.05),
                      Thirty360(Thirty360::BondBasis), Following,
                      100.0, Date(5,February,2005)));
    // equivalent generic bond given fixed leg
    boost::shared_ptr<Bond> fixedGenericBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(5,February,2005),fixedBondLeg2));

    fixedBond2->setPricingEngine(bondEngine);
    fixedGenericBond2->setPricingEngine(bondEngine);

    Real fixedBondTheoValue2 = fixedBond2->cleanPrice();
    Real fixedGenericBondTheoValue2 = fixedGenericBond2->cleanPrice();

    Real error3 = std::fabs(fixedBondTheoValue2-fixedGenericBondTheoValue2);
    if (error3>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized fixed rate bond's theo clean price: " 
                    << fixedBondTheoValue2
                    << "\n  generic equivalent bond's theo clean price: " 
                    << fixedGenericBondTheoValue2
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }
    Real fixedBondTheoDirty2 = fixedBondTheoValue2+
                               fixedBond2->accruedAmount();
    Real fixedGenericBondTheoDirty2 = fixedGenericBondTheoValue2+
                                      fixedGenericBond2->accruedAmount();

    Real error4 = std::fabs(fixedBondTheoDirty2-fixedGenericBondTheoDirty2);
    if (error4>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized fixed rate bond's dirty clean price: " 
                    << fixedBondTheoDirty2
                    << "\n  generic equivalent bond's theo dirty price: " 
                    << fixedGenericBondTheoDirty2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN bond (Isin: IT0003543847 ISPIM 0 09/29/13)
    // maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);

    Leg floatingBondLeg1 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule1,
                                   iborIndex_,
                                   Actual360(),
                                   Following,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0056),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption1 = bondCalendar.adjust(Date(29,September,2013),
                                                       Following);
    floatingBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption1)));
    // specialized floater
    boost::shared_ptr<Bond> floatingBond1(new
           FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule1,
                            iborIndex_, Actual360(),
                            Following, fixingDays,
                            std::vector<Real>(1,1), 
                            std::vector<Spread>(1,0.0056),
                            std::vector<Rate>(), std::vector<Rate>(),
                            inArrears,
                            100.0, Date(29,September,2003)));
    // equivalent generic floater
    boost::shared_ptr<Bond> floatingGenericBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(29,September,2003),floatingBondLeg1));

    floatingBond1->setPricingEngine(bondEngine);
    floatingGenericBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    setCouponPricer(floatingGenericBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondTheoValue1 = floatingBond1->cleanPrice();
    Real floatingGenericBondTheoValue1= floatingGenericBond1->cleanPrice();

    Real error5 = std::fabs(floatingBondTheoValue1-
                            floatingGenericBondTheoValue1);
    if (error5>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized fixed rate bond's theo clean price: " 
                    << floatingBondTheoValue1
                    << "\n  generic equivalent bond's theo clean price: " 
                    << floatingGenericBondTheoValue1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error5
                    << "\n  tolerance:             " << tolerance);
    }
    Real floatingBondTheoDirty1 = floatingBondTheoValue1+
                                  floatingBond1->accruedAmount();
    Real floatingGenericBondTheoDirty1 = floatingGenericBondTheoValue1+
                                         floatingGenericBond1->accruedAmount();
    Real error6 = std::fabs(floatingBondTheoDirty1-
                            floatingGenericBondTheoDirty1);
    if (error6>tolerance) {
        BOOST_ERROR("wrong dirty price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized frn bond's dirty clean price: " 
                    << floatingBondTheoDirty1
                    << "\n  generic equivalent bond's theo dirty price: " 
                    << floatingGenericBondTheoDirty1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

    // FRN bond (Isin: XS0090566539 COE 0 09/24/18)
    // maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    Leg floatingBondLeg2 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule2,
                                   iborIndex_,
                                   Actual360(),
                                   ModifiedFollowing,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0025),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption2 = bondCalendar.adjust(Date(24,September,2018),
                                                       ModifiedFollowing);
    floatingBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption2)));
    // specialized floater
   boost::shared_ptr<Bond> floatingBond2(new
          FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule2,
                           iborIndex_, Actual360(),
                           ModifiedFollowing, fixingDays,
                           std::vector<Real>(1,1), 
                           std::vector<Spread>(1,0.0025),
                           std::vector<Rate>(), std::vector<Rate>(),
                           inArrears,
                           100.0, Date(24,September,2004)));
    // equivalent generic floater
    boost::shared_ptr<Bond> floatingGenericBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(24,September,2004),floatingBondLeg2));

    floatingBond2->setPricingEngine(bondEngine);
    floatingGenericBond2->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond2->cashflows(),pricer_);
    setCouponPricer(floatingGenericBond2->cashflows(),pricer_);

    iborIndex_->addFixing(Date(22,March,2007), 0.04013);

    Real floatingBondTheoValue2 = floatingBond2->cleanPrice();
    Real floatingGenericBondTheoValue2= floatingGenericBond2->cleanPrice();

    Real error7 = std::fabs(floatingBondTheoValue2-floatingGenericBondTheoValue2);
    if (error7>tolerance) {
        BOOST_ERROR("wrong clean price for floater bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized floater bond's theo clean price: " 
                    << floatingBondTheoValue2
                    << "\n  generic equivalent bond's theo clean price: " 
                    << floatingGenericBondTheoValue2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error7
                    << "\n  tolerance:             " << tolerance);
    }
    Real floatingBondTheoDirty2 = floatingBondTheoValue2+
                                  floatingBond2->accruedAmount();
    Real floatingGenericTheoDirty2 = floatingGenericBondTheoValue2+
                                     floatingGenericBond2->accruedAmount();

    Real error8 = std::fabs(floatingBondTheoDirty2-floatingGenericTheoDirty2);
    if (error8>tolerance) {
        BOOST_ERROR("wrong dirty price for floater bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized floater bond's theo dirty price: " 
                    << floatingBondTheoDirty2
                    << "\n  generic equivalent bond's theo dirty price: " 
                    << floatingGenericTheoDirty2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error8
                    << "\n  tolerance:             " << tolerance);
    }


    //// CMS bond (Isin: XS0228052402 CRDIT 0 8/22/20)
    //// maturity doesn't occur on a business day

    Schedule cmsBondSchedule1(Date(22,August,2005),
                              Date(22,August,2020),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg1 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule1,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,1.0), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(1,0.055), 
                             std::vector<Rate>(1,0.025),
                             inArrears);
    Date cmsbondRedemption1 = bondCalendar.adjust(Date(22,August,2020),
                                                  Following);
    cmsBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption1)));

    // specialized cms bond
    boost::shared_ptr<Bond> cmsBond1(new
    CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule1,
                swapIndex_, Thirty360(),
                Following, fixingDays,
                std::vector<Real>(1,1.0), std::vector<Spread>(1,0.0),
                std::vector<Rate>(1,0.055), std::vector<Rate>(1,0.025),
                inArrears,
                100.0, Date(22,August,2005)));

    // equivalent generic cms bond
    boost::shared_ptr<Bond> cmsGenericBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(22,August,2005),cmsBondLeg1));

    cmsBond1->setPricingEngine(bondEngine);
    cmsGenericBond1->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond1->cashflows(),cmspricer_);
    setCouponPricer(cmsGenericBond1->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(18,August,2006), 0.04158);
    Real cmsBondTheoValue1 = cmsBond1->cleanPrice();
    Real cmsGenericBondTheoValue1 = cmsGenericBond1->cleanPrice();
    Real error9 = std::fabs(cmsBondTheoValue1-cmsGenericBondTheoValue1);
    if (error9>tolerance) {
        BOOST_ERROR("wrong clean price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized cms bond's theo clean price: " 
                    << cmsBondTheoValue1
                    <<  "\n  generic equivalent bond's theo clean price: " 
                    << cmsGenericBondTheoValue1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error9
                    << "\n  tolerance:             " << tolerance);
    }
    Real cmsBondTheoDirty1 = cmsBondTheoValue1+cmsBond1->accruedAmount();
    Real cmsGenericBondTheoDirty1 = cmsGenericBondTheoValue1+
                                    cmsGenericBond1->accruedAmount();
    Real error10 = std::fabs(cmsBondTheoDirty1-cmsGenericBondTheoDirty1);
    if (error10>tolerance) {
        BOOST_ERROR("wrong dirty price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized cms bond's theo dirty price: " 
                    << cmsBondTheoDirty1
                    << "\n  generic cms bond's theo dirty price: " 
                    << cmsGenericBondTheoDirty1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error10
                    << "\n  tolerance:             " << tolerance);
    }

     // CMS bond (Isin: XS0218766664 ISPIM 0 5/6/15)
     // maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg2 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule2,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,0.84), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(), 
                             std::vector<Rate>(),
                             inArrears);
    Date cmsbondRedemption2 = bondCalendar.adjust(Date(06,May,2015),
                                                  Following);
    cmsBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption2)));

    // specialized cms bond
    boost::shared_ptr<Bond> cmsBond2(new
    CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule2,
                swapIndex_, Thirty360(),
                Following, fixingDays,
                std::vector<Real>(1,0.84), std::vector<Spread>(1,0.0),
                std::vector<Rate>(), std::vector<Rate>(),
                inArrears,
                100.0, Date(06,May,2005)));

    // equivalent generic cms bond
    boost::shared_ptr<Bond> cmsGenericBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(06,May,2005),cmsBondLeg2));

    cmsBond2->setPricingEngine(bondEngine);
    cmsGenericBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    setCouponPricer(cmsGenericBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondTheoValue2 = cmsBond2->cleanPrice();
    Real cmsGenericBondTheoValue2 = cmsGenericBond2->cleanPrice();

    Real error11 = std::fabs(cmsBondTheoValue2-cmsGenericBondTheoValue2);
    if (error11>tolerance) {
        BOOST_ERROR("wrong clean price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized cms bond's theo clean price: " 
                    << cmsBondTheoValue2
                    << "\n  generic cms bond's theo clean price: " 
                    << cmsGenericBondTheoValue2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error11
                    << "\n  tolerance:             " << tolerance);
    }
    Real cmsBondTheoDirty2 = cmsBondTheoValue2+cmsBond2->accruedAmount();
    Real cmsGenericBondTheoDirty2 = cmsGenericBondTheoValue2+cmsGenericBond2->accruedAmount();
    Real error12 = std::fabs(cmsBondTheoDirty2-cmsGenericBondTheoDirty2);
    if (error12>tolerance) {
        BOOST_ERROR("wrong dirty price for cms bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized cms bond's dirty price: " 
                    << cmsBondTheoDirty2
                    << "\n  generic cms bond's theo dirty price: " 
                    << cmsGenericBondTheoDirty2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error12
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero-Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
    // maturity doesn't occur on a business day

    Date zerocpbondRedemption1 = bondCalendar.adjust(Date(20,December,2015),
                                                      Following);
    Leg zeroCpnBondLeg1 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption1)));
    // specialized zerocpn bond
    boost::shared_ptr<Bond> zeroCpnBond1(new
    ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                  Date(20,December,2015),
                  Following,
                  100.0, Date(19,December,1985)));

    // generic zerocpn bond
    boost::shared_ptr<Bond> zeroCpnGenericBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(19,December,1985),zeroCpnBondLeg1));

    zeroCpnBond1->setPricingEngine(bondEngine);
    zeroCpnGenericBond1->setPricingEngine(bondEngine);

    Real zeroCpnBondTheoValue1 = zeroCpnBond1->cleanPrice();
    Real zeroCpnGenericBondTheoValue1 = zeroCpnGenericBond1->cleanPrice();

    Real error13 = std::fabs(zeroCpnBondTheoValue1-zeroCpnGenericBondTheoValue1);
    if (error13>tolerance) {
        BOOST_ERROR("wrong clean price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized cms bond's clean price: " 
                    << zeroCpnBondTheoValue1
                    << "\n  generic cms bond's clean price: " 
                    << zeroCpnGenericBondTheoValue1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error13
                    << "\n  tolerance:             " << tolerance);
    }
    Real zeroCpnBondTheoDirty1 = zeroCpnBondTheoValue1+
                                 zeroCpnBond1->accruedAmount();
    Real zeroCpnGenericBondTheoDirty1 = zeroCpnGenericBondTheoValue1+
                                        zeroCpnGenericBond1->accruedAmount();
    Real error14 = std::fabs(zeroCpnBondTheoDirty1-zeroCpnGenericBondTheoDirty1);
    if (error14>tolerance) {
        BOOST_ERROR("wrong dirty price for zero bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized zerocpn bond's dirty price: " 
                    << zeroCpnBondTheoDirty1
                    << "\n  generic zerocpn bond's clean price: " 
                    << zeroCpnGenericBondTheoDirty1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error14
                    << "\n  tolerance:             " << tolerance);
    }

    // Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
    // maturity doesn't occur on a business day

    Date zerocpbondRedemption2 = bondCalendar.adjust(Date(17,February,2028),
                                                      Following);
    Leg zeroCpnBondLeg2 = Leg(1, boost::shared_ptr<CashFlow>(new
            SimpleCashFlow(100.0, zerocpbondRedemption2)));
    // specialized zerocpn bond
    boost::shared_ptr<Bond> zeroCpnBond2(new
    ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
                   Date(17,February,2028),
                   Following,
                   100.0, Date(17,February,1998)));

    // generic zerocpn bond
    boost::shared_ptr<Bond> zeroCpnGenericBond2(new
           Bond(settlementDays, bondCalendar, faceAmount_,
                Date(17,February,1998),zeroCpnBondLeg2));

    zeroCpnBond2->setPricingEngine(bondEngine);
    zeroCpnGenericBond2->setPricingEngine(bondEngine);

    Real zeroCpnBondTheoValue2 = zeroCpnBond2->cleanPrice();
    Real zeroCpnGenericBondTheoValue2 = zeroCpnGenericBond2->cleanPrice();

    Real error15 = std::fabs(zeroCpnBondTheoValue2 -zeroCpnGenericBondTheoValue2);
    if (error15>tolerance) {
        BOOST_ERROR("wrong clean price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized cms bond's clean price: "  
                    << zeroCpnBondTheoValue2 
                    << "\n  generic cms bond's clean price: " 
                    << zeroCpnGenericBondTheoValue2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error15
                    << "\n  tolerance:             " << tolerance);
    }
    Real zeroCpnBondTheoDirty2 = zeroCpnBondTheoValue2+
                                 zeroCpnBond2->accruedAmount();

    Real zeroCpnGenericBondTheoDirty2 = zeroCpnGenericBondTheoValue2+
                                        zeroCpnGenericBond2->accruedAmount();

    Real error16 = std::fabs(zeroCpnBondTheoDirty2-zeroCpnGenericBondTheoDirty2);
    if (error16>tolerance) {
        BOOST_ERROR("wrong dirty price for zero coupon bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized zerocpn bond's dirty price: " 
                    << zeroCpnBondTheoDirty2
                    << "\n  generic zerocpn bond's dirty price: " 
                    << zeroCpnGenericBondTheoDirty2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error16
                    << "\n  tolerance:             " << tolerance);
    }
}


void AssetSwapTest::testSpecializedBondVsGenericBondUsingAswSpread() {

    BOOST_MESSAGE(
    "Testing assetswap prices for specialized bond are equal to"
    " theo clean and dirty prices for equivalent generic bond..."
        );

    SavedSettings backup;

    setup();

    Calendar bondCalendar = TARGET();
    Natural settlementDays = 3;
    Natural fixingDays = 2;
    bool payFixedRate = true;
    bool parAssetSwap = true;
    bool inArrears = false;

    //// Fixed bond (Isin: DE0001135275 DBR 4 01/04/37)
    //// maturity doesn't occur on a business day

    //Schedule fixedBondSchedule1(Date(4,January,2005),
    //                            Date(4,January,2037),
    //                            Period(Annual), bondCalendar,
    //                            Unadjusted, Unadjusted, true, false);
    //Leg fixedBondLeg1 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
    //                                 fixedBondSchedule1,
    //                                 std::vector<Rate>(1, 0.04),
    //                                 ActualActual(ActualActual::ISDA),
    //                                 Following);
    //Date fixedbondRedemption1 = bondCalendar.adjust(Date(4,January,2037),
    //                                                Following);
    //fixedBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
    //                        SimpleCashFlow(100.0, fixedbondRedemption1)));

    //// specialized fixed rate bond 
    //boost::shared_ptr<Bond> fixedBond1(new
    //    FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule1,
    //                  std::vector<Rate>(1, 0.04),
    //                  ActualActual(ActualActual::ISDA), Following,
    //                  100.0, Date(4,January,2005) ));
    //// equivalent generic bond given fixed leg
    //boost::shared_ptr<Bond> fixedGenericBond1(new
    //                        Bond(settlementDays, bondCalendar, faceAmount_,Date(4,January,2005),
    //                        fixedBondLeg1));

    boost::shared_ptr<PricingEngine> bondEngine(
                                   new DiscountingBondEngine(termStructure_));
    //fixedBond1->setPricingEngine(bondEngine);
    //fixedGenericBond1->setPricingEngine(bondEngine);

    //Real fixedBondPrice1 = fixedBond1->cleanPrice();
    //Real fixedGenericBondPrice1 = fixedGenericBond1->cleanPrice();
    //AssetSwap fixedBondAssetSwap1(payFixedRate,
    //                              fixedBond1, fixedBondPrice1,
    //                              iborIndex_, nonnullspread_, termStructure_,
    //                              Schedule(),
    //                              iborIndex_->dayCounter(),
    //                              parAssetSwap);
    //AssetSwap fixedGenericBondAssetSwap1(payFixedRate,
    //                                     fixedGenericBond1, 
    //                                     fixedGenericBondPrice1,
    //                                     iborIndex_, nonnullspread_, termStructure_,
    //                                     Schedule(),
    //                                     iborIndex_->dayCounter(),
    //                                     parAssetSwap);
    //Real fixedBondAssetSwapPrice1 = fixedBondAssetSwap1.fairPrice();
    //Real fixedGenericBondAssetSwapPrice1 = fixedGenericBondAssetSwap1.fairPrice();
    Real tolerance = 1.0e-13;
    //Real error1 = std::fabs(fixedBondAssetSwapPrice1-fixedGenericBondAssetSwapPrice1);
    //if (error1>tolerance) {
    //    BOOST_ERROR("wrong clean price for fixed bond:"
    //                << QL_FIXED << std::setprecision(4)
    //                << "\n  specialized fixed rate bond's  clean price: " 
    //                << fixedBondAssetSwapPrice1
    //                << "\n  generic equivalent bond's clean price: " 
    //                << fixedGenericBondAssetSwapPrice1
    //                << QL_SCIENTIFIC << std::setprecision(2)
    //                << "\n  error:                 " << error1
    //                << "\n  tolerance:             " << tolerance);
    //}

   //  Fixed bond (Isin: IT0006527060 IBRD 5 02/05/19)
   //  maturity occurs on a business day

    Schedule fixedBondSchedule2(Date(5,February,2005),
                                Date(5,February,2019),
                                Period(Annual), bondCalendar,
                                Unadjusted, Unadjusted, true, false);
    Leg fixedBondLeg2 = FixedRateLeg(std::vector<Real>(1, faceAmount_),
                                     fixedBondSchedule2,
                                     std::vector<Rate>(1, 0.05),
                                     Thirty360(Thirty360::BondBasis),
                                     Following);
    Date fixedbondRedemption2 = bondCalendar.adjust(Date(5,February,2019),
                                                    Following);
    fixedBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                            SimpleCashFlow(100.0, fixedbondRedemption2)));
     //specialized fixed rate bond 
    boost::shared_ptr<Bond> fixedBond2(new
         FixedRateBond(settlementDays, faceAmount_, fixedBondSchedule2,
                      std::vector<Rate>(1, 0.05),
                      Thirty360(Thirty360::BondBasis), Following,
                      100.0, Date(5,February,2005)));
     //equivalent generic bond given fixed leg
    boost::shared_ptr<Bond> fixedGenericBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(5,February,2005),fixedBondLeg2));

    fixedBond2->setPricingEngine(bondEngine);
    fixedGenericBond2->setPricingEngine(bondEngine);

    Real fixedBondPrice2 = fixedBond2->cleanPrice();
    Real fixedGenericBondPrice2 = fixedGenericBond2->cleanPrice();
    AssetSwap fixedBondAssetSwap2(payFixedRate,
                                  fixedBond2, fixedBondPrice2,
                                  iborIndex_, nonnullspread_, termStructure_,
                                  Schedule(),
                                  iborIndex_->dayCounter(),
                                  parAssetSwap);
    AssetSwap fixedGenericBondAssetSwap2(payFixedRate,
                                         fixedGenericBond2, 
                                         fixedGenericBondPrice2,
                                         iborIndex_, nonnullspread_, termStructure_,
                                         Schedule(),
                                         iborIndex_->dayCounter(),
                                         parAssetSwap);
    Real fixedBondAssetSwapPrice2 = fixedBondAssetSwap2.fairPrice();
    Real fixedGenericBondAssetSwapPrice2 = fixedGenericBondAssetSwap2.fairPrice();

    Real error2 = std::fabs(fixedBondAssetSwapPrice2-fixedGenericBondAssetSwapPrice2);
    if (error2>tolerance) {
        BOOST_ERROR("wrong clean price for fixed bond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized fixed rate bond's clean price: " 
                    << fixedBondAssetSwapPrice2
                    << "\n  generic equivalent bond's clean price: " 
                    << fixedGenericBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error2
                    << "\n  tolerance:             " << tolerance);
    }

   //  FRN bond (Isin: IT0003543847 ISPIM 0 09/29/13)
   //  maturity doesn't occur on a business day

    Schedule floatingBondSchedule1(Date(29,September,2003),
                                   Date(29,September,2013),
                                   Period(Semiannual), bondCalendar,
                                   Unadjusted, Following,
                                   true, false);

    Leg floatingBondLeg1 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule1,
                                   iborIndex_,
                                   Actual360(),
                                   Following,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0056),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption1 = bondCalendar.adjust(Date(29,September,2013),
                                                       Following);
    floatingBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption1)));
     //specialized floater
    boost::shared_ptr<Bond> floatingBond1(new
           FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule1,
                            iborIndex_, Actual360(),
                            Following, fixingDays,
                            std::vector<Real>(1,1), 
                            std::vector<Spread>(1,0.0056),
                            std::vector<Rate>(), std::vector<Rate>(),
                            inArrears,
                            100.0, Date(29,September,2003)));
     //equivalent generic floater
    boost::shared_ptr<Bond> floatingGenericBond1(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(29,September,2003),floatingBondLeg1));

    floatingBond1->setPricingEngine(bondEngine);
    floatingGenericBond1->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond1->cashflows(),pricer_);
    setCouponPricer(floatingGenericBond1->cashflows(),pricer_);
    iborIndex_->addFixing(Date(27,March,2007), 0.0402);
    Real floatingBondPrice1 = floatingBond1->cleanPrice();
    Real floatingGenericBondPrice1= floatingGenericBond1->cleanPrice();
    AssetSwap floatingBondAssetSwap1(payFixedRate,
                                     floatingBond1, floatingBondPrice1,
                                     iborIndex_, nonnullspread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    AssetSwap floatingGenericBondAssetSwap1(payFixedRate,
                                            floatingGenericBond1, 
                                            floatingGenericBondPrice1,
                                            iborIndex_, nonnullspread_, termStructure_,
                                            Schedule(),
                                            iborIndex_->dayCounter(),
                                            parAssetSwap);
    Real floatingBondAssetSwapPrice1 = floatingBondAssetSwap1.fairPrice();
    Real floatingGenericBondAssetSwapPrice1 = floatingGenericBondAssetSwap1.fairPrice();

    Real error3 = std::fabs(floatingBondAssetSwapPrice1-floatingGenericBondAssetSwapPrice1);
    if (error3>tolerance) {
        BOOST_ERROR("wrong clean price for frnbond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized frn rate bond's clean price: " 
                    << floatingBondAssetSwapPrice1
                    << "\n  generic equivalent bond's price: " 
                    << floatingBondAssetSwapPrice1
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error3
                    << "\n  tolerance:             " << tolerance);
    }

   //  FRN bond (Isin: XS0090566539 COE 0 09/24/18)
   //  maturity occurs on a business day

    Schedule floatingBondSchedule2(Date(24,September,2004),
                                   Date(24,September,2018),
                                   Period(Semiannual), bondCalendar,
                                   ModifiedFollowing, ModifiedFollowing,
                                   true, false);
    Leg floatingBondLeg2 = IborLeg(std::vector<Real>(1, faceAmount_),
                                   floatingBondSchedule2,
                                   iborIndex_,
                                   Actual360(),
                                   ModifiedFollowing,
                                   std::vector<Natural>(1,fixingDays),
                                   std::vector<Real>(1,1), 
                                   std::vector<Spread>(1,0.0025),
                                   std::vector<Rate>(),
                                   std::vector<Rate>(),
                                   inArrears);
    Date floatingbondRedemption2 = bondCalendar.adjust(Date(24,September,2018),
                                                       ModifiedFollowing);
    floatingBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, floatingbondRedemption2)));
     //specialized floater
   boost::shared_ptr<Bond> floatingBond2(new
          FloatingRateBond(settlementDays, faceAmount_, floatingBondSchedule2,
                           iborIndex_, Actual360(),
                           ModifiedFollowing, fixingDays,
                           std::vector<Real>(1,1), 
                           std::vector<Spread>(1,0.0025),
                           std::vector<Rate>(), std::vector<Rate>(),
                           inArrears,
                           100.0, Date(24,September,2004)));
     //equivalent generic floater
    boost::shared_ptr<Bond> floatingGenericBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(24,September,2004),floatingBondLeg2));

    floatingBond2->setPricingEngine(bondEngine);
    floatingGenericBond2->setPricingEngine(bondEngine);

    setCouponPricer(floatingBond2->cashflows(),pricer_);
    setCouponPricer(floatingGenericBond2->cashflows(),pricer_);

    iborIndex_->addFixing(Date(22,March,2007), 0.04013);

    Real floatingBondPrice2 = floatingBond2->cleanPrice();
    Real floatingGenericBondPrice2= floatingGenericBond2->cleanPrice();
    AssetSwap floatingBondAssetSwap2(payFixedRate,
                                     floatingBond2, floatingBondPrice2,
                                     iborIndex_, nonnullspread_, termStructure_,
                                     Schedule(),
                                     iborIndex_->dayCounter(),
                                     parAssetSwap);
    AssetSwap floatingGenericBondAssetSwap2(payFixedRate,
                                            floatingGenericBond2, 
                                            floatingGenericBondPrice2,
                                            iborIndex_, nonnullspread_, termStructure_,
                                            Schedule(),
                                            iborIndex_->dayCounter(),
                                            parAssetSwap);
    Real floatingBondAssetSwapPrice2 = floatingBondAssetSwap2.fairPrice();
    Real floatingGenericBondAssetSwapPrice2 = floatingGenericBondAssetSwap2.fairPrice();
    Real error4 = std::fabs(floatingBondAssetSwapPrice2-floatingGenericBondAssetSwapPrice2);
    if (error4>tolerance) {
        BOOST_ERROR("wrong clean price for frnbond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized frn rate bond's clean price: " 
                    << floatingBondAssetSwapPrice2
                    << "\n  generic equivalent bond's price: " 
                    << floatingBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error4
                    << "\n  tolerance:             " << tolerance);
    }

   //// // CMS bond (Isin: XS0228052402 CRDIT 0 8/22/20)
   //// // maturity doesn't occur on a business day

   // Schedule cmsBondSchedule1(Date(22,August,2005),
   //                           Date(22,August,2020),
   //                           Period(Annual), bondCalendar,
   //                           Unadjusted, Unadjusted,
   //                           true, false);
   // Leg cmsBondLeg1 = CmsLeg(std::vector<Real>(1, faceAmount_),
   //                          cmsBondSchedule1,
   //                          swapIndex_,
   //                          Thirty360(),
   //                          Following,
   //                          std::vector<Natural>(1,fixingDays),
   //                          std::vector<Real>(1,1.0), 
   //                          std::vector<Spread>(1,0.0),
   //                          std::vector<Rate>(1,0.055), 
   //                          std::vector<Rate>(1,0.025),
   //                          inArrears);
   // Date cmsbondRedemption1 = bondCalendar.adjust(Date(22,August,2020),
   //                                               Following);
   // cmsBondLeg1.push_back(boost::shared_ptr<CashFlow>(new
   //                       SimpleCashFlow(100.0, cmsbondRedemption1)));

   ////  specialized cms bond
   // boost::shared_ptr<Bond> cmsBond1(new
   // CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule1,
   //             swapIndex_, Thirty360(),
   //             Following, fixingDays,
   //             std::vector<Real>(1,1.0), std::vector<Spread>(1,0.0),
   //             std::vector<Rate>(1,0.055), std::vector<Rate>(1,0.025),
   //             inArrears,
   //             100.0, Date(22,August,2005)));

   ////  equivalent generic cms bond
   // boost::shared_ptr<Bond> cmsGenericBond1(new
   //                         Bond(settlementDays, bondCalendar, faceAmount_,
   //                         Date(22,August,2005),cmsBondLeg1));

   // cmsBond1->setPricingEngine(bondEngine);
   // cmsGenericBond1->setPricingEngine(bondEngine);

   // setCouponPricer(cmsBond1->cashflows(),cmspricer_);
   // setCouponPricer(cmsGenericBond1->cashflows(),cmspricer_);
   // swapIndex_->addFixing(Date(18,August,2006), 0.04158);
   // Real cmsBondPrice1 = cmsBond1->cleanPrice();
   // Real cmsGenericBondPrice1 = cmsGenericBond1->cleanPrice();
   // AssetSwap cmsBondAssetSwap1(payFixedRate,cmsBond1, cmsBondPrice1,
   //                             iborIndex_, nonnullspread_, termStructure_,
   //                             Schedule(),iborIndex_->dayCounter(),parAssetSwap);
   // AssetSwap cmsGenericBondAssetSwap1(payFixedRate,cmsGenericBond1, 
   //                                    cmsGenericBondPrice1, iborIndex_, 
   //                                    nonnullspread_, termStructure_,
   //                                    Schedule(),iborIndex_->dayCounter(),
   //                                    parAssetSwap);
   // Real cmsBondAssetSwapPrice1 = cmsBondAssetSwap1.fairPrice();
   // Real cmsGenericBondAssetSwapPrice1 = cmsGenericBondAssetSwap1.fairPrice();
   // Real error5 = std::fabs(cmsBondAssetSwapPrice1-cmsGenericBondAssetSwapPrice1);
   // if (error5>tolerance) {
   //     BOOST_ERROR("wrong clean price for cmsbond:"
   //                 << QL_FIXED << std::setprecision(4)
   //                 << "\n  specialized cms rate bond's clean price: " 
   //                 << cmsBondAssetSwapPrice1
   //                 << "\n  generic equivalent bond's price: " 
   //                 << cmsGenericBondAssetSwapPrice1
   //                 << QL_SCIENTIFIC << std::setprecision(2)
   //                 << "\n  error:                 " << error5
   //                 << "\n  tolerance:             " << tolerance);
   // }



      //CMS bond (Isin: XS0218766664 ISPIM 0 5/6/15)
      //maturity occurs on a business day

    Schedule cmsBondSchedule2(Date(06,May,2005),
                              Date(06,May,2015),
                              Period(Annual), bondCalendar,
                              Unadjusted, Unadjusted,
                              true, false);
    Leg cmsBondLeg2 = CmsLeg(std::vector<Real>(1, faceAmount_),
                             cmsBondSchedule2,
                             swapIndex_,
                             Thirty360(),
                             Following,
                             std::vector<Natural>(1,fixingDays),
                             std::vector<Real>(1,0.84), 
                             std::vector<Spread>(1,0.0),
                             std::vector<Rate>(), 
                             std::vector<Rate>(),
                             inArrears);
    Date cmsbondRedemption2 = bondCalendar.adjust(Date(06,May,2015),
                                                  Following);
    cmsBondLeg2.push_back(boost::shared_ptr<CashFlow>(new
                          SimpleCashFlow(100.0, cmsbondRedemption2)));

     //specialized cms bond
    boost::shared_ptr<Bond> cmsBond2(new
    CmsRateBond(settlementDays, faceAmount_, cmsBondSchedule2,
                swapIndex_, Thirty360(),
                Following, fixingDays,
                std::vector<Real>(1,0.84), std::vector<Spread>(1,0.0),
                std::vector<Rate>(), std::vector<Rate>(),
                inArrears,
                100.0, Date(06,May,2005)));

     //equivalent generic cms bond
    boost::shared_ptr<Bond> cmsGenericBond2(new
                            Bond(settlementDays, bondCalendar, faceAmount_,
                            Date(06,May,2005),cmsBondLeg2));

    cmsBond2->setPricingEngine(bondEngine);
    cmsGenericBond2->setPricingEngine(bondEngine);

    setCouponPricer(cmsBond2->cashflows(),cmspricer_);
    setCouponPricer(cmsGenericBond2->cashflows(),cmspricer_);
    swapIndex_->addFixing(Date(04,May,2006), 0.04217);
    Real cmsBondPrice2 = cmsBond2->cleanPrice();
    Real cmsGenericBondPrice2 = cmsGenericBond2->cleanPrice();
    AssetSwap cmsBondAssetSwap2(payFixedRate,cmsBond2, cmsBondPrice2,
                                iborIndex_, nonnullspread_, termStructure_,
                                Schedule(),iborIndex_->dayCounter(),parAssetSwap);
    AssetSwap cmsGenericBondAssetSwap2(payFixedRate,cmsGenericBond2, 
                                       cmsGenericBondPrice2, iborIndex_, 
                                       nonnullspread_, termStructure_,
                                       Schedule(),iborIndex_->dayCounter(),
                                       parAssetSwap);
    Real cmsBondAssetSwapPrice2 = cmsBondAssetSwap2.fairPrice();
    Real cmsGenericBondAssetSwapPrice2 = cmsGenericBondAssetSwap2.fairPrice();
    Real error6 = std::fabs(cmsBondAssetSwapPrice2-cmsGenericBondAssetSwapPrice2);
    if (error6>tolerance) {
        BOOST_ERROR("wrong clean price for cmsbond:"
                    << QL_FIXED << std::setprecision(4)
                    << "\n  specialized cms rate bond's clean price: " 
                    << cmsBondAssetSwapPrice2
                    << "\n  generic equivalent bond's price: " 
                    << cmsGenericBondAssetSwapPrice2
                    << QL_SCIENTIFIC << std::setprecision(2)
                    << "\n  error:                 " << error6
                    << "\n  tolerance:             " << tolerance);
    }

   ////  Zero-Coupon bond (Isin: DE0004771662 IBRD 0 12/20/15)
   ////  maturity doesn't occur on a business day

   // Date zerocpbondRedemption1 = bondCalendar.adjust(Date(20,December,2015),
   //                                                   Following);
   // Leg zeroCpnBondLeg1 = Leg(1, boost::shared_ptr<CashFlow>(new
   //         SimpleCashFlow(100.0, zerocpbondRedemption1)));
   //  //specialized zerocpn bond
   // boost::shared_ptr<Bond> zeroCpnBond1(new
   // ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
   //               Date(20,December,2015),
   //               Following,
   //               100.0, Date(19,December,1985)));

   //  //generic zerocpn bond
   // boost::shared_ptr<Bond> zeroCpnGenericBond1(new
   //                         Bond(settlementDays, bondCalendar, faceAmount_,
   //                         Date(19,December,1985),zeroCpnBondLeg1));

   // zeroCpnBond1->setPricingEngine(bondEngine);
   // zeroCpnGenericBond1->setPricingEngine(bondEngine);

   // Real zeroCpnBondPrice1 = zeroCpnBond1->cleanPrice();
   // Real zeroCpnGenericBondPrice1 = zeroCpnGenericBond1->cleanPrice();
   // AssetSwap zeroCpnBondAssetSwap1(payFixedRate,zeroCpnBond1, zeroCpnBondPrice1,
   //                             iborIndex_, nonnullspread_, termStructure_,
   //                             Schedule(),iborIndex_->dayCounter(),parAssetSwap);
   // AssetSwap zeroCpnGenericBondAssetSwap1(payFixedRate,zeroCpnGenericBond1, 
   //                                        zeroCpnGenericBondPrice1, iborIndex_, 
   //                                        nonnullspread_, termStructure_,
   //                                        Schedule(),iborIndex_->dayCounter(),
   //                                        parAssetSwap);
   // Real zeroCpnBondAssetSwapPrice1 = zeroCpnBondAssetSwap1.fairPrice();
   // Real zeroCpnGenericBondAssetSwapPrice1 = 
   //                            zeroCpnGenericBondAssetSwap1.fairPrice();
   // Real error7 = std::fabs(zeroCpnBondAssetSwapPrice1-zeroCpnGenericBondAssetSwapPrice1);
   // if (error7>tolerance) {
   //     BOOST_ERROR("wrong clean price for zerocpn bond:"
   //                 << QL_FIXED << std::setprecision(4)
   //                 << "\n  specialized zero cpn bond's clean price: " 
   //                 << zeroCpnBondAssetSwapPrice1
   //                 << "\n  generic equivalent bond's price: " 
   //                 << zeroCpnGenericBondAssetSwapPrice1
   //                 << "\n  error:                 " << error7
   //                 << "\n  tolerance:             " << tolerance);
   // }

   ////  Zero Coupon bond (Isin: IT0001200390 ISPIM 0 02/17/28)
   ////  maturity doesn't occur on a business day

   // Date zerocpbondRedemption2 = bondCalendar.adjust(Date(17,February,2028),
   //                                                  Following);
   // Leg zeroCpnBondLeg2 = Leg(1, boost::shared_ptr<CashFlow>(new
   //         SimpleCashFlow(100.0, zerocpbondRedemption2)));
   //  //specialized zerocpn bond
   // boost::shared_ptr<Bond> zeroCpnBond2(new
   // ZeroCouponBond(settlementDays, bondCalendar, faceAmount_,
   //                Date(17,February,2028),
   //                Following,
   //                100.0, Date(17,February,1998)));

   //  //generic zerocpn bond
   // boost::shared_ptr<Bond> zeroCpnGenericBond2(new
   //        Bond(settlementDays, bondCalendar, faceAmount_,
   //             Date(17,February,1998),zeroCpnBondLeg2));

   // zeroCpnBond2->setPricingEngine(bondEngine);
   // zeroCpnGenericBond2->setPricingEngine(bondEngine);

   // Real zeroCpnBondPrice2 = zeroCpnBond2->cleanPrice();
   // Real zeroCpnGenericBondPrice2 = zeroCpnGenericBond2->cleanPrice();

   // AssetSwap zeroCpnBondAssetSwap2(payFixedRate,zeroCpnBond2, zeroCpnBondPrice2,
   //                                 iborIndex_, nonnullspread_, termStructure_,
   //                                 Schedule(),iborIndex_->dayCounter(),parAssetSwap);
   // AssetSwap zeroCpnGenericBondAssetSwap2(payFixedRate,zeroCpnGenericBond2, 
   //                                        zeroCpnGenericBondPrice2, iborIndex_, 
   //                                        nonnullspread_, termStructure_,
   //                                        Schedule(),iborIndex_->dayCounter(),
   //                                        parAssetSwap);
   // Real zeroCpnBondAssetSwapPrice2 = zeroCpnBondAssetSwap2.fairPrice();
   // Real zeroCpnGenericBondAssetSwapPrice2 = 
   //                            zeroCpnGenericBondAssetSwap2.fairPrice();
   // Real error8 = std::fabs(zeroCpnBondAssetSwapPrice2
   //                         -zeroCpnGenericBondAssetSwapPrice2);
   // if (error8>tolerance) {
   //     BOOST_ERROR("wrong clean price for zerocpn bond:"
   //                 << QL_FIXED << std::setprecision(4)
   //                 << "\n  specialized zero cpn bond's clean price: " 
   //                 << zeroCpnBondAssetSwapPrice2
   //                 << "\n  generic equivalent bond's price: " 
   //                 << zeroCpnGenericBondAssetSwapPrice2
   //                 << "\n  error:                 " << error8
   //                 << "\n  tolerance:             " << tolerance);
   // }
}


test_suite* AssetSwapTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("AssetSwap tests");
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testImpliedValue));
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testMarketASWSpread));
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testZSpread));
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testGenericBondImplied));
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testMASWWithGenericBond));
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testZSpreadWithGenericBond));
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testSpecializedBondVsGenericBond));
    suite->add(BOOST_TEST_CASE(&AssetSwapTest::testSpecializedBondVsGenericBondUsingAswSpread));


    return suite;
}
