/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006 Theo Boafo
 Copyright (C) 2006 StatPro Italia srl

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

/*! \file binomialconvertibleengine.hpp
    \brief binomial engine for convertible bonds
*/

#ifndef quantlib_binomial_convertible_engine_hpp
#define quantlib_binomial_convertible_engine_hpp

#include <ql/methods/lattices/tflattice.hpp>
#include <ql/pricingengines/hybrid/discretizedconvertible.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/termstructures/yieldtermstructures/flatforward.hpp>
#include <ql/termstructures/voltermstructures/equityfx/blackconstantvol.hpp>
#include <ql/instruments/bonds/convertiblebond.hpp>

namespace QuantLib {

    //! Binomial Tsiveriotis-Fernandes engine for convertible bonds
    /*  \ingroup hybridengines

        \test the correctness of the returned value is tested by
              checking it against known results in a few corner cases.
    */
    template <class T>
    class BinomialConvertibleEngine : public ConvertibleBond::option::engine {
      public:
        BinomialConvertibleEngine(Size timeSteps)
        : timeSteps_(timeSteps) {}
        void calculate() const;
      private:
        Size timeSteps_;
    };


    template <class T>
    void BinomialConvertibleEngine<T>::calculate() const {

        boost::shared_ptr<GeneralizedBlackScholesProcess> process =
            boost::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                                          this->arguments_.stochasticProcess);
        QL_REQUIRE(process, "Black-Scholes process required");

        DayCounter rfdc  = process->riskFreeRate()->dayCounter();
        DayCounter divdc = process->dividendYield()->dayCounter();
        DayCounter voldc = process->blackVolatility()->dayCounter();

        Real s0 = process->stateVariable()->value();
        Volatility v = process->blackVolatility()->blackVol(
                                         arguments_.exercise->lastDate(), s0);
        Date maturityDate = arguments_.exercise->lastDate();
        Rate riskFreeRate = process->riskFreeRate()->zeroRate(
                                 maturityDate, rfdc, Continuous, NoFrequency);
        Rate q = process->dividendYield()->zeroRate(
                                maturityDate, divdc, Continuous, NoFrequency);
        Date referenceDate = process->riskFreeRate()->referenceDate();

        // subtract dividends
        Size i;
        for (i=0; i<arguments_.dividends.size(); i++) {
            if (arguments_.dividends[i]->date() >= referenceDate)
                s0 -= arguments_.dividends[i]->amount() *
                      process->riskFreeRate()->discount(
                                             arguments_.dividends[i]->date());
        }
        QL_REQUIRE(s0 > 0.0,
                   "negative value after subtracting dividends");

        // binomial trees with constant coefficient
        Handle<Quote> underlying(boost::shared_ptr<Quote>(new SimpleQuote(s0)));
        Handle<YieldTermStructure> flatRiskFree(
            boost::shared_ptr<YieldTermStructure>(
                new FlatForward(referenceDate, riskFreeRate, rfdc)));
        Handle<YieldTermStructure> flatDividends(
            boost::shared_ptr<YieldTermStructure>(
                new FlatForward(referenceDate, q, divdc)));
        Handle<BlackVolTermStructure> flatVol(
            boost::shared_ptr<BlackVolTermStructure>(
                new BlackConstantVol(referenceDate, v, voldc)));

        boost::shared_ptr<PlainVanillaPayoff> payoff =
            boost::dynamic_pointer_cast<PlainVanillaPayoff>(arguments_.payoff);
        QL_REQUIRE(payoff, "non-plain payoff given");

        Time maturity = rfdc.yearFraction(arguments_.settlementDate,
                                          maturityDate);

        boost::shared_ptr<StochasticProcess1D> bs(
                 new GeneralizedBlackScholesProcess(underlying, flatDividends,
                                                    flatRiskFree, flatVol));
        boost::shared_ptr<T> tree(new T(bs, maturity, timeSteps_,
                                        payoff->strike()));

        Real creditSpread = arguments_.creditSpread->value();

        boost::shared_ptr<Lattice> lattice(
              new TsiveriotisFernandesLattice<T>(tree,riskFreeRate,maturity,
                                                 timeSteps_,creditSpread,v,q));

        // adjust times to grid
        TimeGrid grid(maturity, timeSteps_);
        for (i=0; i<arguments_.couponTimes.size(); i++)
            arguments_.couponTimes[i] =
                grid.closestTime(arguments_.couponTimes[i]);
        for (i=0; i<arguments_.stoppingTimes.size(); i++)
            arguments_.stoppingTimes[i] =
                grid.closestTime(arguments_.stoppingTimes[i]);
        for (i=0; i<arguments_.callabilityTimes.size(); i++)
            arguments_.callabilityTimes[i] =
                grid.closestTime(arguments_.callabilityTimes[i]);
        for (i=0; i<arguments_.dividendTimes.size(); i++)
            arguments_.dividendTimes[i] =
                grid.closestTime(arguments_.dividendTimes[i]);

        DiscretizedConvertible convertible(arguments_);

        convertible.initialize(lattice, maturity);
        convertible.rollback(0.0);
        results_.value = convertible.presentValue();
    }

}


#endif
