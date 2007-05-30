/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano

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

#include <ql/legacy/pricers/mcperformanceoption.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/processes/blackscholesprocess.hpp>

namespace QuantLib {

    namespace {

        class PerformanceOptionPathPricer : public PathPricer<Path> {
          public:
            PerformanceOptionPathPricer(
                                 Option::Type type,
                                 Real moneyness,
                                 const std::vector<DiscountFactor>& discounts)
            : discounts_(discounts), payoff_(type, moneyness) {
                QL_REQUIRE(moneyness>0.0,
                           "moneyness less/equal zero not allowed");
            }

            Real operator()(const Path& path) const {
                Size n = path.length();
                QL_REQUIRE(n>1,
                           "at least one option is required");
                QL_REQUIRE(n==3,
                           "only one option for the time being");
                QL_REQUIRE(n==discounts_.size()+1,
                           "discounts/options mismatch");

                std::vector<Real> result(n-1);
                std::vector<Real> assetValue(n-1);
                assetValue[0] = path[1];
                // removing first option
                result[0] = 0.0;
                for (Size i = 1 ; i < n-1; i++) {
                    assetValue[i] = path[i+1];
                    result[i] = discounts_[i] *
                        payoff_(assetValue[i]/assetValue[i-1]);
                }

                return result[1];
            }

          private:
            std::vector<DiscountFactor> discounts_;
            PlainVanillaPayoff payoff_;
        };

    }

    McPerformanceOption::McPerformanceOption(
                              Option::Type type,
                              Real underlying,
                              Real moneyness,
                              const Handle<YieldTermStructure>& dividendYield,
                              const Handle<YieldTermStructure>& riskFreeRate,
                              const Handle<BlackVolTermStructure>& volatility,
                              const std::vector<Time>& times,
                              BigNatural seed) {

        std::vector<DiscountFactor> discounts(times.size());
        for (Size i = 0; i<times.size(); i++)
            discounts[i] = riskFreeRate->discount(times[i]);

        // Initialize the path generator
        Handle<Quote> u(boost::shared_ptr<Quote>(new SimpleQuote(underlying)));
        boost::shared_ptr<StochasticProcess1D> diffusion(
                           new GeneralizedBlackScholesProcess(u,
                                                              dividendYield,
                                                              riskFreeRate,
                                                              volatility));
        TimeGrid grid(times.begin(), times.end());
        PseudoRandom::rsg_type rsg =
            PseudoRandom::make_sequence_generator(grid.size()-1,seed);

        bool brownianBridge = false;

        typedef SingleVariate<PseudoRandom>::path_generator_type generator;
        boost::shared_ptr<generator> pathGenerator(new
            generator(diffusion, grid, rsg, brownianBridge));

        // Initialize the pricer on the single Path
        boost::shared_ptr<PathPricer<Path> > performancePathPricer(
                 new PerformanceOptionPathPricer(type, moneyness, discounts));

        // Initialize the one-factor Monte Carlo
        mcModel_ = boost::shared_ptr<MonteCarloModel<SingleVariate,
                                                     PseudoRandom> > (
            new MonteCarloModel<SingleVariate,PseudoRandom> (
                pathGenerator, performancePathPricer,
                Statistics(), false));
    }

}
