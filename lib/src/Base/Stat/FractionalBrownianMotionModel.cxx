//                                               -*- C++ -*-
/**
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
#include "openturns/FractionalBrownianMotionModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/TriangularComplexMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FractionalBrownianMotionModel)

static const Factory<FractionalBrownianMotionModel> Factory_FractionalBrownianMotionModel;


/* Parameters constructor */
FractionalBrownianMotionModel::FractionalBrownianMotionModel()
  : CovarianceModelImplementation(1)
  , exponent_(1, 0.25)
  , eta_(0)
{
  // Nothing to do
}

/* Parameters constructor */
FractionalBrownianMotionModel::FractionalBrownianMotionModel(const Scalar scale,
    const Scalar amplitude,
    const Scalar exponent)
  : CovarianceModelImplementation(Point(1, scale), Point(1, amplitude))
  , exponent_(0)
{
  setExponentEtaRho(Point(1, exponent), SquareMatrix(1), CorrelationMatrix(1));
}


/* Parameters constructor */
FractionalBrownianMotionModel::FractionalBrownianMotionModel(const Scalar scale,
    const Point & amplitude,
    const Point & exponent,
    const SquareMatrix & eta,
    const CorrelationMatrix & rho)
  : CovarianceModelImplementation(Point(1, scale), amplitude)
{
  setExponentEtaRho(exponent, eta, rho);
}


/* Virtual constructor */
FractionalBrownianMotionModel * FractionalBrownianMotionModel::clone() const
{
  return new FractionalBrownianMotionModel(*this);
}

Scalar FractionalBrownianMotionModel::computeAsScalar(const Point &s,
    const Point &t) const
{
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if(outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "FractionalBrownianMotionModel::computeAsScalar(s,t) should be used only if output dimension is 1. Here, output dimension = " << outputDimension_;

  const Scalar sOverTheta = s[0] / scale_[0];
  const Scalar tOverTheta = t[0] / scale_[0];
  const Scalar stOverTheta = tOverTheta - sOverTheta;
  const Scalar absSOverTheta = std::abs(sOverTheta);
  const Scalar absTOverTheta = std::abs(tOverTheta);
  const Scalar absSTOverTheta = std::abs(stOverTheta);
  const Scalar Hi = exponent_[0];
  const Scalar sigmaI = amplitude_[0];
  Scalar result = 0.5 * sigmaI * sigmaI * (std::pow(absSOverTheta, 2.0 * Hi) + std::pow(absTOverTheta, 2.0 * Hi) - std::pow(absSTOverTheta, 2.0 * Hi));
  if (absSTOverTheta <= SpecFunc::ScalarEpsilon)
    result *= 1.0 + nuggetFactor_;
  return result;
}

Scalar FractionalBrownianMotionModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error : FractionalBrownianMotionModel::computeAsScalar(it, it) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;

  const Scalar sOverTheta = *s_begin / scale_[0];
  const Scalar tOverTheta = *t_begin / scale_[0];
  const Scalar stOverTheta = tOverTheta - sOverTheta;
  const Scalar absSOverTheta = std::abs(sOverTheta);
  const Scalar absTOverTheta = std::abs(tOverTheta);
  const Scalar absSTOverTheta = std::abs(stOverTheta);
  const Scalar Hi = exponent_[0];
  const Scalar sigmaI = amplitude_[0];
  Scalar result = 0.5 * sigmaI * sigmaI * (std::pow(absSOverTheta, 2.0 * Hi) + std::pow(absTOverTheta, 2.0 * Hi) - std::pow(absSTOverTheta, 2.0 * Hi));
  if (absSTOverTheta <= SpecFunc::ScalarEpsilon)
    result *= 1.0 + nuggetFactor_;
  return result;
}

/* Computation of the covariance function
 */
SquareMatrix FractionalBrownianMotionModel::operator() (const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  MatrixImplementation result(outputDimension_, outputDimension_);
  const Scalar sOverTheta = s[0] / scale_[0];
  const Scalar tOverTheta = t[0] / scale_[0];
  const Scalar stOverTheta = tOverTheta - sOverTheta;
  const Scalar absSOverTheta = std::abs(sOverTheta);
  const Scalar absTOverTheta = std::abs(tOverTheta);
  const Scalar absSTOverTheta = std::abs(stOverTheta);
  const Scalar sgnSOverTheta = sOverTheta > 0.0 ? 1.0 : -1.0;
  const Scalar sgnTOverTheta = tOverTheta > 0.0 ? 1.0 : -1.0;
  const Scalar sgnSTOverTheta = stOverTheta > 0.0 ? 1.0 : -1.0;
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
  {
    const Scalar Hi = exponent_[i];
    const Scalar sigmaI = amplitude_[i];
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      const Scalar Hij = Hi + exponent_[j];
      Scalar correlation = 0.0;
      Scalar asymmetry = 0.0;
      // To ensure a full double precision
      if (std::abs(Hij - 1.0) < 1e-7)
      {
        const Scalar sPowHij = (absSOverTheta == 0.0 ? 0.0 : absSOverTheta * std::log(absSOverTheta));
        const Scalar tPowHij = (absTOverTheta == 0.0 ? 0.0 : absTOverTheta * std::log(absTOverTheta));
        const Scalar stPowHij = (absSTOverTheta == 0.0 ? 0.0 : absSTOverTheta * std::log(absSTOverTheta));
        asymmetry = eta_(i, j) * (Hij - 1.0) * (sPowHij - tPowHij + stPowHij);
        if (!isDiagonal_)
          correlation = outputCorrelation_(i, j) * (absSOverTheta + absTOverTheta - absSTOverTheta);
      }
      else
      {
        const Scalar sPowHij = (absSOverTheta == 0.0 ? 0.0 : std::pow(absSOverTheta, Hij));
        const Scalar tPowHij = (absTOverTheta == 0.0 ? 0.0 : std::pow(absTOverTheta, Hij));
        const Scalar stPowHij = (absSTOverTheta == 0.0 ? 0.0 : std::pow(absSTOverTheta, Hij));
        asymmetry = eta_(i, j) * (sgnSOverTheta * sPowHij - sgnTOverTheta * tPowHij + sgnSTOverTheta * stPowHij);
        if (!isDiagonal_)
          correlation = outputCorrelation_(i, j) * (sPowHij + tPowHij - stPowHij);
      } // |Hij - 1| >= 1e-7
      result(i, j) = 0.5 * sigmaI * amplitude_[j] * (correlation + asymmetry);
    } // j
    result(i, i) = 0.5 * sigmaI * sigmaI * (std::pow(absSOverTheta, 2.0 * Hi) + std::pow(absTOverTheta, 2.0 * Hi) - std::pow(absSTOverTheta, 2.0 * Hi));
    if (absSTOverTheta <= SpecFunc::ScalarEpsilon) result(i, i) *= 1.0 + nuggetFactor_;
  } // i
  return result;
}

/* Gradient */
Matrix FractionalBrownianMotionModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  return CovarianceModelImplementation::partialGradient(s, t);
}

/* Exponent accessor */
void FractionalBrownianMotionModel::setExponentEtaRho(const Point & exponent,
    const SquareMatrix & eta,
    const CorrelationMatrix & rho)
{
  if (exponent.getDimension() != outputDimension_) throw InvalidArgumentException(HERE) << "Error: the exponent has dimension=" << exponent.getDimension() << ", expected dimension=" << outputDimension_;
  if (eta.getDimension() != outputDimension_) throw InvalidArgumentException(HERE) << "Error: the dissymetry matrix has dimension=" << eta.getDimension() << ", expected dimension=" << outputDimension_;
  if (rho.getDimension() != outputDimension_) throw InvalidArgumentException(HERE) << "Error: the correlation matrix has dimension=" << rho.getDimension() << ", expected dimension=" << outputDimension_;
  // First check the Hurst exponents
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    if (!((exponent[i] > 0.0) && (exponent[i] < 1.0))) throw InvalidArgumentException(HERE) << "Error: the components of the exponent must be in (0,1), here exponent[" << i << "]=" << exponent[i];
  // Second check the admissibility of the exponent, the dissymmetry and the correlation if outputDimension > 1
  if (outputDimension_ > 1)
  {
    HermitianMatrix A(outputDimension_);
    for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        const Scalar Hij = exponent[i] + exponent[j];
        A(i, j) = SpecFunc::Gamma(Hij + 1.0) * Complex(rho(i, j) * std::sin(M_PI_2 * Hij), -eta(i, j) * std::cos(M_PI_2 * Hij));
      } // j
      const Scalar Hi = 2.0 * exponent[i];
      A(i, i) = SpecFunc::Gamma(Hi + 1.0) * std::sin(M_PI_2 * Hi);
    } // i
    try
    {
      const TriangularComplexMatrix C(A.computeCholesky(false));
    }
    catch (const InvalidArgumentException &)
    {
      throw InvalidArgumentException(HERE) << "Error: the given Hurst exponents, dissymmetry matrix and correlation matrix are not compatible.";
    }
  } // outputDimension > 1
  exponent_ = exponent;
  eta_ = eta;
  // Ensure the antisymmetry of eta
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
  {
    for (UnsignedInteger j = 0; j < i; ++j)
      eta_(j, i) = -eta_(i, j);
    eta_(i, i) = 0.0;
  }
  setOutputCorrelation(rho);
}


Point FractionalBrownianMotionModel::getExponent() const
{
  return exponent_;
}


/* Dissymetry accessor */
SquareMatrix FractionalBrownianMotionModel::getEta() const
{
  return eta_;
}


/* Correlation accessor */
CorrelationMatrix FractionalBrownianMotionModel::getRho() const
{
  return outputCorrelation_;
}

void FractionalBrownianMotionModel::setFullParameter(const Point & parameter)
{
  const UnsignedInteger totalSize = inputDimension_ + outputDimension_ * (outputDimension_ + 1);
  if (!(parameter.getSize() >= totalSize))
    throw InvalidArgumentException(HERE) << "In FractionalBrownianMotionModel::setFullParameter, points have incompatible size. Point size = " << parameter.getSize()
                                         << " whereas expected size = " << totalSize ;
  CovarianceModelImplementation::setFullParameter(parameter);
  // Now the Hurst exponent
  UnsignedInteger index = inputDimension_ + outputDimension_ * (outputDimension_ + 1) / 2;
  Point exponent(outputDimension_);
  for (UnsignedInteger i = 0; i < outputDimension_; ++i, ++index)
    exponent[i] = parameter[index];
  SquareMatrix eta(outputDimension_);
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      eta(i, j) = parameter[index];
      ++index;
    }
  setExponentEtaRho(exponent, eta, getOutputCorrelation());
}

Point FractionalBrownianMotionModel::getFullParameter() const
{
  // Get the generic parameter
  Point parameter(CovarianceModelImplementation::getFullParameter());
  // Add the specific ones
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    parameter.add(exponent_[i]);
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      parameter.add(eta_(i, j));
  return parameter;
}

Description FractionalBrownianMotionModel::getFullParameterDescription() const
{
  // Description of the generic parameter
  Description description(CovarianceModelImplementation::getFullParameterDescription());
  // Description of the specific parameter
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    description.add(String(OSS() << "H_" << i));
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      description.add(String(OSS() << "eta_" << i << "_" << j));
  return description;
}

/* String converter */
String FractionalBrownianMotionModel::__repr__() const
{
  OSS oss;
  oss << "class=" << FractionalBrownianMotionModel::GetClassName()
      << ", exponent=" << exponent_
      << ", eta=" << eta_
      << ", rho=" << outputCorrelation_;
  return oss;
}

/* String converter */
String FractionalBrownianMotionModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << FractionalBrownianMotionModel::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ", exponent=" << exponent_;
  if (outputDimension_ > 1)
  {
    oss << ", eta=\n" << eta_
        << ", rho=\n" << getOutputCorrelation();
  }
  oss << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FractionalBrownianMotionModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("exponent_", exponent_);
  adv.saveAttribute("eta_", eta_);
}

/* Method load() reloads the object from the StorageManager */
void FractionalBrownianMotionModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("exponent_", exponent_);
  adv.loadAttribute("eta_", eta_);
}

END_NAMESPACE_OPENTURNS
