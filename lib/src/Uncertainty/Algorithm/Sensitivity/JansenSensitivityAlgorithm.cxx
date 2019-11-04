//                                               -*- C++ -*-
/**
 *  @brief Implementation for Jansen sensitivity algorithm
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/JansenSensitivityAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(JansenSensitivityAlgorithm)

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
    const Point squaredSumyBMinusyE(computeSumDotSamples(yEMinusyB, yEMinusyB));
    // Sum of squared elements
    const Point squaredSumyAMinusyE(computeSumDotSamples(yEMinusyA, yEMinusyA));

    for (UnsignedInteger q = 0; q < outputDimension; ++q)
    {
      varianceI(q, p) =  -squaredSumyBMinusyE[q] / (2.0 * size - 1.0) +  referenceVariance_[q];
      // Vti = Var - V_{-i}
      VTi(q, p) = squaredSumyAMinusyE[q] / (2.0 * size - 1.0);
    }
  }
  return varianceI;
}

void JansenSensitivityAlgorithm::computeAsymptoticDistribution() const
{
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();

  // psi
  Description X(Description::BuildDefault(outputDimension, "X"));
  Description Y(Description::BuildDefault(outputDimension, "Y"));
  Description XY(2 * outputDimension);
  String sumX;
  String sumY;
  for (UnsignedInteger q = 0; q < outputDimension; ++ q)
  {
    XY[2 * q] = X[q];
    XY[2 * q + 1] = Y[q];
    sumX += X[q];
    sumY += Y[q];
    if (q < outputDimension - 1)
    {
      sumX += "+";
      sumY += "+";
    }
  }
  sumX = "(" + sumX + ")";
  sumY = "(" + sumY + ")";
  Function psiFO = SymbolicFunction(XY, Description(1, "1 - 0.5*" + sumX + "/" + sumY));
  Function psiTO = SymbolicFunction(XY, Description(1, "0.5*" + sumX + "/" + sumY));

  Point varianceFO(inputDimension);
  Point varianceTO(inputDimension);

  for (UnsignedInteger p = 0; p < inputDimension; ++ p)
  {
    Sample uFO(size_, 0);
    Sample uTO(size_, 0);
    for (UnsignedInteger q = 0; q < outputDimension; ++ q)
    {
      const Sample yAc2(ComputeProdSample(outputDesign_, q, size_, 0, 0));

      // (yE - yB)**2
      Sample yEmB(size_, 1);
      for (UnsignedInteger i = 0; i < size_; ++ i)
      {
        const Scalar eMb = outputDesign_((2 + p) * size_ + i, q) - outputDesign_(size_ + i, q);
        yEmB(i, 0) = eMb * eMb;
      }

      uFO.stack(yEmB);
      uFO.stack(yAc2);

      // (yA - yE)**2
      Sample yAmB(size_, 1);
      for (UnsignedInteger i = 0; i < size_; ++ i)
      {
        const Scalar eMb = outputDesign_(i, q) - outputDesign_((2 + p) * size_ + i, q);
        yAmB(i, 0) = eMb * eMb;
      }

      uTO.stack(yAmB);
      uTO.stack(yAc2);
    }
    varianceFO[p] = computeVariance(uFO, psiFO);
    varianceTO[p] = computeVariance(uTO, psiTO);
  }
  setConfidenceInterval(varianceFO, varianceTO);
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
