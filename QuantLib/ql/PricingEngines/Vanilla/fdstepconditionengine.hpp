/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 Joseph Wang
 Copyright (C) 2007 StatPro Italia srl

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

/*! \file fdstepconditionengine.hpp
    \brief Finite-differences step-condition engine
*/

#ifndef quantlib_fd_step_condition_engine_hpp
#define quantlib_fd_step_condition_engine_hpp

#include <ql/PricingEngines/Vanilla/fdvanillaengine.hpp>
#include <ql/FiniteDifferences/fdtypedefs.hpp>
#include <ql/FiniteDifferences/boundarycondition.hpp>

namespace QuantLib {

    //! Finite-differences pricing engine for American-style vanilla options
    /*! \ingroup vanillaengines */
    class FDStepConditionEngine :  public FDVanillaEngine {
      public:
        FDStepConditionEngine(Size timeSteps, Size gridPoints,
                              bool timeDependent = false)
        : FDVanillaEngine(timeSteps, gridPoints,
                          timeDependent),
          controlBCs_(2), controlPrices_(gridPoints) {}
      protected:
        mutable boost::shared_ptr<StandardStepCondition> stepCondition_;
        mutable SampledCurve prices_;
        mutable TridiagonalOperator controlOperator_;
        mutable std::vector<boost::shared_ptr<bc_type> > controlBCs_;
        mutable SampledCurve controlPrices_;
        virtual void initializeStepCondition() const = 0;
        virtual void calculate(PricingEngine::results*) const;
    };

}


#endif
