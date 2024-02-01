//                                               -*- C++ -*-
/**
 *  @brief Implement the Filon quadrature for oscilatory integrands
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
#include "openturns/FilonQuadrature.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FilonQuadrature
 */

CLASSNAMEINIT(FilonQuadrature)

static const Factory<FilonQuadrature> Factory_FilonQuadrature;

/* Constructor without parameters */
FilonQuadrature::FilonQuadrature()
  : IntegrationAlgorithmImplementation()
  , n_(1)
  , omega_(1.0)
  , kind_(0)
{
  // Nothing to do
}

/* Parameters constructor */
FilonQuadrature::FilonQuadrature(const UnsignedInteger n,
                                 const Scalar omega,
                                 const UnsignedInteger kind)
  : IntegrationAlgorithmImplementation()
  , n_(n)
  , omega_(omega)
  , kind_(kind)
{
  // Check the maximum number of sub-intervals
  if (!(n > 0)) throw InvalidArgumentException(HERE) << "Error: the discretization must be at least 1, here n=" << n;
}

/* Virtual constructor */
FilonQuadrature * FilonQuadrature::clone() const
{
  return new FilonQuadrature(*this);
}

/* Compute an approximation of \int_{[a,b]}f(x)cos(t*x)dx, where [a,b]
 * is an 1D interval and f a scalar function
 */
Point FilonQuadrature::integrate(const Function & function,
                                 const Interval & interval) const
{
  return integrate(function, omega_, interval);
}

Point FilonQuadrature::integrate(const Function & function,
                                 const Scalar omega,
                                 const Interval & interval) const
{
  if (interval.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given interval should be 1D, here dimension=" << interval.getDimension();
  if (function.getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a function with input dimension=1, here input dimension=" << function.getInputDimension();
  const UnsignedInteger outputDimension = function.getOutputDimension();
  const UnsignedInteger size = 2 * n_ + 1;
  const Scalar a = interval.getLowerBound()[0];
  const Scalar b = interval.getUpperBound()[0];
  const Sample x(RegularGrid(a, (b - a) / (size - 1.0), size).getVertices());
  const Scalar h = (b - a) / (size - 1.0);
  const Scalar theta = omega * h;
  const Scalar sinTheta = std::sin(theta);
  const Scalar cosTheta = std::cos(theta);
  Scalar alpha;
  Scalar beta;
  Scalar gamma;
  // Use series for small values of theta
  if (6.0 * std::abs(theta) <= 1.0)
  {
    const Scalar theta2 = theta * theta;
    alpha = (2.0 / 45.0 + (-2.0 / 315.0 + 2.0 * theta2 / 4725.0) * theta2) * theta2 * theta;
    beta = 2.0 / 3.0 + (2.0 / 15.0 + (-4.0 / 105.0 + 2.0 * theta2 / 567.0) * theta2) * theta2;
    gamma = 4.0 / 3.0 + (-2.0 / 15.0 + (1.0 / 210.0 - theta2 / 11340.0) * theta2) * theta2;
  } // small theta
  else
  {
    const Scalar theta2 = theta * theta;
    alpha = 1.0 / theta + sinTheta * cosTheta / theta2 - 2.0 * sinTheta * sinTheta / (theta2 * theta);
    beta = 2.0 / theta2 + 2.0 * cosTheta * cosTheta / theta2 - 4.0 * cosTheta * sinTheta / (theta2 * theta);
    gamma = 4.0 * sinTheta / (theta2 * theta) - 4.0 * cosTheta / theta2;
  } // large theta
  // Evaluate the function over the grid
  const Sample f(function(x));
  // Select the oscillatory kernel depending on the value of kind:
  // kind == 0: cos(t*x)
  // kind == 1: sin(t*x)
  // kind == 2: exp(I*t*x)
  Point c2n;
  Point c2nm1;
  Point s2n;
  Point s2nm1;
  Point valueCos;
  Point valueSin;
  if (kind_ == 0 || kind_ >= 2)
  {
    c2n = f[0] * (0.5 * std::cos(omega * x(0, 0)));
    for (UnsignedInteger i = 2; i < size - 1; i += 2)
      c2n += f[i] * std::cos(omega * x(i, 0));
    c2n += f[size - 1] * (0.5 * std::cos(omega * x(size - 1, 0)));

    c2nm1 = Point(outputDimension);
    for (UnsignedInteger i = 1; i < size - 1; i += 2)
      c2nm1 += f[i] * std::cos(omega * x(i, 0));

    valueCos = ((f[size - 1] * std::sin(omega * x(size - 1, 0)) - f[0] * std::sin(omega * x(0, 0))) * alpha + c2n * beta + c2nm1 * gamma) * h;

    if (kind_ == 0) return valueCos;
  } // cos(t*x) or exp(I*t*x)
  if (kind_ == 1 || kind_ >= 2)
  {
    s2n = f[0] * (0.5 * std::sin(omega * x(0, 0)));
    for (UnsignedInteger i = 2; i < size - 1; i += 2)
      s2n += f[i] * std::sin(omega * x(i, 0));
    s2n += f[size - 1] * (0.5 * std::sin(omega * x(size - 1, 0)));

    s2nm1 = Point(outputDimension);
    for (UnsignedInteger i = 1; i < size - 1; i += 2)
      s2nm1 += f[i] * std::sin(omega * x(i, 0));
    valueSin = ((f[0] * std::cos(omega * x(0, 0)) - f[size - 1] * std::cos(omega * x(size - 1, 0))) * alpha + s2n * beta + s2nm1 * gamma) * h;
    if (kind_ == 1) return valueSin;
  } // sin(t*x) or exp(I*t*x)
  // Here we know that we integrate wrt exp(I*t*x)
  Point value(2 * outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    value[i] = valueCos[i];
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    value[outputDimension + i] = valueSin[i];
  return value;
}

/* N accessor */
UnsignedInteger FilonQuadrature::getN() const
{
  return n_;
}

void FilonQuadrature::setN(const UnsignedInteger n)
{
  if (!(n > 0)) throw InvalidArgumentException(HERE) << "Error: n must be at least 1, here n=" << n;
  n_ = n;
}

/* Omega accessor */
Scalar FilonQuadrature::getOmega() const
{
  return omega_;
}

void FilonQuadrature::setOmega(const Scalar omega)
{
  omega_ = omega;
}

/* Kind accessor */
UnsignedInteger FilonQuadrature::getKind() const
{
  return kind_;
}

void FilonQuadrature::setKind(const UnsignedInteger kind)
{
  kind_ = kind;
}

/* String converter */
String FilonQuadrature::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FilonQuadrature::GetClassName()
      << ", n=" << n_
      << ", omega=" << omega_
      << ", kind=" << kind_;
  return oss;
}

/* String converter */
String FilonQuadrature::__str__(const String & ) const
{
  OSS oss(false);
  oss << FilonQuadrature::GetClassName()
      << "(n=" << n_
      << ", omega=" << omega_
      << ", kernel=" << (kind_ == 0 ? "cos(omega*x)" : (kind_ == 1 ? "sin(omega*x)" : "exp(I*omega*x)"))
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FilonQuadrature::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("n_", n_);
  adv.saveAttribute("omega_", omega_);
  adv.saveAttribute("kind_", kind_);
}

/* Method load() reloads the object from the StorageManager */
void FilonQuadrature::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("n_", n_);
  adv.loadAttribute("omega_", omega_);
  adv.loadAttribute("kind_", kind_);
}

END_NAMESPACE_OPENTURNS
