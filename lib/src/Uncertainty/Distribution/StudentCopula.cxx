//                                               -*- C++ -*-
/**
 *  @brief Student or T-Copula
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/StudentCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Student.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StudentCopula)

static const Factory<StudentCopula> Factory_StudentCopula;

/* Default constructor */
StudentCopula::StudentCopula()
  : SklarCopula(Student())
{
  // Nothing to do
}

StudentCopula::StudentCopula(const Scalar nu,
                             const CorrelationMatrix & R)
  : SklarCopula(Student(nu, Point(R.getNbRows(), 0.0), Point(R.getNbRows(), 1.0), R))
{
  // Nothing to do
}

/* String converter */
String StudentCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << StudentCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " nu=" << getDistribution().getParameter()[0]
      << " R=" << getDistribution().getCorrelation();
  return oss;
}

String StudentCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(nu = " << getNu()
      << ", R = " << getR() << ")";
  return oss;
}

/* Virtual constructor */
StudentCopula * StudentCopula::clone() const
{
  return new StudentCopula(*this);
}

/* Parameters value accessor */
Point StudentCopula::getParameter() const
{
  Point parameter(SklarCopula::getParameter());
  // drop mu, sigma
  parameter.erase(parameter.begin() + 1, parameter.begin() + 1 + 2 * getDimension());
  return parameter;
}

void StudentCopula::setParameter(const Point & parameter)
{
  // N = 1 + ((d-1)*d)/2
  const UnsignedInteger size = parameter.getSize();
  Scalar dimReal = 0.5 + 0.5 * std::sqrt(8.0 * size - 7.0);
  if (dimReal != round(dimReal)) throw InvalidArgumentException(HERE) << "Error: invalid parameter number for StudentCopula";
  const UnsignedInteger dimension = dimReal;

  // full parameters of the Student include mu, sigma vectors, N = 1 + 2*d + ((d-1)*d)/2
  Point fullParameter(1 + 2 * dimension + (dimension * (dimension - 1)) / 2);
  fullParameter[0] = parameter[0]; // nu
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    fullParameter[1 + 2 * i + 1] = 1.0; // sigma=1 (mu=0)
  std::copy(parameter.begin() + 1, parameter.end(), fullParameter.begin() + 1 + 2 * dimension); // R
  SklarCopula::setParameter(fullParameter);
}

/* Parameters description accessor */
Description StudentCopula::getParameterDescription() const
{
  Description description(SklarCopula::getParameterDescription());
  // drop mu, sigma
  description.erase(description.begin() + 1, description.begin() + 1 + 2 * getDimension());
  return description;
}

/* Nu parameter accessor */
void StudentCopula::setNu(const Scalar nu)
{
  Point parameter(getParameter());
  parameter[0] = nu;
  setParameter(parameter);
}

Scalar StudentCopula::getNu() const
{
  return getParameter()[0];
}

/* R parameter accessor */
void StudentCopula::setR(const CorrelationMatrix & R)
{
  Point parameter(getParameter());
  if (R.getDimension() != getDimension())
    throw InvalidArgumentException(HERE) << "R must be of dimension " << getDimension() << " got " << R.getDimension();
  UnsignedInteger index = 1;
  for (UnsignedInteger i = 1; i < getDimension(); ++ i)
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      parameter[index] = R(i, j);
      ++ index;
    }
  setParameter(parameter);
}

CorrelationMatrix StudentCopula::getR() const
{
  const Point parameter(getParameter());
  CorrelationMatrix R(getDimension());
  UnsignedInteger index = 1;
  for (UnsignedInteger i = 1; i < getDimension(); ++ i)
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      R(i, j) = parameter[index];
      ++ index;
    }
  return R;
}

END_NAMESPACE_OPENTURNS
