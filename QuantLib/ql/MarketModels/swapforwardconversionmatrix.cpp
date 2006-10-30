/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Marco Bianchetti
 Copyright (C) 2006 Giorgio Facchinetti
 Copyright (C) 2006 Mark Joshi

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


#include <ql/MarketModels/swapforwardconversionmatrix.hpp>
#include <vector>

namespace QuantLib {

    SwapCovarianceApproximator::SwapCovarianceApproximator(
                                const CurveState& initialCurveState,
                                Size expiry,
                                Size maturity,
                                const std::vector<Spread>& displacements,
                                const Matrix& forwardCovarianceMatrix)
    : initialCurveState_(initialCurveState), 
      expiry_(expiry), maturity_(maturity), displacements_(displacements), 
      forwardCovarianceMatrix_(forwardCovarianceMatrix) {

        // Check requirements
        QL_REQUIRE(maturity_ <= forwardCovarianceMatrix_.columns(), 
            "maturity out of range");
        QL_REQUIRE(expiry_ < maturity_, "expiry larger than maturity");
        // QL_REQUIRE(alive???, "xxx");
        swapCovarianceMatrix_ = Matrix(forwardCovarianceMatrix_.rows(),
                                       forwardCovarianceMatrix_.columns(),
                                       0.0);
     }

    Disposable<Matrix> SwapCovarianceApproximator::swapCovarianceMatrix()
    {
        Disposable<Matrix> zzzMatrix = zzMatrix();
        swapCovarianceMatrix_ = 
            zzzMatrix * forwardCovarianceMatrix_ * transpose(zzzMatrix);
        return swapCovarianceMatrix_;
    }

    Disposable<Matrix> SwapCovarianceApproximator::zzMatrix()
    {
        CurveState newCurveState(
                        initialCurveState_.rateTimes().begin()+expiry_,
                        initialCurveState_.rateTimes().begin()+maturity_);
        newCurveState.setOnForwardRates(
                        initialCurveState_.forwardRates().begin()+expiry_,
                        initialCurveState_.forwardRates().begin()+maturity_);
        return zMatrix(newCurveState);
    }

    Disposable<Matrix> SwapCovarianceApproximator::zMatrix(const CurveState& cs) 
    {
        Matrix result = swapForwardJacobian(cs);    // derivative dsr[i]/df[j]
        const std::vector<Rate> f = cs.forwardRates();  // forward rates
        const std::vector<Rate> sr = cs.coterminalSwapRates();  // coterminal swap rates
        for (Size i=0; i<sr.size(); ++i) {          // i = swap rate index
            for (Size j=i; j<f.size(); ++j) {       // j = forward rate index
                result[i][j] *= (f[j]+displacements_[j])
                                /(sr[i]+displacements_[i]);
            }
        }
        return result;  // zMatrix = f[j]/sr[j] * dsr[i]/df[j]
    }

    Disposable<Matrix> SwapCovarianceApproximator::swapForwardJacobian(
        const CurveState& cs) 
    {
        std::vector<Real> b = cs.coterminalSwapRatesAnnuities();    // coterminal annuities
        std::vector<Real> a = std::vector<Real>(b.size());          // coterminal floating leg values
        Size n = b.size();                                          // n� of coterminal swaps
        const std::vector<Real> p = cs.discountRatios();            // discount factors
        const std::vector<Rate> f = cs.forwardRates();              // forward rates
        const std::vector<Time> tau = cs.rateTaus();                // accrual factors

        for (Size k=0; k<n; ++k)
            a[k] = p[k]-p[n];          // coterminal floating leg values
        Matrix jacobian = Matrix(n, n, 0.0);
        for (Size i=0; i<n; ++i) {     // i = swap rate index
            for (Size j=i; j<n; ++j) { // j = forward rate index
                //Real temp = tau[j]/1.0+f[j]*t[j];   ERROR: it's temp = tau[j]/(1.0+f[j]*t[j])
                //jacobian[i][j] =
                //    p[j+1]*tau[j]/b[i]-temp*a[j]/b[i]+temp*a[i]*b[j]/(b[i]*b[i]);
                // QUESTION: no displacements here, right ?
                jacobian[i][j] =    // derivative dsr[i]/df[j]
                    p[j+1]*tau[j]/b[i] + tau[j]/(1.0+f[j]*tau[j])*
                    (-a[j]*b[i]+a[i]*b[j])/(b[i]*b[i]);
            }
        }
        return jacobian;
    }
}
