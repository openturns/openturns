//                                               -*- C++ -*-
/**
 *  @brief Canonical tensor representation and evaluation
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/CanonicalTensorEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/UniVariateFunctionEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CanonicalTensorEvaluation)

static const Factory<CanonicalTensorEvaluation> Factory_CanonicalTensorEvaluation;


CanonicalTensorEvaluation::CanonicalTensorEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}


CanonicalTensorEvaluation::CanonicalTensorEvaluation(const FunctionFamilyCollection & functionFamilies,
    const Indices & nk,
    const UnsignedInteger rank)
  : EvaluationImplementation()
  , degrees_(nk)
  , coefficients_(nk.getSize())
  , basis_(nk.getSize())
{
  const UnsignedInteger dimension = functionFamilies.getSize();
  if (dimension != nk.getSize())
    throw InvalidArgumentException(HERE) << "The number of function factories (" << dimension << ") is different from number of basis sizes (" << nk.getSize() << ")";
  if (rank == 0)
    throw InvalidArgumentException(HERE) << "Rank cannot be null";

  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    coefficients_[j] = Sample(rank, nk[j]);
    basis_[j] = Basis(nk[j]);
    for (UnsignedInteger k = 0; k < nk[j]; ++ k)
    {
      basis_[j][k] = Function(UniVariateFunctionEvaluation(functionFamilies[j].build(k)));
    }
  }
}

/* Virtual constructor */
CanonicalTensorEvaluation * CanonicalTensorEvaluation::clone() const
{
  return new CanonicalTensorEvaluation(*this);
}


Indices CanonicalTensorEvaluation::getDegrees() const
{
  return degrees_;
}


/* Coefficients accessor along i-th component */
void CanonicalTensorEvaluation::setCoefficients(const UnsignedInteger i, const UnsignedInteger j, const Point & coefficients)
{
  coefficients_[j][i] = coefficients;
}


Point CanonicalTensorEvaluation::getCoefficients(const UnsignedInteger i, const UnsignedInteger j) const
{
  return coefficients_[j][i];
}


/* Basis accessor along i-th component */
Basis CanonicalTensorEvaluation::getBasis(const UnsignedInteger i) const
{
  return basis_[i];
}


void CanonicalTensorEvaluation::setRank(const UnsignedInteger rank)
{
  if (rank == 0)
    throw InvalidArgumentException(HERE) << "Rank cannot be null";

  const UnsignedInteger oldRank = getRank();

  // initialize added tensor coefficients
  for (UnsignedInteger j = 0; j < getInputDimension(); ++ j)
  {
    if (rank > oldRank)
    {
      // add coefficients
      coefficients_[j].add(Sample(rank - oldRank, degrees_[j]));
    }
    else if (rank < oldRank)
    {
      // remove coefficients
      coefficients_[j] = Sample(coefficients_[j], 0, rank);
    }
  }
}


UnsignedInteger CanonicalTensorEvaluation::getRank() const
{
  return coefficients_[0].getSize();
}


CanonicalTensorEvaluation CanonicalTensorEvaluation::getMarginalRank(const UnsignedInteger i) const
{
  CanonicalTensorEvaluation rankOne(*this);
  rankOne.setRank(1);
  for (UnsignedInteger j = 0; j < getInputDimension(); ++ j)
  {
    rankOne.setCoefficients(0, j, getCoefficients(i, j));
  }
  return rankOne;
}

/* String converter */
String CanonicalTensorEvaluation::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " degrees=" << degrees_
         << " coefficients=" << coefficients_
         << " basis=" << basis_;
}

String CanonicalTensorEvaluation::__str__(const String & offset) const
{
  return __repr__();
}

/* Evaluation operator */
Point CanonicalTensorEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << ", got dimension=" << inP.getDimension();

  const UnsignedInteger m = getRank();
  Point prodI(m, 1.0);

  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
  {
    const Point xj(1, inP[j]);
    const Basis basisI(getBasis(j));
    const UnsignedInteger basisSize = degrees_[j];

    // compute phi_(j,k)(xj)
    Point phiX(basisSize);
    for (UnsignedInteger k = 0; k < basisSize; ++ k)
    {
      phiX[k] = basisI[k](xj)[0];
    }

    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      const Point coeffI(getCoefficients(i, j));
      Scalar sumI = 0.0;
      for (UnsignedInteger k = 0; k < basisSize; ++ k)
      {
        if (coeffI[k] != 0.0)
        {
          sumI += coeffI[k] * phiX[k];
        }
      }
      prodI[i] *= sumI;
    }
  }

  Scalar sumR = 0.0;
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    sumR += prodI[i];
  }

  const Point outP(1, sumR);
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(outP);
  }
  ++ callsNumber_;
  return outP;
}

Sample CanonicalTensorEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << ", got dimension=" << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();

  const UnsignedInteger m = getRank();
  SampleImplementation prodI(m, Point(size, 1.0));

  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
  {
    const Sample xj(inSample.getMarginal(j));
    const Basis basisI(getBasis(j));
    const UnsignedInteger basisSize = degrees_[j];

    // compute phi_(j,k)(xj)
    // Note: phiX is transposed
    Sample phiX(0, size);
    for (UnsignedInteger k = 0; k < basisSize; ++ k)
    {
      phiX.add(basisI[k](xj).asPoint());
    }

    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      const Point coeffI(getCoefficients(i, j));
      Point sumI(size);
      for (UnsignedInteger k = 0; k < basisSize; ++ k)
      {
        if (coeffI[k] != 0.0)
        {
          sumI += coeffI[k] * phiX[k];
        }
      }
      for (UnsignedInteger l = 0; l < size; ++ l)
      {
        prodI(i, l) *= sumI[l];
      }
    }
  }

  Point result(size);
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    result += prodI[i];
  }

  Sample outSample(size, 1);
  outSample.getImplementation()->setData(result);
  outSample.setDescription(getOutputDescription());

  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inSample);
    outputStrategy_.store(outSample);
  }
  callsNumber_ += size;
  return outSample;
}

/* Dimension accessor */
UnsignedInteger CanonicalTensorEvaluation::getInputDimension() const
{
  return coefficients_.getSize();
}

UnsignedInteger CanonicalTensorEvaluation::getOutputDimension() const
{
  return 1;
}

/* Method save() stores the object through the StorageManager */
void CanonicalTensorEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("degrees_", degrees_);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("basis_", basis_);
}

/* Method load() reloads the object from the StorageManager */
void CanonicalTensorEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("degrees_", degrees_);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("basis_", basis_);
}

END_NAMESPACE_OPENTURNS
