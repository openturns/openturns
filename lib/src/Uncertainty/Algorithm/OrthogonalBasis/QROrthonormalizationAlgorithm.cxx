//                                               -*- C++ -*-
/**
 *  @brief QR-based orthonormalization algorithm
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
#include "openturns/QROrthonormalizationAlgorithm.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Interval.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QROrthonormalizationAlgorithm)

static const Factory<QROrthonormalizationAlgorithm> Factory_QROrthonormalizationAlgorithm;

QROrthonormalizationAlgorithm::QROrthonormalizationAlgorithm()
  : PersistentObject()
  , initialFunctions_()
  , measure_()
  , experiment_()
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
}

QROrthonormalizationAlgorithm::QROrthonormalizationAlgorithm(const FunctionCollection & functions,
    const Distribution & measure,
    const WeightedExperiment & experiment)
  : PersistentObject()
  , initialFunctions_(functions)
  , measure_(measure)
  , experiment_(experiment)
  , orthonormalFunctions_()
  , coefficients_()
  , mapping_()
  , isAlreadyComputed_(false)
{
}

QROrthonormalizationAlgorithm * QROrthonormalizationAlgorithm::clone() const
{
  return new QROrthonormalizationAlgorithm(*this);
}

void QROrthonormalizationAlgorithm::run()
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
  const Point a(support.getLowerBound());
  const Point b(support.getUpperBound());
  WeightedExperiment effectiveExperiment(experiment_);
  if (effectiveExperiment.getDistribution().getDimension() != dimension)
  {
    effectiveExperiment = buildDefaultExperiment(measure_);
  }
  Point weights;
  Sample nodes(effectiveExperiment.generateWithWeights(weights));
  const UnsignedInteger nNodes = nodes.getSize();
  Sample adaptedNodes(nNodes, dimension);
  const Scalar volume = support.getVolume();
  for (UnsignedInteger i = 0; i < nNodes; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      adaptedNodes(i, j) = nodes(i, j) * ((b[j] - a[j]) * 0.5) + (a[j] + b[j]) * 0.5;
    }
  }
  for (UnsignedInteger i = 0; i < nNodes; ++i)
  {
    weights[i] *= volume * measure_.computePDF(adaptedNodes[i]);
  }
  MatrixImplementation weightedMImpl(nNodes, nFuns);
  for (UnsignedInteger i = 0; i < nNodes; ++i)
  {
    const Scalar sqrtW = sqrt(weights[i]);
    for (UnsignedInteger j = 0; j < nFuns; ++j)
    {
      weightedMImpl(i, j) = sqrtW * initialFunctions_[j](nodes[i])[0];
    }
  }
  Matrix weightedM(weightedMImpl);
  Matrix R;
  Matrix Q(weightedM.computeQR(R));
  (void)Q;
  TriangularMatrix Rtri(*R.getImplementation(), false);
  Matrix RinvMatrix(Rtri.solveLinearSystemInPlace(IdentityMatrix(nFuns)));
  const Scalar epsilon = ResourceMap::HasKey("QROrthonormalizationAlgorithm-Epsilon")
    ? ResourceMap::GetAsScalar("QROrthonormalizationAlgorithm-Epsilon")
    : 1.0e-11;
  MatrixImplementation coeffImpl(nFuns, nFuns);
  mapping_ = buildMapping();
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

QROrthonormalizationAlgorithm::FunctionPersistentCollection QROrthonormalizationAlgorithm::getOrthonormalFunctions() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<QROrthonormalizationAlgorithm*>(this)->run();
  }
  return orthonormalFunctions_;
}

SquareMatrix QROrthonormalizationAlgorithm::getCoefficients() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<QROrthonormalizationAlgorithm*>(this)->run();
  }
  return coefficients_;
}

Function QROrthonormalizationAlgorithm::getMapping() const
{
  if (!isAlreadyComputed_)
  {
    const_cast<QROrthonormalizationAlgorithm*>(this)->run();
  }
  return mapping_;
}

void QROrthonormalizationAlgorithm::setFunctionsCollection(const FunctionCollection & functions)
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

QROrthonormalizationAlgorithm::FunctionCollection QROrthonormalizationAlgorithm::getFunctionsCollection() const
{
  return initialFunctions_;
}

void QROrthonormalizationAlgorithm::setMeasure(const Distribution & measure)
{
  measure_ = measure;
  isAlreadyComputed_ = false;
}

Distribution QROrthonormalizationAlgorithm::getMeasure() const
{
  return measure_;
}

void QROrthonormalizationAlgorithm::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  isAlreadyComputed_ = false;
}

WeightedExperiment QROrthonormalizationAlgorithm::getExperiment() const
{
  return experiment_;
}

String QROrthonormalizationAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName();
  return oss;
}

Function QROrthonormalizationAlgorithm::buildMapping() const
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

WeightedExperiment QROrthonormalizationAlgorithm::buildDefaultExperiment(const Distribution & distribution)
{
  const UnsignedInteger dimension = distribution.getDimension();
  const UnsignedInteger Ndiscretization = ResourceMap::HasKey("QROrthonormalizationAlgorithm-DefaultDiscretization")
    ? ResourceMap::GetAsUnsignedInteger("QROrthonormalizationAlgorithm-DefaultDiscretization")
    : 128;
  const Indices marginalSizes(dimension, Ndiscretization);
  Collection<Distribution> uniforms(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    uniforms[i] = Uniform();
  }
  return GaussProductExperiment(JointDistribution(uniforms), marginalSizes);
}

void QROrthonormalizationAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("initialFunctions_", initialFunctions_);
  adv.saveAttribute("measure_", measure_);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("mapping_", mapping_);
  adv.saveAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

void QROrthonormalizationAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("initialFunctions_", initialFunctions_);
  adv.loadAttribute("measure_", measure_);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("orthonormalFunctions_", orthonormalFunctions_);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("mapping_", mapping_);
  adv.loadAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

END_NAMESPACE_OPENTURNS
