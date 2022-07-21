//                                               -*- C++ -*-
/**
 *  @brief Implementation for Saltelli sensitivity algorithm
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/SaltelliSensitivityAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SaltelliSensitivityAlgorithm)

static const Factory<SaltelliSensitivityAlgorithm> Factory_SaltelliSensitivityAlgorithm;

/* Default constructor */
SaltelliSensitivityAlgorithm::SaltelliSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
{
  // Nothing to do
}

/** Constructor with parameters */
SaltelliSensitivityAlgorithm::SaltelliSensitivityAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign,
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
Sample SaltelliSensitivityAlgorithm::computeIndices(const Sample & sample,
    Sample & VTi) const
{
  const UnsignedInteger inputDimension = inputDescription_.getSize();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  const UnsignedInteger size = size_;
  Sample varianceI(outputDimension, inputDimension);
  VTi = Sample(outputDimension, inputDimension);

  // Compute muA = mean(yA)
  const Sample yA(sample, 0, size);
  const Point muA(yA.computeMean());

  // Compute muB
  const Sample yB(sample, size, 2 * size);
  const Point muB(yB.computeMean());

  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    // yE correspond to the block that start at index (p + 2) * size_
    // For first order indices, compute yE * yB
    const Point yEDotyB(computeSumDotSamples(sample, size_, size_, (2 + p) * size_));
    // For total order indices, compute yE * yA
    const Point yEDotyA(computeSumDotSamples(sample, size_, 0, (2 + p) * size_));

    for (UnsignedInteger q = 0; q < outputDimension; ++q)
    {
      varianceI(q, p) +=  yEDotyB[q]  / (size - 1.0) -  muA[q] * muB[q];
      // Vti = Var - V_{-i}
      VTi(q, p) += muA[q] * muA[q] + referenceVariance_[q] - yEDotyA[q]  / (size - 1.0);
    }
  }
  return varianceI;
}

void SaltelliSensitivityAlgorithm::computeAsymptoticDistribution() const
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

      // yB * yE
      uFO.stack(ComputeProdSample(outputDesign_, q, size_, size_, (2 + p) * size_));
      uFO.stack(yAc2);

      // yA * yE
      uTO.stack(ComputeProdSample(outputDesign_, q, size_, 0, (2 + p) * size_));
      uTO.stack(yAc2);
    }
    varianceFO[p] = computeVariance(uFO, psiFO);
    varianceTO[p] = computeVariance(uTO, psiTO);
  }
  setConfidenceInterval(varianceFO, varianceTO);
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
