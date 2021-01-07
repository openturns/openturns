//                                               -*- C++ -*-
/**
 *  @brief Implementation for Martinez sensitivity algorithm
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

#include "openturns/MartinezSensitivityAlgorithm.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MartinezSensitivityAlgorithm)

static const Factory<MartinezSensitivityAlgorithm> Factory_MartinezSensitivityAlgorithm;

/* Default constructor */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
{
  // Nothing to do
}

/** Constructor with parameters */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
  : SobolIndicesAlgorithmImplementation(inputDesign, outputDesign, size)
{
  // Nothing to do
}

/** Constructor with distribution / model parameters */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(distribution, size, model, computeSecondOrder)
{
  // Nothing to do
}

/** Constructor with experiment / model parameters */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm(const WeightedExperiment & experiment,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(experiment, model, computeSecondOrder)
{
  // Nothing to do
}

/* Virtual constructor */
MartinezSensitivityAlgorithm * MartinezSensitivityAlgorithm::clone() const
{
  return new MartinezSensitivityAlgorithm(*this);
}

Sample MartinezSensitivityAlgorithm::computeIndices(const Sample & sample,
    Sample & VTi) const
{
  const UnsignedInteger inputDimension = inputDescription_.getSize();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  const UnsignedInteger size = size_;
  Sample varianceI(outputDimension, inputDimension);
  VTi = Sample(outputDimension, inputDimension);

  // Use reference samples
  // Reference sample yA
  Sample yA(sample, 0, size);
  const Point muA(yA.computeMean());
  const Point sigmaA(yA.computeStandardDeviation());
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
    if (!(sigmaA[j] > 0.0))
      throw InvalidArgumentException(HERE) << "Null output sample variance";

  // center sample yA
  yA -= muA;
  // Reference sample yB
  Sample yB(sample, size, 2 * size);
  const Point muB(yB.computeMean());
  const Point sigmaB(yB.computeStandardDeviation());
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
    if (!(sigmaB[j] > 0.0))
      throw InvalidArgumentException(HERE) << "Null output sample variance";

  // center-reduce sample yB
  yB -= muB;
  yB /= sigmaB;

  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    Sample yE(sample, (2 + p) * size, (3 + p) * size);
    const Point muE(yE.computeMean());
    const Point sigmaE(yE.computeStandardDeviation());
    for (UnsignedInteger j = 0; j < outputDimension; ++ j)
      if (!(sigmaE[j] > 0.0))
        throw InvalidArgumentException(HERE) << "Null output sample variance";

    // center-reduce sample yB
    yE -= muE;
    yE /= sigmaE;
    // Compute yE * yB
    const Point yEDotyB(computeSumDotSamples(yE, yB));
    // Compute yE * yA
    const Point yEDotyA(computeSumDotSamples(yE, yA));
    for (UnsignedInteger q = 0; q < outputDimension; ++q)
    {
      // Compute rho(yB, yE) with rho : Pearson correlation
      // Si = rho(yB, yE) thus Vi = Si * V
      varianceI(q, p) =  yEDotyB[q] / (size - 1.0)  * sigmaA[q] * sigmaA[q];
      // STi = 1 - rho(yA, yE),  Vti = V -VTi thus VTi = rho(yA, yE) * var(yA)
      VTi(q, p) = referenceVariance_[q] - yEDotyA[q] / (size - 1.0)  * sigmaA[q];
    }
  }
  return varianceI;
}


/* String converter */
String MartinezSensitivityAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesAlgorithmImplementation::__repr__();
  return oss;
}

void MartinezSensitivityAlgorithm::computeAsymptoticDistribution() const
{
  const UnsignedInteger inputDimension = inputDescription_.getSize();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();

  // psi
  Description X(Description::BuildDefault(outputDimension, "X"));
  Description Y(Description::BuildDefault(outputDimension, "Y"));
  Description Z(Description::BuildDefault(outputDimension, "Z"));
  Description XYZ(3 * outputDimension);
  String sumX;
  String sumSqrtYZ;
  for (UnsignedInteger q = 0; q < outputDimension; ++ q)
  {
    XYZ[3 * q] = X[q];
    XYZ[3 * q + 1] = Y[q];
    XYZ[3 * q + 2] = Z[q];
    sumX += X[q];
    sumSqrtYZ += "sqrt(" + Y[q] + "*" + Z[q] + ")";
    if (q < outputDimension - 1)
    {
      sumX += "+";
      sumSqrtYZ += "+";
    }
  }
  sumX = "(" + sumX + ")";
  sumSqrtYZ = "(" + sumSqrtYZ + ")";
  Function psiFO = SymbolicFunction(XYZ, Description(1, sumX + "/" + sumSqrtYZ));
  Function psiTO = SymbolicFunction(XYZ, Description(1, "1 - " + sumX + "/" + sumSqrtYZ));

  Point varianceFO(inputDimension);
  Point varianceTO(inputDimension);

  for (UnsignedInteger p = 0; p < inputDimension; ++ p)
  {
    Sample uFO(size_, 0);
    Sample uTO(size_, 0);
    for (UnsignedInteger q = 0; q < outputDimension; ++ q)
    {
      // yE^2
      const Sample yEc2(ComputeProdSample(outputDesign_, q, size_, (2 + p) * size_, (2 + p) * size_));

      // yB * yE
      Sample yByE(ComputeProdSample(outputDesign_, q, size_, size_, (2 + p) * size_));
      uFO.stack(yByE);
      // yB^2
      const Sample yBc2(ComputeProdSample(outputDesign_, q, size_, size_, size_));
      uFO.stack(yBc2);
      uFO.stack(yEc2);

      // yA * yE
      Sample yAyE(ComputeProdSample(outputDesign_, q, size_, 0, (2 + p) * size_));
      uTO.stack(yAyE);
      // yA^2
      const Sample yAc2(ComputeProdSample(outputDesign_, q, size_, 0, 0));
      uTO.stack(yAc2);
      // yE^2
      uTO.stack(yEc2);
    }
    varianceFO[p] = computeVariance(uFO, psiFO);
    varianceTO[p] = computeVariance(uTO, psiTO);
  }
  setConfidenceInterval(varianceFO, varianceTO);
}

/* Method save() stores the object through the StorageManager */
void MartinezSensitivityAlgorithm::save(Advocate & adv) const
{
  SobolIndicesAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void MartinezSensitivityAlgorithm::load(Advocate & adv)
{
  SobolIndicesAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
