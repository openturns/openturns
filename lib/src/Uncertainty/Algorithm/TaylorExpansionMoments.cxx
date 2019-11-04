//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMoments implements Taylor expansion for moments estimation
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
#include <iomanip>
#include "openturns/TaylorExpansionMoments.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/Function.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SobolIndicesAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS


typedef Pointer<RandomVectorImplementation> Implementation;

CLASSNAMEINIT(TaylorExpansionMoments)

static const Factory<TaylorExpansionMoments> Factory_TaylorExpansionMoments;

/*
 * @class TaylorExpansionMoments
 * TaylorExpansionMoments estimates the mean and covariance of a random vector
 Y=G(X) by computing the Taylor approximation of G
*/

TaylorExpansionMoments::TaylorExpansionMoments()
  : PersistentObject(),
    limitStateVariable_(CompositeRandomVector()),
    isAlreadyComputedValue_(false),
    isAlreadyComputedGradient_(false),
    isAlreadyComputedHessian_(false),
    isAlreadyComputedMeanFirstOrder_(false),
    isAlreadyComputedMeanSecondOrder_(false),
    isAlreadyComputedCovariance_(false),
    isAlreadyComputedImportanceFactors_(false)
{
  // Nothing to to do
}

/* Constructor with parameters */
TaylorExpansionMoments::TaylorExpansionMoments(const RandomVector & limitStateVariable)
  : PersistentObject(),
    limitStateVariable_(limitStateVariable),
    meanInputVector_(0),
    valueAtMean_(0),
    gradientAtMean_(0, 0),
    hessianAtMean_(0, 0),
    isAlreadyComputedValue_(false),
    isAlreadyComputedGradient_(false),
    isAlreadyComputedHessian_(false),
    isAlreadyComputedMeanFirstOrder_(false),
    isAlreadyComputedMeanSecondOrder_(false),
    isAlreadyComputedCovariance_(false),
    isAlreadyComputedImportanceFactors_(false),
    inputCovariance_(0),
    meanFirstOrder_(0),
    meanSecondOrder_(0),
    covariance_(0),
    importanceFactors_(0)
{
  /* Check if the given random vector is a composite random vector, which is mandatory */
  if (!limitStateVariable.isComposite()) throw InvalidArgumentException(HERE) << "Taylor algorithm requires a composite random vector as an input";
  /** Check if the given composite random vector is based on a function with a gradient implementation */
  if (!limitStateVariable.getImplementation()->getFunction().getGradient().getImplementation()->isActualImplementation()) throw InvalidArgumentException(HERE) << "Quadratic cumul algorithm requires a composite random vector based on a function with an actual gradient implementation";
  /** Check if the given composite random vector is based on a function with a gradient implementation */
  if (!limitStateVariable.getImplementation()->getFunction().getHessian().getImplementation()->isActualImplementation()) throw InvalidArgumentException(HERE) << "Quadratic cumul algorithm requires a composite random vector based on a function with an actual hessian implementation";
}

/* Virtual constructor */
TaylorExpansionMoments * TaylorExpansionMoments::clone() const
{
  return new TaylorExpansionMoments(*this);
}

/* String converter */
String TaylorExpansionMoments::__repr__() const
{
  return OSS() << "limitStateVariable=" << limitStateVariable_
         << " meanInputVector=" << meanInputVector_
         << " hessianAtMean=" << hessianAtMean_
         << " gradientAtMean=" << gradientAtMean_
         << " valueAtMean=" << valueAtMean_
         << " meanFirstOrder=" << meanFirstOrder_
         << " meanSecondOrder=" << meanSecondOrder_
         << " covariance=" << covariance_
         << " inputCovariance=" << inputCovariance_
         << " importanceFactors=" << importanceFactors_;
}

/* limitStateVariable accessor */
RandomVector TaylorExpansionMoments::getLimitStateVariable() const
{
  return limitStateVariable_;
}

/* meanFirstOrder accessor */
Point TaylorExpansionMoments::getMeanFirstOrder() const
{
  if(!isAlreadyComputedMeanFirstOrder_) computeMeanFirstOrder();
  return meanFirstOrder_;
}


/* meanSecondOrder accessor */
Point TaylorExpansionMoments::getMeanSecondOrder() const
{
  if(!isAlreadyComputedMeanSecondOrder_) computeMeanSecondOrder();
  return meanSecondOrder_;
}

/* covariance accessor */
CovarianceMatrix TaylorExpansionMoments::getCovariance() const
{
  if(!isAlreadyComputedCovariance_) computeCovariance();
  return covariance_;
}

/* importance factors accessor */
PointWithDescription TaylorExpansionMoments::getImportanceFactors() const
{
  if(!isAlreadyComputedImportanceFactors_) computeImportanceFactors();
  return importanceFactors_;
}

/* Value at mean accessor */
Point TaylorExpansionMoments::getValueAtMean() const
{
  return valueAtMean_;
}

/* Gradient at mean accessor */
Matrix TaylorExpansionMoments::getGradientAtMean() const
{
  return gradientAtMean_;
}

/* Hessian at mean accessor */
SymmetricTensor TaylorExpansionMoments::getHessianAtMean() const
{
  return hessianAtMean_;
}

/* ImportanceFactors graph */
Graph TaylorExpansionMoments::drawImportanceFactors() const
{
  // To ensure that the importance factors are up to date
  getImportanceFactors();
  OSS oss;
  oss << "Importance Factors from Taylor expansions - " << limitStateVariable_.getDescription()[0];
  return SobolIndicesAlgorithm::DrawImportanceFactors(importanceFactors_, oss.str());
}

/* the function that computes the first order evaluation of the mean vector and the matrix covariance */
void TaylorExpansionMoments::computeMeanFirstOrder() const
{
  if (!isAlreadyComputedValue_)
  {
    meanInputVector_ = limitStateVariable_.getImplementation()->getAntecedent().getMean();
    valueAtMean_ = limitStateVariable_.getImplementation()->getFunction().operator()(meanInputVector_);
    isAlreadyComputedValue_ = true;
  }
  /* we compute here the meanFirstOrder_ */
  meanFirstOrder_ = valueAtMean_;
  isAlreadyComputedMeanFirstOrder_ = true;
} // TaylorExpansionMoments::computeMeanFirstOrder()


/* the function that computes the first order evaluation of the mean vector and the matrix covariance */
void TaylorExpansionMoments::computeCovariance() const
{
  inputCovariance_ = limitStateVariable_.getImplementation()->getAntecedent().getCovariance();
  if (!isAlreadyComputedGradient_)
  {
    meanInputVector_ = limitStateVariable_.getImplementation()->getAntecedent().getMean();
    gradientAtMean_ = limitStateVariable_.getImplementation()->getFunction().gradient(meanInputVector_);
    isAlreadyComputedGradient_ = true;
  }

  /* we compute here the covariance_
     hessianAtMean_ is symmetrical
     The following straightforward implementation does'nt work because OT has no way to discover
     that the resulting matrice is square and symmetric only by checking the type of the arguments
     setCovariance(getGradientAtMean().transpose() * getInputCovariance() * getGradientAtMean());
     We use an intermediate rectangular matrix, then we make an element-wise copy of this matrix into
     the output covariance matrix */

  /* a matrix, in fact a square symmetric matrix but we don't know it from its C++ definition */
  /* outputCovariance is a pxp matrix */
  const UnsignedInteger inputDimension = gradientAtMean_.getNbRows();
  const UnsignedInteger outputDimension = gradientAtMean_.getNbColumns();
  covariance_ = CovarianceMatrix(outputDimension);
  /* we unroll the matrix multiplications transpose(gradient).covariance.gradient */
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
    {
      covariance_(i, j) = 0.0;
      for (UnsignedInteger k = 0; k < inputDimension; ++k)
        for (UnsignedInteger l = 0; l < inputDimension; ++l)
          covariance_(i, j) += gradientAtMean_(l, i) * inputCovariance_(l, k) * gradientAtMean_(k, j);
    }
  /* We check that outputCovariance is really symmetric positive definite */
  if (!covariance_.isPositiveDefinite()) LOGWARN(OSS() << "Warning! The covariance matrix computed by quadratic cumul is not positive definite outputCovariance=" << covariance_);
  isAlreadyComputedCovariance_ = true;
} // TaylorExpansionMoments::computeCovariance()

/* the function that computes the importance factors only in the scalar case */
void TaylorExpansionMoments::computeImportanceFactors() const
{
  /* importance factors are evaluated only if the outputRandomVector is scalar */
  /* this case is tested with the  dimension of limitStateVariable_ : it must be equal to 1 in the right case */
  if (limitStateVariable_.getDimension() != 1) throw InvalidDimensionException(HERE) << "The importance factors are computed only if the output random vector is scalar";
  computeCovariance();
  /* we compute here the importance factors */
  /* in this scalar case, gradientAtMean is a Point */
  const UnsignedInteger dimension = gradientAtMean_.getNbRows();

  /* in this scalar case, importance factors is a Point, defined as importanceFactors = gradient .* inputCovariance * gradient / outputCovariance, where .* means an element-wise multiplication between vectors */
  importanceFactors_ = PointWithDescription(dimension, 0.0);
  // This line looks strange, but it is here to ensure that the covariance has actually been computed
  getCovariance();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j) importanceFactors_[i] += inputCovariance_(i, j) * gradientAtMean_(j, 0);
    importanceFactors_[i] *= gradientAtMean_(i, 0) / covariance_(0, 0);
  }
  importanceFactors_.setDescription(limitStateVariable_.getImplementation()->getAntecedent().getDescription());
  isAlreadyComputedImportanceFactors_ = true;
} // TaylorExpansionMoments::computeImportanceFactors()

/* the function that computes the second order evaluation of the mean vector */
void TaylorExpansionMoments::computeMeanSecondOrder() const
{
  inputCovariance_ = limitStateVariable_.getImplementation()->getAntecedent().getCovariance();
  if (!isAlreadyComputedValue_)
  {
    meanInputVector_ = limitStateVariable_.getImplementation()->getAntecedent().getMean();
    valueAtMean_ = limitStateVariable_.getImplementation()->getFunction().operator()(meanInputVector_);
    isAlreadyComputedValue_ = true;
  }
  if (!isAlreadyComputedHessian_)
  {
    meanInputVector_ = limitStateVariable_.getImplementation()->getAntecedent().getMean();
    hessianAtMean_ = limitStateVariable_.getImplementation()->getFunction().hessian(meanInputVector_);
    isAlreadyComputedHessian_ = true;
  }
  /* we compute here the meanSecondOrder_ */
  /* tensoriel writting : setCovariance(getGradientAtMean() + 0.5*getHessianAtMean().dotdot(getInputCovariance())); */
  /* developped formula */

  const UnsignedInteger rowDimension = hessianAtMean_.getNbRows();
  /* i */
  const UnsignedInteger sheetDimension = hessianAtMean_.getNbSheets();
  /* k */
  meanSecondOrder_ = valueAtMean_;
  /* loop on k */
  for (UnsignedInteger k = 0; k < sheetDimension; ++k)
  {
    Scalar kSecondOrderContribution = 0.0;
    /* loop on i */
    for (UnsignedInteger i = 0; i < rowDimension; ++i)
    {
      kSecondOrderContribution += 0.5 * inputCovariance_(i, i) * hessianAtMean_(i, i, k);
      /* loop on j */
      for (UnsignedInteger j = 0; j < i; ++j) kSecondOrderContribution += inputCovariance_(i, j) * hessianAtMean_(i, j, k);
    }/* end loop on i */
    meanSecondOrder_[k] += kSecondOrderContribution;
  } /* end loop on k */
  isAlreadyComputedMeanSecondOrder_ = true;
} // TaylorExpansionMoments::computeMeanSecondOrder()

/* Method save() stores the object through the StorageManager */
void TaylorExpansionMoments::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "limitStateVariable_", limitStateVariable_ );
  adv.saveAttribute( "meanInputVector_", meanInputVector_ );
  adv.saveAttribute( "valueAtMean_", valueAtMean_ );
  adv.saveAttribute( "gradientAtMean_", gradientAtMean_ );
  adv.saveAttribute( "hessianAtMean_", hessianAtMean_ );
  adv.saveAttribute( "isAlreadyComputedValue_", isAlreadyComputedValue_ );
  adv.saveAttribute( "isAlreadyComputedGradient_", isAlreadyComputedGradient_ );
  adv.saveAttribute( "isAlreadyComputedHessian_", isAlreadyComputedHessian_ );
  adv.saveAttribute( "isAlreadyComputedMeanFirstOrder_", isAlreadyComputedMeanFirstOrder_ );
  adv.saveAttribute( "isAlreadyComputedMeanSecondOrder_", isAlreadyComputedMeanSecondOrder_ );
  adv.saveAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
  adv.saveAttribute( "isAlreadyComputedImportanceFactors_", isAlreadyComputedImportanceFactors_ );
  adv.saveAttribute( "inputCovariance_", inputCovariance_ );
  adv.saveAttribute( "meanFirstOrder_", meanFirstOrder_ );
  adv.saveAttribute( "meanSecondOrder_", meanSecondOrder_ );
  adv.saveAttribute( "covariance_", covariance_ );
  adv.saveAttribute( "importanceFactors_", importanceFactors_ );
}

/* Method load() reloads the object from the StorageManager */
void TaylorExpansionMoments::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "limitStateVariable_", limitStateVariable_ );
  adv.loadAttribute( "meanInputVector_", meanInputVector_ );
  adv.loadAttribute( "valueAtMean_", valueAtMean_ );
  adv.loadAttribute( "gradientAtMean_", gradientAtMean_ );
  adv.loadAttribute( "hessianAtMean_", hessianAtMean_ );
  adv.loadAttribute( "isAlreadyComputedValue_", isAlreadyComputedValue_ );
  adv.loadAttribute( "isAlreadyComputedGradient_", isAlreadyComputedGradient_ );
  adv.loadAttribute( "isAlreadyComputedHessian_", isAlreadyComputedHessian_ );
  adv.loadAttribute( "isAlreadyComputedMeanFirstOrder_", isAlreadyComputedMeanFirstOrder_ );
  adv.loadAttribute( "isAlreadyComputedMeanSecondOrder_", isAlreadyComputedMeanSecondOrder_ );
  adv.loadAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
  adv.loadAttribute( "isAlreadyComputedImportanceFactors_", isAlreadyComputedImportanceFactors_ );
  adv.loadAttribute( "inputCovariance_", inputCovariance_ );
  adv.loadAttribute( "meanFirstOrder_", meanFirstOrder_ );
  adv.loadAttribute( "meanSecondOrder_", meanSecondOrder_ );
  adv.loadAttribute( "covariance_", covariance_ );
  adv.loadAttribute( "importanceFactors_", importanceFactors_ );
}

END_NAMESPACE_OPENTURNS
