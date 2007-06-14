
/*
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2007 Chiara Fornarola
 Copyright (C) 2006, 2007 Marco Bianchetti
 Copyright (C) 2006, 2007 Cristina Duminuco
 Copyright (C) 2006, 2007 Giorgio Facchinetti
 Copyright (C) 2007 Katiuscia Manzoni

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

#ifndef qla_correlation_hpp
#define qla_correlation_hpp

#include <oh/objecthandler.hpp>
#include <ql/types.hpp>

namespace QuantLib {
    class LmCorrelationModel;
    class PiecewiseConstantCorrelation;
    class Matrix;
    class CurveState;
    class EvolutionDescription;
    class Date;
    class Period;
    class InterestRateIndex;
    class IborIndex;
    class SwapIndex;
    class DayCounter;
    class HistoricalCorrelation;
}

namespace QuantLibAddin {

    class LmCorrelationModel : public ObjectHandler::LibraryObject<
        QuantLib::LmCorrelationModel> { };

    class LmLinearExponentialCorrelationModel : public LmCorrelationModel {
     public:
        LmLinearExponentialCorrelationModel(QuantLib::Size size,
                                            QuantLib::Real rho,
                                            QuantLib::Real beta,
                                            QuantLib::Size factors);

    };

    class PiecewiseConstantCorrelation : public
        ObjectHandler::LibraryObject<QuantLib::PiecewiseConstantCorrelation>{};

    class TimeHomogeneousForwardCorrelation : public PiecewiseConstantCorrelation {
      public:
        TimeHomogeneousForwardCorrelation(
                const QuantLib::Matrix& fwdCorrelation,
                const std::vector<QuantLib::Time>& rateTimes);
    };

    class TimeHomogeneousTimeDependentForwardCorrelation : public PiecewiseConstantCorrelation {
      public:
        TimeHomogeneousTimeDependentForwardCorrelation(
                const std::vector<QuantLib::Time>& rateTimes,
                QuantLib::Real longTermCorr,
                QuantLib::Real beta,
                QuantLib::Real gamma);
    };

    class CotSwapFromFwdCorrelation : public PiecewiseConstantCorrelation {
      public:
        CotSwapFromFwdCorrelation(
            const QuantLib::Matrix& correlations,
            const QuantLib::CurveState& curveState,
            QuantLib::Real displacement,
            const QuantLib::EvolutionDescription& evolution);
    };
  
    QuantLib::Matrix qlHistCorrZeroYieldLinear(
            const QuantLib::Date& startDate, 
            const QuantLib::Date& endDate, 
            const QuantLib::Period& step,
            const boost::shared_ptr<QuantLib::InterestRateIndex>& fwdIndex,
            const QuantLib::Period& initialGap,
            const QuantLib::Period& horizon,
            const std::vector<boost::shared_ptr<QuantLib::IborIndex> >&,
            const std::vector<boost::shared_ptr<QuantLib::SwapIndex> >&,
            const QuantLib::DayCounter& yieldCurveDayCounter,
            QuantLib::Real yieldCurveAccuracy);

    class HistoricalCorrelation : public
        ObjectHandler::LibraryObject<QuantLib::HistoricalCorrelation> {
      public:
        HistoricalCorrelation(
                const QuantLib::Date& startDate,
                const QuantLib::Date& endDate,
                const QuantLib::Period& step,
                const boost::shared_ptr<QuantLib::InterestRateIndex>& fwdIndex,
                const QuantLib::Period& initialGap,
                const QuantLib::Period& horizon,
                const std::vector<boost::shared_ptr<QuantLib::IborIndex> >& iborIndexes,
                const std::vector<boost::shared_ptr<QuantLib::SwapIndex> >& swapIndexes,
                const QuantLib::DayCounter& yieldCurveDayCounter,
                QuantLib::Real yieldCurveAccuracy);

         QuantLib::Matrix calculate(
               const QuantLib::Date& startDate, 
               const QuantLib::Date& endDate, 
               const QuantLib::Period& step,
               const boost::shared_ptr<QuantLib::InterestRateIndex>& fwdIndex,
               const QuantLib::Period& initialGap,
               const QuantLib::Period& horizon,
               const std::vector<boost::shared_ptr<QuantLib::IborIndex> >& iborIndexes,
               const std::vector<boost::shared_ptr<QuantLib::SwapIndex> >& swapIndexes,
               const QuantLib::DayCounter& yieldCurveDayCounter,
               QuantLib::Real yieldCurveAccuracy); 
    };
}

#endif
