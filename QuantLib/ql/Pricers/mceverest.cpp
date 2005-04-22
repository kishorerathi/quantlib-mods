/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

#include <ql/Pricers/mceverest.hpp>
#include <ql/Processes/blackscholesprocess.hpp>

namespace QuantLib {

    namespace {

        class EverestPathPricer : public PathPricer<MultiPath> {
          public:
            EverestPathPricer(DiscountFactor discount)
            : discount_(discount) {}

            Real operator()(const MultiPath& multiPath) const {
                Size numAssets = multiPath.assetNumber();
                Size numSteps = multiPath.pathSize();

                Real log_variation;
                Size i,j;
                Real minPrice = QL_MAX_REAL;
                for( j = 0; j < numAssets; j++) {
                    log_variation = 0.0;
                    for( i = 0; i < numSteps; i++)
                        log_variation += multiPath[j][i];
                    minPrice = std::min(minPrice,
                                        std::exp(log_variation));
                }

                return discount_ * minPrice;
            }
          private:
            DiscountFactor discount_;
        };

    }

    McEverest::McEverest(
                const std::vector<Handle<YieldTermStructure> >& dividendYield,
                const Handle<YieldTermStructure>& riskFreeRate,
                const std::vector<Handle<BlackVolTermStructure> >&
                                                             volatilities,
                const Matrix& correlation,
                Time residualTime,
                BigNatural seed) {

        Size n = correlation.rows();
        QL_REQUIRE(correlation.columns() == n,
                   "correlation matrix not square");
        QL_REQUIRE(dividendYield.size() == n,
                   "dividendYield size does not match"
                   " that of correlation matrix");
        QL_REQUIRE(residualTime > 0,
                   "residualTime must be positive");

        // initialize the path generator
        std::vector<boost::shared_ptr<StochasticProcess1D> > processes(n);
        Handle<Quote> u(boost::shared_ptr<Quote>(new SimpleQuote(1.0)));
        for (Size i=0; i<n; i++)
            processes[i] = boost::shared_ptr<StochasticProcess1D>(
                                    new BlackScholesProcess(u,
                                                            dividendYield[i],
                                                            riskFreeRate,
                                                            volatilities[i]));

        TimeGrid grid(residualTime, 1);
        PseudoRandom::rsg_type rsg =
            PseudoRandom::make_sequence_generator(n*(grid.size()-1),seed);

        bool brownianBridge = false;

        typedef MultiAsset<PseudoRandom>::path_generator_type generator;
        boost::shared_ptr<generator> pathGenerator(new
            generator(processes, correlation, grid, rsg, brownianBridge));

        // initialize the path pricer
        DiscountFactor discount = riskFreeRate->discount(residualTime);
        boost::shared_ptr<PathPricer<MultiPath> > pathPricer(
                                             new EverestPathPricer(discount));

        // initialize the multi-factor Monte Carlo
        mcModel_ = boost::shared_ptr<MonteCarloModel<MultiAsset<
                                                      PseudoRandom> > > (
            new MonteCarloModel<MultiAsset<PseudoRandom> > (
                             pathGenerator, pathPricer, Statistics(), false));

    }

}
