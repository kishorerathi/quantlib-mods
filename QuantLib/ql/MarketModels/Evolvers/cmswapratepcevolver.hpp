/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2006, 2007 Mark Joshi

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


#ifndef quantlib_swap_rate_pc_evolver_hpp
#define quantlib_swap_rate_pc_evolver_hpp

#include <ql/MarketModels/marketmodelevolver.hpp>
#include <ql/MarketModels/CurveStates/cmswapcurvestate.hpp>
#include <ql/MarketModels/DriftComputation/cmsmmdriftcalculator.hpp>

namespace QuantLib {

    class MarketModel;
    class BrownianGenerator;
    class BrownianGeneratorFactory;

    //! Predictor-Corrector
    class CmSwapRatePcEvolver : public MarketModelEvolver {
      public:
        CmSwapRatePcEvolver(const Size spanningForwards,
                            const boost::shared_ptr<MarketModel>&,
                            const BrownianGeneratorFactory&,
                            const std::vector<Size>& numeraires,
                            Size initialStep = 0);
        //! \name MarketModelEvolver interface
        //@{
        const std::vector<Size>& numeraires() const;
        Real startNewPath();
        Real advanceStep();
        Size currentStep() const;
        const CurveState& currentState() const;
        void setInitialState(const CurveState&);
        //@}
      private:
        void setCMSwapRates(const std::vector<Real>& swapRates);
        // inputs
        Size spanningForwards_;
        boost::shared_ptr<MarketModel> marketModel_;
        std::vector<Size> numeraires_;
        Size initialStep_;
        boost::shared_ptr<BrownianGenerator> generator_;
        // fixed variables
        std::vector<std::vector<Real> > fixedDrifts_;
         // working variables
        Size n_, F_;
        CMSwapCurveState curveState_;
        Size currentStep_;
        std::vector<Rate> swapRates_, displacements_, logSwapRates_, initialLogSwapRates_;
        std::vector<Real> drifts1_, drifts2_, initialDrifts_;
        Array brownians_, correlatedBrownians_;
        std::vector<Size> alive_;
        // helper classes
        std::vector<CMSMMDriftCalculator> calculators_;
    };

}

#endif
