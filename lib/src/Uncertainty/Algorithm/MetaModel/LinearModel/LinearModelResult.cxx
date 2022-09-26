//                                               -*- C++ -*-
/**
 *  @brief The result of a linear model estimation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LinearModelResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DatabaseFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/SampleImplementation.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearModelResult)
static const Factory<LinearModelResult> Factory_LinearModelResult;

/* Default constructor */
LinearModelResult::LinearModelResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Parameter constructor */
LinearModelResult::LinearModelResult(const Sample & inputSample,
                                     const Basis & basis,
                                     const Matrix & design,
                                     const Sample & outputSample,
                                     const Function & metaModel,
                                     const Point & trendCoefficients,
                                     const String & formula,
                                     const Description & coefficientsNames,
                                     const Sample & sampleResiduals,
                                     const Sample & standardizedResiduals,
                                     const Point & diagonalGramInverse,
                                     const Point & leverages,
                                     const Point & cookDistances,
                                     const Scalar sigma2)
  : MetaModelResult(inputSample, outputSample, DatabaseFunction(inputSample, outputSample), metaModel, Point(1, 0.0), Point(1, 0.0))
  , basis_(basis)
  , design_(design)
  , beta_(trendCoefficients)
  , condensedFormula_(formula)
  , coefficientsNames_(coefficientsNames)
  , sampleResiduals_(sampleResiduals)
  , standardizedResiduals_(standardizedResiduals)
  , diagonalGramInverse_(diagonalGramInverse)
  , leverages_(leverages)
  , cookDistances_(cookDistances)
  , sigma2_(sigma2)
  , hasIntercept_(false)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In LinearModelResult::LinearModelResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
  // Check DoF >= 0
  const SignedInteger degreesOfFreedom = getDegreesOfFreedom();
  if (degreesOfFreedom < 0)
    throw InvalidArgumentException(HERE) << "Degrees of freedom is less than 0. Data size = " << outputSample.getSize()
                                         << ", basis size = " << beta_.getSize()
                                         << ", degrees of freedom = " << degreesOfFreedom;
  checkIntercept();
}

/* Virtual constructor */
LinearModelResult * LinearModelResult::clone() const
{
  return new LinearModelResult(*this);
}

void LinearModelResult::checkIntercept()
{
  // Check the presence of intercept in the design
  // As we might set any arbitrary basis, there is no notion of formula
  // TODO : add attribute intercept_ in LinearModelAlgorithm
  //  We check if there is a constant column (std = 0)
  const UnsignedInteger size = design_.getNbRows();
  const UnsignedInteger p = design_.getNbColumns();
  MatrixImplementation design(*design_.getImplementation());
  SampleImplementation sample(size, 1);
  Bool cont = true;
  UnsignedInteger j = 0;
  while((j < p) && cont)
  {
    const Point column(design.getColumn(j));
    sample.setData(column);
    const Scalar min = sample.getMin()[0];
    const Scalar max = sample.getMax()[0];
    if (min == max)
    {
      cont = false;
      hasIntercept_ = true;
    }
    j++;
  }
}

Bool LinearModelResult::hasIntercept() const
{
  return hasIntercept_;
}

/* String converter */
String LinearModelResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " beta=" << beta_
         << " formula=" << condensedFormula_;
}

Basis LinearModelResult::getBasis() const
{
  return basis_;
}

/* Fitted sample accessor */
Sample LinearModelResult::getFittedSample() const
{
  return metaModel_(inputSample_);
}

/* Formula accessor */
Point LinearModelResult::getCoefficients() const
{
  return beta_;
}

/* Formula accessor */
String LinearModelResult::getFormula() const
{
  return condensedFormula_;
}

Description LinearModelResult::getCoefficientsNames() const
{
  return coefficientsNames_;
}

Sample LinearModelResult::getSampleResiduals() const
{
  return sampleResiduals_;
}

/* Number of degrees of freedom */
SignedInteger LinearModelResult::getDegreesOfFreedom() const
{
  const UnsignedInteger size = inputSample_.getSize();
  const UnsignedInteger basisSize = beta_.getSize();
  return size - basisSize;
}

/** Noise distribution */
Normal LinearModelResult::getNoiseDistribution() const
{
  // Gaussian output
  const SignedInteger dof = getDegreesOfFreedom();
  if (dof <= 0)
    throw NotDefinedException(HERE) << "The noise variance is undefined when DOF is null";
  return Normal(0, std::sqrt(sigma2_));
}

Sample LinearModelResult::getStandardizedResiduals() const
{
  return standardizedResiduals_;
}

Point LinearModelResult::getLeverages() const
{
  return leverages_;
}

Point LinearModelResult::getDiagonalGramInverse() const
{
  return diagonalGramInverse_;
}

Point LinearModelResult::getCookDistances() const
{
  return cookDistances_;
}

/* R-squared test */
Scalar LinearModelResult::getRSquared() const
{
  // Get residuals and RSS
  const Sample residuals(getSampleResiduals());
  const Scalar RSS = residuals.computeRawMoment(2)[0];
  // get outputSample and SYY
  // See https://stats.stackexchange.com/questions/26176/removal-of-statistically-significant-intercept-term-increases-r2-in-linear-mo
  // In case there is no intercept convention for R^2 is to have the ration between sum of squared predicted over sum of squared real
  // values.
  const Sample outputSample(getOutputSample());
  Scalar SYY = 1.0;
  if (!hasIntercept_)
    SYY = outputSample.computeRawMoment(2)[0];
  else
    SYY = outputSample.computeCenteredMoment(2)[0];
  const Scalar rSquared = 1.0 - RSS / SYY;
  return rSquared;
}

/* Adjusted R-squared test */
Scalar LinearModelResult::getAdjustedRSquared() const
{
  const SignedInteger dof = getDegreesOfFreedom();
  if (dof <= 0)
    throw NotDefinedException(HERE) << "The adjusted R2 is undefined with a null DOF";
  const UnsignedInteger size = getSampleResiduals().getSize();
  const Scalar R2  = getRSquared();
  Scalar aR2 = (1.0 - R2);
  if (hasIntercept_)
    aR2 *= size - 1;
  else
    aR2 *= size;
  return 1 - aR2 / dof;
}

Point LinearModelResult::getCoefficientsStandardErrors() const
{
  const Scalar sigma2 = getNoiseDistribution().getCovariance()(0, 0);
  const Point diagGramInv(getDiagonalGramInverse());
  const UnsignedInteger basisSize = diagGramInv.getSize();
  Point standardErrors(basisSize, 1);
  for (UnsignedInteger i = 0; i < standardErrors.getSize(); ++i)
  {
    standardErrors[i] = std::sqrt(std::abs(sigma2 * diagGramInv[i]));
  }
  return standardErrors;
}

/* Method save() stores the object through the StorageManager */
void LinearModelResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "design_", design_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "condensedFormula_", condensedFormula_ );
  adv.saveAttribute( "coefficientsNames_", coefficientsNames_ );
  adv.saveAttribute( "sampleResiduals_", sampleResiduals_ );
  adv.saveAttribute( "standardizedResiduals_", standardizedResiduals_ );
  adv.saveAttribute( "diagonalGramInverse_", diagonalGramInverse_ );
  adv.saveAttribute( "leverages_", leverages_ );
  adv.saveAttribute( "cookDistances_", cookDistances_ );
  adv.saveAttribute( "sigma2_", sigma2_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearModelResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "design_", design_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "condensedFormula_", condensedFormula_ );
  adv.loadAttribute( "coefficientsNames_", coefficientsNames_ );
  adv.loadAttribute( "sampleResiduals_", sampleResiduals_ );
  adv.loadAttribute( "standardizedResiduals_", standardizedResiduals_ );
  adv.loadAttribute( "diagonalGramInverse_", diagonalGramInverse_ );
  adv.loadAttribute( "leverages_", leverages_ );
  adv.loadAttribute( "cookDistances_", cookDistances_ );
  adv.loadAttribute( "sigma2_", sigma2_ );
}

END_NAMESPACE_OPENTURNS
