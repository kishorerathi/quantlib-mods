/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000-2005 StatPro Italia srl

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

#include <ql/FiniteDifferences/bsmoperator.hpp>

namespace QuantLib {

    BSMOperator::BSMOperator(Size size, Real dx, Rate r,
                             Rate q, Volatility sigma)
    : TridiagonalOperator(size) {
        Real sigma2 = sigma*sigma;
        Real nu = r-q-sigma2/2;
        Real pd = -(sigma2/dx-nu)/(2*dx);
        Real pu = -(sigma2/dx+nu)/(2*dx);
        Real pm = sigma2/(dx*dx)+r;
        setMidRows(pd,pm,pu);
    }

    BSMOperator::BSMOperator(
                        const Array& grid,
                        const boost::shared_ptr<BlackScholesProcess>& process,
                        Time residualTime)
    : TridiagonalOperator(grid.size()) {
        Real u = process->stateVariable()->value();
        Volatility sigma =
            process->diffusion(residualTime,u);
        Rate r = process->riskFreeRate()->zeroRate(residualTime,Continuous);
        Array logGrid = Log(grid);
        Real sigma2 = sigma * sigma;
        Real nu = process->drift(residualTime,u);
        for (Size i=1; i < logGrid.size()-1; i++) {
            Real dxm = logGrid[i] - logGrid[i-1];
            Real dxp = logGrid[i+1] - logGrid[i];
            Real pd = -(sigma2/dxm-nu)/(dxm+dxp);
            Real pu = -(sigma2/dxp+nu)/(dxm+dxp);
            Real pm = sigma2/(dxm*dxp)+r;
            setMidRow(i, pd,pm,pu);
        }
    }

}
