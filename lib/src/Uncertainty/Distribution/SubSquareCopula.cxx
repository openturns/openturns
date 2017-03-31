//                                               -*- C++ -*-
/**
 *  @brief A class that implements a SubSquare copula
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
#include <cmath>
#include "openturns/SubSquareCopula.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SubSquareCopula);

static const Factory<SubSquareCopula> Factory_SubSquareCopula;

/* Default constructor */
SubSquareCopula::SubSquareCopula()
  : CopulaImplementation("SubSquareCopula")
  , phi_(SymbolicFunction("x", "0.0"))
  , nullPhi_(true)
  , mass_(1.0)
{
  // The range is generic for all the copulas
  setDimension(2);
  computeRange();
}

/* Default constructor */
SubSquareCopula::SubSquareCopula(const NumericalMathFunction & phi)
  : CopulaImplementation("SubSquareCopula")
  , phi_(phi)
  , nullPhi_(false)
  , mass_(0.0)
{
  // The range is generic for all the copulas
  setDimension(2);
  computeRange();
  setPhi(phi);
}

/* Comparison operator */
Bool SubSquareCopula::operator ==(const SubSquareCopula & other) const
{
  if (this == &other) return true;
  return phi_ == other.phi_;
}

Bool SubSquareCopula::equals(const DistributionImplementation & other) const
{
  const SubSquareCopula* p_other = dynamic_cast<const SubSquareCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String SubSquareCopula::__repr__() const
{
  OSS oss;
  oss << "class=" << SubSquareCopula::GetClassName()
      << " name=" << getName()
      << " phi=" << phi_
      << " nullPhi=" << nullPhi_
      << " mass=" << mass_;
  return oss;
}

String SubSquareCopula::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(phi = " << phi_.__str__(offset) << ")";
  return oss;
}

/* Virtual constructor */
SubSquareCopula * SubSquareCopula::clone() const
{
  return new SubSquareCopula(*this);
}

/* Phi accessors */
void SubSquareCopula::setPhi(const NumericalMathFunction & phi)
{
  if (phi.getInputDimenson() != 1) throw InvalidArgumentException(HERE) << "Error: phi must have an input dimension equal to 1, here input dimension=" << phi.getInputDimension();
  if (phi.getOutputDimenson() != 1) throw InvalidArgumentException(HERE) << "Error: phi must have an output dimension equal to 1, here output dimension=" << phi.getOutputDimension();
  const NumericalScalar phi0 = phi(NumericalPoint(1, 0.0))[0];
  if (phi0 != 0.0) throw InvalidArgumentException(HERE) << "Error: phi(0) must be null, here phi(0)=" << phi0;
  const NumericalScalar phi1 = phi(NumericalPoint(1, 1.0))[0];
  if (phi1 > 1.0) throw InvalidArgumentException(HERE) << "Error: phi(1) must be less or equal to 1, here phi(1)=" << phi1;
  nullPhi_ = false;
  // Here, we will use the integration algorithm to evaluate phi on a meaningfull grid in order to check if phi is increasing and takes its values in [0, 1]
  phi_ = phi;
  phi_.enableHistory();
  phi_.clearHistory();
  mass_ = 1.0 - GaussKronrod().integrate(phi_, Interval(0.0, 1.0));
  NumericalSample inputOutput(phi_.getHistoryInput().getSample());
  inputOutput.stack(phi_.getHistoryOutput().getSample());
  inputOutput = inputOutput.sortAccordingToAComponent(0);
  NumericalScalar lastX = inputOutput[0][0];
  NumericalScalar lastValue = inputOutput[0][1];
  if (lastValue < 0.0) throw InvalidArgumentException(HERE) << "Error: phi must be nonnegative, here phi(" << lastX << ")=" << lastValue;
  if (lastValue > 1.0) throw InvalidArgumentException(HERE) << "Error: phi must be less or equal to 1, here phi(" << lastX << ")=" << lastValue;
  for (UnsignedInteger i = 1; i < inputOutput.getSize(); ++i)
  {
    const NumericalScalar x = inputOutput[i][0];
    const NumericalScalar value = inputOutput[i][1];
    if (value < 0.0) throw InvalidArgumentException(HERE) << "Error: phi must be nonnegative, here phi(" << inputOutput[i][0] << ")=" << value;
    if (lastValue > 1.0) throw InvalidArgumentException(HERE) << "Error: phi must be less or equal to 1, here phi(" << inputOutput[0][0] << ")=" << inputOutput[0][1];
    if (value < lastValue) throw InvalidArgumentException(HERE) << "Error: phi must be nondecreasing, here phi(" << lastX << ")=" << lastValue << " and phi(" << x << ")=" << value;
    lastX = x;
    lastValue = value;
  }
  if (mass_ < error[0])
  {
    mass_ = 0.0;
    nullPhi_ = true;
  }
}

/* Get one realization of the distribution */
NumericalPoint SubSquareCopula::getRealization() const
{
  UnsignedInteger dimension = getDimension();
  if (hasIndependentCopula()) return RandomGenerator::Generate(dimension);
  else
  {
    NumericalPoint realization(subSquare_.getRealization());
    for (UnsignedInteger i = 0; i < dimension; ++i) realization[i] = DistFunc::pSubSquare(realization[i]);
    return realization;
  }
}

/* Get the PDF of the distribution */
NumericalScalar SubSquareCopula::computePDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  // Be careful to evaluate the copula only in the interior of its support
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If outside of the support return 0.0
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return 0.0;
  }
  throw NotYetImplementedException(HERE) << "In SubSquareCopula::computePDF(const NumericalPoint & point) const";
}

/* Get the CDF of the distribution */
NumericalScalar SubSquareCopula::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  // Compute the subSquare point such that a subSquare distribution with this copula
  // and standard 1D subSquare marginals has the same CDF at this subSquare point
  // than the copula at the given point.
  // Be careful to evaluate the copula only in the interior of its support
  Indices indices;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If outside of the support, in the lower part, return 0.0
    if (point[i] <= 0.0) return 0.0;
    // If the current component is in the interior of the support, its index
    // is taken into account
    if (point[i] < 1.0) indices.add(i);
  }
  const UnsignedInteger activeDimension = indices.getSize();
  // Quick return if all the components are >= 1
  if (activeDimension == 0) return 1.0;
  throw NotYetImplementedException(HERE) << "In SubSquareCopula::computeCDF(const NumericalPoint & point) const";
} // computeCDF

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar SubSquareCopula::computeConditionalPDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return 1.0;
  throw NotYetImplementedException(HERE) << "In SubSquareCopula::computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const";
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar SubSquareCopula::computeConditionalCDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  // General case
  throw NotYetImplementedException(HERE) << "In SubSquareCopula::computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const";
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar SubSquareCopula::computeConditionalQuantile(const NumericalScalar q,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  // General case
  throw NotYetImplementedException(HERE) << "In SubSquareCopula::computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const";
}

/* Tell if the distribution has independent copula */
Bool SubSquareCopula::hasIndependentCopula() const
{
  return nullPhi_;
}

/* Parameters value accessor */
NumericalPoint MinCopula::getParameter() const
{
  return NumericalPoint();
}

void MinCopula::setParameter(const NumericalPoint & parameter)
{
  if (parameters.getSize() != 0) throw InvalidArgumentException(HERE) << "Error: expected 0 parameters, got " << parameters.getSize();
}

/* Parameters description accessor */
Description MinCopula::getParameterDescription() const
{
  return Description();
}

/* Method save() stores the object through the StorageManager */
void SubSquareCopula::save(Advocate & adv) const
{
  CopulaImplementation::save(adv);
  adv.saveAttribute( "phi_", phi_ );
  adv.saveAttribute( "nullPhi_", nullPhi_ );
  adv.saveAttribute( "mass_", mass_ );
}

/* Method load() reloads the object from the StorageManager */
void SubSquareCopula::load(Advocate & adv)
{
  // The range is generic for all the copulas
  CopulaImplementation::load(adv);
  adv.loadAttribute( "phi_", phi_ );
  adv.loadAttribute( "nullPhi_", nullPhi_ );
  adv.loadAttribute( "mass_", mass_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
