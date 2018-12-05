//                                               -*- C++ -*-
/**
 *  @brief LinearModelAlgorithm implements the linear model
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/LinearModelAlgorithm.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearModelAlgorithm)

static const Factory<LinearModelAlgorithm> Factory_LinearModelAlgorithm;

/* Default constructor */
LinearModelAlgorithm::LinearModelAlgorithm()
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , outputSample_(0, 0)
  , result_()
  , hasRun_(false)
{
  // Nothing to do
}

/* Parameters constructor */
LinearModelAlgorithm::LinearModelAlgorithm(const Sample & inputSample,
    const Sample & outputSample)
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , outputSample_(0, 0)
  , result_()
  , hasRun_(false)
{
  // Check the sample sizes
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In LinearModelAlgorithm::LinearModelAlgorithm, input sample size (" << inputSample.getSize() << ") does not match output sample size (" << outputSample.getSize() << ").";
  // Set samples
  inputSample_ = inputSample;
  outputSample_ = outputSample;
  Collection<Function> functions;
  const Description inputDescription(inputSample_.getDescription());
  functions.add(SymbolicFunction(inputSample_.getDescription(), Description(1, "1")));
  for(UnsignedInteger i = 0; i < inputSample_.getDimension(); ++i)
  {
    functions.add(SymbolicFunction(inputSample_.getDescription(), Description(1, inputDescription[i])));
  }
  basis_ = Basis(functions);
}


/* Parameters constructor */
LinearModelAlgorithm::LinearModelAlgorithm(const Sample & inputSample,
    const Basis & basis,
    const Sample & outputSample)
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , outputSample_(0, 0)
  , result_()
  , hasRun_(false)
{
  // Check the sample sizes
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In LinearModelAlgorithm::LinearModelAlgorithm, input sample size (" << inputSample.getSize() << ") does not match output sample size (" << outputSample.getSize() << ").";
  // Set samples
  inputSample_ = inputSample;
  outputSample_ = outputSample;
  basis_ = basis;
}


/* Virtual constructor */
LinearModelAlgorithm * LinearModelAlgorithm::clone() const
{
  return new LinearModelAlgorithm(*this);
}


/* Perform regression */
void LinearModelAlgorithm::run()
{
  // Do not run again if already computed
  if (hasRun_) return;

  const UnsignedInteger size = inputSample_.getSize();
  const UnsignedInteger p = basis_.getSize();

  // No particular strategy : using the full basis
  Indices indices(basis_.getSize());
  indices.fill();

  // Define the design proxy
  DesignProxy proxy(inputSample_, basis_);

  // Compute using a least squares method
  LeastSquaresMethod algo;
  algo = LeastSquaresMethod::Build(ResourceMap::GetAsString("LinearModelAlgorithm-DecompositionMethod"), proxy, indices);

  // Solve linear system
  Point coefficients(algo.solve(outputSample_.asPoint()));

  // Define the metamodel
  LinearCombinationFunction metaModel(basis_, coefficients);

  // Get the GramInverse
  const Point diagonalGramInverse(algo.getGramInverseDiag());

  // Leverage = diagonal of the Hat matrix
  Point leverages(algo.getHDiag());

  // The design proxy evaluated on the basis function
  Matrix fX(proxy.computeDesign(indices));

  // Description of the basis
  Description coefficientsNames(0);
  for (UnsignedInteger k = 0; k < basis_.getSize(); ++k)
  {
    coefficientsNames.add(basis_[k].__str__());
  }

  // Residual sample
  const Sample residualSample(outputSample_ - metaModel(inputSample_));
  // Sigma2 
  const Point sigma2(residualSample.computeRawMoment(2));

  const Scalar factor = size * sigma2[0] / (size - p);
  Sample standardizedResiduals(size, 1);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    standardizedResiduals(i, 0) = residualSample(i, 0) / std::sqrt(factor * (1.0 - leverages[i]));
  }

  Point cookDistances(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    cookDistances[i] = (1.0 / p) * standardizedResiduals(i, 0) * standardizedResiduals(i, 0) * (leverages[i] / (1.0 - leverages[i]));
  }

  result_ = LinearModelResult(inputSample_, basis_, fX, outputSample_, metaModel,
                              coefficients, basis_.__str__(), coefficientsNames, residualSample,
                              diagonalGramInverse, leverages, cookDistances);

  hasRun_ = true;
}


/* String converter */
String LinearModelAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << ", inputSample=" << inputSample_
      << ", basis=" << basis_
      << ", outputSample=" << outputSample_
      << ", result=" << result_;
  return oss;
}


Sample LinearModelAlgorithm::getInputSample() const
{
  return inputSample_;
}


Basis LinearModelAlgorithm::getBasis() const
{
  return basis_;
}


Sample LinearModelAlgorithm::getOutputSample() const
{
  return outputSample_;
}


LinearModelResult LinearModelAlgorithm::getResult()
{
  run();
  return result_;
}


/* Method save() stores the object through the StorageManager */
void LinearModelAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "hasRun_", hasRun_ );
}

/* Method load() reloads the object from the StorageManager */
void LinearModelAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "hasRun_", hasRun_ );
}

END_NAMESPACE_OPENTURNS