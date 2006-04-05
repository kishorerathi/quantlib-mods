
/*
 Copyright (C) 2005 Plamen Neykov
 Copyright (C) 2005, 2006 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <qla/config.hpp>
#endif
#include <qla/cliquetoption.hpp>
#include <qla/typefactory.hpp>
#include <qla/generalutils.hpp>
#include <qla/exercise.hpp>

namespace QuantLibAddin {

    CliquetOption::CliquetOption(
            const std::string &handleBlackScholes,
            const std::string &optionTypeID,
            const double &strike,
            const long &expiryDate,
            const std::vector < long > &resetDates,
            const std::string &engineID,
            const long &timeSteps) {

        OH_GET_REFERENCE(blackScholesProcess, handleBlackScholes, 
            BlackScholesProcess, QuantLib::BlackScholesProcess)

        QuantLib::Option::Type type = 
            Create<QuantLib::Option::Type>()(optionTypeID);

        boost::shared_ptr<QuantLib::PercentageStrikePayoff> payoff(
            new QuantLib::PercentageStrikePayoff(type, strike));
        boost::shared_ptr<QuantLib::EuropeanExercise> exercise(
            new QuantLib::EuropeanExercise(
                QuantLib::Date(expiryDate)));
        boost::shared_ptr<QuantLib::PricingEngine> pricingEngine =
            Create<boost::shared_ptr<QuantLib::PricingEngine> >()(engineID, timeSteps);
        std::vector<QuantLib::Date> resetDatesQL =
            longVectorToDateVector(resetDates);
        mInstrument = boost::shared_ptr<QuantLib::CliquetOption>(
            new QuantLib::CliquetOption(
                blackScholesProcess, 
                payoff, 
                exercise, 
                resetDatesQL,
                pricingEngine));
    }

}

