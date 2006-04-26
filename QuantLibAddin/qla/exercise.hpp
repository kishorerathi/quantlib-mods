
/*
 Copyright (C) 2006 Eric Ehlers

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

#ifndef qla_exercise_hpp
#define qla_exercise_hpp

#include <oh/objhandler.hpp>
#include <ql/exercise.hpp>

namespace QuantLibAddin {

    class Exercise : public ObjHandler::Object {
    public:
        Exercise(const boost::shared_ptr < InstanceName > &instanceName) : ObjHandler::Object(instanceName) {}
        virtual boost::shared_ptr<void> getReference() const {
            return boost::static_pointer_cast<void>(exercise_);
        }
        const QuantLib::Exercise& getObject() const { 
            return *exercise_; 
        }
    protected:
        boost::shared_ptr<QuantLib::Exercise> exercise_;
    };

    class AmericanExercise : public Exercise {
    public:
        AmericanExercise(
            const boost::shared_ptr < InstanceName > &instanceName,
            const long &earliestDate,
            const long &latestDate,
            const bool &payoffAtExpiry);
    };

    class EuropeanExercise : public Exercise {
    public:
        EuropeanExercise(
            const boost::shared_ptr < InstanceName > &instanceName,
            const long &expiryDate);
    };

    class BermudanExercise : public Exercise {
    public:
        BermudanExercise(
            const boost::shared_ptr < InstanceName > &instanceName,
            const std::vector < long > &dates,
            const bool &payoffAtExpiry);
    };

}

#endif

