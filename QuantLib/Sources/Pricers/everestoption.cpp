
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, contact ferdinando@ametrano.net
 * The license is also available at http://quantlib.sourceforge.net/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.sourceforge.net/Authors.txt
*/

/*! \file everestoption.cpp

    $Source$
    $Log$
    Revision 1.8  2001/05/24 13:57:52  nando
    smoothing #include xx.hpp and cutting old Log messages

*/

#include "ql/handle.hpp"
#include "ql/MonteCarlo/everestpathpricer.hpp"
#include "ql/Pricers/everestoption.hpp"

namespace QuantLib {

    namespace Pricers {

        using MonteCarlo::MultiPathPricer;
        using MonteCarlo::StandardMultiPathGenerator;
        using MonteCarlo::EverestPathPricer;
        using MonteCarlo::MultiFactorMonteCarloOption;

        EverestOption::EverestOption(const Array &dividendYield,
                                     const Math::Matrix &covariance,
                                     Rate riskFreeRate,
                                     Time residualTime,
                                     long samples, long seed)
        : MultiFactorPricer(samples, seed){
            int  n = covariance.rows();
            QL_REQUIRE(covariance.columns() == n,
                "EverestOption: covariance matrix not square");
            QL_REQUIRE(dividendYield.size() == n,
                "EverestOption: dividendYield size does not match"
                " that of covariance matrix");
            QL_REQUIRE(residualTime > 0,
                "EverestOption: residualTime must be positive");

            //! Initialize the path generator
            Array mu(riskFreeRate - dividendYield
                                    - 0.5 * covariance.diagonal());

            std::vector<Time> timeDisp(1);
            timeDisp[0] = residualTime;
            Handle<StandardMultiPathGenerator> pathGenerator(
                    new StandardMultiPathGenerator(timeDisp,
                                                   covariance,
                                                   mu,
                                                   seed));

            //! Initialize the pricer on the path pricer
            Handle<MultiPathPricer> pathPricer(
                new EverestPathPricer(QL_EXP(-riskFreeRate*residualTime)));

             //! Initialize the multi-factor Monte Carlo
            montecarloPricer_ = MultiFactorMonteCarloOption(
                                        pathGenerator, pathPricer);
        }

    }

}
