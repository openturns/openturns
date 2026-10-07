//                                               -*- C++ -*-
/**
 *  @brief Finite orthonormalization algorithm
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/FiniteOrthonormalizationAlgorithm.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/ExperimentIntegration.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Interval.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FiniteOrthonormalizationAlgorithm)

static const Factory<FiniteOrthonormalizationAlgorithm> Factory_FiniteOrthonormalizationAlgorithm;

namespace
{

class GramEvaluation final : public EvaluationImplementation
{
public:
  GramEvaluation()
    : EvaluationImplementation()
  {
  }

  GramEvaluation(const Function & left,
                 const Function & right,
                 const Distribution & measure)
    : EvaluationImplementation()
    , left_(left)
    , right_(right)
    , measure_(measure)
  {
  }

  GramEvaluation * clone() const override
  {
    return new GramEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return measure_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & x) const override
  {
    return Point(1, left_(x)[0] * right_(x)[0] * measure_.computePDF(x));
  }

  String __repr__() const override
  {
    return OSS() << "class=" << getClassName();
  }

private:
  Function left_;
  Function right_;
  Distribution measure_;
};

}

FiniteOrthonormalizationAlgorithm::FiniteOrthonormalizationAlgorithm()
  : PersistentObject()
  , initialFunctions_()
  , measure_()
  , experiment_()
  , integrationAlgorithm_()
  , hasExperiment_(false)
  , hasIntegrationAlgorithm_(false)
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
}

FiniteOrthonormalizationAlgorithm::FiniteOrthonormalizationAlgorithm(const FunctionCollection & functions,
    const Distribution & measure)
  : PersistentObject()
  , initialFunctions_(functions)
  , measure_(measure)
  , experiment_()
  , integrationAlgorithm_()
  , hasExperiment_(false)
  , hasIntegrationAlgorithm_(false)
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
}

FiniteOrthonormalizationAlgorithm::FiniteOrthonormalizationAlgorithm(const FunctionCollection & functions,
    const Distribution & measure,
    const WeightedExperiment & experiment)
  : PersistentObject()
  , initialFunctions_(functions)
  , measure_(measure)
  , experiment_(experiment)
  , integrationAlgorithm_()
  , hasExperiment_(true)
  , hasIntegrationAlgorithm_(false)
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
}

FiniteOrthonormalizationAlgorithm::FiniteOrthonormalizationAlgorithm(const FunctionCollection & functions,
    const Distribution & measure,
    const IntegrationAlgorithm & integrationAlgorithm)
  : PersistentObject()
  , initialFunctions_(functions)
  , measure_(measure)
  , experiment_()
  , integrationAlgorithm_(integrationAlgorithm)
  , hasExperiment_(false)
  , hasIntegrationAlgorithm_(true)
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
}

FiniteOrthonormalizationAlgorithm * FiniteOrthonormalizationAlgorithm::clone() const
{
  return new FiniteOrthonormalizationAlgorithm(*this);
}

void FiniteOrthonormalizationAlgorithm::run()
{
  if (isAlreadyComputed_) return;
  const UnsignedInteger nFuns = initialFunctions_.getSize();
  if (nFuns == 0)
  {
    orthonormalFunctions_ = FunctionPersistentCollection(0);
    coefficients_ = SquareMatrix(0);
    isAlreadyComputed_ = true;
    return;
  }
  const UnsignedInteger dimension = measure_.getDimension();
  const Interval support(measure_.getRange());
  mapping_ = buildMapping();
  // Effective integration rule. Experiment-derived rules expose their nodes:
  // factorize the weighted design matrix (QR), which preserves the conditioning
  // of the design, unlike the Gram matrix whose condition number is squared.
  // Opaque rules only provide integrals: factorize the Gram matrix (Cholesky).
  IntegrationAlgorithm effectiveIntegration;
  if (hasIntegrationAlgorithm_)
  {
    effectiveIntegration = integrationAlgorithm_;
  }
  else if (hasExperiment_)
  {
    effectiveIntegration = ExperimentIntegration(experiment_);
  }
  else
  {
    effectiveIntegration = buildDefaultIntegration(measure_);
  }
  // Initial functions pulled back from the reference cube to the physical support
  FunctionPersistentCollection pulledBack(nFuns);
  for (UnsignedInteger i = 0; i < nFuns; ++i)
  {
    pulledBack[i] = ComposedFunction(initialFunctions_[i], mapping_);
  }
  Matrix RinvMatrix;
  const Scalar epsilon = ResourceMap::GetAsScalar("FiniteOrthonormalizationAlgorithm-Epsilon");
  if (!(epsilon >= 0.0)) throw InvalidArgumentException(HERE) << "Error: FiniteOrthonormalizationAlgorithm-Epsilon must be >= 0";
  const ExperimentIntegration * experimentRule = dynamic_cast<const ExperimentIntegration *>(effectiveIntegration.getImplementation().get());
  if (experimentRule == nullptr)
  {
    // Gram matrix of the pulled-back functions with respect to the measure
    MatrixImplementation gramImpl(nFuns, nFuns);
    for (UnsignedInteger i = 0; i < nFuns; ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        const Function integrand(GramEvaluation(pulledBack[i], pulledBack[j], measure_));
        const Scalar gij = effectiveIntegration.integrate(integrand, support)[0];
        gramImpl(i, j) = gij;
        gramImpl(j, i) = gij;
      }
    }
    Matrix factorL;
    try
    {
      factorL = gramImpl.computeCholesky();
    }
    catch (const NotSymmetricDefinitePositiveException &)
    {
      throw InvalidArgumentException(HERE) << "Error: the initial functions are linearly dependent";
    }
    const Matrix factorR(factorL.transpose());
    TriangularMatrix Rtri(*factorR.getImplementation(), false);
    RinvMatrix = Rtri.solveLinearSystemInPlace(IdentityMatrix(nFuns));
  }
  else
  {
    // Weighted design matrix on the rule nodes. Nodes either live in the
    // physical support (measure-adapted rule) or on the reference cube
    // (reference rule, mapped below with the density correction).
    const WeightedExperiment ruleExperiment(experimentRule->getWeightedExperiment());
    const Interval ruleRange(ruleExperiment.getDistribution().getRange());
    const Bool physicalNodes = (ruleRange == support);
    if (!physicalNodes)
    {
      const Interval referenceCube(Point(dimension, -1.0), Point(dimension, 1.0));
      if (ruleRange != referenceCube) throw InvalidArgumentException(HERE) << "Error: the experiment range=" << ruleRange << " matches neither the support=" << support << " nor the reference cube " << referenceCube;
    }
    Point weights;
    const Sample nodes(ruleExperiment.generateWithWeights(weights));
    const UnsignedInteger nNodes = nodes.getSize();
    const Point a(support.getLowerBound());
    const Point b(support.getUpperBound());
    const Scalar volume = support.getVolume();
    MatrixImplementation weightedMImpl(nNodes, nFuns);
    for (UnsignedInteger i = 0; i < nNodes; ++i)
    {
      Point physNode(nodes[i]);
      Scalar weight = weights[i];
      if (!physicalNodes)
      {
        for (UnsignedInteger j = 0; j < dimension; ++j)
        {
          physNode[j] = nodes(i, j) * ((b[j] - a[j]) * 0.5) + (a[j] + b[j]) * 0.5;
        }
        weight *= volume * measure_.computePDF(physNode);
      }
      const Scalar sqrtW = std::sqrt(weight);
      for (UnsignedInteger j = 0; j < nFuns; ++j)
      {
        weightedMImpl(i, j) = sqrtW * pulledBack[j](physNode)[0];
      }
    }
    Matrix weightedM(weightedMImpl);
    Matrix R;
    Matrix Q(weightedM.computeQR(R));
    (void)Q;
    for (UnsignedInteger j = 0; j < nFuns; ++j)
    {
      if (!(std::abs(R(j, j)) > epsilon)) throw InvalidArgumentException(HERE) << "Error: the initial functions are linearly dependent";
    }
    TriangularMatrix Rtri(*R.getImplementation(), false);
    RinvMatrix = Rtri.solveLinearSystemInPlace(IdentityMatrix(nFuns));
  }
  MatrixImplementation coeffImpl(nFuns, nFuns);
  orthonormalFunctions_ = FunctionPersistentCollection(nFuns);
  for (UnsignedInteger j = 0; j < nFuns; ++j)
  {
    const Scalar sign = (RinvMatrix(j, j) > 0.0) ? 1.0 : -1.0;
    Point coeffs;
    FunctionCollection funcs;
    for (UnsignedInteger i = 0; i <= j; ++i)
    {
      const Scalar cij = sign * RinvMatrix(i, j);
      if (std::abs(cij) > epsilon)
      {
        coeffs.add(cij);
        funcs.add(initialFunctions_[i]);
      }
      coeffImpl(i, j) = cij;
    }
    LinearCombinationFunction lc(funcs, coeffs);
    orthonormalFunctions_[j] = ComposedFunction(lc, mapping_);
  }
  coefficients_ = SquareMatrix(coeffImpl);
  isAlreadyComputed_ = true;
}

FiniteOrthonormalizationAlgorithm::FunctionPersistentCollection FiniteOrthonormalizationAlgorithm::getOrthonormalFunctions() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<FiniteOrthonormalizationAlgorithm*>(this)->run();
  }
  return orthonormalFunctions_;
}

SquareMatrix FiniteOrthonormalizationAlgorithm::getCoefficients() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<FiniteOrthonormalizationAlgorithm*>(this)->run();
  }
  return coefficients_;
}

Function FiniteOrthonormalizationAlgorithm::getMapping() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<FiniteOrthonormalizationAlgorithm*>(this)->run();
  }
  return mapping_;
}

void FiniteOrthonormalizationAlgorithm::setFunctionsCollection(const FunctionCollection & functions)
{
  const UnsignedInteger size = functions.getSize();
  const UnsignedInteger dimension = measure_.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (functions[i].getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an input dimension=" << functions[i].getInputDimension() << ", expected an input dimension=" << dimension;
    if (functions[i].getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an output dimension=" << functions[i].getOutputDimension() << ", expected an output dimension=1";
  }
  initialFunctions_ = functions;
  isAlreadyComputed_ = false;
}

FiniteOrthonormalizationAlgorithm::FunctionCollection FiniteOrthonormalizationAlgorithm::getFunctionsCollection() const
{
  return initialFunctions_;
}

void FiniteOrthonormalizationAlgorithm::setMeasure(const Distribution & measure)
{
  measure_ = measure;
  isAlreadyComputed_ = false;
}

Distribution FiniteOrthonormalizationAlgorithm::getMeasure() const
{
  return measure_;
}

void FiniteOrthonormalizationAlgorithm::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  integrationAlgorithm_ = IntegrationAlgorithm();
  hasExperiment_ = true;
  hasIntegrationAlgorithm_ = false;
  isAlreadyComputed_ = false;
}

WeightedExperiment FiniteOrthonormalizationAlgorithm::getExperiment() const
{
  return experiment_;
}

void FiniteOrthonormalizationAlgorithm::setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm)
{
  integrationAlgorithm_ = integrationAlgorithm;
  experiment_ = WeightedExperiment();
  hasExperiment_ = false;
  hasIntegrationAlgorithm_ = true;
  isAlreadyComputed_ = false;
}

IntegrationAlgorithm FiniteOrthonormalizationAlgorithm::getIntegrationAlgorithm() const
{
  return integrationAlgorithm_;
}

String FiniteOrthonormalizationAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName();
  return oss;
}

Function FiniteOrthonormalizationAlgorithm::buildMapping() const
{
  const UnsignedInteger dimension = measure_.getDimension();
  const Interval support(measure_.getRange());
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  Description inputNames(dimension);
  Description formulas(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    inputNames[i] = (OSS() << "x" << i);
    formulas[i] = (OSS() << "2.0 * (x" << i << " - (" << a[i] << ")) / (" << b[i] - a[i] << ") - 1.0");
  }
  return SymbolicFunction(inputNames, formulas);
}

IntegrationAlgorithm FiniteOrthonormalizationAlgorithm::buildDefaultIntegration(const Distribution & distribution)
{
  const UnsignedInteger dimension = distribution.getDimension();
  const UnsignedInteger Ndiscretization = ResourceMap::GetAsUnsignedInteger("FiniteOrthonormalizationAlgorithm-DefaultDiscretization");
  if (Ndiscretization == 0) throw InvalidArgumentException(HERE) << "Error: FiniteOrthonormalizationAlgorithm-DefaultDiscretization must be > 0";
  const Indices marginalSizes(dimension, Ndiscretization);
  if (distribution.hasIndependentCopula())
  {
    // The Gauss product rule targets the measure itself
    return ExperimentIntegration(GaussProductExperiment(distribution, marginalSizes));
  }
  // No tensor rule available: integrate against the Lebesgue measure,
  // the density being part of the integrand
  return GaussLegendre(marginalSizes);
}

void FiniteOrthonormalizationAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("initialFunctions_", initialFunctions_);
  adv.saveAttribute("measure_", measure_);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("integrationAlgorithm_", integrationAlgorithm_);
  adv.saveAttribute("hasExperiment_", hasExperiment_);
  adv.saveAttribute("hasIntegrationAlgorithm_", hasIntegrationAlgorithm_);
  adv.saveAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("mapping_", mapping_);
  adv.saveAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

void FiniteOrthonormalizationAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("initialFunctions_", initialFunctions_);
  adv.loadAttribute("measure_", measure_);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("integrationAlgorithm_", integrationAlgorithm_);
  adv.loadAttribute("hasExperiment_", hasExperiment_);
  adv.loadAttribute("hasIntegrationAlgorithm_", hasIntegrationAlgorithm_);
  adv.loadAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("mapping_", mapping_);
  adv.loadAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

END_NAMESPACE_OPENTURNS
