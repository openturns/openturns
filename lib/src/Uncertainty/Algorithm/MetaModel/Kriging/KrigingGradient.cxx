//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
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

#include "openturns/KrigingGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/AggregatedFunction.hxx"

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
KrigingGradient::KrigingGradient(const Basis & basis,
                                 const Sample & inputSample,
                                 const CovarianceModel & covarianceModel,
                                 const Point & beta,
                                 const Sample & gamma)
  : GradientImplementation()
  , basis_(basis)
  , inputSample_(inputSample)
  , covarianceModel_(covarianceModel)
  , beta_(beta)
  , gamma_(gamma)
{

  if (!basis.isFinite())
    throw InvalidArgumentException(HERE) << "In KrigingGradient::KrigingGradient, basis should be finite!";
  const UnsignedInteger size = basis.getSize();
  for (UnsignedInteger index = 0; index < size; ++index)
  {
    if (basis[index].getOutputDimension() != covarianceModel.getOutputDimension())
      throw InvalidArgumentException(HERE) << "In KrigingGradient::KrigingGradient, output sample dimension=" << covarianceModel.getOutputDimension() << " does not match basis[=" << index << "] dimension=" << basis[index].getOutputDimension();
    if (basis[index].getInputDimension() != inputSample_.getDimension())
      throw InvalidArgumentException(HERE) << "In KrigingGradient::KrigingGradient, input sample dimension=" << inputSample_.getDimension() << " does not match basis[=" << index << "] dimension=" << basis[index].getInputDimension();
  }
  if (covarianceModel.getInputDimension() != inputSample.getDimension())
    throw InvalidArgumentException(HERE) << "In KrigingGradient::KrigingGradient, error: the input dimension=" << covarianceModel.getInputDimension() << " of the covariance model should match the dimension=" << inputSample.getDimension() << " of the input sample";
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
  // Number of basis is 0 or outputDimension
  for (UnsignedInteger k = 0; k < basis_.getSize(); ++k)
  {
    const Matrix gradient_bk(basis_[k].gradient(inP));
    for (UnsignedInteger j = 0; j < dimension; ++j)
      for (UnsignedInteger i = 0; i < p; ++i)
        result(i, j) += gradient_bk(i, j) * beta_[k * dimension + j];
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
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("gamma_", gamma_);
  if (adv.getStudyVersion() >= 102100)
  {
    adv.loadAttribute("basis_", basis_);
    adv.loadAttribute("beta_", beta_);
  }
  else
  {
    // Backward load method
    // Here the implementation suggests that we rely on a collection of Basis and Point
    // Partially compatible with new implementation : we should have here the basis of
    // same size
    PersistentCollection<Basis> basis;
    PersistentCollection<Point> beta;
    adv.loadAttribute("basis_", basis);
    adv.loadAttribute("beta_", beta);
    if (basis.getSize() != covarianceModel_.getOutputDimension())
      throw InvalidArgumentException(HERE) << "Collection size differ from covariance model output dimension. Collection size="
                                           << basis.getSize() << " whereas covariance model output dimension = " << covarianceModel_.getOutputDimension();
    const UnsignedInteger basisSize = basis[0].getSize();
    for (UnsignedInteger outputMarginalIndex = 1; outputMarginalIndex < basis.getSize(); ++outputMarginalIndex)
    {
      if (basis[outputMarginalIndex].getSize() != basisSize)
        throw InvalidArgumentException(HERE) << "With new implementation, we should have all basis of same size. Here, basis[0].size = " << basisSize
                                             << " whereas basis[" << outputMarginalIndex << "].size = " << basis[outputMarginalIndex].getSize();
    }
    // Now we convert the persistent collection of Basis/Point into a standard multivariate Basis
    beta_ = Point(basisSize * basis.getSize());
    Collection<Function> marginalCollection(covarianceModel_.getOutputDimension());
    Collection<Function> phi(basisSize);
    UnsignedInteger index = 0;

    for (UnsignedInteger j = 0; j < basisSize; ++j)
    {
      for (UnsignedInteger outputMarginalIndex = 0; outputMarginalIndex < basis.getSize(); ++outputMarginalIndex)
      {
        marginalCollection[outputMarginalIndex] = basis[outputMarginalIndex].build(j);
        beta_[index] = beta[outputMarginalIndex][j];
      }
      phi[j] = AggregatedFunction(marginalCollection);
    }
    basis_ = Basis(phi);
  } // else
}


END_NAMESPACE_OPENTURNS
