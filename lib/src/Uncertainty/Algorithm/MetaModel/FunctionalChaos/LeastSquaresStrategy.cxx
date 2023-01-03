//                                               -*- C++ -*-
/**
 *  @brief An implementation of projection strategy as a leastSquares
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LeastSquaresStrategy.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ApproximationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LeastSquaresStrategy)

static const Factory<LeastSquaresStrategy> Factory_LeastSquaresStrategy;


/* Default constructor */
LeastSquaresStrategy::LeastSquaresStrategy(const ApproximationAlgorithmImplementationFactory & factory)
  : ProjectionStrategyImplementation()
  , p_approximationAlgorithmImplementationFactory_( factory.clone() )
{
  // Nothing to do
}

/* Parameter constructor */
LeastSquaresStrategy::LeastSquaresStrategy(const Distribution & measure,
    const ApproximationAlgorithmImplementationFactory & factory)
  : ProjectionStrategyImplementation(measure)
  , p_approximationAlgorithmImplementationFactory_( factory.clone() )
{
  // Nothing to do
}

/* Parameter constructor */
LeastSquaresStrategy::LeastSquaresStrategy(const WeightedExperiment & weightedExperiment,
    const ApproximationAlgorithmImplementationFactory & factory)
  : ProjectionStrategyImplementation(weightedExperiment)
  , p_approximationAlgorithmImplementationFactory_( factory.clone() )
{
  // Nothing to do
}

/* Parameter constructor */
LeastSquaresStrategy::LeastSquaresStrategy(const Distribution & measure,
    const WeightedExperiment & weightedExperiment,
    const ApproximationAlgorithmImplementationFactory & factory)
  : ProjectionStrategyImplementation(measure, weightedExperiment)
  , p_approximationAlgorithmImplementationFactory_( factory.clone() )
{
  // Nothing to do
}

/* Parameter constructor */
LeastSquaresStrategy::LeastSquaresStrategy(const Sample & inputSample,
    const Point & weights,
    const Sample & outputSample,
    const ApproximationAlgorithmImplementationFactory & factory)
  : ProjectionStrategyImplementation(inputSample, weights, outputSample)
  , p_approximationAlgorithmImplementationFactory_( factory.clone() )
{
  // Nothing to do
}

/* Parameter constructor */
LeastSquaresStrategy::LeastSquaresStrategy(const Sample & inputSample,
    const Sample & outputSample,
    const ApproximationAlgorithmImplementationFactory & factory)
  : ProjectionStrategyImplementation(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample)
  , p_approximationAlgorithmImplementationFactory_( factory.clone() )
{
  // Nothing to do
}

/* Virtual constructor */
LeastSquaresStrategy * LeastSquaresStrategy::clone() const
{
  return new LeastSquaresStrategy(*this);
}

/* String converter */
String LeastSquaresStrategy::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " experiment=" << weightedExperiment_;
}


/* Compute the components alpha_k_p_ by projecting the model on the partial L2 basis
   For the moment, there is no specific strategy for improving the approximation of
   the L2 integral by a finite sum: the same input sample is used for all the calls
   to this method */
void LeastSquaresStrategy::computeCoefficients(const Function & function,
    const FunctionCollection & basis,
    const Indices & indices,
    const Indices &,
    const Indices &,
    const Indices &,
    const UnsignedInteger marginalIndex)
{
  // Check if the marginal index is not compatible with the function output dimension
  if (marginalIndex >= function.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the marginal index is too large with respect to the function output dimension.";
  // If the input sample has not yet been generated
  // we have to generate it and to evaluate the model
  // upon this sample
  if (inputSample_.getSize() == 0)
  {
    LOGINFO("Generate the weighted DOE");
    inputSample_  = weightedExperiment_.generateWithWeights(weights_);
    LOGINFO("Evaluate the model on the DOE");
    outputSample_ = function(inputSample_);
  }

  // check if the proxy is initialized and if the given basis is the one in the proxy
  if (proxy_.getInputSample().getSize() == 0 || !(proxy_.getBasis() == basis))
  {
    LOGINFO(OSS() << "Initialize the proxy, reason=" << (proxy_.getInputSample().getSize() == 0 ? "empty input sample" : "new basis"));
    proxy_ = DesignProxy(inputSample_, basis);
  }

  // Run the Approximation Algorithm
  LOGINFO("Generate the approximation algorithm");
  ApproximationAlgorithm approximationAlgorithm(p_approximationAlgorithmImplementationFactory_->build(inputSample_, outputSample_.getMarginal(marginalIndex), weights_, basis, indices));
  LOGINFO("Run the approximation algorithm");
  approximationAlgorithm.run(proxy_);
  LOGINFO("Exploit the approximation");
  alpha_k_p_ = approximationAlgorithm.getCoefficients();
  residual_p_ = approximationAlgorithm.getResidual();
  relativeError_p_ = approximationAlgorithm.getRelativeError();
}


/* Method save() stores the object through the StorageManager */
void LeastSquaresStrategy::save(Advocate & adv) const
{
  ProjectionStrategyImplementation::save(adv);
  adv.saveAttribute( "p_approximationAlgorithmImplementationFactory_", *p_approximationAlgorithmImplementationFactory_ );
}


/* Method load() reloads the object from the StorageManager */
void LeastSquaresStrategy::load(Advocate & adv)
{
  ProjectionStrategyImplementation::load(adv);
  ApproximationAlgorithmImplementationFactory approximationAlgorithmImplementationFactory;
  adv.loadAttribute( "p_approximationAlgorithmImplementationFactory_", approximationAlgorithmImplementationFactory );
  p_approximationAlgorithmImplementationFactory_ = approximationAlgorithmImplementationFactory.clone();
}


END_NAMESPACE_OPENTURNS
