/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Mark Joshi

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

#include <ql/models/marketmodels/models/capletcoterminalalphacalibration.hpp>
#include <ql/models/marketmodels/models/alphafinder.hpp>
#include <ql/models/marketmodels/models/piecewiseconstantvariance.hpp>
#include <ql/models/marketmodels/models/pseudorootfacade.hpp>
#include <ql/models/marketmodels/models/cotswaptofwdadapter.hpp>
#include <ql/models/marketmodels/swapforwardmappings.hpp>
#include <ql/models/marketmodels/marketmodel.hpp>
#include <ql/math/matrixutilities/pseudosqrt.hpp>
#include <ql/math/matrixutilities/basisincompleteordered.hpp>
#include <ql/math/optimization/spherecylinder.hpp>
#include <ql/math/quadratic.hpp>

namespace QuantLib {

    namespace {

        bool singleRateClosestPointFinder(
                 Size capletNumber,
                 const std::vector<Volatility>& homogeneousSolution,
                 const std::vector<Volatility>& previousRateSolution,
                 Real capletVariance,
                 const std::vector<Real>& correlations,
                 Real w0,
                 Real w1,
                 Size maxIterations,
                 Real tolerance,
                 std::vector<Volatility>& solution) 
		{
			if (capletNumber ==0) // there only is one point so to go through everything would be silly
			{
			    Real previousSwapVariance = previousRateSolution[0] *previousRateSolution[0];
				Real thisSwapVariance = homogeneousSolution[0] *homogeneousSolution[0]
														+ homogeneousSolution[1] *homogeneousSolution[1];
      			Real crossTerm =  2*w0*w1*correlations[0]*previousRateSolution[0];
				Real constantTerm = w0*w0* previousSwapVariance - capletVariance;
				Real theta = w1*w1;

				quadratic q(theta,crossTerm, constantTerm);
				Real volminus, volplus;
				bool success = q.roots(volminus,volplus);
				Real residual = thisSwapVariance - volminus*volminus;
				success = success && (residual >=0);

				solution[0] = volminus;
				solution[1] = sqrt(residual);

				return success;
			}

            // first get in correct format
            Real previousSwapVariance=0.0;
            Real thisSwapVariance =0.0;
            {
                Size i=0;
                for (; i<capletNumber+1; ++i) {
                    previousSwapVariance += previousRateSolution[i] *
                                                previousRateSolution[i];
                    thisSwapVariance += homogeneousSolution[i] *
                                                homogeneousSolution[i];
                }
                thisSwapVariance+= homogeneousSolution[i]*homogeneousSolution[i];
            }

            Real theta = w1*w1;
            std::vector<Real> b(capletNumber+1);
            Array cylinderCentre(capletNumber+1);
            Array targetArray(capletNumber+2);
			Array targetArrayRestricted(capletNumber+1);


            Real bsq = 0.0;
            for (Size i=0; i<capletNumber+1; ++i) {
                b[i] = 2*w0*w1*correlations[i]*previousRateSolution[i]/theta;
                cylinderCentre[i] = -0.5*b[i];
                targetArray[i] = homogeneousSolution[i];
				targetArrayRestricted[i] = targetArray[i];
                bsq+=b[i]*b[i];
            }
            targetArray[capletNumber+1] = homogeneousSolution[capletNumber+1];

            Real A = previousSwapVariance*w0*w0/theta;
            Real constQuadraticTerm = A - 0.25*bsq;
            Real S2 = capletVariance/theta - constQuadraticTerm;
            Real S = sqrt(S2);

            Real R = sqrt(thisSwapVariance);

            basisincompleteordered basis(capletNumber+1);
            basis.addVector(cylinderCentre);
            basis.addVector(targetArrayRestricted);
            for (Size i=0; i<capletNumber+1; ++i) {
                Array ei(capletNumber+1,0.0);
                ei[i]=1.0;
                basis.addVector(ei);
            }

            Matrix orthTransformationRestricted(basis.getBasisAsRowsInMatrix());
            Matrix orthTransformation(capletNumber+2, capletNumber+2, 0.0);

            orthTransformation[capletNumber+1][capletNumber+1]=1.0;
            for (Size k=0; k<capletNumber+1; ++k)
                for (Size l=0; l<capletNumber+1; ++l)
                    orthTransformation[k][l]=orthTransformationRestricted[k][l];

            Array movedCentre = orthTransformationRestricted*cylinderCentre;
            Real alpha = movedCentre[0];
            Array movedTarget = orthTransformation*targetArray;

            Real Z1=0.0, Z2=0.0, Z3=0.0;

            spherecylinderoptimizer optimizer(R, S, alpha, movedTarget[0], movedTarget[1],movedTarget[movedTarget.size()-1]);

            if (!optimizer.isIntersectionNonEmpty())
                return false;

            optimizer.findClosest(maxIterations,
                                  tolerance,
                                  Z1,
                                  Z2,
                                  Z3);

            Array rotatedSolution(capletNumber+2,0.0);
            rotatedSolution[0] = Z1;
            rotatedSolution[1] = Z2;
            rotatedSolution[capletNumber+1] = Z3;

            Array arraySolution(transpose(orthTransformation) *
                                rotatedSolution);
			{
				Size i=0;
				for (; i < arraySolution.size(); ++i)
					solution[i]=arraySolution[i];
				for (; i < solution.size(); ++i)
					solution[i]=0.0;
			}

            return true;

        }

    }

    bool calibrationOfMaxHomogeneity(
            const EvolutionDescription& evolution,
            const PiecewiseConstantCorrelation& corr,
            const std::vector<boost::shared_ptr<
            PiecewiseConstantVariance> >&
            displacedSwapVariances,
            const std::vector<Volatility>& capletVols,
            const CurveState& cs,
            const Spread displacement,
            const Size numberOfFactors,
            Real toleranceForMinimization,
            Size iterationsForMinimzation,
			Real& deformationSize,
            std::vector<Matrix>& swapCovariancePseudoRoots) {

        QL_REQUIRE(evolution.evolutionTimes()==corr.times(),
                   "evolutionTimes not equal to correlation times");

        Size numberOfRates = evolution.numberOfRates();
        QL_REQUIRE(numberOfFactors<=numberOfRates,
            "number of factors (" << numberOfFactors <<
            ") cannot be greater than numberOfRates (" <<
            numberOfRates << ")");
        QL_REQUIRE(numberOfFactors>0,
            "number of factors (" << numberOfFactors <<
            ") must be greater than zero");

        QL_REQUIRE(numberOfRates==displacedSwapVariances.size(),
            "mismatch between number of rates (" << numberOfRates <<
            ") and displacedSwapVariances");

        QL_REQUIRE(numberOfRates==capletVols.size(),
            "mismatch between number of rates (" << numberOfRates <<
            ") and capletVols (" << capletVols.size() <<
            ")");

        const std::vector<Time>& rateTimes = evolution.rateTimes();
        QL_REQUIRE(rateTimes==cs.rateTimes(),
            "mismatch between EvolutionDescriptionand CurveState rate times ");
        QL_REQUIRE(numberOfRates==cs.numberOfRates(),
            "mismatch between number of rates (" << numberOfRates <<
            ") and CurveState");


        const std::vector<Time>& evolutionTimes = evolution.evolutionTimes();
        std::vector<Time> temp(rateTimes.begin(), rateTimes.end()-1);
        QL_REQUIRE(temp==evolutionTimes,
                   "mismatch between evolutionTimes and rateTimes");

		deformationSize=0.0;


        Size numberOfSteps = evolution.numberOfSteps();

        // factor reduction
        std::vector<Matrix> corrPseudo(corr.times().size());
        for (Size i=0; i<corrPseudo.size(); ++i)
            corrPseudo[i] = rankReducedSqrt(corr.correlation(i),
            numberOfFactors, 1.0,
            SalvagingAlgorithm::None);




        // vectors for new vol
        std::vector<std::vector<Volatility> > newVols;
        std::vector<Volatility> theseNewVols(numberOfRates);

        // get Zinverse, we can get wj later
        Matrix zedMatrix =
            SwapForwardMappings::coterminalSwapZedMatrix(cs, displacement);
        Matrix invertedZedMatrix = inverse(zedMatrix);

        std::vector<Volatility> rateonevols(numberOfRates);
        rateonevols[0] = sqrt(displacedSwapVariances[0]->variances()[0]);
        std::vector<Volatility> ratetwovols(numberOfRates);
        std::vector<Real> correlations(numberOfRates);
        newVols.push_back(rateonevols);


        for (Size i =0; i < numberOfRates-1; ++i) {
              const std::vector<Real>& var =
                                    displacedSwapVariances[i+1]->variances();

            for (Size j =0; j < i+2; ++j)
                ratetwovols[j] = sqrt(var[j]);

            for (Size k=0; k < i+1; k++) {
                Real correlation=0.0;
                for (Size l=0; l < numberOfFactors; ++l) {
                    Real term1 = corrPseudo[k][i][l];
                    Real term2 = corrPseudo[k][i+1][l];
                    correlation += term1*term2;
                }
                correlations[k] = correlation;
            }

            Real w0 = invertedZedMatrix[i][i];
            Real w1 = invertedZedMatrix[i][i+1];
            // w0 adjustment
            for (Size k = i+2; k <invertedZedMatrix.columns(); ++k)
                w0+= invertedZedMatrix[i][k];

            Real targetVariance= capletVols[i]*capletVols[i]*rateTimes[i];

            bool success = singleRateClosestPointFinder(
                i, ratetwovols, rateonevols, targetVariance, correlations,
                w0, w1, iterationsForMinimzation, toleranceForMinimization,
                theseNewVols);

            if (!success)
                return success; // i.e. false

			for (Size j=0; j < i+2; ++j)
				deformationSize += (theseNewVols[i]-ratetwovols[i])*(theseNewVols[i]-ratetwovols[i]);


            newVols.push_back(theseNewVols);
            rateonevols = theseNewVols;
        }

        swapCovariancePseudoRoots.resize(numberOfSteps);
        for (Size k=0; k<numberOfSteps; ++k) {
            swapCovariancePseudoRoots[k] = corrPseudo[k];
            for (Size j=0; j<numberOfRates; ++j) {
                Real coeff =newVols[j][k];
                for (Size i=0; i<numberOfFactors; ++i)
                    swapCovariancePseudoRoots[k][j][i]*=coeff;
            }
            QL_ENSURE(swapCovariancePseudoRoots[k].rows()==numberOfRates,
                "step " << k
                << " abcd vol wrong number of rows: "
                << swapCovariancePseudoRoots[k].rows()
                << " instead of " << numberOfRates);
            QL_ENSURE(swapCovariancePseudoRoots[k].columns()==numberOfFactors,
                "step " << k
                << " abcd vol wrong number of columns: "
                << swapCovariancePseudoRoots[k].columns()
                << " instead of " << numberOfFactors);
        }

        return true;
    }


    bool calibrationOfMaxHomogeneityIterative(
        const EvolutionDescription& evolution,
        const PiecewiseConstantCorrelation& corr,
        const std::vector<boost::shared_ptr<
        PiecewiseConstantVariance> >&
        displacedSwapVariances,
        const std::vector<Volatility>& capletVols,
        const CurveState& cs,
        const Spread displacement,
        const Size numberOfFactors,
        Integer iterationsForHomogeneous,
        Real toleranceHomogeneousSolving,
        Size maxIterationsForIterative,
        Real toleranceForIterativeSolving,
		Real& deformationSize,
        std::vector<Matrix>& swapCovariancePseudoRoots)
    {
        std::vector<Volatility> modifiedCapletVols=capletVols;
        Real error;
        Size iterations=0;
        Size numberOfRates = evolution.numberOfRates();
		deformationSize=0.0;

        do
        {
            bool success = calibrationOfMaxHomogeneity(
                evolution,
                corr,
                displacedSwapVariances,
                modifiedCapletVols,
                cs,
                displacement,
                numberOfFactors,
                toleranceHomogeneousSolving,
                iterationsForHomogeneous,
				deformationSize,
                swapCovariancePseudoRoots);

            if (!success)
                return false;

            std::vector<Spread> displacements(evolution.numberOfRates(),
                                              displacement);
            boost::shared_ptr<MarketModel> smm(new
                PseudoRootFacade(swapCovariancePseudoRoots,
                                 evolution.rateTimes(),
                                 cs.coterminalSwapRates(),
                                 displacements));

            CotSwapToFwdAdapter flmm(smm);
            Matrix capletTotCovariance =
                            flmm.totalCovariance(evolution.numberOfRates()-1);

            std::vector<Volatility> capletVolsOutput(numberOfRates);
            for (Size i=0; i<numberOfRates; ++i) {
                capletVolsOutput[i] = std::sqrt(capletTotCovariance[i][i] /
                                                evolution.rateTimes()[i]);
            }

            error=0.0;
            // check caplet fit
            for (Size i=0; i<numberOfRates-1; ++i) {
                Real thisError = capletVols[i]-capletVolsOutput[i];
                error += thisError*thisError;

                modifiedCapletVols[i] = modifiedCapletVols[i] *
                                        capletVols[i] / capletVolsOutput[i];
            }
            ++iterations;
        }

        while (iterations<maxIterationsForIterative &&
               sqrt(error)>toleranceForIterativeSolving);

        return true;
    }

}
