/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file analyticcapfloorengine.hpp
    \brief Analytic engine for caps/floors
*/

#ifndef quantlib_pricers_analytical_cap_floor_hpp
#define quantlib_pricers_analytical_cap_floor_hpp

#include <ql/instruments/capfloor.hpp>
#include <ql/pricingengines/genericmodelengine.hpp>
#include <ql/models/shortrate/model.hpp>

namespace QuantLib {

    //! Analytic engine for cap/floor
    /*! \ingroup capfloorengines */
    class AnalyticCapFloorEngine
        : public GenericModelEngine<AffineModel,
                                    CapFloor::arguments,
                                    CapFloor::results > {
      public:
        AnalyticCapFloorEngine(const boost::shared_ptr<AffineModel>& model)
        : GenericModelEngine<AffineModel,
                             CapFloor::arguments,
                             CapFloor::results >(model)
        {}
        void calculate() const;
    };

}


#endif
