/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003, 2004 Ferdinando Ametrano
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

/*! \file quantoengine.hpp
    \brief Quanto option engine
*/

#ifndef quantlib_quanto_engine_hpp
#define quantlib_quanto_engine_hpp

#include <ql/pricingengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/termstructures/yieldtermstructures/quantotermstructure.hpp>
#include <ql/instruments/payoffs.hpp>

namespace QuantLib {

    //! %Arguments for quanto option calculation
    template<class ArgumentsType>
    class QuantoOptionArguments : public ArgumentsType {
      public:
        QuantoOptionArguments() : correlation(Null<Real>()) {}
        void validate() const;
        Real correlation;
        Handle<YieldTermStructure> foreignRiskFreeTS;
        Handle<BlackVolTermStructure> exchRateVolTS;
    };

    //! %Results from quanto option calculation
    template<class ResultsType>
    class QuantoOptionResults : public ResultsType {
      public:
        QuantoOptionResults() { reset() ;}
        void reset() {
            ResultsType::reset();
            qvega = qrho = qlambda = Null<Real>();
        }
        Real qvega;
        Real qrho;
        Real qlambda;
    };


    //! Quanto engine base class
    /*! \warning for the time being, this engine will only work with
                 simple Black-Scholes processes (i.e., no Merton.)

        \ingroup quantoengines

        \test
        - the correctness of the returned value is tested by
          reproducing results available in literature.
        - the correctness of the returned greeks is tested by
          reproducing numerical derivatives.
    */
    template <class ArgumentsType, class ResultsType>
    class QuantoEngine
        : public GenericEngine<QuantoOptionArguments<ArgumentsType>,
                               QuantoOptionResults<ResultsType> > {
      public:
        QuantoEngine(const boost::shared_ptr<GenericEngine<ArgumentsType,
                                                           ResultsType> >&);
        void calculate() const;
        /*! Access to the arguments of the underlying engine is needed as
            this engine is not able to set them completely. When necessary,
            it must be done by the instrument: see QuantoForwardVanillaOption
            for an example.
        */
        ArgumentsType* underlyingArgs() const { return originalArguments_; }
      protected:
        boost::shared_ptr<GenericEngine<ArgumentsType,
                                        ResultsType> > originalEngine_;
        ArgumentsType* originalArguments_;
        const ResultsType* originalResults_;
    };


    // template definitions

    template <class ArgumentsType>
    void QuantoOptionArguments<ArgumentsType>::validate() const {
        ArgumentsType::validate();
        QL_REQUIRE(!foreignRiskFreeTS.empty(),
                   "null foreign risk free term structure");
        QL_REQUIRE(!exchRateVolTS.empty(),
                   "null exchange rate vol term structure");
        QL_REQUIRE(correlation != Null<Real>(),
                   "null correlation given");
    }


    template<class ArgumentsType, class ResultsType>
    QuantoEngine<ArgumentsType, ResultsType>::QuantoEngine(
        const boost::shared_ptr<GenericEngine<ArgumentsType, ResultsType> >&
            originalEngine)
    : originalEngine_(originalEngine) {
        QL_REQUIRE(originalEngine_, "null engine");
        originalResults_ =
            dynamic_cast<const ResultsType*>(originalEngine_->getResults());
        originalArguments_ =
            dynamic_cast<ArgumentsType*>(originalEngine_->getArguments());
    }

    template<class ArgumentsType, class ResultsType>
    void QuantoEngine<ArgumentsType, ResultsType>::calculate() const {

        // ATM exchangeRate level needed here
        Real exchangeRateATMlevel = 1.0;

        originalEngine_->reset();

        // determine strike from payoff
        boost::shared_ptr<StrikedTypePayoff> payoff =
            boost::dynamic_pointer_cast<StrikedTypePayoff>(
                this->arguments_.payoff);
        QL_REQUIRE(payoff, "non-striked payoff given");
        Real strike = payoff->strike();

        originalArguments_->payoff = this->arguments_.payoff;

        boost::shared_ptr<GeneralizedBlackScholesProcess> process =
            boost::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                this->arguments_.stochasticProcess);
        QL_REQUIRE(process, "Black-Scholes process required");

        Handle<Quote> spot = process->stateVariable();
        Handle<YieldTermStructure> riskFreeRate = process->riskFreeRate();
        // dividendTS needs modification
        Handle<YieldTermStructure> dividendYield(
            boost::shared_ptr<YieldTermStructure>(
                new QuantoTermStructure(process->dividendYield(),
                                        process->riskFreeRate(),
                                        this->arguments_.foreignRiskFreeTS,
                                        process->blackVolatility(),
                                        strike,
                                        this->arguments_.exchRateVolTS,
                                        exchangeRateATMlevel,
                                        this->arguments_.correlation)));
        Handle<BlackVolTermStructure> blackVol = process->blackVolatility();
        originalArguments_->stochasticProcess =
            boost::shared_ptr<StochasticProcess>(
                  new GeneralizedBlackScholesProcess(spot, dividendYield,
                                                     riskFreeRate, blackVol));

        originalArguments_->exercise = this->arguments_.exercise;

        originalArguments_->validate();
        originalEngine_->calculate();

        this->results_.value = originalResults_->value;
        this->results_.delta = originalResults_->delta;
        this->results_.gamma = originalResults_->gamma;
        this->results_.theta = originalResults_->theta;
        this->results_.rho = originalResults_->rho +
            originalResults_->dividendRho;
        this->results_.dividendRho = originalResults_->dividendRho;
        Volatility exchangeRateFlatVol =
            this->arguments_.exchRateVolTS->blackVol(
                this->arguments_.exercise->lastDate(),
                exchangeRateATMlevel);
        this->results_.vega = originalResults_->vega +
            this->arguments_.correlation * exchangeRateFlatVol *
            originalResults_->dividendRho;


        Volatility volatility = process->blackVolatility()->blackVol(
                                        this->arguments_.exercise->lastDate(),
                                        process->stateVariable()->value());
        this->results_.qvega = this->arguments_.correlation *
            process->blackVolatility()->blackVol(
                                        this->arguments_.exercise->lastDate(),
                                        process->stateVariable()->value()) *
                                            originalResults_->dividendRho;
        this->results_.qrho = - originalResults_->dividendRho;
        this->results_.qlambda = exchangeRateFlatVol *
            volatility * originalResults_->dividendRho;
    }

}


#endif
