//                                               -*- C++ -*-
/**
 *  @brief Space filling criterion based on centered L2-discrepancy
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
#include "openturns/SpaceFillingC2.hxx"
#include "openturns/PersistentObjectFactory.hxx"


namespace OT
{

CLASSNAMEINIT(SpaceFillingC2);

static const Factory<SpaceFillingC2> Factory_SpaceFillingC2;


/* Default constructor */
SpaceFillingC2::SpaceFillingC2()
  : SpaceFillingImplementation(true)
{
  setName("C2");
}

/* Virtual constructor method */
SpaceFillingC2 * SpaceFillingC2::clone() const
{
  return new SpaceFillingC2(*this);
}

/* String converter */
String SpaceFillingC2::__repr__() const
{
  OSS oss;
  oss << "class=" << SpaceFillingC2::GetClassName()
      << " minimization=" << isMinimizationProblem();
  return oss;
}

/** Evaluate criterion on a sample */
NumericalScalar SpaceFillingC2::evaluate(const NumericalSample & sample) const
{
  const NumericalSample normalizedSample(normalize(sample));
  const UnsignedInteger size(sample.getSize());
  const UnsignedInteger dimension(sample.getDimension());
  const NumericalScalar* addr_sample = &normalizedSample[0][0];
  NumericalScalar sum1 = 0.0;
  NumericalScalar sum2 = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar* ptI(addr_sample + dimension * i);
    NumericalScalar prod = 1.0;
    for (UnsignedInteger d = 0; d < dimension; ++d)
    {
      const NumericalScalar delta(ptI[d] - 0.5);
      prod *= (1.0 + 0.5 * std::abs(delta) - 0.5 * delta * delta);
    }
    sum1 += prod;
  }
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar* ptI(addr_sample + dimension * i);
    for (UnsignedInteger j = 0; j < size; ++j)
    {
      const NumericalScalar* ptJ(addr_sample + dimension * j);
      NumericalScalar prod = 1.0;
      for (UnsignedInteger d = 0; d < dimension; ++d)
      {
        const NumericalScalar deltaI(ptI[d] - 0.5);
        const NumericalScalar deltaJ(ptJ[d] - 0.5);
        prod *= (1.0 + 0.5 * std::abs(deltaI) + 0.5 * std::abs(deltaJ) - 0.5 * std::abs(deltaI - deltaJ));
      }
      sum2 += prod;
    }
  }
  return std::sqrt(std::exp(dimension * std::log(13.0 / 12.0)) - (2.0 / size) * sum1 + (1.0 / size / size) * sum2);
}

/** Compute criterion when performing an elementary perturbation */
NumericalScalar SpaceFillingC2::perturbLHS(NumericalSample& oldDesign, OT::NumericalScalar oldCriterion,
    UnsignedInteger row1, UnsignedInteger row2, UnsignedInteger column) const
{
  const UnsignedInteger size(oldDesign.getSize());
  const UnsignedInteger dimension(oldDesign.getDimension());
  const NumericalScalar* addr_sample(&oldDesign[0][0]);
  NumericalScalar* pt1(&oldDesign[0][0] + dimension * row1);
  NumericalScalar* pt2(&oldDesign[0][0] + dimension * row2);
  // Part to remove
  NumericalScalar oldSum = 0.0;
  for(UnsignedInteger j = 0; j < size; ++j)
  {
    if (j == row1 || j == row2) continue;
    const NumericalScalar* ptJ(addr_sample + dimension * j);
    NumericalScalar prodRow1 = 1.0;
    NumericalScalar prodRow2 = 1.0;
    for (UnsignedInteger d = 0; d < dimension; ++d)
    {
      const NumericalScalar deltaRow1(pt1[d] - 0.5);
      const NumericalScalar deltaRow2(pt2[d] - 0.5);
      const NumericalScalar deltaJ(ptJ[d] - 0.5);
      prodRow1 *= (1.0 + 0.5 * std::abs(deltaRow1) + 0.5 * std::abs(deltaJ) - 0.5 * std::abs(deltaRow1 - deltaJ));
      prodRow2 *= (1.0 + 0.5 * std::abs(deltaRow2) + 0.5 * std::abs(deltaJ) - 0.5 * std::abs(deltaRow2 - deltaJ));
    }
    oldSum += prodRow1 + prodRow2;
  }
  // Scale oldSum
  oldSum *= 2.0 / size / size;
  // Take into account diagonal elements
  NumericalScalar prod1Row1 = 1.0;
  NumericalScalar prod2Row1 = 1.0;
  NumericalScalar prod1Row2 = 1.0;
  NumericalScalar prod2Row2 = 1.0;
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    // c_{row1, row1}
    const NumericalScalar deltaI(pt1[d] - 0.5);
    prod1Row1 *= (1.0 + std::abs(deltaI));
    prod2Row1 *= (1.0 + 0.5 * std::abs(deltaI) - 0.5 * deltaI * deltaI);
    // for c_{row2, row2}
    const NumericalScalar deltaJ(pt2[d] - 0.5);
    prod1Row2 *= (1.0 + std::abs(deltaJ));
    prod2Row2 *= (1.0 + 0.5 * std::abs(deltaJ) - 0.5 * deltaJ * deltaJ);
  }
  // Add c_{row1, row1}
  oldSum += prod1Row1 / size / size - 2.0 * prod2Row1 / size;
  // Add c_{row2, row2}
  oldSum += prod1Row2 / size / size - 2.0 * prod2Row2/ size;
  // Swap coordinates
  std::swap(pt1[column], pt2[column]);
  NumericalScalar newSum = 0.0;
  for(UnsignedInteger j = 0; j < size; ++j)
  {
    if (j == row1 || j == row2) continue;
    const NumericalScalar* ptJ(addr_sample + dimension * j);
    NumericalScalar prodRow1 = 1.0;
    NumericalScalar prodRow2 = 1.0;
    for (UnsignedInteger d = 0; d < dimension; ++d)
    {
      const NumericalScalar deltaRow1(pt1[d] - 0.5);
      const NumericalScalar deltaRow2(pt2[d] - 0.5);
      const NumericalScalar deltaJ(ptJ[d] - 0.5);
      prodRow1 *= (1.0 + 0.5 * std::abs(deltaRow1) + 0.5 * std::abs(deltaJ) - 0.5 * std::abs(deltaRow1 - deltaJ));
      prodRow2 *= (1.0 + 0.5 * std::abs(deltaRow2) + 0.5 * std::abs(deltaJ) - 0.5 * std::abs(deltaRow2 - deltaJ));
    }
    newSum += prodRow1 + prodRow2;
  }
  // Scale newSum
  newSum *= 2.0 / size / size;
  // Take into account diagonal elements
  prod1Row1 = 1.0;
  prod2Row1 = 1.0;
  prod1Row2 = 1.0;
  prod2Row2 = 1.0;
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    // c_{row1, row1}
    const NumericalScalar deltaI(pt1[d] - 0.5);
    prod1Row1 *= (1.0 + std::abs(deltaI));
    prod2Row1 *= (1.0 + 0.5 * std::abs(deltaI) - 0.5 * deltaI * deltaI);
    // for c_{row2, row2}
    const NumericalScalar deltaJ(pt2[d] - 0.5);
    prod1Row2 *= (1.0 + std::abs(deltaJ));
    prod2Row2 *= (1.0 + 0.5 * std::abs(deltaJ) - 0.5 * deltaJ * deltaJ);
  }
  // Add c_{row1, row1}
  newSum += prod1Row1 / size / size - 2.0 * prod2Row1 / size;
  // Add c_{row2, row2}
  newSum += prod1Row2 / size / size - 2.0 * prod2Row2/ size;
  // Swap coordinates to restore original sample
  std::swap(pt1[column], pt2[column]);
  // Final result
  NumericalScalar result = oldCriterion * oldCriterion + newSum - oldSum;
  if (result <= 0.0) return 0.0;
  return std::sqrt(result);
}

} /* namespace OT */
