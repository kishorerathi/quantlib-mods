/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2006 Cristina Duminuco

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

#include <ql/PricingEngines/Swaption/blackswaptionengine.hpp>
#include <ql/Instruments/swaption.hpp>

namespace QuantLib {

    void BlackSwaptionEngine::calculate() const {
        static const Spread basisPoint = 1.0e-4;
        Time exercise = arguments_.stoppingTimes[0];
        Real w = arguments_.payFixed ? 1.0 : -1.0;
        Real annuity;
        switch(arguments_.settlementType) {
          case Settlement::Physical :
            annuity = arguments_.fixedBPS/basisPoint;
            break;
          case Settlement::Cash:
            annuity = arguments_.fixedCashBPS/basisPoint;
            break;
          default:
            QL_FAIL("unknown settlement type");
        }
        results_.value = annuity * BlackModel::formula(
                                 arguments_.fairRate, arguments_.fixedRate,
                                 model_->volatility()*std::sqrt(exercise), w);
    }

}
