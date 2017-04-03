//                                               -*- C++ -*-
/**
 *  @brief Implementation for Jansen sensitivity algorithm
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

#include "openturns/JansenSensitivityAlgorithm.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(JansenSensitivityAlgorithm);

static const Factory<JansenSensitivityAlgorithm> Factory_JansenSensitivityAlgorithm;

/* Default constructor */
JansenSensitivityAlgorithm::JansenSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
{
  // Nothing to do
}

/** Constructor with parameters */
JansenSensitivityAlgorithm::JansenSensitivityAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
  : SobolIndicesAlgorithmImplementation(inputDesign, outputDesign, size)
{
  // Nothing to do
}

/** Constructor with distribution / model parameters */
JansenSensitivityAlgorithm::JansenSensitivityAlgorithm(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(distribution, size, model, computeSecondOrder)
{
  // Nothing to do
}


/** Constructor with experiment / model parameters */
JansenSensitivityAlgorithm::JansenSensitivityAlgorithm(const WeightedExperiment & experiment,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(experiment, model, computeSecondOrder)
{
  // Nothing to do
}

/* Virtual constructor */
JansenSensitivityAlgorithm * JansenSensitivityAlgorithm::clone() const
{
  return new JansenSensitivityAlgorithm(*this);
}

/** Internal method that compute Vi/VTi using a huge sample */
Sample JansenSensitivityAlgorithm::computeIndices(const Sample & sample,
    Sample & VTi) const
{
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  const UnsignedInteger size = size_;
  Sample varianceI(outputDimension, inputDimension);
  VTi = Sample(outputDimension, inputDimension);

  // Use reference samples
  // Reference sample yA
  const Sample yA(sample, 0, size);
  // Reference sample yB
  const Sample yB(sample, size, 2 * size);

  // main loop
  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {

    // Compute yE - yB / yE - yA
    // Copy elements of yE
    Sample yEMinusyB(sample, (2 + p) * size, (3 + p) * size);
    // Copy in yEMinusyA
    Sample yEMinusyA(yEMinusyB);
    // Remove yB from yEMinusyB
    yEMinusyB -= yB;
    // Remove yA from yEMinusyA
    yEMinusyA -= yA;
    // Sum of squared elements
    const NumericalPoint squaredSumyBMinusyE(computeSumDotSamples(yEMinusyB, yEMinusyB));
    // Sum of squared elements
    const NumericalPoint squaredSumyAMinusyE(computeSumDotSamples(yEMinusyA, yEMinusyA));

    for (UnsignedInteger q = 0; q < outputDimension; ++q)
    {
      varianceI[q][p] =  -squaredSumyBMinusyE[q] / (2.0 * size - 1.0) +  referenceVariance_[q];
      // Vti = Var - V_{-i}
      VTi[q][p] = squaredSumyAMinusyE[q] / (2.0 * size - 1.0);
    }
  }
  return varianceI;
}

/* String converter */
String JansenSensitivityAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesAlgorithmImplementation::__repr__();
  return oss;
}


END_NAMESPACE_OPENTURNS
