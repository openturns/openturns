//                                               -*- C++ -*-
/**
 *  @brief Implementation for Mauntz-Kucherenko sensitivity algorithm
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

#include "openturns/MauntzKucherenkoSensitivityAlgorithm.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MauntzKucherenkoSensitivityAlgorithm);

static const Factory<MauntzKucherenkoSensitivityAlgorithm> RegisteredFactory;

/* Default constructor */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
{
  // Nothing to do
}

/** Constructor with parameters */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm(const NumericalSample & inputDesign,
                                                                           const NumericalSample & outputDesign,
                                                                           const UnsignedInteger size)
  : SobolIndicesAlgorithmImplementation(inputDesign, outputDesign, size)
{
  // Nothing to do
}

/** Constructor with distribution / model parameters */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm(const Distribution & distribution,
                                                                           const UnsignedInteger size,
                                                                           const NumericalMathFunction & model,
                                                                           const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(distribution, size, model, computeSecondOrder)
{
  // Nothing to do
}

/** Constructor with experiment / model parameters */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm(const WeightedExperiment & experiment,
                                                                           const NumericalMathFunction & model,
                                                                           const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(experiment, model, computeSecondOrder)
{
  // Nothing to do
}

/* Virtual constructor */
MauntzKucherenkoSensitivityAlgorithm * MauntzKucherenkoSensitivityAlgorithm::clone() const
{
  return new MauntzKucherenkoSensitivityAlgorithm(*this);
}

/** Internal method that compute Vi/VTi using a huge sample */
NumericalSample MauntzKucherenkoSensitivityAlgorithm::computeIndices(const NumericalSample & sample,
                                                                     NumericalSample & VTi) const
{
  const UnsignedInteger inputDimension(inputDesign_.getDimension());
  const UnsignedInteger outputDimension(outputDesign_.getDimension());
  const UnsignedInteger size(size_);
  NumericalSample varianceI(outputDimension, inputDimension);
  VTi = NumericalSample(outputDimension, inputDimension);

  // Use reference samples
  // Compute muA = mean(yA)
  const NumericalSample yA(sample, 0, size);
  const NumericalPoint muA(yA.computeMean());

  // Compute crossMean
  const NumericalPoint yADotyB(computeSumDotSamples(sample, size_, 0,  size_));
  // main loop
  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    // yE correspond to the block that start at index (p + 2) * size_
    // For first order indices, compute yE * yB
    const NumericalPoint yEDotyB(computeSumDotSamples(sample, size_, size_, (2 + p) * size_));
    // Compute yE * yA
    const NumericalPoint yEDotyA(computeSumDotSamples(sample, size_, 0, (2 + p) * size_));
    for (UnsignedInteger k = 0; k < size; ++k)
    {
      for (UnsignedInteger q = 0; q < outputDimension; ++q)
      {
        varianceI[q][p] =  (yEDotyB[q] - yADotyB[q]) / (size - 1.0);
        // Vti = Var - V_{-i}
        // \sum_{k} yA[k] * yA[k] - yA[k]*yE[k]
        // yA[k] * yA[k]  = sigma_a^2 + muA^2
        VTi[q][p] = referenceVariance_[q] + (size * muA[q] *  muA[q] - yEDotyA[q])/ (size - 1.0);
      }
    }
  }
  return varianceI;
}

/* String converter */
String MauntzKucherenkoSensitivityAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesAlgorithmImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MauntzKucherenkoSensitivityAlgorithm::save(Advocate & adv) const
{
  SobolIndicesAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void MauntzKucherenkoSensitivityAlgorithm::load(Advocate & adv)
{
  SobolIndicesAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
