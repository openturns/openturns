//                                               -*- C++ -*-
/**
 *  @brief Tensor approximation algorithm
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/TensorApproximationAlgorithm.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalMathFunctionImplementation.hxx"
#include "openturns/DatabaseNumericalMathEvaluationImplementation.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/MarginalTransformationHessian.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/RankOneTensorEvaluation.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/PenalizedLeastSquaresAlgorithm.hxx"
#include "openturns/OrthogonalProductFunctionFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/LeastSquaresMetaModelSelection.hxx"
#include "openturns/PenalizedLeastSquaresAlgorithmFactory.hxx"
#include "openturns/ApproximationAlgorithm.hxx"
#include "openturns/Cobyla.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Distribution> DistributionCollection;
typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;

CLASSNAMEINIT(TensorApproximationAlgorithm);

static const Factory<TensorApproximationAlgorithm> Factory_TensorApproximationAlgorithm;


/* Default constructor */
TensorApproximationAlgorithm::TensorApproximationAlgorithm()
  : MetaModelAlgorithm()
  , p_approximationAlgorithmImplementationFactory_(PenalizedLeastSquaresAlgorithmFactory(true).clone())
  , maximumAlternatingLeastSquaresIteration_(ResourceMap::GetAsUnsignedInteger("TensorApproximationAlgorithm-DefaultMaximumAlternatingLeastSquaresIteration"))
  , maximumRadiusError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumRadiusError"))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumResidualError"))
  , rankSelection_(false)
{
  // Nothing to do
}

/* Constructor */
TensorApproximationAlgorithm::TensorApproximationAlgorithm(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const Distribution & distribution,
    const OrthogonalProductFunctionFactory & basisFactory,
    const Indices & nk,
    const UnsignedInteger maxRank)
  : MetaModelAlgorithm(distribution, NumericalMathFunction(NumericalMathFunctionImplementation(DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, false).clone())))
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , basisFactory_(basisFactory)
  , p_approximationAlgorithmImplementationFactory_(PenalizedLeastSquaresAlgorithmFactory(true).clone())
  , maximumAlternatingLeastSquaresIteration_(ResourceMap::GetAsUnsignedInteger("TensorApproximationAlgorithm-DefaultMaximumAlternatingLeastSquaresIteration"))
  , maximumRadiusError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumRadiusError"))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumResidualError"))
  , rankSelection_(false)
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";

  FunctionFamilyCollection functionFamilies(basisFactory.getFunctionFamilyCollection());

  if (nk.getSize() != functionFamilies.getSize()) throw InvalidArgumentException(HERE) << "nk size ("<<nk.getSize()<<") must match orthogonal basis factories ("<<functionFamilies.getSize()<<")";
  tensor_ = Collection<CanonicalTensor>(outputSample.getDimension(), CanonicalTensor(functionFamilies, nk, maxRank));
}

/* Virtual constructor */
TensorApproximationAlgorithm * TensorApproximationAlgorithm::clone() const
{
  return new TensorApproximationAlgorithm(*this);
}


/* String converter */
String TensorApproximationAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


/* Computes the functional chaos */
void TensorApproximationAlgorithm::run()
{
  const UnsignedInteger outputDimension(outputSample_.getDimension());
  // First, compute all the parts that are independent of the marginal output
  // Create the isoprobabilistic transformation
  // If we call Z a random vector which probability distribution is measure and X a random vector which probability distribution is distribution_, and T the isoprobabilistic transformation, we have:
  // Z = T(X)
  // X = T^{-1}(Z)
  // The transformation is built using the isoprobabilistic transformations of both X and Z
  // First, get the measure upon which the orthogonal basis is built
//   const OrthogonalBasis basis(basis_);
  const Distribution measure(basisFactory_.getMeasure());
  // Correct the measure of the projection strategy
//   projectionStrategy_.setMeasure(measure);
  // Check that it is a product measure
  LOGINFO("Build the iso-probabilistic transformation");
  if (!measure.hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error: cannot use TensorApproximationAlgorithm with an orthogonal basis not based on a product measure";
  // Has the distribution an independent copula? Simply use marginal transformations
  if (distribution_.hasIndependentCopula())
  {
    const UnsignedInteger dimension(distribution_.getDimension());
    // We use empty collections to avoid the construction of default distributions
    DistributionCollection marginalX(0);
    DistributionCollection marginalZ(0);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      marginalX.add(distribution_.getMarginal(i));
      marginalZ.add(measure.getMarginal(i));
    }
    // The distributions have an independent copula, they can be converted one into another by marginal transformation. T is such that T(X) = Z
    const MarginalTransformationEvaluation evaluationT(MarginalTransformationEvaluation(marginalX, marginalZ));
    const MarginalTransformationGradient gradientT(evaluationT);
    const MarginalTransformationHessian hessianT(evaluationT);
    transformation_ = NumericalMathFunction(evaluationT.clone(), gradientT.clone(), hessianT.clone());
    const MarginalTransformationEvaluation evaluationTinv(MarginalTransformationEvaluation(marginalZ, marginalX));
    const MarginalTransformationGradient gradientTinv(evaluationTinv);
    const MarginalTransformationHessian hessianTinv(evaluationTinv);
    inverseTransformation_ = NumericalMathFunction(evaluationTinv.clone(), gradientTinv.clone(), hessianTinv.clone());
  }
  else
  {
    // Is the input distribution mapped into a normal standard space?
    // We know that the standard space associated with Z is a normal space, as Z has an independent copula. We can check that X has the same standard space if its standard distribution has also an independent copula
    if (distribution_.getStandardDistribution().hasIndependentCopula())
    {
      // The distributions share the same standard space, it is thus possible to transform one into the other by composition between their isoprobabilistic transformations. T = T^{-1}_Z o T_X and T^{-1} = T^{-1}_X o T_Z
      const NumericalMathFunction TX(distribution_.getIsoProbabilisticTransformation());
      const NumericalMathFunction TinvX(distribution_.getInverseIsoProbabilisticTransformation());
      const NumericalMathFunction TZ(measure.getIsoProbabilisticTransformation());
      const NumericalMathFunction TinvZ(measure.getInverseIsoProbabilisticTransformation());
      transformation_ = NumericalMathFunction(TinvZ, TX);
      inverseTransformation_ = NumericalMathFunction(TinvX, TZ);
    }
    // The standard space is not normal, use Rosenblatt transformation instead of the native iso-probabilistic transformation.
    else
    {
      // The distributions share the same standard space if we use a Rosenblatt transforation, it is thus possible to transform one into the other by composition between their isoprobabilistic transformations. T = T^{-1}_Z o T_X and T^{-1} = T^{-1}_X o T_Z
      const NumericalMathFunction TX(NumericalMathFunctionImplementation(RosenblattEvaluation(distribution_.getImplementation()).clone()));
      const NumericalMathFunction TinvX(NumericalMathFunctionImplementation(InverseRosenblattEvaluation(distribution_.getImplementation()).clone()));
      const NumericalMathFunction TZ(measure.getIsoProbabilisticTransformation());
      const NumericalMathFunction TinvZ(measure.getInverseIsoProbabilisticTransformation());
      transformation_ = NumericalMathFunction(TinvZ, TX);
      inverseTransformation_ = NumericalMathFunction(TinvX, TZ);
    }
  } // Non-independent input copula
  // Build the composed model g = f o T^{-1}, which is a function of Z so it can be decomposed upon an orthonormal basis based on Z distribution
  LOGINFO("Transform the input sample in the measure space if needed");
//   composedModel_ = NumericalMathFunction(model_, inverseTransformation_);
  transformedInputSample_ = transformation_(inputSample_);

  NumericalMathFunctionCollection marginals(0);
  NumericalPoint residuals(outputDimension);
  NumericalPoint relativeErrors(outputDimension); 
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
  {
    runMarginal(outputIndex, residuals[outputIndex], relativeErrors[outputIndex]);
  }
  // Build the result
  result_ = TensorApproximationResult(distribution_, transformation_, inverseTransformation_, composedModel_, tensor_, residuals, relativeErrors);
}

/* Marginal computation */
void TensorApproximationAlgorithm::runMarginal(const UnsignedInteger marginalIndex,
                                               NumericalScalar & marginalResidual,
                                               NumericalScalar & marginalRelativeError)
{
  // proxies are reused accross marginals because the basis is the same
  const UnsignedInteger dimension = transformedInputSample_.getDimension();
  if (proxy_.getSize() == 0)
  {
    proxy_ = Collection<DesignProxy>(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      proxy_[i] = DesignProxy(transformedInputSample_.getMarginal(i), tensor_[marginalIndex].rank1tensors_[0].getBasis(i));
    }
  }

  if (rankSelection_)
  {
    greedyRankOneSelection (marginalIndex,
                            marginalResidual,
                            marginalRelativeError);

  }
  else
  {
    greedyRankOne(marginalIndex,
                  transformedInputSample_,
                  outputSample_.getMarginal(marginalIndex),
                  marginalResidual,
                  marginalRelativeError);
  }
}


class RankKResidualEvaluation
: public NumericalMathEvaluationImplementation
{
public:
  RankKResidualEvaluation(const NumericalSample & x,
                        const NumericalSample & y,
                        const CanonicalTensor & tensor)
    : NumericalMathEvaluationImplementation()
    , x_(x)
    , y_(y)
    , tensor_(tensor)
  {
  }

  virtual RankKResidualEvaluation * clone() const
  {
    return new RankKResidualEvaluation(*this);
  }

  NumericalPoint operator()(const NumericalPoint & beta) const
  {
    if (beta.getDimension() != getInputDimension()) throw InvalidDimensionException(HERE) << "Invalid beta size";
    const Indices nk(tensor_.getNk());
    const UnsignedInteger rank = tensor_.getRank();

    NumericalMathFunctionCollection prodColl(rank);
    NumericalPoint rk(rank, 1.0);
    UnsignedInteger index = 0;
    for (UnsignedInteger k = 0; k < rank; ++ k)
    {
      RankOneTensor rankOne(tensor_);
      for (UnsignedInteger i = 0; i < nk.getSize(); ++ i)
      {
        NumericalPoint tmp(nk[i]);
        std::copy(beta.begin() + index, beta.begin() + index+nk[i], tmp.begin());
        rankOne.setCoefficients(i, tmp);
        index += nk[i];
      }
      tensor_.setRankOneTensor(k, rankOne);
      Pointer<NumericalMathEvaluationImplementation> p_evaluation = new RankOneTensorEvaluation(tensor_.getRankOneTensor(k));
      prodColl[k] = NumericalMathFunction(p_evaluation);
    }
    NumericalMathFunction combination(prodColl, rk);
    return NumericalPoint(1, (y_ - combination(x_)).getImplementation()->getData().normSquare());
  }

  UnsignedInteger getInputDimension() const
  {
    UnsignedInteger sumNk = 0;
    const Indices nk(tensor_.getNk());
    for (UnsignedInteger i = 0; i < nk.getSize(); ++ i)
    {
      sumNk += nk[i];
    }
    return sumNk * tensor_.getRank();
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

protected:
  NumericalSample x_;
  NumericalSample y_;
  mutable CanonicalTensor tensor_;
};


void TensorApproximationAlgorithm::rankK (const UnsignedInteger marginalIndex,
                                          NumericalScalar & marginalResidual,
                                          NumericalScalar & marginalRelativeError)
{
  const Indices nk(tensor_[marginalIndex].getNk());
  const UnsignedInteger rank = tensor_[marginalIndex].getRank();
  RankKResidualEvaluation residualFunction(transformedInputSample_, outputSample_.getMarginal(marginalIndex), tensor_[marginalIndex]);
  NumericalPoint start;
  for (UnsignedInteger k = 0; k < rank; ++ k)
  {
    for (UnsignedInteger i = 0; i < nk.getSize(); ++ i)
    {
      NumericalPoint tmp(nk[i]);
      tmp[0] = 1.0; // vi(xi) = 1.0
      start.add(tmp);
    }
  }
  OptimizationProblem problem;
  problem.setObjective(NumericalMathFunction(residualFunction.clone()));
  problem.setMinimization(true);
  Cobyla solver(problem);
  solver.setStartingPoint(start);
  solver.setMaximumIterationNumber(1000);
  solver.setMaximumEvaluationNumber(1000000);
  solver.run();
  OptimizationResult result(solver.getResult());
  marginalResidual = result.getOptimalValue()[0];

  NumericalPoint beta(result.getOptimalValue());
  UnsignedInteger index = 0;
  for (UnsignedInteger k = 0; k < rank; ++ k)
  {
    RankOneTensor rankOne(tensor_[marginalIndex]);
    for (UnsignedInteger i = 0; i < nk.getSize(); ++ i)
    {
      NumericalPoint tmp(nk[i]);
      std::copy(beta.begin() + index, beta.begin() + index + nk[i], tmp.begin());
      rankOne.setCoefficients(i, tmp);
      index += nk[i];
    }
    tensor_[marginalIndex].setRankOneTensor(k, rankOne);
  }
}


void TensorApproximationAlgorithm::greedyRankOneSelection(const UnsignedInteger marginalIndex,
                                                          NumericalScalar & marginalResidual,
                                                          NumericalScalar & marginalRelativeError)
{
  const UnsignedInteger r = tensor_[marginalIndex].getRank();
  const UnsignedInteger size = outputSample_.getSize();
  const UnsignedInteger dimension = inputSample_.getDimension();
  const NumericalSample y(outputSample_.getMarginal(marginalIndex));
  const NumericalScalar variance = y.computeVariance()[0];

  const NumericalScalar alpha = std::max(1.0, ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-MaximumErrorFactor"));
  const NumericalScalar errorThreshold = std::max(0.0, ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-ErrorThreshold"));

  UnsignedInteger optimalRank = 0;
  NumericalScalar minimumError = SpecFunc::MaxNumericalScalar;

  const UnsignedInteger nFolds = ResourceMap::GetAsUnsignedInteger("TensorApproximationAlgorithm-K");
  const UnsignedInteger testSize = size / nFolds;

  // Persistent residual per fold across successive ranks
  Collection<NumericalSample> yResLearn(nFolds, NumericalSample(0, 1));
  for (UnsignedInteger fold = 0; fold < nFolds; ++ fold)
  {
    for (UnsignedInteger l = 0; l < testSize * nFolds; ++ l)
    {
      const UnsignedInteger jModK = l % nFolds;
      if (jModK != fold)
      {
        yResLearn[fold].add(y[l]);
      }
    }
  }

  for (UnsignedInteger k = 0; k < r; ++ k)
  {
    NumericalScalar quadraticResidual = 0.0;
    for (UnsignedInteger fold = 0; fold < nFolds; ++ fold)
    {
      // build cross-validation samples
      NumericalSample xLearn(0, dimension);
      NumericalSample yLearn(0, 1);
      NumericalSample xTest(0, dimension);
      NumericalSample yTest(0, 1);
      Indices rowFilter;

      // build learn/validation samples
      for (UnsignedInteger l = 0; l < testSize * nFolds; ++ l)
      {
        const UnsignedInteger jModK = l % nFolds;
        if (jModK != fold)
        {
          xLearn.add(transformedInputSample_[l]);
          yLearn.add(y[l]);
          rowFilter.add(l);
        }
        else
        {
          xTest.add(transformedInputSample_[l]);
          yTest.add(y[l]);
        }
      }

      // indices of kfold
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        proxy_[i].setRowFilter(rowFilter);
      }

      tensor_[marginalIndex].rank1tensors_[k] = rankOne(marginalIndex,
                                                        tensor_[marginalIndex],
                                                        xLearn,
                                                        yResLearn[fold],
                                                        marginalResidual,
                                                        marginalRelativeError);

      // build basis
      NumericalMathFunctionCollection prodColl(k + 1);
      for (UnsignedInteger k2 = 0; k2 <= k; ++ k2)
      {
        Pointer<NumericalMathEvaluationImplementation> p_evaluation = new RankOneTensorEvaluation(tensor_[marginalIndex].rank1tensors_[k2]);
        prodColl[k2] = NumericalMathFunction(p_evaluation);
      }
      const Basis basis(prodColl);
      Indices indices(prodColl.getSize());
      indices.fill();

      // update alpha
      PenalizedLeastSquaresAlgorithm algo(xLearn, yLearn, basis, indices);
      algo.run();
      NumericalPoint rk(algo.getCoefficients());
      for (UnsignedInteger k2 = 0; k2 <= k; ++ k2)
      {
        tensor_[marginalIndex].rank1tensors_[k2].setRadius(rk[k2]);
      }

      // compute residual
      NumericalMathFunction combination(prodColl, rk);
      yResLearn[fold] = yLearn - combination(xLearn);

      quadraticResidual += (yTest - combination(xTest)).computeRawMoment(2)[0];

    } // k-fold

    const NumericalScalar empiricalError = quadraticResidual / (testSize * nFolds);
    const NumericalScalar error = empiricalError / variance;

    Log::Info(OSS() << "marginalIndex=" << marginalIndex << " rank=" << k + 1 << " relativeError=" << error);

    if (error < minimumError)
    {
      optimalRank = k + 1;
      minimumError = error;
    }
    else
    {
      if (error > alpha * minimumError)
      {
        LOGINFO(OSS() << "Error=" << error << " larger than " << alpha << "*" << minimumError << "=" << alpha * minimumError);
        break;
      }
    }
    if (minimumError < errorThreshold)
    {
      LOGINFO(OSS() << "Minimum error=" << minimumError << " smaller than threshold=" << errorThreshold);
      break;
    }

  }

  // shrink tensor if smaller optimal rank
  tensor_[marginalIndex].setRank(optimalRank);

  // reset proxies to expose entire sample
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    proxy_[i].setRowFilter(Indices());
  }

  greedyRankOne(marginalIndex,
                transformedInputSample_,
                outputSample_.getMarginal(marginalIndex),
                marginalResidual,
                marginalRelativeError);
}


void TensorApproximationAlgorithm::greedyRankOne (const UnsignedInteger marginalIndex,
                                                  const NumericalSample & x,
                                                  const NumericalSample & y,
                                                  NumericalScalar & marginalResidual,
                                                  NumericalScalar & marginalRelativeError)
{
  const UnsignedInteger r = tensor_[marginalIndex].getRank();
  NumericalSample yRes(y);
  for (UnsignedInteger k = 0; k < r; ++ k)
  {
    tensor_[marginalIndex].rank1tensors_[k] = rankOne(marginalIndex,
                                                      tensor_[marginalIndex],
                                                      x,
                                                      yRes,
                                                      marginalResidual,
                                                      marginalRelativeError);

    // build basis
    NumericalMathFunctionCollection prodColl(k + 1);
    for (UnsignedInteger k2 = 0; k2 <= k; ++ k2)
    {
      Pointer<NumericalMathEvaluationImplementation> p_evaluation = new RankOneTensorEvaluation(tensor_[marginalIndex].rank1tensors_[k2]);
      prodColl[k2] = NumericalMathFunction(p_evaluation);
    }
    const Basis basis(prodColl);
    Indices indices(prodColl.getSize());
    indices.fill();

    // update radius
    PenalizedLeastSquaresAlgorithm algo(x, y, basis, indices);
    algo.run();
    NumericalPoint rk(algo.getCoefficients());
    for (UnsignedInteger k2 = 0; k2 <= k; ++ k2)
    {
      tensor_[marginalIndex].rank1tensors_[k2].setRadius(rk[k2]);
    }

    // compute residual
    NumericalMathFunction combination(prodColl, rk);
    yRes = y - combination(x);

  }
}

RankOneTensor TensorApproximationAlgorithm::rankOne(const UnsignedInteger marginalIndex,
                                                    const CanonicalTensor & canonicalTensor,
                                                    const NumericalSample & x,
                                                    const NumericalSample & y,
                                                    NumericalScalar & marginalResidual,
                                                    NumericalScalar & marginalRelativeError)
{
  Bool convergence = false;
  const UnsignedInteger dimension = x.getDimension();
  const UnsignedInteger size = x.getSize();
  NumericalPoint weight(size, 1.0 / size);
  NumericalSample V;

  NumericalScalar currentResidual = SpecFunc::MaxNumericalScalar;;

  // initialize
  RankOneTensor rank1Tensor(canonicalTensor);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    NumericalPoint coefficients(rank1Tensor.getCoefficients(i));
    coefficients[0] = 1.0;// vi(xi) = 1.0
    rank1Tensor.setCoefficients(i, coefficients);
  }

  V = NumericalSample(size, 0);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    NumericalMathFunction v(rank1Tensor.getBasis(i), rank1Tensor.getCoefficients(i));
    V.stack(v(x.getMarginal(i)));
  }

  UnsignedInteger iteration = 0;

  while (!convergence && (iteration < maximumAlternatingLeastSquaresIteration_))
  {
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      Log::Info(OSS() << " i=" << i << "/"<< dimension);
      const UnsignedInteger basisSize = rank1Tensor.getCoefficients(i).getSize();
      Indices indices(basisSize);
      indices.fill();

      NumericalPoint w(size, 1.0);// w_l
      for (UnsignedInteger l = 0; l < size; ++ l)
      {
        for (UnsignedInteger i2 = 0; i2 < dimension; ++ i2)
        {
          if (i2 != i) w[l] *= V[l][i2];
        }
      }

      // solve a least-squares problem along component i
      NumericalSample rhs(size, 1);
      NumericalPoint w2(w);
      for (UnsignedInteger l = 0; l < size; ++ l) { rhs[l][0] = y[l][0] / w[l]; w2[l] *= w[l]; } 
      ApproximationAlgorithm approximationAlgorithm(p_approximationAlgorithmImplementationFactory_->build(x.getMarginal(i), rhs, w2, rank1Tensor.getBasis(i), indices));
      approximationAlgorithm.run(proxy_[i]);
      rank1Tensor.setCoefficients(i, approximationAlgorithm.getCoefficients());

      // update current contribution
      NumericalMathFunction v(rank1Tensor.getBasis(i), rank1Tensor.getCoefficients(i));
      NumericalSample tXi(x.getMarginal(i));
      NumericalSample vi(v(tXi));
      for (UnsignedInteger l = 0; l < size; ++ l)
      {
        V[l][i] = vi[l][0];
      }

    } // i loop

    // update alpha
    NumericalScalar currentRadius = 1.0;
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      NumericalPoint coefficients(rank1Tensor.getCoefficients(i));
      const NumericalScalar norm = coefficients.norm();
      currentRadius *= norm;
      coefficients /= norm;
      rank1Tensor.setCoefficients(i, coefficients);
    }
    V = NumericalSample(size, 0);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      NumericalMathFunction v(rank1Tensor.getBasis(i), rank1Tensor.getCoefficients(i));
      V.stack(v(x.getMarginal(i)));
    }
    NumericalPoint w(size, 1.0);// w_l
    for (UnsignedInteger l = 0; l < size; ++ l)
    {
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        w[l] *= V[l][i];
      }
    }

    // update radius
    NumericalSample rhs(size, 1);
    NumericalPoint w2(w);
    for (UnsignedInteger l = 0; l < size; ++ l) { rhs[l][0] = y[l][0] / w[l]; w2[l] *= w[l]; } 
    PenalizedLeastSquaresAlgorithm algo(NumericalSample(size, 1), rhs, w2, ConstantBasisFactory().build(), Indices(1));
    algo.run();
    rank1Tensor.setRadius(algo.getCoefficients()[0]);
    const NumericalScalar radiusError = std::abs(rank1Tensor.getRadius() - currentRadius);
    Log::Info(OSS() << "alpha=" << rank1Tensor.getRadius());

    // compute residual
    marginalResidual = 0.0;
    for (UnsignedInteger l = 0; l < size; ++ l)
    {
      NumericalScalar prod = rank1Tensor.getRadius();
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        prod *= V[l][i];
      }
      const NumericalScalar slack = y[l][0] - prod;
      marginalResidual += slack * slack / size;
    }
    marginalRelativeError = marginalResidual / y.computeVariance()[0];

    const NumericalScalar residualError = std::abs(currentResidual - marginalResidual);
    currentResidual = marginalResidual;

    convergence = (residualError < maximumResidualError_) && (radiusError < maximumRadiusError_);

    Log::Info(OSS() << "iteration=" << iteration << " residualError=" << residualError << " radiusError=" << radiusError);

    ++ iteration;
  }

  return rank1Tensor;
}





/* Get the  result */
TensorApproximationResult TensorApproximationAlgorithm::getResult() const
{
  return result_;
}


NumericalSample TensorApproximationAlgorithm::getInputSample() const
{
  return inputSample_;
}


NumericalSample TensorApproximationAlgorithm::getOutputSample() const
{
  return outputSample_;
}

void TensorApproximationAlgorithm::setApproximationAlgorithmFactory(const ApproximationAlgorithmImplementationFactory & factory)
{
  p_approximationAlgorithmImplementationFactory_ = factory.clone();
}

/* Max ALS iteration accessor */
void TensorApproximationAlgorithm::setMaximumAlternatingLeastSquaresIteration(const UnsignedInteger maximumAlternatingLeastSquaresIteration)
{
  maximumAlternatingLeastSquaresIteration_ = maximumAlternatingLeastSquaresIteration;
}

UnsignedInteger TensorApproximationAlgorithm::getMaximumAlternatingLeastSquaresIteration() const
{
  return maximumAlternatingLeastSquaresIteration_;
}

/* Radius error accessor */
void TensorApproximationAlgorithm::setMaximumRadiusError(const NumericalScalar maximumRadiusError)
{
  maximumRadiusError_ = maximumRadiusError_;
}

NumericalScalar TensorApproximationAlgorithm::getMaximumRadiusError() const
{
  return maximumRadiusError_;
}

/* Residual error accessor */
void TensorApproximationAlgorithm::setMaximumResidualError(const NumericalScalar maximumResidualError)
{
  maximumResidualError_ = maximumResidualError;
}

NumericalScalar TensorApproximationAlgorithm::getMaximumResidualError() const
{
  return maximumResidualError_;
}

void TensorApproximationAlgorithm::setRankSelection(const Bool rankSelection)
{
  rankSelection_ = rankSelection;
}

/* Method save() stores the object through the StorageManager */
void TensorApproximationAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("p_approximationAlgorithmImplementationFactory_" , *p_approximationAlgorithmImplementationFactory_);
  adv.saveAttribute("maximumAlternatingLeastSquaresIteration_", maximumAlternatingLeastSquaresIteration_);
  adv.saveAttribute("maximumRadiusError_", maximumRadiusError_);
  adv.saveAttribute("maximumResidualError_", maximumResidualError_);
  adv.saveAttribute("rankSelection_", rankSelection_);
}


/* Method load() reloads the object from the StorageManager */
void TensorApproximationAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("result_", result_);
  ApproximationAlgorithmImplementationFactory approximationAlgorithmImplementationFactory;
  adv.loadAttribute("p_approximationAlgorithmImplementationFactory_" , approximationAlgorithmImplementationFactory);
  p_approximationAlgorithmImplementationFactory_ = approximationAlgorithmImplementationFactory.clone();
  adv.loadAttribute("maximumAlternatingLeastSquaresIteration_", maximumAlternatingLeastSquaresIteration_);
  adv.loadAttribute("maximumRadiusError_", maximumRadiusError_);
  adv.loadAttribute("maximumResidualError_", maximumResidualError_);
  adv.loadAttribute("rankSelection_", rankSelection_);
}



END_NAMESPACE_OPENTURNS
