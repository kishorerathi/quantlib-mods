/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006 Klaus Spanderen

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

#include <ql/Math/kronrodintegral.hpp>
#include <ql/ShortRateModels/LiborMarketModels/lfmcovarproxy.hpp>

namespace QuantLib {
    LfmCovarianceProxy::LfmCovarianceProxy(
        const boost::shared_ptr<LmVolatilityModel>& volaModel,
        const boost::shared_ptr<LmCorrelationModel>& corrModel)

    : LfmCovarianceParameterization(corrModel->size(), corrModel->factors()),
      volaModel_(volaModel),
      corrModel_(corrModel) {

        QL_REQUIRE(volaModel_->size() == corrModel_->size(),
                   "volatility and correlation model hava to have"
                   " the same size");
    }

    boost::shared_ptr<LmVolatilityModel>
    LfmCovarianceProxy::volatilityModel() const {
        return volaModel_;
    }

    boost::shared_ptr<LmCorrelationModel>
    LfmCovarianceProxy::correlationModel() const {
        return corrModel_;
    }

    Disposable<Matrix> LfmCovarianceProxy::diffusion(Time t,
                                                     const Array& x) const {

        Matrix pca = corrModel_->pseudoSqrt(t, x);
        Array  vol = volaModel_->volatility(t, x);
        for (Size i=0; i<size_; ++i) {
            std::transform(pca.row_begin(i), pca.row_end(i),
                           pca.row_begin(i),
                           std::bind2nd(std::multiplies<Real>(), vol[i]));
        }

        return pca;
    }

    Disposable<Matrix> LfmCovarianceProxy::covariance(Time t,
                                                      const Array& x) const {

        Array  volatility  = volaModel_->volatility(t, x);
        Matrix correlation = corrModel_->correlation(t, x);

        Matrix tmp(size_, size_);
        for (Size i=0; i<size_; ++i) {
            for (Size j=0; j<size_; ++j) {
                tmp[i][j] = volatility[i]*correlation[i][j]*volatility[j];
            }
        }

        return tmp;
    }

    class LfmCovarianceProxy::Var_Helper {
      public:
        Var_Helper(const LfmCovarianceProxy* proxy, Size i, Size j);

        Real operator()(Real t) const;
      private:
        const Size i_, j_;
        const LmVolatilityModel*  const volaModel_;
        const LmCorrelationModel* const corrModel_;
    };

    LfmCovarianceProxy::Var_Helper::Var_Helper(const LfmCovarianceProxy* proxy,
                                               Size i, Size j)
    : i_(i),
      j_(j),
      volaModel_(proxy->volaModel_.get()),
      corrModel_(proxy->corrModel_.get()) {
    }

    Real LfmCovarianceProxy::Var_Helper::operator()(Real t) const {
        Volatility v1, v2;

        if (i_ == j_) {
            v1 = v2 = volaModel_->volatility(i_, t);
        } else {
            v1 = volaModel_->volatility(i_, t);
            v2 = volaModel_->volatility(j_, t);
        }

        return  v1 * corrModel_->correlation(i_, j_, t) * v2;
    }

    Real LfmCovarianceProxy::integratedCovariance(
                               Size i, Size j, Time t, const Array& x) const {

        if (corrModel_->isTimeIndependent()) {
            try {
                // if all objects support these methods
                // thats by far the fastest way to get the
                // integrated covariance
                return corrModel_->correlation(i, j, 0.0, x)
                        * volaModel_->integratedVariance(j, i, t, x);
            }
            catch (Error&) {
                // okay proceed with the
                // slow numerical integration routine
            }
        }

        QL_REQUIRE(x.size() == 0, "can not handle given x here");

        Real tmp=0.0;
        Var_Helper helper(this, i, j);

        //don't run away immediately
        for (Size k=0; k<64; ++k) {
            tmp+=KronrodIntegral(1e-10)(helper, k*t/64., (k+1)*t/64.);
        }

        return tmp;
    }

}

