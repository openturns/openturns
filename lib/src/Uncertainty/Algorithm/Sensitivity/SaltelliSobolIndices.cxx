
//                                               -*- C++ -*-
/**
 *  @brief Implementation for Sobol indices with Saltelli formula
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

#include "openturns/SaltelliSobolIndices.hxx"
#include "openturns/SobolIndicesImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SaltelliSobolIndices)

static const Factory<SaltelliSobolIndices> Factory_SaltelliSobolIndices;

/* Default constructor */
SaltelliSobolIndices::SaltelliSobolIndices()
  : SobolIndicesImplementation()
{
  // Nothing to do
}

/* Default constructor */
SaltelliSobolIndices::SaltelliSobolIndices(const UnsignedInteger modelInputDimension,
                                           const UnsignedInteger modelOutputDimension)
  : SobolIndicesImplementation(modelInputDimension, modelOutputDimension)
{
  // Nothing to do
}

/* Virtual constructor */
SaltelliSobolIndices * SaltelliSobolIndices::clone() const
{
  return new SaltelliSobolIndices(*this);
}

void SaltelliSobolIndices::computeIndices(const Sample & inputSample)
{
  if (modelInputDimension_ < 2 || modelOutputDimension_ < 1)
    throw InvalidArgumentException(HERE) << "Sobol indices model dimensions not initialized";

  if (inputSample.getSize() % (modelInputDimension_ + 2) != 0 || inputSample.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Sample size does not match model input dimension";

  if (inputSample.getDimension() != modelOutputDimension_)
    throw InvalidArgumentException(HERE) << "Sample dimension does not match model output dimension";

  if (iteration_ != 0)
    LOGWARN(OSS() << "Already computed indices, previous values will be lost");

  const UnsignedInteger size = inputSample.getSize() / (modelInputDimension_ + 2);

  // Use reference samples
  // Reference sample yA
  const Sample yA(inputSample, 0, size);
  referenceMean_ = yA.computeMean();
  referenceVariance_ = yA.computeVariance();
  yEDotyA_ = Sample(modelInputDimension_, modelOutputDimension_);
  yEDotyB_ = Sample(modelInputDimension_, modelOutputDimension_);

  // Compute muB
  const Sample yB(inputSample, size, 2 * size);
  muB_ = Point(yB.computeMean());

  // main loop
  for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
  {
    // yE correspond to the block that start at index (p + 2) * size
    // For first order indices, compute yE * yB
    yEDotyB_[p] = computeSumDotSamples(inputSample, size, size, (2 + p) * size);
    // Compute yE * yA
    yEDotyA_[p] = computeSumDotSamples(inputSample, size, 0, (2 + p) * size);

    for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
    {
      firstOrderValues_(q, p) =  (yEDotyB_(p, q) / (size - 1.0) - referenceMean_[q] * muB_[q]) / referenceVariance_[q];
      totalOrderValues_(q, p) = 1 + (referenceMean_[q] * referenceMean_[q] - yEDotyA_(p, q)  / (size - 1.0)) / referenceVariance_[q];
    }
  }
  iteration_ = size;
}

void SaltelliSobolIndices::incrementIndices(const Sample & inputSample)
{
  if (modelInputDimension_ < 2 || modelOutputDimension_ < 1)
    throw InvalidArgumentException(HERE) << "Sobol indices model dimensions not initialized";

  if (inputSample.getSize() % (modelInputDimension_ + 2) != 0 || inputSample.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Sample size does not match model input dimension";

  if (inputSample.getDimension() != modelOutputDimension_)
    throw InvalidArgumentException(HERE) << "Sample dimension does not match model output dimension";

  if (varianceA_.getIteration() == 0)
  {
    varianceA_ = IterativeVariance(modelOutputDimension_);
    yEDotyA_ = Sample(modelInputDimension_, modelOutputDimension_);
    yEDotyB_ = Sample(modelInputDimension_, modelOutputDimension_);
    muB_ = Point(modelOutputDimension_);
    if (iteration_ != 0)
      LOGWARN(OSS() << "Already computed indices, previous values will be lost");
    iteration_ = 0;
  }

  const UnsignedInteger size = inputSample.getSize() / (modelInputDimension_ + 2);

  for (UnsignedInteger iter = 0; iter < size; ++iter)
  {
    Point yA = inputSample[iter];
    Point yB = inputSample[size + iter];
    iteration_ += 1;
    varianceA_.increment(yA);
    for (UnsignedInteger q = 0; q < modelOutputDimension_; ++ q)
    {
      if (!(varianceA_.getVariance()[q] > 0.0) && iteration_ > 2)
        throw InvalidArgumentException(HERE) << "Null output sample variance";
      muB_[q] = muB_[q] + (yB[q] - muB_[q])/iteration_;
    }
    for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
    {
      Point yE = inputSample[(2 + p) * size + iter];
      for (UnsignedInteger q = 0; q < modelOutputDimension_; ++ q)
      {
        yEDotyA_(p, q) += yA[q]*yE[q];
        yEDotyB_(p, q) += yB[q]*yE[q];
      }
    }
  }

  referenceVariance_ = varianceA_.getVariance();
  referenceMean_ = varianceA_.getMean();

  if (iteration_ > 2)
  {
    for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
    {
      for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
      {
        firstOrderValues_(q, p) =  (yEDotyB_(p, q) / (iteration_ - 1.0) - referenceMean_[q] * muB_[q]) / referenceVariance_[q];
        totalOrderValues_(q, p) = 1 + (referenceMean_[q] * referenceMean_[q] - yEDotyA_(p, q)  / (iteration_ - 1.0)) / referenceVariance_[q];
      }
    }
  }
}

Interval SaltelliSobolIndices::getFirstOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Saltelli/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

Interval SaltelliSobolIndices::getTotalOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Saltelli/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

/* String converter */
String SaltelliSobolIndices::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SaltelliSobolIndices::save(Advocate & adv) const
{
  SobolIndicesImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void SaltelliSobolIndices::load(Advocate & adv)
{
  SobolIndicesImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS


