

/*
 Copyright (C) 2000, 2001, 2002 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file onefactormodel.hpp
    \brief Abstract one-factor interest rate model class

    \fullpath
    ql/InterestRateModelling/%onefactormodel.hpp
*/

// $Id$

#ifndef quantlib_interest_rate_modelling_one_factor_model_h
#define quantlib_interest_rate_modelling_one_factor_model_h

#include <ql/InterestRateModelling/model.hpp>
#include <ql/InterestRateModelling/trinomialtree.hpp>

namespace QuantLib {

    namespace InterestRateModelling {

        class OneFactorModel : public Model {
          public:
            OneFactorModel(
                Size nParams,
                const RelinkableHandle<TermStructure>& termStructure)
            : Model(nParams, OneFactor, termStructure) {}
            virtual ~OneFactorModel() {}
            virtual double minStateVariable() const { return -QL_MAX_DOUBLE;}
            virtual double maxStateVariable() const { return QL_MAX_DOUBLE;}

            const Handle<ShortRateProcess>& process() const {
                return process_;
            }

            virtual Handle<Lattices::Tree> tree(
                const Lattices::TimeGrid& timeGrid) const {
                return Handle<Lattices::Tree>(
                    new TrinomialTree(process(), timeGrid));
            }

            //!Used by Jamshidian's decomposition pricing
            virtual double discountBond(Time now, Time maturity, Rate r) {
                return Null<double>();
            }

          protected:
            Handle<ShortRateProcess> process_;

          private:
            class FitFunction;
            friend class FitFunction;
        };

    }

}
#endif
