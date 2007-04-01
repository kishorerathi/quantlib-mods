/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Sadruddin Rejeb
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

#include <ql/pricingengines/vanilla/discretizedvanillaoption.hpp>
#include <vector>

namespace QuantLib {

    void DiscretizedVanillaOption::reset(Size size) {
        values_ = Array(size, 0.0);
        adjustValues();
    }

    void DiscretizedVanillaOption::postAdjustValuesImpl() {

        Time now = time();
        Size i;
        switch (arguments_.exercise->type()) {
          case Exercise::American:
            if (now <= arguments_.stoppingTimes[1] &&
                now >= arguments_.stoppingTimes[0])
                applySpecificCondition();
            break;
          case Exercise::European:
            if (isOnTime(arguments_.stoppingTimes[0]))
                applySpecificCondition();
            break;
          case Exercise::Bermudan:
            for (i = 0; i<arguments_.stoppingTimes.size(); i++) {
                if (isOnTime(arguments_.stoppingTimes[i]))
                    applySpecificCondition();
            }
            break;
          default:
            QL_FAIL("invalid option type");
        }
    }

    void DiscretizedVanillaOption::applySpecificCondition() {
        Array grid = method()->grid(time());
        for (Size j=0; j<values_.size(); j++) {
            values_[j] = std::max(values_[j],
                                  (*arguments_.payoff)(grid[j]));
        }
    }

}

