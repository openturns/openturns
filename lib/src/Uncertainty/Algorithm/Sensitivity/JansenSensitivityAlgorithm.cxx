//                                               -*- C++ -*-
/**
 *  @brief Implementation for Jansen sensitivity algorithm
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  sobolIndices_ = JansenSobolIndices();
}

/** Constructor with parameters */
JansenSensitivityAlgorithm::JansenSensitivityAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
  : SobolIndicesAlgorithmImplementation(inputDesign, outputDesign, size)
{
  sobolIndices_ = JansenSobolIndices(inputDesign.getDimension(), outputDesign.getDimension());
}

/** Constructor with distribution / model parameters */
JansenSensitivityAlgorithm::JansenSensitivityAlgorithm(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(distribution, size, model, computeSecondOrder)
{
  sobolIndices_ = JansenSobolIndices(inputDesign_.getDimension(), outputDesign_.getDimension());
}


/** Constructor with experiment / model parameters */
JansenSensitivityAlgorithm::JansenSensitivityAlgorithm(const WeightedExperiment & experiment,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(experiment, model, computeSecondOrder)
{
  sobolIndices_ = JansenSobolIndices(inputDesign_.getDimension(), outputDesign_.getDimension());
}

/* Virtual constructor */
JansenSensitivityAlgorithm * JansenSensitivityAlgorithm::clone() const
{
  return new JansenSensitivityAlgorithm(*this);
}

void JansenSensitivityAlgorithm::computeIndices(const Sample & sample) const
{
  sobolIndices_.computeIndices(sample);
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
