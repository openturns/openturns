
//                                               -*- C++ -*-
/**
 *  @brief Implementation for Sobol indices with Jansen formula
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

#include "openturns/JansenSobolIndices.hxx"
#include "openturns/SobolIndicesImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(JansenSobolIndices)

static const Factory<JansenSobolIndices> Factory_JansenSobolIndices;

/* Default constructor */
JansenSobolIndices::JansenSobolIndices()
  : SobolIndicesImplementation()
{
 // Nothing to do
}

/* Default constructor */
JansenSobolIndices::JansenSobolIndices(const UnsignedInteger modelInputDimension,
                                           const UnsignedInteger modelOutputDimension)
  : SobolIndicesImplementation(modelInputDimension, modelOutputDimension)
{
 // Nothing to do
}

/* Virtual constructor */
JansenSobolIndices * JansenSobolIndices::clone() const
{
  return new JansenSobolIndices(*this);
}

void JansenSobolIndices::computeIndices(const Sample & inputSample)
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
//   varianceA_ = IterativeVariance(modelOutputDimension_);
  squaredSumyBMinusyE_ = Sample(modelInputDimension_, modelOutputDimension_);
  squaredSumyAMinusyE_ = Sample(modelInputDimension_, modelOutputDimension_);

  // Use reference samples
  // Reference sample yA
  const Sample yA(inputSample, 0, size);
  referenceMean_ = yA.computeMean();
  referenceVariance_ = yA.computeVariance();
//   varianceA_.increment(yA);
//   referenceVariance_ = varianceA_.getVariance();
//   referenceMean_ = varianceA_.getMean();
  // Reference sample yB
  const Sample yB(inputSample, size, 2 * size);

  // main loop
  for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
  {

    // Compute yE - yB / yE - yA
    // Copy elements of yE
    Sample yEMinusyB(inputSample, (2 + p) * size, (3 + p) * size);
    // Copy in yEMinusyA
    Sample yEMinusyA(yEMinusyB);
    // Remove yB from yEMinusyB
    yEMinusyB -= yB;
    // Remove yA from yEMinusyA
    yEMinusyA -= yA;
    // Sum of squared elements
    squaredSumyBMinusyE_[p] = (computeSumDotSamples(yEMinusyB, yEMinusyB));
    // Sum of squared elements
    squaredSumyAMinusyE_[p] = (computeSumDotSamples(yEMinusyA, yEMinusyA));

    for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
    {
      firstOrderValues_(q, p) =  1.0 - ((squaredSumyBMinusyE_(p, q) / (2.0 * size - 1.0)) / referenceVariance_[q]);
      totalOrderValues_(q, p) = (squaredSumyAMinusyE_(p, q) / (2.0 * size - 1.0)) / referenceVariance_[q];
    }
  }
  iteration_ = size;
}

void JansenSobolIndices::incrementIndices(const Sample & inputSample)
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
    squaredSumyBMinusyE_ = Sample(modelInputDimension_, modelOutputDimension_);
    squaredSumyAMinusyE_ = Sample(modelInputDimension_, modelOutputDimension_);
    if (iteration_ != 0)
      LOGWARN(OSS() << "Already computed indices, previous values will be lost");
    iteration_ = 0;
  }

  const UnsignedInteger size = inputSample.getSize() / (modelInputDimension_ + 2);

//   Sample yA(inputSample, 0, size);
//   Sample yB(inputSample, size, 2 * size);

  for (UnsignedInteger iter = 0; iter < size; ++iter)
  {
    Point yA = inputSample[iter];
    Point yB = inputSample[size + iter];
    iteration_ += 1;
    varianceA_.increment(yA);

    referenceVariance_ = varianceA_.getVariance();
    referenceMean_ = varianceA_.getMean();

    for (UnsignedInteger q = 0; q < modelOutputDimension_; ++ q)
      if (!(referenceVariance_[q] > 0.0) && iteration_ > 2)
        throw InvalidArgumentException(HERE) << "Null output sample variance";
    for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
    {
      Point yE = inputSample[(2 + p) * size + iter];
      // Copy elements of yE
      Point yEMinusyB(yE);
      // Remove yB from yEMinusyB
        yEMinusyB -= yB;
      // square
      for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
      {
        squaredSumyBMinusyE_(p, q) += yEMinusyB[q] * yEMinusyB[q];
      }
      // Copy elements of yE in yEMinusyA
      Point yEMinusyA(yE);
      // Remove yA from yEMinusyA
      yEMinusyA -= yA;
      // square and add
      for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
      {
        squaredSumyAMinusyE_(p, q) += yEMinusyA[q] * yEMinusyA[q];
      }
    }
  }

  if (iteration_ > 2)
  {
    for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
    {
      for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
      {
        firstOrderValues_(q, p) =  1.0 - ((squaredSumyBMinusyE_(p, q) / (2.0 * iteration_ - 1.0)) / referenceVariance_[q]);
        // Vti = Var - V_{-i}
        totalOrderValues_(q, p) = (squaredSumyAMinusyE_(p, q) / (2.0 * iteration_ - 1.0)) / referenceVariance_[q];
      }
    }
  }
}

Interval JansenSobolIndices::getFirstOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Jansen/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

Interval JansenSobolIndices::getTotalOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Jansen/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

/* String converter */
String JansenSobolIndices::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void JansenSobolIndices::save(Advocate & adv) const
{
  SobolIndicesImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void JansenSobolIndices::load(Advocate & adv)
{
  SobolIndicesImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS


