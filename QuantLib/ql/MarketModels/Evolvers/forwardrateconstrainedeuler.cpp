/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
Copyright (C) 2006 Ferdinando Ametrano
Copyright (C) 2006 Mark Joshi

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
#include <ql/MarketModels/Evolvers/forwardrateconstrainedeuler.hpp>
#include <ql/MarketModels/duffsdeviceinnerproduct.hpp>
#include <ql/Math/normaldistribution.hpp>

namespace QuantLib {


    ForwardRateConstrainedEuler::ForwardRateConstrainedEuler(
        const boost::shared_ptr<MarketModel>& marketModel,
        const BrownianGeneratorFactory& factory,
        const std::vector<Size>& numeraires)
    : marketModel_(marketModel),
      numeraires_(numeraires),
      n_(marketModel->numberOfRates()), F_(marketModel_->numberOfFactors()),
      curveState_(marketModel->evolution().rateTimes()),
      forwards_(marketModel->initialRates()),
      displacements_(marketModel->displacements()),
      logForwards_(n_), initialLogForwards_(n_), drifts1_(n_),
      initialDrifts_(n_), brownians_(F_), correlatedBrownians_(n_),
      alive_(marketModel->evolution().firstAliveRate())
    {
        checkCompatibility(marketModel->evolution(), numeraires);
        const std::vector<Rate>& initialForwards = marketModel_->initialRates();

        Size steps = marketModel->evolution().numberOfSteps();

        generator_ = factory.create(F_, steps);
        currentStep_ = 0;

        for (Size i=0; i<n_; ++i) {
            initialLogForwards_[i] = std::log(initialForwards[i] +
                displacements_[i]);
        }

        for (Size j=0; j<steps; ++j) {
            const Matrix& A = marketModel_->pseudoRoot(j);
            calculators_.push_back(DriftCalculator(A,
                displacements_,
                marketModel->evolution().rateTaus(),
                numeraires[j],
                alive_[j]));
            std::vector<Real> fixed(n_);
            std::vector<Real> variances(n_);
            for (Size k=0; k<n_; ++k) {
                Real variance =
                    std::inner_product(A.row_begin(k), A.row_end(k),
                    A.row_begin(k), 0.0);
                variances[k] = variance;
                fixed[k] = -0.5*variance;
            }
            variances_.push_back(variances);
            fixedDrifts_.push_back(fixed);
        }

        calculators_.front().compute(initialForwards, initialDrifts_);
    }

    const std::vector<Size>& ForwardRateConstrainedEuler::numeraires() const {
        return numeraires_;
    }

    void ForwardRateConstrainedEuler::SetConstraintType(
        const std::vector<Size>& startIndexOfSwapRate,
        const std::vector<Size>& endIndexOfSwapRate)
    {
        QL_REQUIRE(startIndexOfSwapRate.size() == numeraires_.size(),
            "Size mismatch in constraint specification.");
        QL_REQUIRE(endIndexOfSwapRate.size() == numeraires_.size(), "Size mismatch in constraint specification.");

        startIndexOfSwapRate_=startIndexOfSwapRate;
        endIndexOfSwapRate_ = endIndexOfSwapRate;

        covariances_.clear();
        std::vector<Real> covariances(n_);

        for (unsigned long i=0; i < startIndexOfSwapRate_.size(); i++)
        {
                QL_REQUIRE(startIndexOfSwapRate_[i]+1 == endIndexOfSwapRate_[i],
                                        "constrained euler currently only implemented for forward rates");

                const Matrix& A = marketModel_->pseudoRoot(currentStep_);

                for (unsigned long j=0; j < n_; j++)
                {
                    double cov=0.0;
                    for (unsigned long k=0; k < F_; k++)
                        cov += A[startIndexOfSwapRate_[i]][k]*A[j][k];
                    covariances[j] = cov;

                }
                covariances_.push_back(covariances);
        }

    }

    void ForwardRateConstrainedEuler::SetThisConstraint(
                                const std::vector<Rate>& rateConstraints,
                                const std::vector<bool>& isConstraintActive)
    {
        QL_REQUIRE(rateConstraints.size() == numeraires_.size(),
                   "wrong number of constraints specified");

        QL_REQUIRE(isConstraintActive.size() == numeraires_.size(),
                   "wrong number of isConstraintActive specified");

        rateConstraints_=rateConstraints;
        isConstraintActive_ = isConstraintActive;

        for (unsigned long i=0; i < rateConstraints_.size(); i++)
            rateConstraints_[i] = log(rateConstraints_[i]+displacements_[i]);
    }


    Real ForwardRateConstrainedEuler::startNewPath() {
        currentStep_ = 0;
        std::copy(initialLogForwards_.begin(), initialLogForwards_.end(),
                  logForwards_.begin());
        return generator_->nextPath();
    }

    Real ForwardRateConstrainedEuler::advanceStep()
    {
        // we're going from T1 to T2

        // a) compute drifts D1 at T1;
        if (currentStep_ > 0) {
            calculators_[currentStep_].compute(forwards_, drifts1_);
        } else {
            std::copy(initialDrifts_.begin(), initialDrifts_.end(),
                      drifts1_.begin());
        }

        // b) evolve forwards up to T2 using D1;
        Real weight = generator_->nextStep(brownians_);
        const Matrix& A = marketModel_->pseudoRoot(currentStep_);
        const std::vector<Real>& fixedDrift = fixedDrifts_[currentStep_];

        Size alive = alive_[currentStep_];
        for (Size i=alive; i<n_; i++) {
            logForwards_[i] += drifts1_[i] + fixedDrift[i];
            logForwards_[i] +=
                std::inner_product(A.row_begin(i), A.row_end(i),
                brownians_.begin(), 0.0);
        }

        // check constraint active
        if (isConstraintActive_[currentStep_]) {
            Size index =startIndexOfSwapRate_[currentStep_];

            // compute error
            Real requiredShift =  rateConstraints_[index] - logForwards_[index];

            Real multiplier = requiredShift/variances_[currentStep_][index];

            // now shift each rate by multiplier * weighting of index rate
            // across the step
            for (Size i=alive; i<n_; i++) {
                // we only need a small part of cov matrix
                logForwards_[i] += multiplier*covariances_[currentStep_][i];
            }

            // now we need to calculate the LR shift, we have moved Gaussian Z_k by multiplier * a_{index,k}
            // divide original density by density of shifted normal
            Real weightsEffect = 1.0;

            CumulativeNormalDistribution phi;
            for (Size k=0; k < F_; k++) {
                Real shift = multiplier * A[index][k];
                Real originalDensity = phi.derivative(brownians_[k]+shift);
                // the density of the draw after changes in original measure
                Real newDensity = phi.derivative(brownians_[k]);
                // the density of the draw after changes in new measure, shifts cancel
                weightsEffect*= originalDensity/newDensity;
            }

            weight *= weightsEffect;
        }

        for (Size i=alive; i<n_; i++)
            forwards_[i] = std::exp(logForwards_[i]) - displacements_[i];

        // c) update curve state
        curveState_.setOnForwardRates(forwards_);

        ++currentStep_;

        return weight;
    }

    Size ForwardRateConstrainedEuler::currentStep() const {
        return currentStep_;
    }

    const CurveState& ForwardRateConstrainedEuler::currentState() const {
        return curveState_;
    }

}
