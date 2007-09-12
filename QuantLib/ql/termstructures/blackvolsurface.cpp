/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano

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

#include <ql/termstructures/blackvolsurface.hpp>
#include <ql/termstructures/volatilities/smilesection.hpp>

namespace QuantLib {

    BlackVolSurface::BlackVolSurface(const DayCounter& dc)
    : BlackAtmVolCurve(dc) {}

    BlackVolSurface::BlackVolSurface(const Date& refDate,
                                     const Calendar& cal,
                                     const DayCounter& dc)
    : BlackAtmVolCurve(refDate, cal, dc) {}

    BlackVolSurface::BlackVolSurface(Natural settlDays,
                                     const Calendar& cal,
                                     const DayCounter& dc)
    : BlackAtmVolCurve(settlDays, cal, dc) {}

    Real BlackVolSurface::atmVarianceImpl(Time t) const {
        return smileSectionImpl(t)->variance();
    }

    Volatility BlackVolSurface::atmVolImpl(Time t) const {
        return smileSectionImpl(t)->volatility();
    }

    void BlackVolSurface::accept(AcyclicVisitor& v) {
        Visitor<BlackVolSurface>* v1 =
            dynamic_cast<Visitor<BlackVolSurface>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            QL_FAIL("not a BlackVolSurface term structure visitor");
    }

}
