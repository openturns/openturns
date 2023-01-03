//                                               -*- C++ -*-
/**
 *  @brief The ExtremeValueCopula distribution
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
#include <cmath>
#include "openturns/ExtremeValueCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Brent.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExtremeValueCopula)

static const Factory<ExtremeValueCopula> Factory_ExtremeValueCopula;

/* Default constructor */
ExtremeValueCopula::ExtremeValueCopula()
  : DistributionImplementation()
  , pickandFunction_(SymbolicFunction("t", "1.0"))
{
  isCopula_ = true;
  setName( "ExtremeValueCopula" );
  // We set the dimension of the ExtremeValueCopula distribution
  setDimension( 2 );
  computeRange();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
}

/* Parameters constructor */
ExtremeValueCopula::ExtremeValueCopula(const Function & pickandFunction)
  : DistributionImplementation()
  , pickandFunction_(pickandFunction)
{
  isCopula_ = true;
  setName( "ExtremeValueCopula" );
  // We set the dimension of the ExtremeValueCopula distribution
  setDimension( 2 );
  setPickandFunction(pickandFunction);
  computeRange();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
}

/* Comparison operator */
Bool ExtremeValueCopula::operator ==(const ExtremeValueCopula & other) const
{
  if (this == &other) return true;
  return pickandFunction_ == other.pickandFunction_;
}

Bool ExtremeValueCopula::equals(const DistributionImplementation & other) const
{
  const ExtremeValueCopula* p_other = dynamic_cast<const ExtremeValueCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ExtremeValueCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ExtremeValueCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " pickandFunction=" << pickandFunction_;
  return oss;
}

String ExtremeValueCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(A = " << pickandFunction_ << ")";
  return oss;
}

/* Virtual constructor */
ExtremeValueCopula * ExtremeValueCopula::clone() const
{
  return new ExtremeValueCopula(*this);
}

/* Get one realization of the distribution */
Point ExtremeValueCopula::getRealization() const
{
  Point realization(2);
  // We use the general algorithm based on conditional CDF inversion
  const Scalar u = RandomGenerator::Generate();
  realization[0] = u;
  const Scalar q = RandomGenerator::Generate();
  realization[1] = computeConditionalQuantile(q, Point(1, u));
  return realization;
}

/* Get the PDF of the distribution */
Scalar ExtremeValueCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return 0.0;
  }
  const Scalar logU = std::log(u);
  const Scalar logV = std::log(v);
  const Scalar logUV = logU + logV;
  const Point ratio(1, logV / logUV);
  const Scalar A = pickandFunction_(ratio)[0];
  const Scalar dA = pickandFunction_.gradient(ratio)(0, 0);
  const Scalar d2A = pickandFunction_.hessian(ratio)(0, 0, 0);
  return ((A * logUV - dA * logV) * (logU * dA + logUV * A) - d2A * logU * ratio[0]) * std::exp(logUV * A) / (logUV * logUV * u * v);
}

/* Get the log-PDF of the distribution */
Scalar ExtremeValueCopula::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return SpecFunc::LowestScalar;
  }
  const Scalar logU = std::log(u);
  const Scalar logV = std::log(v);
  const Scalar logUV = logU + logV;
  const Point ratio(1, logV / logUV);
  const Scalar A = pickandFunction_(ratio)[0];
  if (!SpecFunc::IsNormal(A)) return SpecFunc::LowestScalar;
  const Scalar dA = pickandFunction_.gradient(ratio)(0, 0);
  if (!SpecFunc::IsNormal(dA)) return SpecFunc::LowestScalar;
  const Scalar d2A = pickandFunction_.hessian(ratio)(0, 0, 0);
  if (!SpecFunc::IsNormal(d2A)) return SpecFunc::LowestScalar;
  return logUV * A - 2.0 * std::log(-logUV) - logUV + std::log((A * logUV - dA * logV) * (logU * dA + logUV * A) - d2A * logU * ratio[0]);
}

/* Get the CDF of the distribution */
Scalar ExtremeValueCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // If we are outside of the support, in the lower parts
  if ((u <= 0.0) || (v <= 0.0))
  {
    return 0.0;
  }
  // If we are outside of the support, in the upper part
  if ((u >= 1.0) && (v >= 1.0))
  {
    return 1.0;
  }
  // If we are outside of the support for u, in the upper part
  if (u >= 1.0)
  {
    return v;
  }
  // If we are outside of the support for v, in the upper part
  if (v >= 1.0)
  {
    return u;
  }
  const Scalar logU = std::log(u);
  const Scalar logV = std::log(v);
  const Scalar logUV = logU + logV;
  const Point ratio(1, logV / logUV);
  const Scalar A = pickandFunction_(ratio)[0];
  return std::exp(logUV * A);
}

// Conditional CDF for CDF inversion
class ConditionalCDF: public FunctionImplementation
{
public:
  ConditionalCDF(const Function & pickandFunction,
                 const Scalar u)
    : FunctionImplementation()
    , pickandFunction_(pickandFunction)
    , u_(u)
  {
    // Nothing to do
  }

  ConditionalCDF * clone() const
  {
    return new ConditionalCDF(*this);
  }

  Point operator() (const Point & point) const
  {
    const Scalar v = point[0];
    const Scalar logV = std::log(v);
    const Scalar logUV = std::log(u_ * v);
    const Point ratio(1, logV / logUV);
    const Scalar A = pickandFunction_(ratio)[0];
    const Scalar dA = pickandFunction_.gradient(ratio)(0, 0);
    const Scalar conditionalCDF = (A - dA * ratio[0]) * std::exp(logUV * A) / u_;
    return Point(1, conditionalCDF);
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description(1, "v");
  }

  Description getOutputDescription() const
  {
    return Description(1, "ConditionalCDF");
  }

  String __repr__() const
  {
    OSS oss;
    oss << "ConditionalCDF(" << pickandFunction_.__str__() << ", " << u_ << ")";
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss;
    oss << "ConditionalCDF(" << pickandFunction_.__str__() << ", " << u_ << ")";
    return oss;
  }

private:
  const Function & pickandFunction_;
  const Scalar u_;
};  // class conditionalCDF

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar ExtremeValueCopula::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  return ConditionalCDF(pickandFunction_, y[0])(Point(1, x))[0];
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar ExtremeValueCopula::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const Scalar res = Brent(SpecFunc::ScalarEpsilon, SpecFunc::ScalarEpsilon, SpecFunc::ScalarEpsilon, 53).solve(ConditionalCDF(pickandFunction_, y[0]), q, SpecFunc::ScalarEpsilon, 1.0 - SpecFunc::ScalarEpsilon);
  return res;
}

/* Tell if the distribution has independent copula */
Bool ExtremeValueCopula::hasIndependentCopula() const
{
  return (pickandFunction_(Point(1, 0.5))[0] == 1.0);
}

/* Pickand function accessor */
void ExtremeValueCopula::setPickandFunction(const Function & pickandFunction,
    const Bool check)
{
  // Checks on input and output dimensions are done unconditionally
  if (pickandFunction.getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the Pickand function must have an input dimension equal to 1, here input dimension=" << pickandFunction.getInputDimension();
  if (pickandFunction.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the Pickand function must have an output dimension equal to 1, here input dimension=" << pickandFunction.getOutputDimension();
  if (check)
  {
    // Check if max(t, 1-t) <= A(t) <= 1
    const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("ExtremeValueCopula-CheckGridSize");
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      const Scalar t = i / (n - 1.0);
      const Scalar valueF(pickandFunction(Point(1, t))[0]);
      const Scalar valueH(pickandFunction.hessian(Point(1, t))(0, 0, 0));
      if (!((valueF <= 1.0) && (valueF >= std::max(t, 1.0 - t))))
        throw InvalidArgumentException(HERE) << "Error: the given Pickand function takes a value=" << valueF << " at t=" << t << " which is not between " << std::max(t, 1 - t) << " and 1.0.";
      if (!(valueH >= 0.0))
        throw InvalidArgumentException(HERE) << "Error: the given Pickand function is not locally convex at t=" << t << " because A\"=" << valueH;
    } // i
  } // check
  pickandFunction_ = pickandFunction;
  isAlreadyComputedCovariance_ = false;
  setParallel(pickandFunction_.getImplementation()->isParallel());
}

/* Pickand function accessor */
Function ExtremeValueCopula::getPickandFunction() const
{
  return pickandFunction_;
}

/* Method save() stores the object through the StorageManager */
void ExtremeValueCopula::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "pickandFunction_", pickandFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void ExtremeValueCopula::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "pickandFunction_", pickandFunction_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
