//                                               -*- C++ -*-
/**
 * @brief HSICEstimator implements the sensivity indices based on HSIC
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HSICEstimator.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimator)

/** Constructor */
HSICEstimator::HSICEstimator(): TypedInterfaceObject<HSICEstimatorImplementation>(new HSICEstimatorImplementation())
{
  // Nothing to do
}

/** Default constructor */
HSICEstimator::HSICEstimator(const HSICEstimatorImplementation & implementation)
  : TypedInterfaceObject<HSICEstimatorImplementation>(implementation.clone())
{
  // Nothing to do
}

/** Set the number of permutation used */
void HSICEstimator::setPermutationSize(const UnsignedInteger B)
{
  getImplementation()->setPermutationSize( B );
}

/** Get the number of permutation used */
UnsignedInteger HSICEstimator::getPermutationSize() const
{
  return getImplementation()->getPermutationSize();
}

/** Get the HSIC indices */
Point HSICEstimator::getHSICIndices() const
{
  return getImplementation()->getHSICIndices();
}

/** Get the R2-HSIC indices */
Point HSICEstimator::getR2HSICIndices() const
{
  return getImplementation()->getR2HSICIndices();
}

/** Get the p-values by permutation */
Point HSICEstimator::getPValuesPermutation() const
{
  return getImplementation()->getPValuesPermutation();
}

/** Draw the HSIC indices */
Graph HSICEstimator::drawHSICIndices() const
{
  return getImplementation()->drawHSICIndices();
}

/** Draw the R2HSIC indices */
Graph HSICEstimator::drawR2HSICIndices() const
{
  return getImplementation()->drawR2HSICIndices();
}

/** Draw the p-values obtained with permutation */
Graph HSICEstimator::drawPValuesPermutation() const
{
  return getImplementation()->drawPValuesPermutation();
}

/** Get the covariance models */
HSICEstimatorImplementation::CovarianceModelCollection HSICEstimator::getCovarianceList() const
{
  return getImplementation()->getCovarianceList();
}

/** Set the covariance models */
void HSICEstimator::setCovarianceList(const CovarianceModelCollection & coll)
{
  getImplementation()->setCovarianceList(coll);
}

/** Get the input sample */
Sample HSICEstimator::getInputSample() const
{
  return getImplementation()->getInputSample();
}

/** Set the input sample */
void HSICEstimator::setInputSample(const Sample & inputSample)
{
  getImplementation()->setInputSample(inputSample);
}

/** Get the output sample */
Sample HSICEstimator::getOutputSample() const
{
  return getImplementation()->getOutputSample();
}

/** Set the output sample */
void HSICEstimator::setOutputSample(const Sample & outputSample)
{
  getImplementation()->setOutputSample(outputSample);
}

/** Get the dimension of the indices: the number of marginals */
UnsignedInteger HSICEstimator::getDimension() const
{
  return getImplementation()->getDimension();
}

/** Get the size of the study sample */
UnsignedInteger HSICEstimator::getSize() const
{
  return getImplementation()->getSize();
}

/** Get the underlying estimator: biased or unbiased*/
HSICStat HSICEstimator::getEstimator() const
{
  return getImplementation()->getEstimator();
}

END_NAMESPACE_OPENTURNS
