/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2007 Cristina Duminuco
 Copyright (C) 2007 Giorgio Facchinetti

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

/*! \file abcdinterpolation.hpp
    \brief Abcd interpolation interpolation between discrete points
*/

#ifndef quantlib_abcd_interpolation_hpp
#define quantlib_abcd_interpolation_hpp

#include <ql/math/interpolation.hpp>
#include <ql/termstructures/volatility/interestrate/abcd.hpp>
#include <ql/termstructures/volatility/interestrate/abcdcalibration.hpp>



namespace QuantLib {

    class EndCriteria;
    class OptimizationMethod;

    namespace detail {

        template <class I1, class I2> class AbcdInterpolationImpl;

        class AbcdCoefficientHolder {
          public:
            AbcdCoefficientHolder(Real a,
                                  Real b,
                                  Real c,
                                  Real d,
                                  bool aIsFixed,
                                  bool bIsFixed,
                                  bool cIsFixed,
                                  bool dIsFixed)
            : a_(a), b_(b), c_(c), d_(d),
              aIsFixed_(false), bIsFixed_(false),
              cIsFixed_(false), dIsFixed_(false),
              k_(std::vector<Real>()),
              error_(Null<Real>()),
              maxError_(Null<Real>()),
              abcdEndCriteria_(EndCriteria::None) { 
            
                // Checks?
            }
            virtual ~AbcdCoefficientHolder() {}

            /*! Abcd parameters */
            Real a_, b_, c_, d_;
            /*! Abcd interpolation settings */
            bool aIsFixed_, bIsFixed_, cIsFixed_, dIsFixed_;
            std::vector<Real> k_;
            /*! Abcd interpolation results */
            Real error_, maxError_;
            EndCriteria::Type abcdEndCriteria_;
        };
    }

    //! %Abcd interpolation between discrete volatility points.
    class AbcdInterpolation : public Interpolation {
      public:
        /*! Constructor */
        template <class I1, class I2>
        AbcdInterpolation(const I1& xBegin,  // x = times
                          const I1& xEnd,
                          const I2& yBegin,  // y = volatilities
                          Real a = -0.06,
                          Real b =  0.17,
                          Real c =  0.54,
                          Real d =  0.17,
                          bool aIsFixed = false,
                          bool bIsFixed = false,
                          bool cIsFixed = false,
                          bool dIsFixed = false,
                          bool vegaWeighted = false,
                          const boost::shared_ptr<EndCriteria>& endCriteria
                              = boost::shared_ptr<EndCriteria>(),
                          const boost::shared_ptr<OptimizationMethod>& optMethod
                              = boost::shared_ptr<OptimizationMethod>()) {

            impl_ = boost::shared_ptr<Interpolation::Impl>(new
                detail::AbcdInterpolationImpl<I1,I2>(xBegin, xEnd, yBegin,
                                                     a, b, c, d,
                                                     aIsFixed, bIsFixed,
                                                     cIsFixed, dIsFixed,
                                                     vegaWeighted,
                                                     endCriteria,
                                                     optMethod));
            impl_->update();
            coeffs_ =
                boost::dynamic_pointer_cast<detail::AbcdCoefficientHolder>(
                                                                       impl_);
        }
        //! \name Inspectors
        //@{
        Real a() const { return coeffs_->a_; }
        Real b() const { return coeffs_->b_; }
        Real c() const { return coeffs_->c_; }
        Real d() const { return coeffs_->d_; }
        std::vector<Real> k() const { return coeffs_->k_; }
        Real interpolationError() const { return coeffs_->error_; }
        Real interpolationMaxError() const { return coeffs_->maxError_; }
        EndCriteria::Type endCriteria(){ return coeffs_->abcdEndCriteria_; }
    private:
        boost::shared_ptr<detail::AbcdCoefficientHolder> coeffs_;
    };

    namespace detail {

        template <class I1, class I2>
        class AbcdInterpolationImpl : public Interpolation::templateImpl<I1,I2>,
                                      public AbcdCoefficientHolder {        

        private:
            // optimization method used for fitting
            const boost::shared_ptr<EndCriteria> endCriteria_;
            const boost::shared_ptr<OptimizationMethod> optMethod_;
            bool vegaWeighted_;
            boost::shared_ptr<AbcdCalibration> abcdCalibrator_;

        public:

            AbcdInterpolationImpl(
                const I1& xBegin, const I1& xEnd,
                const I2& yBegin,
                Real a, Real b, Real c, Real d,
                bool aIsFixed,
                bool bIsFixed,
                bool cIsFixed,
                bool dIsFixed,
                bool vegaWeighted,
                const boost::shared_ptr<EndCriteria>& endCriteria,
                const boost::shared_ptr<OptimizationMethod>& optMethod)
            : Interpolation::templateImpl<I1,I2>(xBegin, xEnd, yBegin),
              AbcdCoefficientHolder(a, b, c, d, aIsFixed, bIsFixed, cIsFixed, dIsFixed),
              endCriteria_(endCriteria), optMethod_(optMethod),
              vegaWeighted_(vegaWeighted) { }
            
            void update() {
                std::vector<Real>::const_iterator x = this->xBegin_;
                std::vector<Real>::const_iterator y = this->yBegin_;
                std::vector<Real> times, blackVols;
                for ( ; x!=this->xEnd_; ++x, ++y) {
                    times.push_back(*x);
                    blackVols.push_back(*y);
                }
                abcdCalibrator_ = boost::shared_ptr<AbcdCalibration>(new
                                AbcdCalibration(times, blackVols,
                                                a_, b_, c_, d_,
                                                aIsFixed_, bIsFixed_,
                                                cIsFixed_, dIsFixed_,
                                                vegaWeighted_,
                                                endCriteria_,
                                                optMethod_));
                abcdCalibrator_->compute();
                a_ = abcdCalibrator_->a();
                b_ = abcdCalibrator_->b();
                c_ = abcdCalibrator_->c();
                d_ = abcdCalibrator_->d();
                k_ = abcdCalibrator_->k(times,blackVols);
                error_ = abcdCalibrator_->error();
                maxError_ = abcdCalibrator_->maxError();
                abcdEndCriteria_ = abcdCalibrator_->endCriteria();
            }
            Real value(Real x) const {
                QL_REQUIRE(x>=0.0, "time must be non negative: " <<
                                   x << " not allowed");
                return abcdCalibrator_->value(x);
            }
            Real primitive(Real) const {
                QL_FAIL("Abcd primitive not implemented");
            }
            Real derivative(Real) const {
                QL_FAIL("Abcd derivative not implemented");
            }
            Real secondDerivative(Real) const {
                QL_FAIL("Abcd secondDerivative not implemented");
            }
        };
    }


}

#endif
