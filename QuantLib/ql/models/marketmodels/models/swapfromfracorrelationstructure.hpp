/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2007 Mark Joshi

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

#ifndef quantlib_fra_time_dep_corr_struct_hpp
#define quantlib_fra_time_dep_corr_struct_hpp

#include <ql/models/marketmodels/models/timedependantcorrelationstructure.hpp>
#include <ql/models/marketmodels/evolutiondescription.hpp>
#include <vector>

namespace QuantLib {

    class CurveState;

    class SwapFromFRACorrelationStructure : public TimeDependantCorrelationStructure {
      public:
        SwapFromFRACorrelationStructure(
            const Matrix& correlations,
            const CurveState& curveState,
            Real displacement,
            const EvolutionDescription& evolution,
            Size numberOfFactors);
        //const EvolutionDescription& evolution() const;
        const std::vector<Time>& times() const;
        const Matrix& pseudoRoot(Size i) const;
        Size numberOfFactors() const;
        Size numberOfRates() const;
    private:
        std::vector<Matrix> fraCorrelationMatrix_;
        std::vector<Matrix> pseudoRoots_;
        Size numberOfFactors_, numberOfRates_;
        EvolutionDescription evolution_;
    };

}

#endif
