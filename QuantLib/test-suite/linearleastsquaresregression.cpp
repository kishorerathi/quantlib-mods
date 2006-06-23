/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Klaus Spanderen

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

#include "linearleastsquaresregression.hpp"
#include "utilities.hpp"

#include <ql/Math/functional.hpp>
#include <ql/RandomNumbers/rngtraits.hpp>
#include <ql/Math/linearleastsquaresregression.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(LinearLeastSquaresRegressionTest)

void teardown() {
    Settings::instance().evaluationDate() = Date();
}

QL_END_TEST_LOCALS(LinearLeastSquaresRegressionTest)

void LinearLeastSquaresRegressionTest::testRegression() {

    BOOST_MESSAGE("Testing linear least squares regression");

    QL_TEST_BEGIN
    const Real tolerance = 0.025;

    const Size nr=100000;
    PseudoRandom::rng_type rng(MersenneTwisterUniformRng(1234u));

    std::vector<boost::function1<Real, Real> > v;
    v.push_back(constant<Real, Real>(1.0));
    v.push_back(identity<Real>());
    v.push_back(square<Real>());
    v.push_back(std::ptr_fun<Real, Real>(std::sin));

    std::vector<boost::function1<Real, Real> > w(v);
    w.push_back(square<Real>());

    for (Size k=0; k<3; ++k) {
        Size i;
        const Real a[] = {rng.next().value,
                          rng.next().value,
                          rng.next().value,
                          rng.next().value};

        Array x(nr), y(nr);
        for (i=0; i<nr; ++i) {
            x[i] = rng.next().value;

            // regression in y = a_1 + a_2*x + a_3*x^2 + a_4*sin(x) + eps
            y[i] =  a[0]*v[0](x[i]) + a[1]*v[1](x[i]) + a[2]*v[2](x[i])
                  + a[3]*v[3](x[i]) + rng.next().value;
        }

        LinearLeastSquaresRegression m(x, y, v);

        for (i=0; i<v.size(); ++i) {
            if (m.err()[i] > tolerance) {
                BOOST_ERROR("Failed to reproduce linear regression coef."
                            << "\n    error:     " << m.err()[i]
                            << "\n    tolerance: " << tolerance);
            }
            if (std::fabs(m.a()[i]-a[i]) > 3*m.err()[i]) {
                BOOST_ERROR("Failed to reproduce linear regression coef."
                            << "\n    calculated: " << m.a()[i]
                            << "\n    error:      " << m.err()[i]
                            << "\n    expected:   " << a[i]);
            }
        }

        m = LinearLeastSquaresRegression(x, y, w);
        const Real ma[] = {m.a()[0], m.a()[1], m.a()[2]+m.a()[4],m.a()[3]};
        const Real err[] = {m.err()[0], m.err()[1],
                            std::sqrt( m.err()[2]*m.err()[2]
                                      +m.err()[4]*m.err()[4]), m.err()[3]};
        for (i=0; i<v.size(); ++i) {
            if (std::fabs(ma[i] - a[i]) > 3*err[i]) {
                BOOST_ERROR("Failed to reproduce linear regression coef."
                            << "\n    calculated: " << ma[i]
                            << "\n    error:      " << err[i]
                            << "\n    expected:   " << a[i]);
            }
        }
    }

    QL_TEST_TEARDOWN
}


test_suite* LinearLeastSquaresRegressionTest::suite() {
    test_suite* suite =
        BOOST_TEST_SUITE("linear least squares regression tests");

    suite->add(BOOST_TEST_CASE(
                          &LinearLeastSquaresRegressionTest::testRegression));
    return suite;
}

