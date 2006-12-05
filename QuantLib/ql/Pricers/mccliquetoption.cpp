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

#include <ql/Pricers/mccliquetoption.hpp>
#include <ql/Instruments/payoffs.hpp>
#include <ql/Processes/blackscholesprocess.hpp>
#ifdef QL_DISABLE_DEPRECATED
#include <ql/Quotes/simplequote.hpp>
#else
#include <ql/quote.hpp>
#endif

namespace QuantLib {

    namespace {

        class CliquetOptionPathPricer : public PathPricer<Path> {
          public:
            CliquetOptionPathPricer(
                                 Option::Type type,
                                 Real moneyness,
                                 Real accruedCoupon,
                                 Real lastFixing,
                                 Real localCap,
                                 Real localFloor,
                                 Real globalCap,
                                 Real globalFloor,
                                 const std::vector<DiscountFactor>& discounts,
                                 bool redemptionOnly)
            : type_(type), moneyness_(moneyness),
              accruedCoupon_(accruedCoupon), lastFixing_(lastFixing),
              localCap_(localCap), localFloor_(localFloor),
              globalCap_(globalCap), globalFloor_(globalFloor),
              discounts_(discounts), redemptionOnly_(redemptionOnly) {
                QL_REQUIRE(moneyness>0.0,
                           "moneyness less/equal zero not allowed");

                if (accruedCoupon == Null<Real>())
                    accruedCoupon_ = 0.0;

                if (localCap == Null<Real>())
                    localCap_ = QL_MAX_REAL;

                if (localFloor == Null<Real>())
                    localFloor_ = 0.0;

                if (globalCap == Null<Real>())
                    globalCap_ = QL_MAX_REAL;

                if (globalFloor == Null<Real>())
                    globalFloor_ = 0.0;
            }

            Real operator()(const Path& path) const {
                Size n = path.length();
                QL_REQUIRE(n>1, "the path cannot be empty");

                QL_REQUIRE(n==discounts_.size(), "discounts/options mismatch");

                Real result, lastFixing, underlying, payoff;

                // start the simulation
                lastFixing = lastFixing_;
                underlying = path.front();
                if (redemptionOnly_)
                    result = accruedCoupon_;
                else
                    result = 0.0;

                // step by step using the discretization of the path
                for (Size i=1; i<n; i++) {
                    underlying = path[i];
                    // incorporate payoff
                    if (lastFixing != Null<Real>()) {
                        payoff =
                            PlainVanillaPayoff(type_,
                                               moneyness_*lastFixing)
                            (underlying) / lastFixing;
                        payoff = std::max(payoff, localFloor_);
                        payoff = std::min(payoff, localCap_);
                        if (redemptionOnly_)
                            result += payoff;
                        else
                            result += payoff * discounts_[i];
                    }
                    // new fixing
                    lastFixing = underlying;
                }
                if (redemptionOnly_) {
                    result = std::max(result, globalFloor_);
                    result = std::min(result, globalCap_);
                }

                if (redemptionOnly_) {
                    return discounts_.back()*result;
                } else {
                    return result;
                }
            }

          private:
            Option::Type type_;
            Real moneyness_, accruedCoupon_;
            Real lastFixing_, localCap_, localFloor_,
                 globalCap_, globalFloor_;
            std::vector<DiscountFactor> discounts_;
            bool redemptionOnly_;
        };

    }

    McCliquetOption::McCliquetOption(
                              Option::Type type,
                              Real underlying, Real moneyness,
                              const Handle<YieldTermStructure>& dividendYield,
                              const Handle<YieldTermStructure>& riskFreeRate,
                              const Handle<BlackVolTermStructure>& volatility,
                              const std::vector<Time>& times,
                              Real accruedCoupon, Real lastFixing,
                              Real localCap, Real localFloor,
                              Real globalCap, Real globalFloor,
                              bool redemptionOnly,
                              BigNatural seed) {

        std::vector<DiscountFactor> discounts(times.size());
        for (Size i = 0; i<times.size(); i++)
            discounts[i] = riskFreeRate->discount(times[i]);

        // initialize the path generator
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

        // initialize the path pricer
        boost::shared_ptr<PathPricer<Path> > cliquetPathPricer(
            new CliquetOptionPathPricer(type, moneyness,
                                        accruedCoupon, lastFixing,
                                        localCap, localFloor,
                                        globalCap, globalFloor,
                                        discounts, redemptionOnly));

        // initialize the one-factor Monte Carlo
        mcModel_ =
            boost::shared_ptr<MonteCarloModel<SingleVariate<PseudoRandom> > >(
                new MonteCarloModel<SingleVariate<PseudoRandom> >(
                      pathGenerator, cliquetPathPricer, Statistics(), false));
    }

}
