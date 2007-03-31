/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2005 StatPro Italia srl

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

#include <ql/PricingEngines/Swaption/treeswaptionengine.hpp>
#include <ql/PricingEngines/Swaption/discretizedswaption.hpp>

namespace QuantLib {

    TreeVanillaSwapEngine::TreeVanillaSwapEngine(
                               const boost::shared_ptr<ShortRateModel>& model,
                               Size timeSteps)
    : LatticeShortRateModelEngine<VanillaSwap::arguments, VanillaSwap::results>
    (model, timeSteps) {}

    TreeVanillaSwapEngine::TreeVanillaSwapEngine(
                               const boost::shared_ptr<ShortRateModel>& model,
                               const TimeGrid& timeGrid)
    : LatticeShortRateModelEngine<VanillaSwap::arguments, VanillaSwap::results>
    (model, timeGrid) {}

    void TreeVanillaSwapEngine::calculate() const {

        QL_REQUIRE(model_, "no model specified");

        DiscretizedSwap swap(arguments_);
        std::vector<Time> times = swap.mandatoryTimes();

        boost::shared_ptr<Lattice> lattice;
        if (lattice_) {
            lattice = lattice_;
        } else {
            TimeGrid timeGrid(times.begin(), times.end(), timeSteps_);
            lattice = model_->tree(timeGrid);
        }

        swap.initialize(lattice, times.back());
        swap.rollback(0.0);

        results_.value = swap.presentValue();
    }


    TreeSwaptionEngine::TreeSwaptionEngine(
                               const boost::shared_ptr<ShortRateModel>& model,
                               Size timeSteps)
    : LatticeShortRateModelEngine<Swaption::arguments, Swaption::results>
    (model, timeSteps) {}

    TreeSwaptionEngine::TreeSwaptionEngine(
                               const boost::shared_ptr<ShortRateModel>& model,
                               const TimeGrid& timeGrid)
    : LatticeShortRateModelEngine<Swaption::arguments, Swaption::results>
    (model, timeGrid) {}

    void TreeSwaptionEngine::calculate() const {

        QL_REQUIRE(arguments_.settlementType==Settlement::Physical,
                   "cash-settled swaptions not priced with tree engine");
        QL_REQUIRE(model_, "no model specified");

        DiscretizedSwaption swaption(arguments_);
        boost::shared_ptr<Lattice> lattice;

        if (lattice_) {
            lattice = lattice_;
        } else {
            std::vector<Time> times = swaption.mandatoryTimes();
            TimeGrid timeGrid(times.begin(), times.end(), timeSteps_);
            lattice = model_->tree(timeGrid);
        }

        swaption.initialize(lattice, arguments_.stoppingTimes.back());

        Time nextExercise =
            *std::find_if(arguments_.stoppingTimes.begin(),
                          arguments_.stoppingTimes.end(),
                          std::bind2nd(std::greater_equal<Time>(), 0.0));
        swaption.rollback(nextExercise);

        results_.value = swaption.presentValue();
    }

}
