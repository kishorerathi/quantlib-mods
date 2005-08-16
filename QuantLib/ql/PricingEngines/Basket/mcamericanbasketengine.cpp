/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Neil Firth

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

#include <ql/qldefines.hpp>
#if defined(QL_PATCH_MSVC70)
#pragma optimize( "", on )
#endif
#include <ql/PricingEngines/Basket/mcamericanbasketengine.hpp>
#include <ql/Processes/blackscholesprocess.hpp>
#include <ql/Processes/stochasticprocessarray.hpp>
#include <ql/grid.hpp>
#include <ql/Math/svd.hpp>
#include <ql/MonteCarlo/montecarlomodel.hpp>
#include <ql/Math/statistics.hpp>
#include <functional>

namespace QuantLib {

    namespace {

        class BasisFunction : std::unary_function<std::vector<Real>,Real> {
          public:
            virtual ~BasisFunction() {}
            virtual Real calculate(const std::vector<Real>& x) const = 0;
        };

        class Constant : public BasisFunction {
          private:
            Real constant_;
          public:
            Constant(Real constant) : constant_(constant) {}
            Real calculate(const std::vector<Real>&) const {
                return constant_;
            }
        };

        class Linear : public BasisFunction {
          private:
            Size index_;
            Real coeff_;
          public:
            Linear(Size index) : index_(index), coeff_(1.0) {}
            Linear(Size index, Real coeff) : index_(index), coeff_(coeff) {}
            Real calculate(const std::vector<Real>& x) const {
                return coeff_*x[index_];
            }
        };

        class Square : public BasisFunction {
          private:
            Size index_;
            Real coeff_;
          public:
            Square(Size index) : index_(index), coeff_(1.0) {}
            Square(Size index, Real coeff) : index_(index), coeff_(coeff) {}
            Real calculate(const std::vector<Real>& x) const {
                return coeff_*x[index_]*x[index_];
            }
        };

        class Cube : public BasisFunction {
          private:
            Size index_;
            Real coeff_;
          public:
            Cube(Size index) : index_(index), coeff_(1.0) {}
            Cube(Size index, Real coeff) : index_(index), coeff_(coeff) {}
            Real calculate(const std::vector<Real>& x) const {
                return coeff_*x[index_]*x[index_]*x[index_];
            }
        };

        class BasisPower : public BasisFunction {
          private:
            Size index_;
            Real power_;
            Real coeff_;
          public:
            BasisPower(Size index, Real power) :
                        index_(index), power_(power), coeff_(1.0) {}
            BasisPower(Size index, Real power, Real coeff) :
                        index_(index), power_(power), coeff_(coeff) {}
            Real calculate(const std::vector<Real>& x) const {
                return coeff_*std::pow(x[index_], power_);
            }
        };

        class LinearCombination : public BasisFunction {
          private:
            Size index1_, index2_;
          public:
            LinearCombination(Size index1, Size index2)
            : index1_(index1), index2_(index2) {}
            Real calculate(const std::vector<Real>& x) const {
                return x[index1_]*x[index2_];
            }
        };

        class LinearCombo : public BasisFunction {
          private:
            boost::shared_ptr<BasisFunction> bf1_, bf2_;
          public:
            LinearCombo(const boost::shared_ptr<BasisFunction>& bf1,
                        const boost::shared_ptr<BasisFunction>& bf2)
            : bf1_(bf1), bf2_(bf2) {}
            Real calculate(const std::vector<Real>& x) const {
                return bf1_->calculate(x)*bf2_->calculate(x);
            }
        };

        class Polynomial : public BasisFunction {
          private:
            Real factor_;
            boost::shared_ptr<BasisFunction> bf1_, bf2_;
          public:
            Polynomial(Real factor,
                       const boost::shared_ptr<BasisFunction>& bf1,
                       const boost::shared_ptr<BasisFunction>& bf2)
            : factor_(factor), bf1_(bf1), bf2_(bf2) {}
            Real calculate(const std::vector<Real>& x) const {
                return factor_*(bf1_->calculate(x) + bf2_->calculate(x));
            }
        };

        class MyPolynomial : public BasisFunction {
          private:
            Real factor_;
            std::vector<boost::shared_ptr<BasisFunction> > basisFunctions_;
          public:
            MyPolynomial(Real factor,
                         const std::vector<boost::shared_ptr<BasisFunction> >&
                                                           basisFunctions)
            : factor_(factor), basisFunctions_(basisFunctions) {}
            Real calculate(const std::vector<Real>& x) const;
        };

        Real MyPolynomial::calculate(const std::vector<Real>& x) const {
            Real result = 0.0;
            for (Size j = 0; j<basisFunctions_.size(); j++) {
                result += (basisFunctions_[j])->calculate(x);
            }
            return factor_*result;
        }

        Real basketPayoff(BasketOption::BasketType basketType,
                          const std::vector<Real>& assetPrices) {

            Real basketPrice = assetPrices[0];
            Size numAssets = assetPrices.size();
            Size j;

            switch (basketType) {
              case BasketOption::Max:
                for (j = 1; j < numAssets; j++) {
                    if (assetPrices[j] > basketPrice) {
                        basketPrice = assetPrices[j];
                    }
                }
                break;

              case BasketOption::Min:
                for (j = 1; j < numAssets; j++) {
                    if (assetPrices[j] < basketPrice) {
                        basketPrice = assetPrices[j];
                    }
                }
                break;
            }

            return basketPrice;
        }
    }

    // calculate
    void MCAmericanBasketEngine::calculate() const {

        boost::shared_ptr<PlainVanillaPayoff> payoff_handle =
            boost::dynamic_pointer_cast<PlainVanillaPayoff>(arguments_.payoff);
        QL_REQUIRE(payoff_handle, "non-plain payoff given");

        Real strike = payoff_handle->strike();
        PlainVanillaPayoff payoff(payoff_handle->optionType(), strike);

        Size numAssets = arguments_.stochasticProcess->size();

        bool brownianBridge = false;

        boost::shared_ptr<StochasticProcessArray> processes =
            boost::dynamic_pointer_cast<StochasticProcessArray>(
                                                arguments_.stochasticProcess);
        boost::shared_ptr<BlackScholesProcess> process =
            boost::dynamic_pointer_cast<BlackScholesProcess>(
                                                       processes->process(0));
        DayCounter rfdc  = process->riskFreeRate()->dayCounter();
        Rate r = process->riskFreeRate()->
            zeroRate(arguments_.exercise->lastDate(), rfdc,
                     Continuous, NoFrequency);

        // counters
        Size i; // for paths
        Size j; // for assets
        Size k; // for basis functions
        Size timeStep; // for time loops

        // Number of paths
        Size N = requiredSamples_;

        // set up the basis functions
        std::vector<boost::shared_ptr<BasisFunction> > basisFunctions;

        bool monomial = true;
        bool legendre = false;
        bool laguerre = false;

        if (numAssets == 1) {
            if (monomial) {
                // monomials
                basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                    (new Constant(1)));

                basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                    (new Linear(0)));

                basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                    (new Square(0)));
/*
                basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                    (new Cube(0)));

                basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                    (new BasisPower(0, 4)));

                basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                    (new BasisPower(0, 5)));
 */
            } else if (legendre) {
                // legendre polynomials
                boost::shared_ptr<BasisFunction> legendre_0(new Constant(1));
                boost::shared_ptr<BasisFunction> legendre_1(new Linear(0));
                boost::shared_ptr<BasisFunction> legendre_2(new Polynomial
                    (0.5,
                     boost::shared_ptr<BasisFunction>(new Constant(-1)),
                     boost::shared_ptr<BasisFunction>(new Square(0, 3))));
                boost::shared_ptr<BasisFunction> legendre_3(new Polynomial
                    (0.5,
                     boost::shared_ptr<BasisFunction>(new Linear(0, -3)),
                     boost::shared_ptr<BasisFunction>(new Cube(0, 5))));

                std::vector<boost::shared_ptr<BasisFunction> > basis4(3);
                basis4[0] = boost::shared_ptr<BasisFunction>(new Constant(3));
                basis4[1] =
                    boost::shared_ptr<BasisFunction>(new Square(0, -30));
                basis4[2] =
                    boost::shared_ptr<BasisFunction>(new BasisPower(0, 4, 35));
                boost::shared_ptr<BasisFunction> legendre_4(
                        new MyPolynomial(0.125, basis4));

                std::vector<boost::shared_ptr<BasisFunction> > basis5(3);
                basis5[0] = boost::shared_ptr<BasisFunction>(new Linear(0,15));
                basis5[1] = boost::shared_ptr<BasisFunction>(new Cube(0, -70));
                basis5[2] =
                    boost::shared_ptr<BasisFunction>(new BasisPower(0, 5, 63));
                boost::shared_ptr<BasisFunction> legendre_5(
                        new MyPolynomial(0.125, basis5));

                basisFunctions.push_back(legendre_0);
                basisFunctions.push_back(legendre_1);
                basisFunctions.push_back(legendre_2);
                basisFunctions.push_back(legendre_3);
                basisFunctions.push_back(legendre_4);
                basisFunctions.push_back(legendre_5);

            } else if (laguerre) {
                // laguerre polynomials
                boost::shared_ptr<BasisFunction> laguerre_0(new Constant(1));
                boost::shared_ptr<BasisFunction> laguerre_1(new Polynomial
                    (1,
                     boost::shared_ptr<BasisFunction> (new Constant(1)),
                     boost::shared_ptr<BasisFunction> (new Linear(0, -1))));

                std::vector<boost::shared_ptr<BasisFunction> > basis2(3);
                basis2[0] = boost::shared_ptr<BasisFunction>(new Constant(2));
                basis2[1] =
                    boost::shared_ptr<BasisFunction>(new Linear(0, -4));
                basis2[2] =
                    boost::shared_ptr<BasisFunction>(new Square(0, 1));
                boost::shared_ptr<BasisFunction> laguerre_2(
                    new MyPolynomial(0.5, basis2));

                std::vector<boost::shared_ptr<BasisFunction> > basis3(4);
                basis3[0] = boost::shared_ptr<BasisFunction>(new Constant(6));
                basis3[1] =
                    boost::shared_ptr<BasisFunction>(new Linear(0, -18));
                basis3[2] = boost::shared_ptr<BasisFunction>(new Square(0, 9));
                basis3[3] = boost::shared_ptr<BasisFunction>(new Cube(0, -1));
                boost::shared_ptr<BasisFunction> laguerre_3(
                    new MyPolynomial(1.0/6.0, basis3));

                basisFunctions.push_back(laguerre_0);
                basisFunctions.push_back(laguerre_1);
                basisFunctions.push_back(laguerre_2);
                basisFunctions.push_back(laguerre_3);
            }

        } else if (numAssets == 3) {

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new Constant(1)));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new Linear(1)));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new Linear(2)));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new Square(1)));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new Square(2)));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new Cube(1)));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new Cube(2)));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                           boost::shared_ptr<BasisFunction>(new Square(1)),
                           boost::shared_ptr<BasisFunction>(new Square(1)))));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                           boost::shared_ptr<BasisFunction>(new Square(2)),
                           boost::shared_ptr<BasisFunction>(new Square(2)))));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                             boost::shared_ptr<BasisFunction>(new Square(1)),
                             boost::shared_ptr<BasisFunction>(new Cube(1)))));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                             boost::shared_ptr<BasisFunction>(new Square(2)),
                             boost::shared_ptr<BasisFunction>(new Cube(2)))));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombination(1,2)));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                           boost::shared_ptr<BasisFunction>(new Linear(1)),
                           boost::shared_ptr<BasisFunction>(new Square(2)))));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                           boost::shared_ptr<BasisFunction>(new Linear(2)),
                           boost::shared_ptr<BasisFunction>(new Square(1)))));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                           boost::shared_ptr<BasisFunction>(new Square(1)),
                           boost::shared_ptr<BasisFunction>(new Square(2)))));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                             boost::shared_ptr<BasisFunction>(new Linear(1)),
                             boost::shared_ptr<BasisFunction>(new Cube(2)))));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                             boost::shared_ptr<BasisFunction>(new Linear(2)),
                             boost::shared_ptr<BasisFunction>(new Cube(1)))));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                             boost::shared_ptr<BasisFunction>(new Square(1)),
                             boost::shared_ptr<BasisFunction>(new Cube(2)))));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                             boost::shared_ptr<BasisFunction>(new Square(2)),
                             boost::shared_ptr<BasisFunction>(new Cube(1)))));

            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                        boost::shared_ptr<BasisFunction>(new Linear(1)),
                        boost::shared_ptr<BasisFunction>
                            (new LinearCombo(boost::shared_ptr<BasisFunction>(
                                                            new Cube(2)),
                                             boost::shared_ptr<BasisFunction>(
                                                            new Cube(2)))))));
            basisFunctions.push_back(boost::shared_ptr<BasisFunction>
                (new LinearCombo(
                        boost::shared_ptr<BasisFunction>(new Linear(2)),
                        boost::shared_ptr<BasisFunction>
                            (new LinearCombo(boost::shared_ptr<BasisFunction>(
                                                            new Cube(2)),
                                             boost::shared_ptr<BasisFunction>(
                                                            new Cube(2)))))));
        }

        Size numBasisFunctions = basisFunctions.size();

        // create the time grid
        Time T = rfdc.yearFraction(process->riskFreeRate()->referenceDate(),
                                   arguments_.exercise->lastDate());
        TimeGrid grid(T, timeSteps_);

        // create a Gaussian Random Sequence Generator
        PseudoRandom::rsg_type gen =
            PseudoRandom::make_sequence_generator(
                                numAssets*(grid.size()-1),seed_);

        LowDiscrepancy::rsg_type quasiGen =
            LowDiscrepancy::make_sequence_generator(
                numAssets*(grid.size()-1),seed_);

        // set up arguments
        Array initialPrices = arguments_.stochasticProcess->initialValues();

        // create the MultiPathGenerator
        boost::shared_ptr<MultiPathGenerator<PseudoRandom::rsg_type> >
            multipathGenerator(
                new MultiPathGenerator<PseudoRandom::rsg_type>(
                                             arguments_.stochasticProcess,
                                             grid, gen, brownianBridge));

        boost::shared_ptr<MultiPathGenerator<LowDiscrepancy::rsg_type> >
            quasiMultipathGenerator(
            new MultiPathGenerator<LowDiscrepancy::rsg_type>(
                                             arguments_.stochasticProcess,
                                             grid, quasiGen, brownianBridge));

        // generate the paths
        std::vector<MultiPath> multipaths(N);
        for (i=0; i<N/2; i++) {
            multipaths[i] = multipathGenerator->next().value;
            multipaths[N/2+i] = multipathGenerator->antithetic().value;
        }

        // initialise rollback vector with payoff
        std::vector<Real> normalizedContinuationValue(N);
        for (i=0; i<N; i++) {
            std::vector<Real> finalPrices(numAssets);
            for (j=0; j<numAssets; j++) {
                finalPrices[j] = multipaths[i][j].back();
            }
            normalizedContinuationValue[i] =
                payoff(basketPayoff(arguments_.basketType,finalPrices))/strike;
        }

        Array temp_coeffs(numBasisFunctions,1.0);
        std::vector<Array> basisCoeffs(timeSteps_-1, temp_coeffs);

        std::vector<Real> assetPrices(numAssets);
        std::vector<Real> normalizedAssetPrices(numAssets);

        // LOOP
        BigInteger timeLoop;
        for (timeLoop = BigInteger(timeSteps_)-2; timeLoop>=0; timeLoop--) {
            timeStep = timeLoop;

            // rollback all paths
            // use rollback vector
            for (i=0; i<N; i++) {
                if (normalizedContinuationValue[i] > 0.0) {
                    // discount
                    // +1 because the grid includes the start time
                    Time from = grid[timeStep+1];
                    Time to = grid[timeStep+2];

                    normalizedContinuationValue[i] =
                        normalizedContinuationValue[i] *
                        std::exp(-r * (to-from));
                }
            }

            // select in the money paths
            std::vector<Size> itmPaths;
            std::vector<Real> y(N);
            for (i=0; i<N; i++) {
                for (j=0; j<numAssets; j++) {
                    assetPrices[j] = multipaths[i][j][timeStep+1];
                }
                y[i] = payoff(basketPayoff(arguments_.basketType,
                                           assetPrices));

                if (y[i] > 0) {
                    itmPaths.push_back(i);
                }
            }

            // loop over all itm paths
            Size num_itmPaths = itmPaths.size();
            if (num_itmPaths > 0) {

                // for itm paths
                std::vector<Real> y_exercise(num_itmPaths);
                Array y_temp(num_itmPaths);
                for (i=0; i<num_itmPaths; i++) {
                    // get the immediate exercise value
                    // normalized..
                    y_exercise[i] = y[itmPaths[i]] / strike;

                    // get discounted continuation value
                    y_temp[i] = normalizedContinuationValue[itmPaths[i]];
                }

                // calculate the basis functions and
                // create the design matrix A
                Matrix A(num_itmPaths,numBasisFunctions);
                for (i=0; i<num_itmPaths; i++) {

                    for (j=0; j<numAssets; j++) {
                        normalizedAssetPrices[j] =
                            multipaths[itmPaths[i]][j][timeStep+1]/strike;
                    }
                    // sort - ascending order
                    std::sort(normalizedAssetPrices.begin(),
                                normalizedAssetPrices.end());

                    for (k=0; k<numBasisFunctions; k++) {
                        A[i][k] =
                          basisFunctions[k]->calculate(normalizedAssetPrices);
                    }
                }

                // do least squares regression
                SVD svd(A);
                Matrix U = svd.U();
                Array s = svd.singularValues();
                Matrix V = svd.V();
                // probably faster to do this directly
                // in MATLAB this is V*(S\(U'*y_temp))
                Matrix Utrans = transpose(U);
                Array temp_1 = transpose(U) * y_temp;
                Array temp_2(V.columns());
                // Some singular values may be zero so we cannot do
                // temp_1/=s;
                for (i=0; i<temp_2.size(); i++) {
                    if (s[i] != 0.0) {
                        temp_1[i] = temp_1[i]/s[i];
                    } else {
                        temp_1[i] = 0.0;
                    }
                }
                Array b = V*temp_1;

                // uncomment if you want to cut and paste
                // basis function coeffs and singular values
                // to Excel
                //std::cout << "\nb=" << b << "s=" << s ;

                // store the basisCoeffs for later
                basisCoeffs[timeStep] = b;

                // calculate continuation value
                Array y_continue = A*b;

                // modify stopping rule
                for (i=0; i<num_itmPaths; i++) {
                    if (y_exercise[i] > y_continue[i]) {
                        normalizedContinuationValue[itmPaths[i]] =
                                y_exercise[i];
                    }
                }

            }
            // END LOOP
        }

        Statistics stats = GeneralStatistics();
        for (i=0; i<N; i++) {
            stats.add(normalizedContinuationValue[i]*strike);
        }

        /*
        std::cout << std::endl;
        std::cout << " ==== Basis Coeffs ===  " << std::endl;
        for (timeStep = 0; timeStep < basisCoeffs.size(); timeStep++) {
            Array b = basisCoeffs[timeStep];
            std::cout << " [";
            for (k = 0; k < numBasisFunctions; k++) {
                std::cout << " " << b[k];
            }
            std::cout << " ]" << std::endl;
        }
        */

        results_.value  = stats.mean();
        results_.errorEstimate = stats.errorEstimate();
    }

}

