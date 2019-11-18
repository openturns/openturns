//                                               -*- C++ -*-
/**
 *  @brief LogNormal distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/LogNormalFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/LogNormalMuErrorFactor.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogNormalMuErrorFactor)
static const Factory<LogNormalMuErrorFactor> Factory_LogNormalMuErrorFactor;

/* Default constructor */
LogNormalMuErrorFactor::LogNormalMuErrorFactor()
  : DistributionParametersImplementation()
  , muLog_(0.0)
  , ef_(std::exp(DistFunc::qNormal(0.95)))
  , gamma_(0.)
{
  // Nothing to do
}

LogNormalMuErrorFactor::LogNormalMuErrorFactor(const Scalar mu, const Scalar ef, const Scalar gamma)
  : DistributionParametersImplementation()
  , muLog_(mu)
  , ef_(ef)
  , gamma_(gamma)
{
  if (!(ef > 1.0)) throw InvalidArgumentException(HERE) << "EF must be > 1, here EF=" << ef;
  if (mu <= gamma) throw InvalidArgumentException(HERE) << "mu must be greater than gamma, here mu=" << mu << " and gamma=" << gamma;
}

/* Virtual constructor */
LogNormalMuErrorFactor * LogNormalMuErrorFactor::clone() const
{
  return new LogNormalMuErrorFactor(*this);
}

/* Comparison operator */
Bool LogNormalMuErrorFactor::operator ==(const LogNormalMuErrorFactor & other) const
{
  if (this == &other) return true;
  return (muLog_ == other.muLog_) && (ef_ == other.ef_) && (gamma_ == other.gamma_);
}


/* Build a distribution based on a set of native parameters */
Distribution LogNormalMuErrorFactor::getDistribution() const
{
  Point newParameters(3);
  newParameters[0] = muLog_;
  newParameters[1] = ef_;
  newParameters[2] = gamma_;

  Point nativeParameters(operator()(newParameters));

  return LogNormalFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix LogNormalMuErrorFactor::gradient() const
{
  const Scalar dsigmalogdef = 1.0 / (DistFunc::qNormal(0.95) * ef_);

  Matrix nativeParametersGradient(IdentityMatrix(3));
  nativeParametersGradient(1, 1) = dsigmalogdef;

  return nativeParametersGradient;
}


/* Conversion operator */
Point LogNormalMuErrorFactor::operator () (const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar ef = inP[1];

  if (!(ef > 1.0)) throw InvalidArgumentException(HERE) << "EF must be > 1, here EF=" << ef;

  Point nativeParameters(inP);
  Scalar sigmaLog = std::log(ef) / DistFunc::qNormal(0.95);
  nativeParameters[1] = sigmaLog;

  return nativeParameters;
}


Point LogNormalMuErrorFactor::inverse(const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar sigmaLog = inP[1];

  if (!(sigmaLog > 0.0)) throw InvalidArgumentException(HERE) << "SigmaLog MUST be positive, here sigmaLog=" << sigmaLog;

  const Scalar ef = std::exp(sigmaLog * DistFunc::qNormal(0.95));

  Point muEFParameters(inP);
  muEFParameters[1] = ef;

  return muEFParameters;
}


/* Parameters value and description accessor */
void LogNormalMuErrorFactor::setValues(const Point & inP)
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  muLog_ = inP[0];
  ef_ = inP[1];
  gamma_ = inP[2];
}

Point LogNormalMuErrorFactor::getValues() const
{
  Point point(3);
  point[0] = muLog_;
  point[1] = ef_;
  point[2] = gamma_;
  return point;
}

Description LogNormalMuErrorFactor::getDescription() const
{
  Description description(3);
  description[0] = "muLog";
  description[1] = "EF";
  description[2] = "gamma";
  return description;
}

/* String converter */
String LogNormalMuErrorFactor::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " muLog=" << muLog_
      << " EF=" << ef_
      << " gamma=" << gamma_;
  return oss;
}


String LogNormalMuErrorFactor::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(muLog = " << muLog_ << ", EF = " << ef_ << ", gamma = " << gamma_ << ")";
  return oss;
}

void LogNormalMuErrorFactor::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "muLog_", muLog_ );
  adv.saveAttribute( "ef_", ef_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

void LogNormalMuErrorFactor::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "muLog_", muLog_ );
  adv.loadAttribute( "ef_", ef_ );
  adv.loadAttribute( "gamma_", gamma_ );
}

END_NAMESPACE_OPENTURNS
