/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Neil Firth
 Copyright (C) 2006 Klaus Spanderen

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

/*! \file mcamericanbasketengine.hpp
    \brief Least-square Monte Carlo engines
*/

#ifndef quantlib_american_basket_montecarlo_engine_hpp
#define quantlib_american_basket_montecarlo_engine_hpp

#include <ql/qldefines.hpp>
#include <ql/instruments/basketoption.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/processes/stochasticprocessarray.hpp>
#include <ql/montecarlo/lsmbasissystem.hpp>
#include <ql/pricingengines/mclongstaffschwartzengine.hpp>
#include <boost/function.hpp>

namespace QuantLib {

    //! least-square Monte Carlo engine
    /*! \warning This method is intrinsically weak for out-of-the-money
                 options.

        \ingroup basketengines
    */
    template <class RNG = PseudoRandom>
    class MCAmericanBasketEngine
        : public MCLongstaffSchwartzEngine<BasketOption::engine,
                                           MultiVariate,RNG> {
      public:
        MCAmericanBasketEngine(Size timeSteps,
                               Size timeStepsPerYear,
                               bool brownianBridge,
                               bool antitheticVariate,
                               bool controlVariate,
                               Size requiredSamples,
                               Real requiredTolerance,
                               Size maxSamples,
                               BigNatural seed,
                               Size nCalibrationSamples = Null<Size>());

        // old constructor
        MCAmericanBasketEngine(Size requiredSamples,
                               Size timeSteps,
                               BigNatural seed = 0,
                               bool antitheticSampling = false);

      protected:
        boost::shared_ptr<LongstaffSchwartzPathPricer<MultiPath> >
            lsmPathPricer() const;
    };


    class AmericanBasketPathPricer
        : public EarlyExercisePathPricer<MultiPath>  {
      public:
        AmericanBasketPathPricer(Size assetNumber,
                                 const boost::shared_ptr<Payoff>& payoff,
                                 Size polynomOrder = 2,
                                 LsmBasisSystem::PolynomType
                                 polynomType = LsmBasisSystem::Monomial);

        Array state(const MultiPath& path, Size t) const;
        Real operator()(const MultiPath& path, Size t) const;

        std::vector<boost::function1<Real, Array> > basisSystem() const;

      protected:
        Real payoff(const Array& state) const;

        const Size assetNumber_;
        const boost::shared_ptr<Payoff> payoff_;

        Real scalingValue_;
        std::vector<boost::function1<Real, Array> > v_;
    };

    template <class RNG> inline
    MCAmericanBasketEngine<RNG>::MCAmericanBasketEngine(
                                           Size timeSteps,
                                           Size timeStepsPerYear,
                                           bool brownianBridge,
                                           bool antitheticVariate,
                                           bool controlVariate,
                                           Size requiredSamples,
                                           Real requiredTolerance,
                                           Size maxSamples,
                                           BigNatural seed,
                                           Size nCalibrationSamples)
        : MCLongstaffSchwartzEngine<BasketOption::engine,
                                    MultiVariate,RNG>(timeSteps,
                                                      timeStepsPerYear,
                                                      brownianBridge,
                                                      antitheticVariate,
                                                      controlVariate,
                                                      requiredSamples,
                                                      requiredTolerance,
                                                      maxSamples,
                                                      seed,
                                                      nCalibrationSamples) {
    }

    template <class RNG> inline
    MCAmericanBasketEngine<RNG>::MCAmericanBasketEngine(
                                                    Size requiredSamples,
                                                    Size timeSteps,
                                                    BigNatural seed,
                                                    bool antitheticSampling)
    : MCLongstaffSchwartzEngine<BasketOption::engine,
                                MultiVariate,RNG>(timeSteps,
                                                  Null<Size>(),
                                                  false,
                                                  antitheticSampling,
                                                  false,
                                                  requiredSamples,
                                                  Null<Real>(),
                                                  Null<Size>(),
                                                  seed,
                                                  requiredSamples/4) {
    }

    template <class RNG>
    inline boost::shared_ptr<LongstaffSchwartzPathPricer<MultiPath> >
    MCAmericanBasketEngine<RNG>::lsmPathPricer() const {

        boost::shared_ptr<StochasticProcessArray> processArray =
            boost::dynamic_pointer_cast<StochasticProcessArray>(
                this->arguments_.stochasticProcess);
        QL_REQUIRE(processArray && processArray->size()>0,
                   "Stochastic process array required");

        boost::shared_ptr<GeneralizedBlackScholesProcess> process =
            boost::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
               processArray->process(0));
        QL_REQUIRE(process, "generailized Black-Scholes proces required");

        boost::shared_ptr<EarlyExercise> exercise =
            boost::dynamic_pointer_cast<EarlyExercise>(
                this->arguments_.exercise);
        QL_REQUIRE(exercise, "wrong exercise given");
        QL_REQUIRE(!exercise->payoffAtExpiry(),
                   "payoff at expiry not handled");

        boost::shared_ptr<AmericanBasketPathPricer> earlyExercisePathPricer(
            new AmericanBasketPathPricer(processArray->size(),
                                         this->arguments_.payoff));

        return boost::shared_ptr<LongstaffSchwartzPathPricer<MultiPath> > (
             new LongstaffSchwartzPathPricer<MultiPath>(
                     this->timeGrid(),
                     earlyExercisePathPricer,
                     process->riskFreeRate().currentLink())
             );
    }

}

#endif
