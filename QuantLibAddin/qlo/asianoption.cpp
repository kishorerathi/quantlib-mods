
/*
 Copyright (C) 2005 Plamen Neykov
 Copyright (C) 2005, 2006 Eric Ehlers

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

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <qlo/config.hpp>
#endif

#include <qlo/asianoption.hpp>

#include <ql/instruments/asianoption.hpp>
#include <ql/processes/blackscholesprocess.hpp>

namespace QuantLibAddin {

    ContinuousAveragingAsianOption::ContinuousAveragingAsianOption(
            QuantLib::Average::Type averageType,
            const boost::shared_ptr<QuantLib::GeneralizedBlackScholesProcess>& blackScholesProcess,
            const boost::shared_ptr<QuantLib::StrikedTypePayoff>& payoff,
            const boost::shared_ptr<QuantLib::Exercise>& exercise,
            const boost::shared_ptr<QuantLib::PricingEngine>& pricingEngine)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::Instrument>(new
            QuantLib::ContinuousAveragingAsianOption(averageType,
                                                     blackScholesProcess, 
                                                     payoff, 
                                                     exercise, 
                                                     pricingEngine));
     }

    DiscreteAveragingAsianOption::DiscreteAveragingAsianOption(
            QuantLib::Average::Type averageType,
            QuantLib::Real runningAccumulator,
            QuantLib::Size pastFixings,
            const std::vector<QuantLib::Date>& fixingDates,
            const boost::shared_ptr<QuantLib::GeneralizedBlackScholesProcess>& blackScholesProcess,
            const boost::shared_ptr<QuantLib::StrikedTypePayoff>& payoff,
            const boost::shared_ptr<QuantLib::Exercise>& exercise,
            const boost::shared_ptr<QuantLib::PricingEngine>& pricingEngine)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::Instrument>(new
            QuantLib::DiscreteAveragingAsianOption(averageType,
                                                   runningAccumulator,
                                                   pastFixings,
                                                   fixingDates,
                                                   blackScholesProcess, 
                                                   payoff, 
                                                   exercise, 
                                                   pricingEngine));
    }

}
