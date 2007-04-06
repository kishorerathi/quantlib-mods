/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Marco Bianchetti
 Copyright (C) 2007 Cristina Duminuco

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

#include "marketmodel_smmcapletcalibration.hpp"
#include "utilities.hpp"
#include <ql/marketmodels/models/swapfromfracorrelationstructure.hpp>
#include <ql/marketmodels/models/piecewiseconstantabcdvariance.hpp>
#include <ql/marketmodels/models/capletcoterminalcalibration.hpp>
#include <ql/marketmodels/products/multistep/multistepcoterminalswaps.hpp>
#include <ql/marketmodels/products/multistep/multistepcoterminalswaptions.hpp>
#include <ql/marketmodels/products/multistep/multistepswap.hpp>
#include <ql/marketmodels/products/multiproductcomposite.hpp>
#include <ql/marketmodels/accountingengine.hpp>
#include <ql/marketmodels/utilities.hpp>
#include <ql/marketmodels/evolvers/coterminalswapratepcevolver.hpp>
#include <ql/marketmodels/evolvers/forwardratepcevolver.hpp>
#include <ql/marketmodels/models/expcorrflatvol.hpp>
#include <ql/marketmodels/models/expcorrabcdvol.hpp>
#include <ql/marketmodels/browniangenerators/mtbrowniangenerator.hpp>
#include <ql/marketmodels/browniangenerators/sobolbrowniangenerator.hpp>
#include <ql/marketmodels/swapforwardmappings.hpp>
#include <ql/marketmodels/curvestates/coterminalswapcurvestate.hpp>
#include <ql/methods/montecarlo/genericlsregression.hpp>
#include <ql/shortratemodels/libormarketmodels/lmlinexpcorrmodel.hpp>
#include <ql/shortratemodels/libormarketmodels/lmextlinexpvolmodel.hpp>
#include <ql/time/schedule.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/daycounters/simpledaycounter.hpp>
#include <ql/pricingengines/blackformula.hpp>
#include <ql/pricingengines/blackcalculator.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/math/integrals/segmentintegral.hpp>
#include <ql/math/convergencestatistics.hpp>
#include <ql/math/functional.hpp>
#include <ql/math/optimization/simplex.hpp>
#include <ql/math/distributions/sequencestatistics.hpp>
#include <sstream>

#if defined(BOOST_MSVC)
#include <float.h>
//namespace { unsigned int u = _controlfp(_EM_INEXACT, _MCW_EM); }
#endif

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(MarketModelTest_smm)

#define BEGIN(x) (x+0)
#define END(x) (x+LENGTH(x))

Date todaysDate, startDate, endDate;
std::vector<Time> rateTimes;
std::vector<Real> accruals;
Calendar calendar;
DayCounter dayCounter;
std::vector<Rate> todaysForwards, todaysSwaps;
std::vector<Real> coterminalAnnuity;
Spread displacement;
std::vector<DiscountFactor> todaysDiscounts;
std::vector<Volatility> swaptionDisplacedVols, swaptionVols;
std::vector<Volatility> capletDisplacedVols, capletVols;
Real a, b, c, d;
Real longTermCorrelation, beta;
Size measureOffset_;
unsigned long seed_;
Size paths_, trainingPaths_;
bool printReport_ = true;

void setup() {

    // Times
    calendar = NullCalendar();
    todaysDate = Settings::instance().evaluationDate();
    //startDate = todaysDate + 5*Years;
    endDate = todaysDate + 10*Years;
    Schedule dates(todaysDate, endDate, Period(Semiannual),
                   calendar, Following, Following, true, false);
    rateTimes = std::vector<Time>(dates.size()-1);
    accruals = std::vector<Real>(rateTimes.size()-1);
    dayCounter = SimpleDayCounter();
    for (Size i=1; i<dates.size(); ++i)
        rateTimes[i-1] = dayCounter.yearFraction(todaysDate, dates[i]);
    for (Size i=1; i<rateTimes.size(); ++i)
        accruals[i-1] = rateTimes[i] - rateTimes[i-1];

    // Rates & displacement
    todaysForwards = std::vector<Rate>(accruals.size());
    displacement = 0.0;
    for (Size i=0; i<todaysForwards.size(); ++i) {
        todaysForwards[i] = 0.03 + 0.0010*i;
        //todaysForwards[i] = 0.04;
    }
    LMMCurveState curveState_lmm(rateTimes);
    curveState_lmm.setOnForwardRates(todaysForwards);
    todaysSwaps = curveState_lmm.coterminalSwapRates();

    // Discounts
    todaysDiscounts = std::vector<DiscountFactor>(rateTimes.size());
    todaysDiscounts[0] = 0.95;
    for (Size i=1; i<rateTimes.size(); ++i)
        todaysDiscounts[i] = todaysDiscounts[i-1] /
            (1.0+todaysForwards[i-1]*accruals[i-1]);

    // Swaption Volatilities
    Volatility mktSwaptionVols[] = {
                            0.15541283,
                            0.18719678,
                            0.20890740,
                            0.22318179,
                            0.23212717,
                            0.23731450,
                            0.23988649,
                            0.24066384,
                            0.24023111,
                            0.23900189,
                            0.23726699,
                            0.23522952,
                            0.23303022,
                            0.23076564,
                            0.22850101,
                            0.22627951,
                            0.22412881,
                            0.22206569,
                            0.22009939
    };
    a = -0.0597;
    b =  0.1677;
    c =  0.5403;
    d =  0.1710;
    Volatility mktCapletVols[] = {
                            0.17,
                            0.20,
                            0.22,
                            0.24,
                            0.25,
                            0.25,
                            0.25,
                            0.26,
                            0.26,
                            0.25,
                            0.25,
                            0.25,
                            0.25,
                            0.25,
                            0.24,
                            0.24,
                            0.24,
                            0.24,
                            0.24
    };
    swaptionDisplacedVols = std::vector<Volatility>(todaysSwaps.size());
    swaptionVols = std::vector<Volatility>(todaysSwaps.size());
    capletDisplacedVols = std::vector<Volatility>(todaysSwaps.size());
    capletVols = std::vector<Volatility>(todaysSwaps.size());
    for (Size i=0; i<todaysSwaps.size(); i++) {
        swaptionDisplacedVols[i] = todaysSwaps[i]*mktSwaptionVols[i]/
                                  (todaysSwaps[i]+displacement);
        swaptionVols[i]= mktSwaptionVols[i];
        capletDisplacedVols[i] = todaysForwards[i]*mktCapletVols[i]/
                                (todaysForwards[i]+displacement);
        capletVols[i]= mktCapletVols[i];
    }

    // Cap/Floor Correlation
    longTermCorrelation = 0.5;
    beta = 0.2;
    measureOffset_ = 5;

    // Monte Carlo
    seed_ = 42;

#ifdef _DEBUG
    paths_ = 127;
    trainingPaths_ = 31;
#else
    paths_ = 32767; //262144-1; //; // 2^15-1
    trainingPaths_ = 8191; // 2^13-1
#endif
}

const boost::shared_ptr<SequenceStatistics> simulate(
        const boost::shared_ptr<MarketModelEvolver>& evolver,
        const MarketModelMultiProduct& product)
{
    Size initialNumeraire = evolver->numeraires().front();
    Real initialNumeraireValue = todaysDiscounts[initialNumeraire];

    AccountingEngine engine(evolver, product, initialNumeraireValue);
    boost::shared_ptr<SequenceStatistics> stats(new
        SequenceStatistics(product.numberOfProducts()));
    engine.multiplePathValues(*stats, paths_);
    return stats;
}


enum MarketModelType { ExponentialCorrelationFlatVolatility,
                       ExponentialCorrelationAbcdVolatility/*,
                       CalibratedMM*/
};

std::string marketModelTypeToString(MarketModelType type) {
    switch (type) {
      case ExponentialCorrelationFlatVolatility:
          return "Exp. Corr. Flat Vol.";
      case ExponentialCorrelationAbcdVolatility:
          return "Exp. Corr. Abcd Vol.";
      //case CalibratedMM:
      //    return "CalibratedMarketModel";
      default:
        QL_FAIL("unknown MarketModelEvolver type");
    }
}


boost::shared_ptr<MarketModel> makeMarketModel(
                                        const EvolutionDescription& evolution,
                                        Size numberOfFactors,
                                        MarketModelType marketModelType,
                                        Spread rateBump = 0.0,
                                        Volatility volBump = 0.0) {

    std::vector<Time> fixingTimes(evolution.rateTimes());
    fixingTimes.pop_back();
    boost::shared_ptr<LmVolatilityModel> volModel(new
        LmExtLinearExponentialVolModel(fixingTimes, 0.5, 0.6, 0.1, 0.1));
    boost::shared_ptr<LmCorrelationModel> corrModel(new
        LmLinearExponentialCorrelationModel(evolution.numberOfRates(),
                                            longTermCorrelation, beta));
    std::vector<Rate> bumpedRates(todaysForwards.size());
    LMMCurveState curveState_lmm(rateTimes);
    curveState_lmm.setOnForwardRates(todaysForwards);
    std::vector<Rate> usedRates = curveState_lmm.coterminalSwapRates();
    std::transform(usedRates.begin(), usedRates.end(),
                   bumpedRates.begin(),
                   std::bind1st(std::plus<Rate>(), rateBump));

    std::vector<Volatility> bumpedVols(swaptionDisplacedVols.size());
    std::transform(swaptionDisplacedVols.begin(), swaptionDisplacedVols.end(),
                   bumpedVols.begin(),
                   std::bind1st(std::plus<Rate>(), volBump));

    switch (marketModelType) {
        case ExponentialCorrelationFlatVolatility:
            return boost::shared_ptr<MarketModel>(new
                ExpCorrFlatVol(longTermCorrelation, beta,
                               bumpedVols,
                               evolution,
                               numberOfFactors,
                               bumpedRates,
                               std::vector<Spread>(bumpedRates.size(), displacement)));
        case ExponentialCorrelationAbcdVolatility:
            return boost::shared_ptr<MarketModel>(new
                ExpCorrAbcdVol(0.0,0.0,1.0,1.0,
                               bumpedVols,
                               longTermCorrelation, beta,
                               evolution,
                               numberOfFactors,
                               bumpedRates,
                               std::vector<Spread>(bumpedRates.size(), displacement)));
        //case CalibratedMM:
        //    return boost::shared_ptr<MarketModel>(new
        //        CalibratedMarketModel(volModel, corrModel,
        //                              evolution,
        //                              numberOfFactors,
        //                              bumpedForwards,
        //                              displacement));
        default:
            QL_FAIL("unknown MarketModel type");
    }
}

enum MeasureType { ProductSuggested, Terminal, MoneyMarket, MoneyMarketPlus };

std::string measureTypeToString(MeasureType type) {
    switch (type) {
      case ProductSuggested:
          return "ProductSuggested measure";
      case Terminal:
          return "Terminal measure";
      case MoneyMarket:
          return "Money Market measure";
      case MoneyMarketPlus:
          return "Money Market Plus measure";
      default:
        QL_FAIL("unknown measure type");
    }
}

std::vector<Size> makeMeasure(const MarketModelMultiProduct& product,
                              MeasureType measureType)
{
    std::vector<Size> result;
    EvolutionDescription evolution(product.evolution());
    switch (measureType) {
      case ProductSuggested:
        result = product.suggestedNumeraires();
        break;
      case Terminal:
        result = terminalMeasure(evolution);
        if (!isInTerminalMeasure(evolution, result)) {
            Array a(result.size());
            std::copy(result.begin(), result.end(), a.begin());
            BOOST_ERROR("\nfailure in verifying Terminal measure:\n" << a);
        }
        break;
      case MoneyMarket:
        result = moneyMarketMeasure(evolution);
        if (!isInMoneyMarketMeasure(evolution, result)) {
            Array a(result.size());
            std::copy(result.begin(), result.end(), a.begin());
            BOOST_ERROR("\nfailure in verifying MoneyMarket measure:\n" << a);
        }
        break;
      case MoneyMarketPlus:
        result = moneyMarketPlusMeasure(evolution, measureOffset_);
        if (!isInMoneyMarketPlusMeasure(evolution, result, measureOffset_)) {
            Array a(result.size());
            std::copy(result.begin(), result.end(), a.begin());
            BOOST_ERROR("\nfailure in verifying MoneyMarketPlus(" <<
                        measureOffset_ << ") measure:\n" << a);
        }
        break;
      default:
        QL_FAIL("unknown measure type");
    }
    checkCompatibility(evolution, result);
    if (printReport_) {
        Array num(result.size());
        std::copy(result.begin(), result.end(), num.begin());
        BOOST_MESSAGE("    " << measureTypeToString(measureType) << ": " << num);
    }
    return result;
}

enum EvolverType { Ipc, Pc , NormalPc};

std::string evolverTypeToString(EvolverType type) {
    switch (type) {
      case Ipc:
          return "iterative predictor corrector";
      case Pc:
          return "predictor corrector";
      case NormalPc:
          return "predictor corrector for normal case";
      default:
        QL_FAIL("unknown MarketModelEvolver type");
    }
}
boost::shared_ptr<MarketModelEvolver> makeMarketModelEvolver(
    const boost::shared_ptr<MarketModel>& marketModel,
    const std::vector<Size>& numeraires,
    const BrownianGeneratorFactory& generatorFactory,
    EvolverType evolverType,
    Size initialStep = 0)
{
    switch (evolverType) {
        case Pc:
            return boost::shared_ptr<MarketModelEvolver>(new
                CoterminalSwapRatePcEvolver(marketModel, generatorFactory,
                                            numeraires,
                                            initialStep));
        default:
            QL_FAIL("unknown CoterminalSwapMarketModelEvolver type");
    }
}

void checkCoterminalSwapsAndSwaptions(const SequenceStatistics& stats,
                                      const Rate fixedRate,
                                      const std::vector<boost::shared_ptr<StrikedTypePayoff> >& displacedPayoff,
                                      const boost::shared_ptr<MarketModel>, //marketModel,
                                      const std::string& config) {

    std::vector<Real> results = stats.mean();
    std::vector<Real> errors = stats.errorEstimate();
    std::vector<Real> discrepancies(todaysForwards.size());

    Size N = todaysForwards.size();

    // check Swaps
    Real maxError = QL_MIN_REAL;
    LMMCurveState curveState_lmm(rateTimes);
    curveState_lmm.setOnForwardRates(todaysForwards);

    std::vector<Real> expectedNPVs(todaysSwaps.size());
    Real errorThreshold = 0.5;
    for (Size i=0; i<N; ++i) {
        Real expectedNPV = curveState_lmm.coterminalSwapAnnuity(i, i)
            * (todaysSwaps[i]-fixedRate) * todaysDiscounts[i];
        expectedNPVs[i] = expectedNPV;
        discrepancies[i] = (results[i]-expectedNPVs[i])/errors[i];
        maxError = std::max(std::fabs(discrepancies[i]), maxError);
    }
    if (maxError > errorThreshold) {
        BOOST_MESSAGE(config);
        for (Size i=0; i<N; ++i) {
            BOOST_MESSAGE(io::ordinal(i+1) << " coterminal swap NPV: "
                          << io::rate(results[i])
                          << " +- " << io::rate(errors[i])
                          << "; expected: " << io::rate(expectedNPVs[i])
                          << "; discrepancy/error = "
                          << discrepancies[N-1-i]
                          << " standard errors");
        }
        BOOST_ERROR("test failed");
    }

    // check Swaptions
    maxError = 0;
    std::vector<Rate> expectedSwaptions(N);
    std::vector<Real> stdDevSwaptions(N);
    for (Size i=0; i<N; ++i) {
        Real expectedSwaption =
            BlackCalculator(displacedPayoff[i],
                            todaysSwaps[i]+displacement,
                            swaptionDisplacedVols[i]*std::sqrt(rateTimes[i]),
                            curveState_lmm.coterminalSwapAnnuity(i,i) *
                                todaysDiscounts[i]).value();
        expectedSwaptions[i] = expectedSwaption;
        discrepancies[i] = (results[N+i]-expectedSwaptions[i])/errors[N+i];
        maxError = std::max(std::fabs(discrepancies[i]), maxError);
    }
    errorThreshold = 2.0;

   if (maxError > errorThreshold) {
        BOOST_MESSAGE(config);
        for (Size i=1; i<=N; ++i) {
            BOOST_MESSAGE(
                    io::ordinal(i) << " Swaption: "
                    << io::rate(results[2*N-i])
                    << " +- " << io::rate(errors[2*N-i])
                    << "; expected: " << io::rate(expectedSwaptions[N-i])
                    << "; discrepancy/error = "
                    << io::percent(discrepancies[N-i])
                    << " standard errors");
        }
        BOOST_ERROR("test failed");
    }
}

QL_END_TEST_LOCALS(MarketModelTest_smm)

void MarketModelSmmCapletCalibrationTest::testFunction() {

    BOOST_MESSAGE("Run coterminal swap + caplet calibration function ...");
    QL_TEST_BEGIN
    QL_TEST_SETUP

    Size numberOfFactors = 3;
    Size numberOfRates = todaysForwards.size();

    EvolutionDescription evolution(rateTimes);

    LMMCurveState cs(rateTimes);
    cs.setOnForwardRates(todaysForwards);

    SwapFromFRACorrelationStructure corr(
            longTermCorrelation,
            beta,
            cs,
            evolution,
            numberOfFactors);

    std::vector<boost::shared_ptr<PiecewiseConstantVariance> >
                                    swapVariances(numberOfRates);
    for (Size i=0; i<numberOfRates; ++i) {
        swapVariances[i] = boost::shared_ptr<PiecewiseConstantVariance>(new
            PiecewiseConstantAbcdVariance(a, b, c, d,
                                          i, evolution));
    }

    Spread displacement = 0.02;
    std::vector<Real> alpha(numberOfRates, 0.01);
    std::vector<Matrix> pseudoRoots;
    capletCoterminalCalibration(
            corr,
            swapVariances,
            capletDisplacedVols,
            cs,
            displacement,
            alpha,
            pseudoRoots);

    QL_TEST_END
}

// --- Call the desired tests
test_suite* MarketModelSmmCapletCalibrationTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("SMM Caplet calibration test");

    suite->add(BOOST_TEST_CASE(&MarketModelSmmCapletCalibrationTest::testFunction));
    return suite;
}
