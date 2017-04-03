//                                               -*- C++ -*-
/**
 *  @brief Implementation for Saltelli sensitivity algorithm
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/SaltelliSensitivityAlgorithm.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SaltelliSensitivityAlgorithm);

static const Factory<SaltelliSensitivityAlgorithm> Factory_SaltelliSensitivityAlgorithm;

/* Default constructor */
SaltelliSensitivityAlgorithm::SaltelliSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
{
  // Nothing to do
}

/** Constructor with parameters */
SaltelliSensitivityAlgorithm::SaltelliSensitivityAlgorithm(const NumericalSample & inputDesign,
    const NumericalSample & outputDesign,
    const UnsignedInteger size)
  : SobolIndicesAlgorithmImplementation(inputDesign, outputDesign, size)
{
  // Nothing to do
}

/** Constructor with distribution / model parameters */
SaltelliSensitivityAlgorithm::SaltelliSensitivityAlgorithm(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(distribution, size, model, computeSecondOrder)
{
  // Nothing to do
}

/** Constructor with experiment / model parameters */
SaltelliSensitivityAlgorithm::SaltelliSensitivityAlgorithm(const WeightedExperiment & experiment,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(experiment, model, computeSecondOrder)
{
  // Nothing to do
}

/** Internal method that compute Vi/VTi using a huge sample */
NumericalSample SaltelliSensitivityAlgorithm::computeIndices(const NumericalSample & sample,
    NumericalSample & VTi) const
{
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  const UnsignedInteger size = size_;
  NumericalSample varianceI(outputDimension, inputDimension);
  VTi = NumericalSample(outputDimension, inputDimension);

  // Compute muA = mean(yA)
  const NumericalSample yA(sample, 0, size);
  const NumericalPoint muA(yA.computeMean());

  // Compute cross mean term
  const NumericalPoint crossSquareMean(computeSumDotSamples(sample, size_, 0, size_) / size_);

  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    // yE correspond to the block that start at index (p + 2) * size_
    // For first order indices, compute yE * yB
    const NumericalPoint yEDotyB(computeSumDotSamples(sample, size_, size_, (2 + p) * size_));
    // For total order indices, compute yE * yA
    const NumericalPoint yEDotyA(computeSumDotSamples(sample, size_, 0, (2 + p) * size_));

    for (UnsignedInteger q = 0; q < outputDimension; ++q)
    {
      varianceI[q][p] +=  yEDotyB[q]  / (size - 1.0) -  crossSquareMean[q];
      // Vti = Var - V_{-i}
      VTi[q][p] += muA[q] * muA[q] + referenceVariance_[q] - yEDotyA[q]  / (size - 1.0);
    }
  }
  return varianceI;
}

/* Virtual constructor */
SaltelliSensitivityAlgorithm * SaltelliSensitivityAlgorithm::clone() const
{
  return new SaltelliSensitivityAlgorithm(*this);
}

/* String converter */
String SaltelliSensitivityAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesAlgorithmImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SaltelliSensitivityAlgorithm::save(Advocate & adv) const
{
  SobolIndicesAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void SaltelliSensitivityAlgorithm::load(Advocate & adv)
{
  SobolIndicesAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
