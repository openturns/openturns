//                                               -*- C++ -*-
/**
 *  @brief Implementation for Mauntz-Kucherenko sensitivity algorithm
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

#include "openturns/MauntzKucherenkoSensitivityAlgorithm.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MauntzKucherenkoSensitivityAlgorithm)

static const Factory<MauntzKucherenkoSensitivityAlgorithm> Factory_MauntzKucherenkoSensitivityAlgorithm;

/* Default constructor */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
{
  // Nothing to do
}

/** Constructor with parameters */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
  : SobolIndicesAlgorithmImplementation(inputDesign, outputDesign, size)
{
  // Nothing to do
}

/** Constructor with distribution / model parameters */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(distribution, size, model, computeSecondOrder)
{
  // Nothing to do
}

/** Constructor with experiment / model parameters */
MauntzKucherenkoSensitivityAlgorithm::MauntzKucherenkoSensitivityAlgorithm(const WeightedExperiment & experiment,
    const Function & model,
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
Sample MauntzKucherenkoSensitivityAlgorithm::computeIndices(const Sample & sample,
    Sample & VTi) const
{
  const UnsignedInteger inputDimension = inputDescription_.getSize();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  const UnsignedInteger size = size_;
  Sample varianceI(outputDimension, inputDimension);
  VTi = Sample(outputDimension, inputDimension);

  // Use reference samples
  // Compute muA = mean(yA)
  const Sample yA(sample, 0, size);
  const Point muA(yA.computeMean());

  // Compute crossMean
  const Point yADotyB(computeSumDotSamples(sample, size_, 0,  size_));
  const Point yADotyA(computeSumDotSamples(sample, size_, 0, 0));
  // main loop
  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    // yE correspond to the block that start at index (p + 2) * size_
    // For first order indices, compute yE * yB
    const Point yEDotyB(computeSumDotSamples(sample, size_, size_, (2 + p) * size_));
    // Compute yE * yA
    const Point yEDotyA(computeSumDotSamples(sample, size_, 0, (2 + p) * size_));
    for (UnsignedInteger k = 0; k < size; ++k)
    {
      for (UnsignedInteger q = 0; q < outputDimension; ++q)
      {
        varianceI(q, p) =  (yEDotyB[q] - yADotyB[q]) / (size - 1.0);
        VTi(q, p) = (yADotyA[q] - yEDotyA[q]) / (size - 1.0);
      }
    }
  }
  return varianceI;
}

void MauntzKucherenkoSensitivityAlgorithm::computeAsymptoticDistribution() const
{
  const UnsignedInteger inputDimension = inputDescription_.getSize();
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
  Function psiFO = SymbolicFunction(XY, Description(1, sumX + "/" + sumY));
  Function psiTO = SymbolicFunction(XY, Description(1, "1 - " + sumX + "/" + sumY));

  Point varianceFO(inputDimension);
  Point varianceTO(inputDimension);

  for (UnsignedInteger p = 0; p < inputDimension; ++ p)
  {
    Sample uFO(size_, 0);
    Sample uTO(size_, 0);
    for (UnsignedInteger q = 0; q < outputDimension; ++ q)
    {
      const Sample yAc2(ComputeProdSample(outputDesign_, q, size_, 0, 0));

      // yB * (yE - yA)
      Sample yBtEmA(size_, 1);
      for (UnsignedInteger i = 0; i < size_; ++ i)
        yBtEmA(i, 0) = outputDesign_(size_ + i, q) * (outputDesign_((2 + p) * size_ + i, q) - outputDesign_(i, q));

      uFO.stack(yBtEmA);
      uFO.stack(yAc2);

      // yA * (yE - yA)
      Sample yAtEmA(size_, 1);
      for (UnsignedInteger i = 0; i < size_; ++ i)
        yAtEmA(i, 0) = outputDesign_(i, q) * (outputDesign_((2 + p) * size_ + i, q) - outputDesign_(i, q));

      uTO.stack(yAtEmA);
      uTO.stack(yAc2);
    }
    varianceFO[p] = computeVariance(uFO, psiFO);
    varianceTO[p] = computeVariance(uTO, psiTO);
  }
  setConfidenceInterval(varianceFO, varianceTO);
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
