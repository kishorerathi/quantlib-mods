/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2004 StatPro Italia srl

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

#include <ql/pricingengines/capfloor/discretizedcapfloor.hpp>

namespace QuantLib {

    void DiscretizedCapFloor::preAdjustValuesImpl() {
        for (Size i=0; i<arguments_.startTimes.size(); i++) {
            if (isOnTime(arguments_.startTimes[i])) {
                Time end = arguments_.endTimes[i];
                Time tenor = arguments_.accrualTimes[i];
                DiscretizedDiscountBond bond;
                bond.initialize(method(), end);
                bond.rollback(time_);

                CapFloor::Type type = arguments_.type;
                Real gearing = arguments_.gearings[i];
                Real nominal = arguments_.nominals[i];

                if ( (type == CapFloor::Cap) ||
                     (type == CapFloor::Collar)) {
                    Real accrual = 1.0 + arguments_.capRates[i]*tenor;
                    Real strike = 1.0/accrual;
                    for (Size j=0; j<values_.size(); j++)
                        values_[j] += nominal*accrual*gearing*
                            std::max<Real>(strike - bond.values()[j], 0.0);
                }

                if ( (type == CapFloor::Floor) ||
                     (type == CapFloor::Collar)) {
                    Real accrual = 1.0 + arguments_.floorRates[i]*tenor;
                    Real strike = 1.0/accrual;
                    Real mult = (type == CapFloor::Floor)?1.0:-1.0;
                    for (Size j=0; j<values_.size(); j++)
                        values_[j] += nominal*accrual*mult*gearing*
                            std::max<Real>(bond.values()[j] - strike, 0.0);
                }
            }
        }
    }

    void DiscretizedCapFloor::postAdjustValuesImpl() {
        for (Size i=0; i<arguments_.endTimes.size(); i++) {
            if (isOnTime(arguments_.endTimes[i])) {
                if (arguments_.startTimes[i] < 0.0) {
                    Real nominal = arguments_.nominals[i];
                    Time accrual = arguments_.accrualTimes[i];
                    Rate fixing = arguments_.forwards[i];
                    Real gearing = arguments_.gearings[i];
                    CapFloor::Type type = arguments_.type;

                    if (type == CapFloor::Cap || type == CapFloor::Collar) {
                        Rate cap = arguments_.capRates[i];
                        Rate capletRate = std::max(fixing-cap, 0.0);
                        values_ += capletRate*accrual*nominal*gearing;
                    }

                    if (type == CapFloor::Floor || type == CapFloor::Collar) {
                        Rate floor = arguments_.floorRates[i];
                        Rate floorletRate = std::max(floor-fixing, 0.0);
                        if (type == CapFloor::Floor)
                            values_ += floorletRate*accrual*nominal*gearing;
                        else
                            values_ -= floorletRate*accrual*nominal*gearing;
                    }
                }
            }
        }
    }

}
