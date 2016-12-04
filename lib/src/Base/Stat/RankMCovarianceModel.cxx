//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/RankMCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RankMCovarianceModel);

static const Factory<RankMCovarianceModel> Factory_RankMCovarianceModel;


/* Default constructor */
RankMCovarianceModel::RankMCovarianceModel(const UnsignedInteger spatialDimension)
  : CovarianceModelImplementation(spatialDimension)
  , coefficients_(0)
  , basis_()
  , functions_(0)
  , covariance_(0)
{
  // Nothing to do
}

/* Parameters constructor */
RankMCovarianceModel::RankMCovarianceModel(const NumericalPoint & coefficients,
    const Basis & basis)
  : CovarianceModelImplementation()
  , coefficients_(coefficients)
  , basis_()
  , functions_(0)
  , covariance_(0)
{
  setBasis(basis);
}

/* Parameters constructor */
RankMCovarianceModel::RankMCovarianceModel(const NumericalPoint & coefficients,
    const Basis & basis,
    const CovarianceMatrix & covariance)
  : CovarianceModelImplementation()
  , coefficients_(0)
  , basis_()
  , functions_(0)
  , covariance_(0)
{
  if (coefficients.getDimension() != covariance.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a covariance of dimension=" << coefficients.getDimension() << ", got dimension=" << covariance.getDimension();
  // Check if the covariance is diagonal
  coefficients_ = coefficients;
  if (covariance.isDiagonal())
  {
    for (UnsignedInteger i = 0; i < coefficients.getDimension(); ++i)
      coefficients_[i] *= covariance(i, i);
  }
  else covariance_ = covariance;
  setBasis(basis);
}

/* Virtual constructor */
RankMCovarianceModel * RankMCovarianceModel::clone() const
{
  return new RankMCovarianceModel(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix RankMCovarianceModel::operator() (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;

  MatrixImplementation result(dimension_, dimension_);
  const UnsignedInteger size = functions_.getSize();
  // If the coefficients are uncorrelated
  if (covariance_.getDimension() == 0)
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const MatrixImplementation phiS(dimension_, 1, functions_[i](s));
      const MatrixImplementation phiT(1, dimension_, functions_[i](t) * coefficients_[i]);
      std::cerr << "i=" << i << ", phiS=" << phiS.transpose() << ", phiT=" << phiT << std::endl;
      result += phiS.genProd(phiT);
      std::cerr << "result=" << result << std::endl;
    }
  else
  {
    NumericalPoint sqrtCoefficients(size);
    for (UnsignedInteger i = 0; i < size; ++i) sqrtCoefficients[i] = std::sqrt(coefficients_[i]);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const MatrixImplementation phiS(dimension_, 1, functions_[i](s) * sqrtCoefficients[i]);
      for (UnsignedInteger j = 0; j < size; ++j)
      {
        const MatrixImplementation phiT(dimension_, 1, functions_[j](t) * sqrtCoefficients[j]);
        result += phiS.genProd(phiT) * covariance_(i, j);
      } // j
    } // i
  } // covariance dimension > 0
  return result;
}

/* Gradient */
Matrix RankMCovarianceModel::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;

  throw NotYetImplementedException(HERE);
}

CovarianceMatrix RankMCovarianceModel::discretize(const NumericalSample & vertices) const
{
  if (vertices.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << spatialDimension_;
  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * dimension_;
  std::cerr << "fullSize=" << fullSize << std::endl;
  std::cerr << "vertices=" << vertices << std::endl;
  MatrixImplementation covarianceMatrix(fullSize, fullSize);
  // Precompute the discretizations of the functions over the vertices
  const UnsignedInteger basisSize = functions_.getSize();
  Collection<MatrixImplementation> basisDiscretization(basisSize);
  // If the coefficients are uncorrelated
  if (covariance_.getDimension() == 0)
    {
      std::cerr << "No correlation" << std::endl;
      for (UnsignedInteger i = 0; i < basisSize; ++i)
	{
	  const NumericalSample data(functions_[i](vertices));
	  std::cerr << "i=" << i << ", data=" << data << std::endl;
	  basisDiscretization[i] = MatrixImplementation(fullSize, 1, data.getImplementation()->getData()) * std::sqrt(coefficients_[i]);
	  std::cerr << "i=" << i << ", basisDiscretization[i]=" << basisDiscretization[i] << std::endl;	  
	}
      for (UnsignedInteger i = 0; i < basisSize; ++i)
	  covarianceMatrix += basisDiscretization[i].genProd(basisDiscretization[i], false, true);
      return covarianceMatrix;
    }
  std::cerr << "Correlation" << std::endl;
  const TriangularMatrix cholesky(const_cast<CovarianceMatrix&>(covariance_).computeCholesky());
  std::cerr << "cholesky=" << cholesky << std::endl;
  for (UnsignedInteger i = 0; i < basisSize; ++i)
    basisDiscretization[i] = MatrixImplementation(fullSize, 1, (functions_[i](vertices) * cholesky).getImplementation()->getData() * std::sqrt(coefficients_[i]));
  for (UnsignedInteger i = 0; i < basisSize; ++i)
    for (UnsignedInteger j = 0; j < basisSize; ++j)
      covarianceMatrix += basisDiscretization[i].genProd(basisDiscretization[j], false, true);
  return covarianceMatrix;
}

/* Is it a stationary model ? */
Bool RankMCovarianceModel::isStationary() const
{
  return false;
}

/* Basis accessor */
const Basis & RankMCovarianceModel::getBasis() const
{
  return basis_;
}

const Basis::NumericalMathFunctionCollection & RankMCovarianceModel::getFunctions() const
{
  return functions_;
}

/* Basis accessor */
void RankMCovarianceModel::setBasis(const Basis & basis)
{
  const UnsignedInteger size = coefficients_.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: there must be at least one coefficient";
  functions_ = Basis::NumericalMathFunctionCollection(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    functions_[i] = basis.build(i);
  scale_ = NumericalPoint(functions_[0].getInputDimension(), 1.0);
  amplitude_ = NumericalPoint(functions_[0].getOutputDimension(), 1.0);
  basis_ = basis;
}

/* String converter */
String RankMCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << RankMCovarianceModel::GetClassName()
      << ", coefficients=" << coefficients_
      << ", basis=" << basis_
      << ", functions=" << functions_
      << ", covariance=" << covariance_;
  return oss;
}

/* String converter */
String RankMCovarianceModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class=" << RankMCovarianceModel::GetClassName()
      << ", coefficients=" << coefficients_
      << ", basis=" << basis_
      << ", functions=" << functions_;
  if (covariance_.getDimension() > 0) oss << ", covariance=" << covariance_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RankMCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("coefficients_", coefficients_);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("functions_", functions_);
  adv.saveAttribute("covariance_", covariance_);
}

/* Method load() reloads the object from the StorageManager */
void RankMCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("coefficients_", coefficients_);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("functions_", functions_);
  adv.loadAttribute("covariance_", covariance_);
}

END_NAMESPACE_OPENTURNS
