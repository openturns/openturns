//                                               -*- C++ -*-
/**
 *  @brief The result of a linear model estimation
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
#include "openturns/LinearModelResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DatabaseFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/SampleImplementation.hxx"
#include "openturns/LeastSquaresMethod.hxx"

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
                                     const Point & coefficients,
                                     const String & formula,
                                     const Description & coefficientsNames,
                                     const Sample & sampleResiduals,
                                     const Sample & standardizedResiduals,
                                     const Point & diagonalGramInverse,
                                     const Point & leverages,
                                     const Point & cookDistances,
                                     const Scalar residualsVariance)
  : MetaModelResult(inputSample, outputSample, metaModel, Point(1, 0.0), Point(1, 0.0))
  , basis_(basis)
  , design_(design)
  , coefficients_(coefficients)
  , condensedFormula_(formula)
  , coefficientsNames_(coefficientsNames)
  , sampleResiduals_(sampleResiduals)
  , standardizedResiduals_(standardizedResiduals)
  , diagonalGramInverse_(diagonalGramInverse)
  , leverages_(leverages)
  , cookDistances_(cookDistances)
  , residualsVariance_(residualsVariance)
  , hasIntercept_(false)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In LinearModelResult::LinearModelResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
  // Check DoF >= 0
  const SignedInteger degreesOfFreedom = getDegreesOfFreedom();
  if (degreesOfFreedom < 0)
    throw InvalidArgumentException(HERE) << "Degrees of freedom is less than 0. Data size = " << outputSample.getSize()
                                         << ", basis size = " << coefficients_.getSize()
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
    const Scalar xMin = sample.getMin()[0];
    const Scalar xMax = sample.getMax()[0];
    if (xMin == xMax)
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
String LinearModelResult::__str__(const String & /*offset*/) const
{
  return __repr_markdown__();
}

String LinearModelResult::__repr_markdown__() const
{
  OSS oss(false);
  const UnsignedInteger inputDimension = basis_[0].getInputDimension();
  oss << getClassName() << "\n"
      << "- input dimension=" << inputDimension << "\n"
      << "- basis size=" << basis_.getSize() << "\n"
      << "- design matrix=" << design_.getNbRows() << " x " << design_.getNbColumns() << "\n"
      << "- coefficients=" << coefficients_.getDimension() << "\n"
      << "- formula=" << condensedFormula_ << "\n"
      << "- coefficients names=" << coefficientsNames_ << "\n"
      << "- residuals size=" << sampleResiduals_.getSize() << "\n"
      << "- standard residuals size=" << standardizedResiduals_.getSize() << "\n"
      << "- inverse Gram diagonal=" << diagonalGramInverse_ << "\n"
      << "- leverages size=" << leverages_.getSize() << "\n"
      << "- Cook's distances size=" << cookDistances_.getSize() << "\n"
      << "- residuals variance=" << residualsVariance_ << "\n"
      << "- has intercept=" << hasIntercept_ << "\n"
      << "- is model selection=" << involvesModelSelection_ << "\n";
  oss << "\n";
  return oss;
}

/* String converter */
String LinearModelResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " coefficients_=" << coefficients_
         << " formula=" << condensedFormula_
         << " basis size=" << basis_.getSize()
         << " design dimension=" << design_.getNbRows() << " x " << design_.getNbColumns()
         << " coefficients dimension=" << coefficients_.getDimension()
         << " coefficientsNames=" << coefficientsNames_
         << " sampleResiduals dimension=" << sampleResiduals_.getSize() << " x " << sampleResiduals_.getDimension()
         << " standardizedResiduals dimension=" << standardizedResiduals_.getSize() << " x " << standardizedResiduals_.getDimension()
         << " diagonalGramInverse dimension=" << diagonalGramInverse_.getDimension()
         << " leverages dimension=" << leverages_.getDimension()
         << " cookDistances dimension=" << cookDistances_.getDimension()
         << " residuals variance= " << residualsVariance_
         << " hasIntercept=" << hasIntercept_
         << " involvesModelSelection=" << involvesModelSelection_;
}

Basis LinearModelResult::getBasis() const
{
  return basis_;
}

Matrix LinearModelResult::getDesign() const
{
  return design_;
}

/* Fitted sample accessor */
Sample LinearModelResult::getFittedSample() const
{
  return metaModel_(inputSample_);
}

/* Formula accessor */
Point LinearModelResult::getCoefficients() const
{
  return coefficients_;
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
  const UnsignedInteger basisSize = coefficients_.getSize();
  return size - basisSize;
}

/** Noise distribution */
Normal LinearModelResult::getNoiseDistribution() const
{
  // Gaussian output
  const SignedInteger dof = getDegreesOfFreedom();
  if (dof <= 0)
    throw NotDefinedException(HERE) << "The noise variance is undefined when DOF is null";
  return Normal(0, std::sqrt(residualsVariance_));
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

Scalar LinearModelResult::getResidualsVariance() const
{
  return residualsVariance_;
}

/* R-squared test */
Scalar LinearModelResult::getRSquared() const
{
  // Get residuals and RSS
  const Sample residuals(getSampleResiduals());
  const Scalar RSS = residuals.computeRawMoment(2)[0];
  // get outputSample and SYY
  // See https://stats.stackexchange.com/questions/26176/removal-of-statistically-significant-intercept-term-increases-r2-in-linear-mo
  // In case there is no intercept, the convention for R^2 is to have the ratio
  // between sum of squared predicted over sum of squared real
  // values.
  const Sample outputSample(getOutputSample());
  Scalar SYY = 1.0;
  if (!hasIntercept_)
    SYY = outputSample.computeRawMoment(2)[0];
  else
    SYY = outputSample.computeCentralMoment(2)[0];
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

LeastSquaresMethod LinearModelResult::buildMethod() const
{
  LeastSquaresMethod leastSquaresMethod;
  leastSquaresMethod = LeastSquaresMethod::Build(ResourceMap::GetAsString("LinearModelAlgorithm-DecompositionMethod"), design_);
  const UnsignedInteger basisSize =  design_.getNbColumns();
  Indices addedIndices(basisSize);
  addedIndices.fill();
  Indices conservedIndices(0);
  Indices removedIndices(0);
  leastSquaresMethod.update(addedIndices, conservedIndices, removedIndices);
  return leastSquaresMethod;
}

/* involvesModelSelection accessor */
Bool LinearModelResult::involvesModelSelection() const
{
  return involvesModelSelection_;
}

/* involvesModelSelection accessor */
void LinearModelResult::setInvolvesModelSelection(const Bool involvesModelSelection)
{
  involvesModelSelection_ = involvesModelSelection;
}

/* Method save() stores the object through the StorageManager */
void LinearModelResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "design_", design_ );
  adv.saveAttribute( "coefficients_", coefficients_ );
  adv.saveAttribute( "condensedFormula_", condensedFormula_ );
  adv.saveAttribute( "coefficientsNames_", coefficientsNames_ );
  adv.saveAttribute( "sampleResiduals_", sampleResiduals_ );
  adv.saveAttribute( "standardizedResiduals_", standardizedResiduals_ );
  adv.saveAttribute( "diagonalGramInverse_", diagonalGramInverse_ );
  adv.saveAttribute( "leverages_", leverages_ );
  adv.saveAttribute( "cookDistances_", cookDistances_ );
  adv.saveAttribute( "residualsVariance_", residualsVariance_ );
  adv.saveAttribute( "involvesModelSelection_", involvesModelSelection_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearModelResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "design_", design_ );
  if (adv.hasAttribute("coefficients_"))
    adv.loadAttribute("coefficients_", coefficients_);
  else
    adv.loadAttribute("beta_", coefficients_);
  adv.loadAttribute( "condensedFormula_", condensedFormula_ );
  adv.loadAttribute( "coefficientsNames_", coefficientsNames_ );
  adv.loadAttribute( "sampleResiduals_", sampleResiduals_ );
  adv.loadAttribute( "standardizedResiduals_", standardizedResiduals_ );
  adv.loadAttribute( "diagonalGramInverse_", diagonalGramInverse_ );
  adv.loadAttribute( "leverages_", leverages_ );
  adv.loadAttribute( "cookDistances_", cookDistances_ );
  if (adv.hasAttribute("residualsVariance_"))
    adv.loadAttribute( "residualsVariance_", residualsVariance_ );
  else
    adv.loadAttribute( "sigma2_", residualsVariance_ );
  if (adv.hasAttribute("involvesModelSelection_"))
    adv.loadAttribute("involvesModelSelection_", involvesModelSelection_);
}

END_NAMESPACE_OPENTURNS
