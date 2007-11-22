
/*
 Copyright (C) 2006 Eric Ehlers

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

#ifndef qla_europeanoption_hpp
#define qla_europeanoption_hpp

#include <qlo/baseinstruments.hpp>

namespace QuantLib {
    class StrikedTypePayoff;
    class Exercise;
}

namespace QuantLibAddin {

    class EuropeanOption : public OneAssetOption {
    public:
        EuropeanOption(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            const boost::shared_ptr<QuantLib::StrikedTypePayoff>& payoff,
            const boost::shared_ptr<QuantLib::Exercise>& exercise,
            bool permanent);
    };

}

#endif
