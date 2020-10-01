//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/KrigingGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingGradient)


static const Factory<KrigingGradient> Factory_KrigingGradient;


/* Constructor with parameters */
KrigingGradient::KrigingGradient()
  : GradientImplementation()
  , basis_()
  , inputSample_()
  , covarianceModel_()
  , beta_()
  , gamma_()
{
  // Nothing to do here
}


/* Constructor with parameters */
KrigingGradient::KrigingGradient(const BasisCollection & basis,
                                 const Sample & inputSample,
                                 const CovarianceModel & covarianceModel,
                                 const PointCollection & beta,
                                 const Sample & gamma)
  : GradientImplementation()
  , basis_(basis)
  , inputSample_(inputSample)
  , covarianceModel_(covarianceModel)
  , beta_(beta)
  , gamma_(gamma)
{
  if (basis.getSize() > 0)
  {
    if (basis.getSize() != covarianceModel.getOutputDimension())
      throw InvalidArgumentException(HERE) << "In KrigingGradient::KrigingGradient, output sample dimension (" << covarianceModel.getOutputDimension()  << ") does not match multi-basis dimension (" << basis_.getSize() << ")";
    // Total basis size = sum of all sizes
    UnsignedInteger basisCollectionTotalSize = 0;
    for (UnsignedInteger i = 0; i < basis.getSize(); ++i) basisCollectionTotalSize += basis[i].getSize();
  }
  if (covarianceModel.getInputDimension() != inputSample.getDimension()) throw InvalidArgumentException(HERE) << "In KrigingGradient::KrigingGradient, error: the input dimension=" << covarianceModel.getInputDimension() << " of the covariance model should match the dimension=" << inputSample.getDimension() << " of the input sample";
  if (gamma.getSize() != inputSample.getSize()) throw InvalidArgumentException(HERE) << "In KrigingGradient::KrigingGradient, error: the number of covariance coefficients=" << gamma.getSize() << " is different from the output sample dimension=" << covarianceModel.getOutputDimension();
  setParameter(Point(getInputDimension()));
}

/* Virtual constructor */
KrigingGradient * KrigingGradient::clone() const
{
  return new KrigingGradient(*this);
}

/* Comparison operator */
Bool KrigingGradient::operator==(const KrigingGradient & ) const
{
  return true;
}

/* String converter */
String KrigingGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " correlationModel=" << covarianceModel_
      << " beta=" << beta_
      << " gamma=" << gamma_;
  return oss;
}

/* String converter */
String KrigingGradient::__str__(const String & ) const
{
  return OSS(false) << GetClassName();
}

Matrix KrigingGradient::gradient(const Point & inP) const
{
  const UnsignedInteger p = inP.getSize();
  if (p != getInputDimension())
    throw InvalidArgumentException(HERE) << "In KrigingGradient::gradient, input point should have the same dimension as the KrigingGradient input dimension. Here, inP dimension = " << inP.getSize()
                                         << " and KrigingGradient dimension = " << getInputDimension();

  const UnsignedInteger dimension = getOutputDimension();
  // output result
  Matrix result(p, dimension);

  // First part : compute the gradient of R * gamma
  // As there is a linear combination, we compute the gradient
  // by using the same sum, i.e \sum_{i=1}^{trainingSize} \nabla{C}(inP, inS[i]) * \gamma_i
  // Training size
  const UnsignedInteger trainingSize = inputSample_.getSize();
  for (UnsignedInteger i = 0; i < trainingSize; ++ i)
  {
    // Partial gradient is a matrix of size p x (dimension * dimension)
    const Matrix gradient_i( covarianceModel_.partialGradient(inP, inputSample_[i]) );
    for (UnsignedInteger j = 0; j < getInputDimension(); ++ j)
    {
      // Get the row of size dimension x dimension
      const Matrix gradient_i_j(dimension, dimension, *gradient_i.getRow(j).getImplementation());
      const Point localValue(gradient_i_j * gamma_[i]);
      for (UnsignedInteger k = 0; k < dimension; ++k) result(j, k) += localValue[k];
    }
  }

  // trend part
  const UnsignedInteger basisSize = basis_.getSize();

  // Evaluate the basis part sequentially
  // Number of basis is 0 or outputDimension
  for (UnsignedInteger i = 0; i < basisSize; ++i)
  {
    // Get local basis -> basis_[i]
    const Basis localBasis(basis_[i]);
    const Point betaBasis(beta_[i]);
    const UnsignedInteger localBasisSize = localBasis.getSize();
    // For the i-th Basis (marginal), take into account the trend
    // We write explicitely the linear combination instead of using a LinearCombinationGradient
    for (UnsignedInteger j = 0; j < localBasisSize; ++j)
    {
      const Matrix gradient_bj(localBasis[j].gradient(inP) * betaBasis[j]);
      for (UnsignedInteger k = 0; k < p; ++k) result(k, i) += gradient_bj(k, 0);
    }
  }

  // update calls number
  callsNumber_.fetchAndAdd(p);
  return result;
}


/* Accessor for input point dimension */
UnsignedInteger KrigingGradient::getInputDimension() const
{
  return inputSample_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger KrigingGradient::getOutputDimension() const
{
  return covarianceModel_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void KrigingGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("gamma_", gamma_);
}

/* Method load() reloads the object from the StorageManager */
void KrigingGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("beta_", beta_);
  adv.loadAttribute("gamma_", gamma_);
}


END_NAMESPACE_OPENTURNS
