
//                                               -*- C++ -*-
/**
 *  @brief Implementation for Sobol indices with Martinez formula
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

#include "openturns/MartinezSobolIndices.hxx"
#include "openturns/SobolIndicesImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MartinezSobolIndices)

static const Factory<MartinezSobolIndices> Factory_MartinezSobolIndices;

/* Default constructor */
MartinezSobolIndices::MartinezSobolIndices()
  : SobolIndicesImplementation()
{
 // Nothing to do
}

/* Default constructor */
MartinezSobolIndices::MartinezSobolIndices(const UnsignedInteger modelInputDimension,
                                           const UnsignedInteger modelOutputDimension)
  : SobolIndicesImplementation(modelInputDimension, modelOutputDimension)
{
 // Nothing to do
}

/* Virtual constructor */
MartinezSobolIndices * MartinezSobolIndices::clone() const
{
  return new MartinezSobolIndices(*this);
}

void MartinezSobolIndices::computeIndices(const Sample & inputSample)
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
  Sample yA(inputSample, 0, size);
  referenceMean_ = yA.computeMean();
  referenceVariance_ = yA.computeVariance();
  const Point sigmaA(yA.computeStandardDeviationPerComponent());
  for (UnsignedInteger j = 0; j < modelOutputDimension_; ++ j)
    if (!(sigmaA[j] > 0.0))
      throw InvalidArgumentException(HERE) << "Null output sample variance";

  // center sample yA
  yA -= referenceMean_;
  yA /= sigmaA;
  // Reference sample yB
  Sample yB(inputSample, size, 2 * size);
  const Point muB(yB.computeMean());
  const Point sigmaB(yB.computeStandardDeviationPerComponent());
  for (UnsignedInteger j = 0; j < modelOutputDimension_; ++ j)
    if (!(sigmaB[j] > 0.0))
      throw InvalidArgumentException(HERE) << "Null output sample variance";

  // center-reduce sample yB
  yB -= muB;
  yB /= sigmaB;

  for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
  {
    Sample yE(inputSample, (2 + p) * size, (3 + p) * size);
    const Point muE(yE.computeMean());
    const Point sigmaE(yE.computeStandardDeviationPerComponent());
    for (UnsignedInteger j = 0; j < modelOutputDimension_; ++ j)
      if (!(sigmaE[j] > 0.0))
        throw InvalidArgumentException(HERE) << "Null output sample variance";

    // center-reduce sample yE
    yE -= muE;
    yE /= sigmaE;
    // Compute yE * yB
    const Point yEDotyB(computeSumDotSamples(yE, yB));
    // Compute yE * yA
    const Point yEDotyA(computeSumDotSamples(yE, yA));
    for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
    {
      // Compute rho(yB, yE) with rho : Pearson correlation
      // Si = rho(yB, yE) thus Vi = Si * V
      firstOrderValues_(q, p) = yEDotyB[q] / (size - 1.0);
      // STi = 1 - rho(yA, yE),  Vti = V -varianceTI thus varianceTI = rho(yA, yE) * var(yA)
      totalOrderValues_(q, p) = 1 - yEDotyA[q] / (size - 1.0);
    }
  }
  iteration_ = size;
}

void MartinezSobolIndices::incrementIndices(const Sample & inputSample)
{
  if (modelInputDimension_ < 2 || modelOutputDimension_ < 1)
    throw InvalidArgumentException(HERE) << "Sobol indices model dimensions not initialized";

  if (inputSample.getSize() % (modelInputDimension_ + 2) != 0 || inputSample.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Sample size does not match model input dimension";

  if (inputSample.getDimension() != modelOutputDimension_)
    throw InvalidArgumentException(HERE) << "Sample dimension does not match model output dimension";

  if (variances_.getSize() == 0)
  {
    variances_ = PersistentCollection<IterativeVariance>(modelInputDimension_ + 2, IterativeVariance(modelOutputDimension_));
    covarianceI_ = Sample(modelInputDimension_, modelOutputDimension_);
    covarianceTI_ = Sample(modelInputDimension_, modelOutputDimension_);
    if (iteration_ != 0)
      LOGWARN(OSS() << "Already computed indices, previous values will be lost");
  }

  const UnsignedInteger size = inputSample.getSize() / (modelInputDimension_ + 2);

//   Sample yA(inputSample, 0, size);
//   Sample yB(inputSample, size, 2 * size);

  for (UnsignedInteger iter = 0; iter < size; ++iter)
  {
    Point yA = inputSample[iter];
    Point yB = inputSample[size + iter];
    iteration_ += 1;
    variances_[0].increment(yA);
    for (UnsignedInteger q = 0; q < modelOutputDimension_; ++ q)
      if (!(variances_[0].getVariance()[q] > 0.0) && iteration_ > 1)
        throw InvalidArgumentException(HERE) << "Null output sample variance";
    variances_[1].increment(yB);
    for (UnsignedInteger q = 0; q < modelOutputDimension_; ++ q)
      if (!(variances_[1].getVariance()[q] > 0.0) && iteration_ > 1)
        throw InvalidArgumentException(HERE) << "Null output sample variance";
    for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
    {
      Point yE = inputSample[(2 + p) * size + iter];
      variances_[2 + p].increment(yE);
      if (iteration_ > 1)
      {
        for (UnsignedInteger q = 0; q < modelOutputDimension_; ++ q)
          if (!(variances_[2 + p].getVariance()[q] > 0.0))
            throw InvalidArgumentException(HERE) << "Null output sample variance";
        covarianceI_[p] *= (Scalar)(iteration_ - 1) / (Scalar)iteration_;
        covarianceTI_[p] *= (Scalar)(iteration_ - 1) / (Scalar)iteration_;
        for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
        {
          covarianceI_(p, q) += (yB[q] - variances_[1].getMean()[q]) * (yE[q] - variances_[2 + p].getMean()[q])  / (Scalar)(iteration_ - 1);
          covarianceTI_(p, q) += (yA[q] - variances_[0].getMean()[q]) * (yE[q] - variances_[2 + p].getMean()[q])  / (Scalar)(iteration_ - 1);
        }
      }
    }
  }

  if (iteration_ > 1)
  {
    for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
    {
      for (UnsignedInteger q = 0; q < modelOutputDimension_; ++q)
      {
        firstOrderValues_(q, p) = covarianceI_(p, q) / (variances_[1].getStandardDeviation()[q] * variances_[2 + p].getStandardDeviation()[q]);
        totalOrderValues_(q, p) = 1.0 - covarianceTI_(p, q) / (variances_[0].getStandardDeviation()[q] * variances_[2 + p].getStandardDeviation()[q]);
      }
    }
  }
  referenceVariance_ = variances_[0].getVariance();
  referenceMean_ = variances_[0].getMean();
}

Interval MartinezSobolIndices::getFirstOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

Interval MartinezSobolIndices::getTotalOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

/* String converter */
String MartinezSobolIndices::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MartinezSobolIndices::save(Advocate & adv) const
{
  SobolIndicesImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void MartinezSobolIndices::load(Advocate & adv)
{
  SobolIndicesImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS


