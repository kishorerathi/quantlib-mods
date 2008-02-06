/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2006 Marco Bianchetti
 Copyright (C) 2006, 2007 Eric Ehlers
 Copyright (C) 2006 Giorgio Facchinetti
 Copyright (C) 2006 Chiara Fornarola
 Copyright (C) 2007 Katiuscia Manzoni
 Copyright (C) 2005 Plamen Neykov

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

#ifndef qla_enumpairctors_hpp
#define qla_enumpairctors_hpp

#include <qlo/qladdindefines.hpp>
#include <qlo/Enumerations/Factories/termstructuresfactory.hpp>
#include <qlo/Enumerations/Factories/historicalforwardratesanalysisfactory.hpp>

namespace QuantLibAddin {

    /* *** YieldTermStructures *** */

    /* *** Discount based *** */
    boost::shared_ptr<QuantLib::YieldTermStructure> DISCOUNT_BACKWARDFLAT_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> DISCOUNT_FORWARDFLAT_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> DISCOUNT_LINEAR_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> DISCOUNT_LOGLINEAR_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> DISCOUNT_LOGCUBIC_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> DISCOUNT_CUBIC_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);

    /* *** ZeroYield based *** */
    boost::shared_ptr<QuantLib::YieldTermStructure> ZEROYIELD_BACKWARDFLAT_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> ZEROYIELD_FORWARDFLAT_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> ZEROYIELD_LINEAR_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> ZEROYIELD_LOGLINEAR_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> ZEROYIELD_LOGCUBIC_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> ZEROYIELD_CUBIC_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);

    /* *** ForwardRate based *** */
    boost::shared_ptr<QuantLib::YieldTermStructure> FORWARDRATE_BACKWARDFLAT_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> FORWARDRATE_FORWARDFLAT_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> FORWARDRATE_LINEAR_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> FORWARDRATE_LOGLINEAR_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> FORWARDRATE_LOGCUBIC_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);
    boost::shared_ptr<QuantLib::YieldTermStructure> FORWARDRATE_CUBIC_PiecewiseYieldCurve(
            QuantLib::Natural nDays,
            const QuantLib::Calendar& calendar,
            const std::vector<boost::shared_ptr<QuantLib::RateHelper> >& rateHelpers,
            const QuantLib::DayCounter& dayCounter,
            const QuantLib::Handle<QuantLib::Quote>& turnOfYearEffect,
            QuantLib::Real accuracy);

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> DISCOUNT_BACKWARDFLAT_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

        boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> DISCOUNT_FORWARDFLAT_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> DISCOUNT_LINEAR_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> DISCOUNT_LOGLINEAR_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> DISCOUNT_CUBIC_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> ZEROYIELD_BACKWARDFLAT_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> ZEROYIELD_FORWARDFLAT_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> ZEROYIELD_LINEAR_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> ZEROYIELD_LOGLINEAR_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> ZEROYIELD_CUBIC_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> FORWARDRATE_BACKWARDFLAT_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> FORWARDRATE_FORWARDFLAT_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> FORWARDRATE_LINEAR_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> FORWARDRATE_LOGLINEAR_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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

    boost::shared_ptr<QuantLib::HistoricalForwardRatesAnalysis> FORWARDRATE_CUBIC_HistoricalForwardRatesAnalysis (
            const boost::shared_ptr<QuantLib::SequenceStatistics>& stats,
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
  
}

#endif
