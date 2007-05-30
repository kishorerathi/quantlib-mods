/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2007 StatPro Italia srl

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

#include <ql/instruments/oneassetoption.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/termstructures/volatilities/equity/blackconstantvol.hpp>
#include <ql/math/solvers1d/brent.hpp>

namespace QuantLib {

    OneAssetOption::OneAssetOption(
        const boost::shared_ptr<StochasticProcess>& process,
        const boost::shared_ptr<Payoff>& payoff,
        const boost::shared_ptr<Exercise>& exercise,
        const boost::shared_ptr<PricingEngine>& engine)
    : Option(payoff, exercise, engine),
      stochasticProcess_(process) {
        registerWith(stochasticProcess_);
    }

    bool OneAssetOption::isExpired() const {
        return exercise_->lastDate() < Settings::instance().evaluationDate();
    }

    Real OneAssetOption::delta() const {
        calculate();
        QL_REQUIRE(delta_ != Null<Real>(), "delta not provided");
        return delta_;
    }

    Real OneAssetOption::deltaForward() const {
        calculate();
        QL_REQUIRE(deltaForward_ != Null<Real>(),
                   "forward delta not provided");
        return deltaForward_;
    }

    Real OneAssetOption::elasticity() const {
        calculate();
        QL_REQUIRE(elasticity_ != Null<Real>(), "elasticity not provided");
        return elasticity_;
    }

    Real OneAssetOption::gamma() const {
        calculate();
        QL_REQUIRE(gamma_ != Null<Real>(), "gamma not provided");
        return gamma_;
    }

    Real OneAssetOption::theta() const {
        calculate();
        QL_REQUIRE(theta_ != Null<Real>(), "theta not provided");
        return theta_;
    }

    Real OneAssetOption::thetaPerDay() const {
        calculate();
        QL_REQUIRE(thetaPerDay_ != Null<Real>(), "theta per-day not provided");
        return thetaPerDay_;
    }

    Real OneAssetOption::vega() const {
        calculate();
        QL_REQUIRE(vega_ != Null<Real>(), "vega not provided");
        return vega_;
    }

    Real OneAssetOption::rho() const {
        calculate();
        QL_REQUIRE(rho_ != Null<Real>(), "rho not provided");
        return rho_;
    }

    Real OneAssetOption::dividendRho() const {
        calculate();
        QL_REQUIRE(dividendRho_ != Null<Real>(), "dividend rho not provided");
        return dividendRho_;
    }

    Real OneAssetOption::itmCashProbability() const {
        calculate();
        QL_REQUIRE(itmCashProbability_ != Null<Real>(),
                   "in-the-money cash probability not provided");
        return itmCashProbability_;
    }

    Volatility OneAssetOption::impliedVolatility(Real targetValue,
                                                 Real accuracy,
                                                 Size maxEvaluations,
                                                 Volatility minVol,
                                                 Volatility maxVol) const {
        calculate();
        QL_REQUIRE(!isExpired(), "option expired");

        Volatility guess = (minVol+maxVol)/2.0;
        ImpliedVolHelper f(engine_,targetValue);
        Brent solver;
        solver.setMaxEvaluations(maxEvaluations);
        Volatility result = solver.solve(f, accuracy, guess, minVol, maxVol);
        return result;
    }

    void OneAssetOption::setupExpired() const {
        Option::setupExpired();
        delta_ = deltaForward_ = elasticity_ = gamma_ = theta_ =
            thetaPerDay_ = vega_ = rho_ = dividendRho_ =
            itmCashProbability_ = 0.0;
    }

    void OneAssetOption::setupArguments(PricingEngine::arguments* args) const {
        OneAssetOption::arguments* arguments =
            dynamic_cast<OneAssetOption::arguments*>(args);
        QL_REQUIRE(arguments != 0, "wrong argument type");

        arguments->stochasticProcess = stochasticProcess_;
        arguments->exercise = exercise_;

        Size n = exercise_->dates().size();
        arguments->stoppingTimes.clear();
        arguments->stoppingTimes.reserve(n);
        for (Size i=0; i<n; ++i) {
            arguments->stoppingTimes.push_back(
                               stochasticProcess_->time(exercise_->date(i)));
        }
    }

    void OneAssetOption::fetchResults(const PricingEngine::results* r) const {
        Option::fetchResults(r);
        const Greeks* results = dynamic_cast<const Greeks*>(r);
        QL_ENSURE(results != 0,
                  "no greeks returned from pricing engine");
        /* no check on null values - just copy.
           this allows:
           a) to decide in derived options what to do when null
           results are returned (throw? numerical calculation?)
           b) to implement slim engines which only calculate the
           value---of course care must be taken not to call
           the greeks methods when using these.
        */
        delta_          = results->delta;
        gamma_          = results->gamma;
        theta_          = results->theta;
        vega_           = results->vega;
        rho_            = results->rho;
        dividendRho_    = results->dividendRho;

        const MoreGreeks* moreResults = dynamic_cast<const MoreGreeks*>(r);
        QL_ENSURE(moreResults != 0,
                  "no more greeks returned from pricing engine");
        /* no check on null values - just copy.
           this allows:
           a) to decide in derived options what to do when null
           results are returned (throw? numerical calculation?)
           b) to implement slim engines which only calculate the
           value---of course care must be taken not to call
           the greeks methods when using these.
        */
        deltaForward_       = moreResults->deltaForward;
        elasticity_         = moreResults->elasticity;
        thetaPerDay_        = moreResults->thetaPerDay;
        itmCashProbability_ = moreResults->itmCashProbability;
    }


    void OneAssetOption::arguments::validate() const {
        Option::arguments::validate();
        // we assume the underlying value to be the first state variable
        QL_REQUIRE(stochasticProcess->initialValues()[0] > 0.0,
                   "negative or zero underlying given");
    }


    OneAssetOption::ImpliedVolHelper::ImpliedVolHelper(
                               const boost::shared_ptr<PricingEngine>& engine,
                               Real targetValue)
    : engine_(engine), targetValue_(targetValue) {
        OneAssetOption::arguments* arguments_ =
            dynamic_cast<OneAssetOption::arguments*>(engine_->getArguments());
        QL_REQUIRE(arguments_ != 0,
                   "pricing engine does not supply needed arguments");
        // make a new stochastic process in order not to modify the given one.
        // stateVariable, dividendTS and riskFreeTS can be copied since
        // they won't be modified.
        // Here the requirement for a Black-Scholes process is hard-coded.
        // Making it work for a generic process would need some reflection
        // technique (which is possible, but requires some thought, hence
        // its postponement.)
        boost::shared_ptr<GeneralizedBlackScholesProcess> originalProcess =
            boost::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                                               arguments_->stochasticProcess);
        QL_REQUIRE(originalProcess, "Black-Scholes process required");
        Handle<Quote> stateVariable = originalProcess->stateVariable();
        Handle<YieldTermStructure> dividendYield =
            originalProcess->dividendYield();
        Handle<YieldTermStructure> riskFreeRate =
            originalProcess->riskFreeRate();

        const Handle<BlackVolTermStructure>& blackVol =
            originalProcess->blackVolatility();
        vol_ = boost::shared_ptr<SimpleQuote>(new SimpleQuote(0.0));
        Handle<BlackVolTermStructure> volatility(
               boost::shared_ptr<BlackVolTermStructure>(
                    new BlackConstantVol(blackVol->referenceDate(),
                                         Handle<Quote>(vol_),
                                         blackVol->dayCounter())));

        boost::shared_ptr<StochasticProcess> process(
               new GeneralizedBlackScholesProcess(stateVariable, dividendYield,
                                                  riskFreeRate, volatility));
        arguments_->stochasticProcess = process;
        results_ =
            dynamic_cast<const Instrument::results*>(engine_->getResults());
        QL_REQUIRE(results_ != 0,
                   "pricing engine does not supply needed results");
    }

    Real OneAssetOption::ImpliedVolHelper::operator()(Volatility x) const {
        vol_->setValue(x);
        engine_->calculate();
        return results_->value-targetValue_;
    }

}

