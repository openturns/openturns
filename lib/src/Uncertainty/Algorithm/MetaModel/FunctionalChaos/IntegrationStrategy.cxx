//                                               -*- C++ -*-
/**
 *  @brief An implementation of projection strategy as a leastSquares
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IntegrationStrategy.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(IntegrationStrategy)


static const Factory<IntegrationStrategy> Factory_IntegrationStrategy;

/* Default constructor */
IntegrationStrategy::IntegrationStrategy()
  : ProjectionStrategyImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
IntegrationStrategy::IntegrationStrategy(const Distribution & measure)
  : ProjectionStrategyImplementation(measure)
{
  // Nothing to do
}

/* Parameter constructor */
IntegrationStrategy::IntegrationStrategy(const WeightedExperiment & weightedExperiment)
  : ProjectionStrategyImplementation(weightedExperiment)
{
  // Nothing to do
}

/* Parameter constructor */
IntegrationStrategy::IntegrationStrategy(const Distribution & measure,
    const WeightedExperiment & weightedExperiment)
  : ProjectionStrategyImplementation(measure, weightedExperiment)
{
  // Nothing to do
}

/* Parameter constructor */
IntegrationStrategy::IntegrationStrategy(const Sample & inputSample,
    const Point & weights,
    const Sample & outputSample)
  : ProjectionStrategyImplementation(inputSample, weights, outputSample)
{
  // Nothing to do
}

/* Parameter constructor */
IntegrationStrategy::IntegrationStrategy(const Sample & inputSample,
    const Sample & outputSample)
  : ProjectionStrategyImplementation(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample)
{
  // Nothing to do
}

/* Virtual constructor */
IntegrationStrategy * IntegrationStrategy::clone() const
{
  return new IntegrationStrategy(*this);
}

/* String converter */
String IntegrationStrategy::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " experiment=" << weightedExperiment_;
}


struct IntegrationStrategyCoefficientsPolicy
{
  const Point & weightedOutput_;
  const Matrix & designMatrix_;
  const Indices & addedRanks_;
  Point & alpha_;

  IntegrationStrategyCoefficientsPolicy(const Point & weightedOutput,
                                        const Matrix & designMatrix,
                                        const Indices & addedRanks,
                                        Point & alpha)
    : weightedOutput_(weightedOutput)
    , designMatrix_(designMatrix)
    , addedRanks_(addedRanks)
    , alpha_(alpha)
  {
    // Nothing to do
  }

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger j = r.begin(); j != r.end(); ++j)
    {
      const UnsignedInteger indexAdded = addedRanks_[j];
      Scalar result = 0.0;
      MatrixImplementation::const_iterator columnMatrix(designMatrix_.getImplementation()->begin() + indexAdded * designMatrix_.getNbRows());
      for (Point::const_iterator outputSample = weightedOutput_.begin(); outputSample != weightedOutput_.end(); ++outputSample, ++columnMatrix)
      {
        result += (*outputSample) * (*columnMatrix);
      } // i
      alpha_[j] = result;
    } // j
  } // operator()

}; /* end struct IntegrationStrategyCoefficientsPolicy */


/* Compute the components alpha_k_p_ by projecting the model on the partial L2 basis
   For the moment, there is no specific strategy for improving the approximation of
   the L2 integral by a finite sum: the same input sample is used for all the calls
   to this method */
void IntegrationStrategy::computeCoefficients(const Function & function,
    const FunctionCollection & basis,
    const Indices & indices,
    const Indices & addedRanks,
    const Indices & conservedRanks,
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
    LOGINFO("Generate output data");
    inputSample_  = weightedExperiment_.generateWithWeights(weights_);
    outputSample_ = function(inputSample_);
  }
  // check if the proxy is initialized and if the given basis is the one in the proxy
  if (proxy_.getInputSample().getSize() == 0 || !(proxy_.getBasis() == basis))
  {
    LOGINFO(OSS() << "Initialize the proxy, reason=" << (proxy_.getInputSample().getSize() == 0 ? "empty input sample" : "new basis"));
    proxy_ = DesignProxy(inputSample_, basis);
  }
  // First, copy the coefficients that are common with the previous partial basis
  Point alpha(0);
  const UnsignedInteger conservedSize = conservedRanks.getSize();
  for (UnsignedInteger i = 0; i < conservedSize; ++i)
    alpha.add(alpha_k_p_[conservedRanks[i]]);
  // We have to compute the coefficients associated to the added indices
  const UnsignedInteger addedSize = addedRanks.getSize();
  const UnsignedInteger sampleSize = inputSample_.getSize();
  // Second, compute the coefficients of the new basis entries
  const Matrix designMatrix(proxy_.computeDesign(indices));
  Point addedAlpha(addedSize, 0.0);
  Point weightedOutput(sampleSize);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
    weightedOutput[i] = weights_[i] * outputSample_(i, marginalIndex);
  IntegrationStrategyCoefficientsPolicy policy(weightedOutput, designMatrix, addedRanks, addedAlpha);
  TBBImplementation::ParallelFor( 0, addedSize, policy );
  alpha.add(addedAlpha);
  alpha_k_p_ = alpha;
  // The residual is the mean squared error between the model and the meta model
  residual_p_ = 0.0;
  const Point values(designMatrix * alpha_k_p_);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    const Scalar delta = outputSample_(i, marginalIndex) - values[i];
    residual_p_ += delta * delta;
  }
  residual_p_ = sqrt(residual_p_) / sampleSize;
  relativeError_p_ = 0.0;
}


/* Method save() stores the object through the StorageManager */
void IntegrationStrategy::save(Advocate & adv) const
{
  ProjectionStrategyImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void IntegrationStrategy::load(Advocate & adv)
{
  ProjectionStrategyImplementation::load(adv);
}

/* Selection history accessor */
Collection<Indices> IntegrationStrategy::getSelectionHistory(Collection<Point> & coefficientsHistory) const
{
  coefficientsHistory = Collection<Point>();
  return Collection<Indices>();
}


END_NAMESPACE_OPENTURNS
