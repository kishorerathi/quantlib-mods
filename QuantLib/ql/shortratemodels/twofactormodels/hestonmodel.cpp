/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 Klaus Spanderen

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

#include <ql/quotes/simplequote.hpp>
#include <ql/shortratemodels/twofactormodels/hestonmodel.hpp>

namespace QuantLib {

    class HestonModel::VolatilityConstraint : public Constraint {
      private:
        class Impl : public Constraint::Impl {
          public:
            bool test(const Array& params) const {
                const Real theta = params[0];
                const Real kappa = params[1];
                const Real sigma = params[2];

                return (sigma >= 0.0 && sigma*sigma < 2.0*kappa*theta);
            }
        };
      public:
        VolatilityConstraint()
        : Constraint(boost::shared_ptr<Constraint::Impl>(
                                           new VolatilityConstraint::Impl)) {}
    };

    HestonModel::HestonModel(const boost::shared_ptr<HestonProcess> & process)
    : CalibratedModel(5),
      v0_   (process->v0()),
      kappa_(process->kappa()),
      theta_(process->theta()),
      sigma_(process->sigma()),
      rho_  (process->rho()) {
        arguments_[0] = ConstantParameter(process->theta()->value(),
                                          PositiveConstraint());
        arguments_[1] = ConstantParameter(process->kappa()->value(),
                                          PositiveConstraint());
        arguments_[2] = ConstantParameter(process->sigma()->value(),
                                          PositiveConstraint());
        arguments_[3] = ConstantParameter(process->rho()->value(),
                                          BoundaryConstraint(-1.0, 1.0));
        arguments_[4] = ConstantParameter(process->v0()->value(), 
                                          PositiveConstraint());
	}

    void HestonModel::generateArguments() {
        v0_.linkTo   (boost::shared_ptr<Quote>(new SimpleQuote(v0())));
        kappa_.linkTo(boost::shared_ptr<Quote>(new SimpleQuote(kappa())));
        theta_.linkTo(boost::shared_ptr<Quote>(new SimpleQuote(theta())));
        sigma_.linkTo(boost::shared_ptr<Quote>(new SimpleQuote(sigma())));
        rho_.linkTo  (boost::shared_ptr<Quote>(new SimpleQuote(rho())));
    }
}

