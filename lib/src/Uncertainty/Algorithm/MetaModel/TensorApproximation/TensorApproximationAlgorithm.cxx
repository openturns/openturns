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
#include "openturns/FixedStrategy.hxx"
#include "openturns/CleaningStrategy.hxx"
#include "openturns/FixedExperiment.hxx"
#include "openturns/LeastSquaresStrategy.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/PenalizedLeastSquaresAlgorithm.hxx"
#include "openturns/OrthogonalProductFunctionFactory.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Distribution> DistributionCollection;
typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;

CLASSNAMEINIT(TensorApproximationAlgorithm);

static const Factory<TensorApproximationAlgorithm> RegisteredFactory;


/* Default constructor */
TensorApproximationAlgorithm::TensorApproximationAlgorithm()
  : MetaModelAlgorithm()
{
  // Nothing to do
}

/* Constructor */
TensorApproximationAlgorithm::TensorApproximationAlgorithm(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const Distribution & distribution,
    const OrthogonalProductFunctionFactory & basisFactory,
    const Indices & nk)
  : MetaModelAlgorithm(distribution, NumericalMathFunction(NumericalMathFunctionImplementation(DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, false).clone())))
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , basisFactory_(basisFactory)
{
  // Check sample size
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";

  FunctionFamilyCollection functionFamilies(basisFactory.getFunctionFamilyCollection());

  if (nk.getSize() != functionFamilies.getSize()) throw InvalidArgumentException(HERE) << "nk size ("<<nk.getSize()<<") must match orthogonal basis factories ("<<functionFamilies.getSize()<<")";
  tensor_ = CanonicalTensor(functionFamilies, nk);
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

    // build the metamodel
    UnsignedInteger r = tensor_.getRank();
//     NumericalPointCollection coll;
    for (UnsignedInteger k = 0; k < r; ++ k)
    {
      const UnsignedInteger dimension = inputSample_.getDimension();
      NumericalMathFunctionCollection prodColl(dimension);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        NumericalMathFunction v(tensor_.rank1tensors_[k].basis_[i], tensor_.rank1tensors_[k].coefficients_[i]);
        prodColl[i] = v;
      }

      // TODO
      NumericalMathFunction prod(prodColl[0]);
      for (UnsignedInteger i = 1; i < dimension; ++ i)
        prod = ProductNumericalMathFunction(prod.getImplementation(), prodColl[i].getImplementation());
//       coll.add(prod);
    }
//     marginals.add();
  }
  // Build the result
  result_ = TensorApproximationResult(distribution_, transformation_, inverseTransformation_, composedModel_, residuals, relativeErrors);
  

}

/* Marginal computation */
void TensorApproximationAlgorithm::runMarginal(const UnsignedInteger marginalIndex, NumericalScalar & marginalResidual, NumericalScalar & marginalRelativeError)
{
  rankOneApproximation(tensor_.rank1tensors_[0], marginalIndex, marginalResidual, marginalRelativeError);
}

void TensorApproximationAlgorithm::rankOneApproximation(RankOneTensor & rank1Tensor, const UnsignedInteger marginalIndex, NumericalScalar & marginalResidual, NumericalScalar & marginalRelativeError)
{
  Bool convergence = false;
  const UnsignedInteger dimension = inputSample_.getDimension();
  const UnsignedInteger size = inputSample_.getSize();
  NumericalPoint weight(size, 1.0 / size);
  UnsignedInteger r = tensor_.getRank();
  Collection<Collection<Basis> > subBasis(r);
  NumericalSample V;

  // initialize
  rank1Tensor.alpha_ = 1.0;
  // nk may be different on each rank
  Indices nk(rank1Tensor.nk_);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
//       rank1Tensor.coefficients_[i] = ComposedDistribution(ComposedDistribution::DistributionCollection(nk[i], Uniform())).getRealization();
//       rank1Tensor.coefficients_[i].normalize();
    if (nk[i] > 0)
      rank1Tensor.coefficients_[i][0] = 1.0;// vi(xi) = 1.0
  }
  V = NumericalSample(size, 0);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    NumericalMathFunction v(rank1Tensor.basis_[i], rank1Tensor.coefficients_[i]);
    V.stack(v(transformedInputSample_.getMarginal(i)));
  }

  UnsignedInteger iteration = 0;
  const NumericalPoint y(outputSample_.getMarginal(marginalIndex).getImplementation()->getData());


  Collection<DesignProxy> proxy(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    proxy[i] = DesignProxy(transformedInputSample_.getMarginal(i), rank1Tensor.basis_[i]);
  }

  while (!convergence || (iteration >= 1000))
  {
    // norm-2 of difference of coefficients along all directions
    // to test for convergence of coefficients
    NumericalScalar coefficientsDeltaNorm2 = 0;

    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      Log::Info(OSS() << " i=" << i << "/"<< dimension);
      const UnsignedInteger basisSize = rank1Tensor.coefficients_[i].getSize();
      Indices indices(basisSize);
      indices.fill();

      NumericalPoint w(size, rank1Tensor.alpha_);// w_l
      for (UnsignedInteger l = 0; l < size; ++ l)
      {
        for (UnsignedInteger i2 = 0; i2 < dimension; ++ i2)
        {
          if (i2 != i) w[l] *= V[l][i2];
        }
      }
      MatrixImplementation basisMatrix(proxy[i].computeDesign(indices));
      for (UnsignedInteger l = 0; l < size; ++ l)
      {
        for (UnsignedInteger j = 0; j < basisSize; ++ j)
        {
          basisMatrix(l, j) *= w[l];
        }
      }

      NumericalPoint coefficients;
      try
      {
        CovarianceMatrix normalMatrix(basisMatrix.computeGram(true));
        coefficients = normalMatrix.solveLinearSystem(basisMatrix.genVectProd(y, true), false);
      }
      catch (NotDefinedException & ex)
      {
        Log::Info(OSS() << ex);
        coefficients = basisMatrix.solveLinearSystemRect(y);
      }
      Log::Info(OSS() << "   coefficients(normal)=" << coefficients.__str__());
//       NumericalSample rhs(size, 1);
//       NumericalPoint w2(w);
//       for (UnsignedInteger l = 0; l < size; ++ l) { rhs[l][0] = y[l] / w[l]; w2[l] *= w[l]; } 
//       PenalizedLeastSquaresAlgorithm algo(transformedInputSample_.getMarginal(i), rhs, w2, rank1Tensor.basis_[i], indices);
//       algo.run();
//       Log::Info(OSS() << "   coefficients(ls)=" << algo.getCoefficients());

      // normalize coefficients
//       NumericalScalar norm = coefficients.norm();
//       rank1Tensor.alpha_ *= norm;
      coefficients.normalize();
      coefficientsDeltaNorm2 += (coefficients - rank1Tensor.coefficients_[i]).normSquare();
      rank1Tensor.coefficients_[i] = coefficients;

      // update V[i], with modulus
      NumericalMathFunction v(rank1Tensor.basis_[i], rank1Tensor.coefficients_[i]);
      NumericalSample tX(transformedInputSample_.getMarginal(i));
      NumericalSample vi(v(tX));
      for (UnsignedInteger l = 0; l < size; ++ l)
      {
        V[l][i] = vi[l][0];
      }

    } // i loop

    // update alpha
    NumericalScalar sumProdVi2 = 0.0;
    NumericalScalar sumylProdvi = 0.0;
    for (UnsignedInteger l = 0; l < size; ++ l)
    {
      NumericalScalar prodVi2 = 1.0;
      NumericalScalar prodVi = 1.0;
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        prodVi2 *= V[l][i] * V[l][i];
        prodVi *= V[l][i];
      }
      sumProdVi2 += prodVi2;
      sumylProdvi += y[l] * prodVi;
    }
    rank1Tensor.alpha_ = sumProdVi2 / sumylProdvi;
    Log::Info(OSS() << "alpha=" << rank1Tensor.alpha_);

    // compute residual
    marginalResidual = 0.0;
    for (UnsignedInteger l = 0; l < size; ++ l)
    {
      NumericalScalar prod = rank1Tensor.alpha_;
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        prod *= V[l][i];
      }
      const NumericalScalar slack = y[l] - prod;
      marginalResidual += slack * slack / size;
    }



    convergence = (marginalResidual < 1e-8) || (coefficientsDeltaNorm2 < 1e-8);
    Log::Info(OSS() << "iteration=" << iteration << " residual=" << marginalResidual << " coefficientsDeltaNorm2=" << coefficientsDeltaNorm2);

    ++ iteration;
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


/* Method save() stores the object through the StorageManager */
void TensorApproximationAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute( "result_", result_ );
}


/* Method load() reloads the object from the StorageManager */
void TensorApproximationAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute( "result_", result_ );
}



END_NAMESPACE_OPENTURNS
