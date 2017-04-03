//                                               -*- C++ -*-
/**
 *  @brief Tensor approximation algorithm
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/FunctionImplementation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/OrthogonalProductFunctionFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/DatabaseEvaluation.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/SparseMethod.hxx"
#include "openturns/DistributionTransformation.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Distribution> DistributionCollection;
typedef Collection<Function> FunctionCollection;
typedef Collection<OrthogonalUniVariateFunctionFamily> FunctionFamilyCollection;

CLASSNAMEINIT(TensorApproximationAlgorithm);

static const Factory<TensorApproximationAlgorithm> Factory_TensorApproximationAlgorithm;


/* Default constructor */
TensorApproximationAlgorithm::TensorApproximationAlgorithm()
  : MetaModelAlgorithm()
  , maxRank_(1)
  , maximumAlternatingLeastSquaresIteration_(ResourceMap::GetAsUnsignedInteger("TensorApproximationAlgorithm-DefaultMaximumAlternatingLeastSquaresIteration"))
  , maximumRadiusError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumRadiusError"))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumResidualError"))
{
  // Nothing to do
}

/* Constructor */
TensorApproximationAlgorithm::TensorApproximationAlgorithm(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const Distribution & distribution,
    const OrthogonalProductFunctionFactory & basisFactory,
    const Indices & degrees,
    const UnsignedInteger maxRank)
  : MetaModelAlgorithm(distribution, Function(FunctionImplementation(DatabaseEvaluation(inputSample, outputSample, false).clone())))
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , maxRank_(maxRank)
  , basisFactory_(basisFactory)
  , maximumAlternatingLeastSquaresIteration_(ResourceMap::GetAsUnsignedInteger("TensorApproximationAlgorithm-DefaultMaximumAlternatingLeastSquaresIteration"))
  , maximumRadiusError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumRadiusError"))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar("TensorApproximationAlgorithm-DefaultMaximumResidualError"))
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";

  FunctionFamilyCollection functionFamilies(basisFactory.getFunctionFamilyCollection());

  if (degrees.getSize() != functionFamilies.getSize()) throw InvalidArgumentException(HERE) << "degrees size ("<<degrees.getSize()<<") must match orthogonal basis factories ("<<functionFamilies.getSize()<<")";
  tensor_ = Collection<CanonicalTensorEvaluation>(outputSample.getDimension(), CanonicalTensorEvaluation(functionFamilies, degrees));
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
    const UnsignedInteger outputDimension = outputSample_.getDimension();
  // First, compute all the parts that are independent of the marginal output
  // Create the isoprobabilistic transformation
  // We have two distributions here:
  // + The distribution of the input, called distribution_
  // + The distribution defining the inner product in basis, called measure
  // The projection is done on the basis, ie wrt measure_, so we have to
  // introduce an isoprobabilistic transformation that maps distribution_ onto
  // measure
  //
  // Get the measure upon which the orthogonal basis is built
//   const OrthogonalBasis basis(adaptiveStrategy_.getImplementation()->basis_);
  const Distribution measure(basisFactory_.getMeasure());

  const DistributionTransformation transformation(distribution_, measure);
  transformation_ = transformation;
  inverseTransformation_ = transformation.inverse();

  // Build the composed model g = f o T^{-1}, which is a function of Z so it can be decomposed upon an orthonormal basis based on Z distribution
  const Bool noTransformation = (measure == distribution_);
  LOGINFO("Transform the input sample in the measure space if needed");
  if (noTransformation) composedModel_ = model_;
  else composedModel_ = ComposedFunction(model_, inverseTransformation_);

  transformedInputSample_ = transformation_(inputSample_);

  FunctionCollection marginals(0);
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
      proxy_[i] = DesignProxy(transformedInputSample_.getMarginal(i), tensor_[marginalIndex].getBasis(i));
    }
  }

  const String method(ResourceMap::Get("TensorApproximationAlgorithm-Method"));
  if (method == "GreedyRankOne")
  {
    greedyRankOne(transformedInputSample_,
                  outputSample_.getMarginal(marginalIndex),
                  tensor_[marginalIndex],
                  marginalResidual,
                  marginalRelativeError);
  }
  else if (method == "RankM")
  {
    rankM(transformedInputSample_,
                  outputSample_.getMarginal(marginalIndex),
                  tensor_[marginalIndex],
                  marginalResidual,
                  marginalRelativeError);
  }
  else throw InvalidArgumentException(HERE) << "Invalid method: " << method;
}


void TensorApproximationAlgorithm::greedyRankOne (const NumericalSample & x,
                                                  const NumericalSample & y,
                                                  CanonicalTensorEvaluation & tensor,
                                                  NumericalScalar & marginalResidual,
                                                  NumericalScalar & marginalRelativeError)
{
  NumericalSample yRes(y);
  const UnsignedInteger dimension = x.getDimension();
  for (UnsignedInteger i = 0; i < maxRank_; ++ i)
  {
    tensor.setRank(i + 1);

    // initialize tensor coefficients on last rank
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      NumericalPoint coefficients(tensor.getCoefficients(i, j));
      coefficients[0] = 1.0;// vj(xj) = 1.0
      tensor.setCoefficients(i, j, coefficients);
    }

    rankOne(x,
            yRes,
            tensor,
            i,
            marginalResidual,
            marginalRelativeError);

    // build basis
    FunctionCollection prodColl(i + 1);
    for (UnsignedInteger i2 = 0; i2 <= i; ++ i2)
    {
      prodColl[i2] = Function(tensor.getMarginalRank(i2).clone());
    }

    const Basis basis(prodColl);
    DesignProxy proxy(x, basis);
    Indices full(prodColl.getSize());
    full.fill();

    // update radius
    const String methodName(ResourceMap::Get("TensorApproximationAlgorithm-DecompositionMethod"));
    LeastSquaresMethod internalMethod(LeastSquaresMethod::Build(methodName, proxy, full));
    SparseMethod method(internalMethod);
    NumericalPoint yFlat(y.getImplementation()->getData());
    NumericalPoint rk(method.solve(yFlat));

    for (UnsignedInteger i2 = 0; i2 <= i; ++ i2)
    {
      // report radius on first component
      tensor.setCoefficients(i2, 0, rk[i2] * tensor.getCoefficients(i2, 0));
    }

    // compute residual
    Function tensorFunction(tensor.clone());
    yRes = y - tensorFunction(x);

    if (rk[i] == 0.0)
    {
      // the last basis term leads to a null coeff: no need to learn more ranks
      tensor.setRank(i);
      break;
    }
  } // for i
}

void TensorApproximationAlgorithm::rankOne(const NumericalSample & x,
                                          const NumericalSample & y,
                                          CanonicalTensorEvaluation & tensor,
                                          const UnsignedInteger i,
                                          NumericalScalar & marginalResidual,
                                          NumericalScalar & marginalRelativeError)
{
  Bool convergence = false;
  const UnsignedInteger dimension = x.getDimension();
  const UnsignedInteger size = x.getSize();
  if (size != y.getSize()) throw InvalidArgumentException(HERE) << " x != y";

  NumericalPoint yFlat(y.getImplementation()->getData());

  NumericalScalar currentResidual = SpecFunc::MaxNumericalScalar;

  NumericalSample V(dimension, NumericalPoint(size, 1.0));

  UnsignedInteger iteration = 0;

  while (!convergence && (iteration < maximumAlternatingLeastSquaresIteration_))
  {
    const NumericalScalar oldRadius = tensor.getCoefficients(i, 0).norm();
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      Log::Info(OSS() << " j=" << j << "/"<< dimension);
      const UnsignedInteger basisSize = tensor.getCoefficients(i, j).getSize();
      Indices full(basisSize);
      full.fill();

      NumericalPoint w(size, 1.0);// w_l
      for (UnsignedInteger p = 0; p < size; ++ p)
      {
        for (UnsignedInteger j2 = 0; j2 < dimension; ++ j2)
        {
          if (j2 != j) w[p] *= V[j2][p];
        }
      }

      if (proxy_[j].hasRowFilter())
      {
        // compute non-filtered weights for reuse in proxy
        Indices rowFilter(proxy_[j].getRowFilter());
        const UnsignedInteger actualSize = proxy_[j].getInputSample().getSize();
        NumericalPoint w_big(actualSize);
        for (UnsignedInteger p = 0; p < size; ++ p)
        {
          w_big[rowFilter[p]] = w[p];
        }
        w = w_big;
      }
      proxy_[j].setWeight(w);

      // solve a least-squares problem along component j
      const String methodName(ResourceMap::Get("TensorApproximationAlgorithm-DecompositionMethod"));
      LeastSquaresMethod method(LeastSquaresMethod::Build(methodName, proxy_[j], full));
      tensor.setCoefficients(i, j, method.solve(yFlat));
      proxy_[j].setWeight(NumericalPoint(0));

      // update current contribution
      V[j] = Matrix(proxy_[j].computeDesign(full)) * tensor.getCoefficients(i, j);


    } // j loop

    // update alpha
    NumericalPoint f(size, 1.0);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      for (UnsignedInteger p = 0; p < size; ++ p)
      {
        f[p] *= V[j][p];
      }
    }
    NumericalScalar currentRadius = dot(f, yFlat) / f.normSquare();
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      NumericalPoint coefficients(tensor.getCoefficients(i, j));
      coefficients = tensor.getCoefficients(i, j);
      const NumericalScalar norm = coefficients.norm();
      currentRadius *= norm;
      coefficients /= norm;
      tensor.setCoefficients(i, j, coefficients);
    }

    Log::Info(OSS() << "alpha=" << currentRadius);

    const NumericalScalar radiusError = std::abs(oldRadius - currentRadius);
    // report radius on first component
    tensor.setCoefficients(i, 0, currentRadius * tensor.getCoefficients(i, 0));

    // compute residual
    marginalResidual = 0.0;
    for (UnsignedInteger p = 0; p < size; ++ p)
    {
      NumericalScalar prod = currentRadius;
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        prod *= V[j][p];
      }
      const NumericalScalar slack = y[p][0] - prod;
      marginalResidual += slack * slack / size;
    }
    marginalRelativeError = marginalResidual / y.computeVariance()[0];

    const NumericalScalar residualError = std::abs(currentResidual - marginalResidual);
    currentResidual = marginalResidual;

    convergence = (residualError < maximumResidualError_) && (radiusError < maximumRadiusError_);

    Log::Info(OSS() << "iteration=" << iteration << " residualError=" << residualError << " radiusError=" << radiusError);

    ++ iteration;
  }
}


void TensorApproximationAlgorithm::rankM (const NumericalSample & x,
                                          const NumericalSample & y,
                                          CanonicalTensorEvaluation & tensor,
                                          NumericalScalar & marginalResidual,
                                          NumericalScalar & marginalRelativeError)
{
  Bool convergence = false;
  const UnsignedInteger dimension = x.getDimension();
  const UnsignedInteger size = x.getSize();
  UnsignedInteger m = maxRank_;
  tensor.setRank(m);

  // initialize tensor coefficients up to rank m
  for (UnsignedInteger i = 0; i < m; ++ i)
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      NumericalPoint coefficients(tensor.getCoefficients(i, j));
      coefficients[0] = 1.0;// vj(xj) = 1.0
      tensor.setCoefficients(i, j, coefficients);
    }

  UnsignedInteger iteration = 0;

  NumericalScalar currentResidual = SpecFunc::MaxNumericalScalar;

  while (!convergence && (iteration < maximumAlternatingLeastSquaresIteration_))
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      rankMComponent(x, y, tensor, j);
    }

    // normalize coefficients
    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      NumericalScalar radius_i = 1.0;
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        NumericalPoint coefficients(tensor.getCoefficients(i, j));
        const NumericalScalar norm = coefficients.norm();
        radius_i *= norm;
        coefficients /= norm;
        tensor.setCoefficients(i, j, coefficients);
      }
    }

    // build basis of rank one tensors
    FunctionCollection prodColl(m);
    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      prodColl[i] = Function(tensor.getMarginalRank(i).clone());
    }

    const Basis basis(prodColl);
    DesignProxy proxy(x, basis);
    Indices full(prodColl.getSize());
    full.fill();

    // update radius
    const String methodName(ResourceMap::Get("TensorApproximationAlgorithm-DecompositionMethod"));
    LeastSquaresMethod internalMethod(LeastSquaresMethod::Build(methodName, proxy, full));
    SparseMethod method(internalMethod);
    NumericalPoint yFlat(y.getImplementation()->getData());
    NumericalPoint rk(method.solve(yFlat));

    // report radius on first component
    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      NumericalPoint coefficients0(tensor.getCoefficients(i, 0));
      coefficients0 *= rk[i];
      tensor.setCoefficients(i, 0, coefficients0);
    }

    // compute residual
    marginalResidual = 0.0;
    for (UnsignedInteger p = 0; p < size; ++ p)
    {
      const NumericalScalar slack = y[p][0] - tensor(x[p])[0];
      marginalResidual += slack * slack / size;
    }
    marginalRelativeError = marginalResidual / y.computeVariance()[0];

    const NumericalScalar residualError = std::abs(currentResidual - marginalResidual);
    currentResidual = marginalResidual;

    convergence = (residualError < maximumResidualError_);// && (radiusError < maximumRadiusError_);

    Log::Info(OSS() << "iteration=" << iteration << " residualError=" << residualError);// << " radiusError=" << radiusError);

    ++ iteration;
  }
}

void TensorApproximationAlgorithm::rankMComponent (const NumericalSample & x,
                                                  const NumericalSample & y,
                                                  CanonicalTensorEvaluation & tensor,
                                                  const UnsignedInteger j)
{
  const UnsignedInteger dimension = x.getDimension();
  const UnsignedInteger size = x.getSize();
  const UnsignedInteger nj = tensor.getDegrees()[j];

  const UnsignedInteger m = tensor.getRank();
  Collection<NumericalSample> V(m, NumericalSample(dimension, NumericalPoint(size, 1.0)));

  // compute contributions
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    for (UnsignedInteger j2 = 0; j2 < dimension; ++ j2)
    {
      const UnsignedInteger basisSize = tensor.getDegrees()[j2];
      Indices full(basisSize);
      full.fill();
      V[i][j2] = Matrix(proxy_[j2].computeDesign(full)) * tensor.getCoefficients(i, j2);
    }
  }

  // compute weight
  NumericalPoint w(size, 0.0);// w_p
  for (UnsignedInteger p = 0; p < size; ++ p)
  {
    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      NumericalScalar wi = 1.0;
      for (UnsignedInteger j2 = 0; j2 < dimension; ++ j2)
      {
        if (j2 != j) wi *= V[i][j2][p];
      }
      w[p] += wi;
    }
  }

  Basis basis;
  const Basis basisj(tensor.getBasis(j));
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    for (UnsignedInteger k = 0; k < nj; ++ k)
    {
      basis.add(basisj[k]);
    }
  }
  DesignProxy proxy(transformedInputSample_.getMarginal(j), basis);
  proxy.setWeight(w);
  Indices full(nj * m);
  full.fill();
  NumericalPoint yFlat(y.getImplementation()->getData());
  const String methodName(ResourceMap::Get("TensorApproximationAlgorithm-DecompositionMethod"));
  LeastSquaresMethod method(LeastSquaresMethod::Build(methodName, proxy, full));
  NumericalPoint beta_ij(method.solve(yFlat));

  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    NumericalPoint beta_j(nj);
    std::copy(beta_ij.begin() + i * nj, beta_ij.begin() + (i + 1) * nj, beta_j.begin());
    tensor.setCoefficients(i, j, beta_j);
  }
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
  maximumRadiusError_ = maximumRadiusError;
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

/* Method save() stores the object through the StorageManager */
void TensorApproximationAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("maxRank_", maxRank_);
  adv.saveAttribute("maximumAlternatingLeastSquaresIteration_", maximumAlternatingLeastSquaresIteration_);
  adv.saveAttribute("maximumRadiusError_", maximumRadiusError_);
  adv.saveAttribute("maximumResidualError_", maximumResidualError_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void TensorApproximationAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("maxRank_", maxRank_);
  adv.loadAttribute("maximumAlternatingLeastSquaresIteration_", maximumAlternatingLeastSquaresIteration_);
  adv.loadAttribute("maximumRadiusError_", maximumRadiusError_);
  adv.loadAttribute("maximumResidualError_", maximumResidualError_);
  adv.saveAttribute("result_", result_);
}



END_NAMESPACE_OPENTURNS
