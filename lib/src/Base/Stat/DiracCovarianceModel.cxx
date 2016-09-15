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
#include "openturns/DiracCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/HMatrixFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiracCovarianceModel);

static const Factory<DiracCovarianceModel> Factory_DiracCovarianceModel;

class DiracAssemblyFunction : public HMatrixTensorRealAssemblyFunction
{
private:
  const CovarianceMatrix covarianceMatrix_;

public:
  DiracAssemblyFunction(const CovarianceMatrix & covariance)
    : HMatrixTensorRealAssemblyFunction(covariance.getDimension())
    , covarianceMatrix_(covariance)
  {
    // Nothing to do
  }

  void compute(UnsignedInteger i, UnsignedInteger j, Matrix* localValues) const
  {
    if (i == j)
    {
      std::copy( covarianceMatrix_.getImplementation()->begin(), covarianceMatrix_.getImplementation()->end(), localValues->getImplementation()->begin() );
    }
  }
};

/* Default constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension)
  , covarianceFactor_()
{
  // Compute the Cholesky factor of the spatial covariance
  CovarianceMatrix covariance(spatialCovariance_);
  if (nuggetFactor_ > 0.0)
    for (UnsignedInteger i = 0; i < covariance.getDimension(); ++i) covariance(i, i) *= (1.0 + nuggetFactor_);
  covarianceFactor_ = covariance.computeCholesky(false);
  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill(spatialDimension);
}

/* Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension,
                                           const NumericalPoint & amplitude)
  : StationaryCovarianceModel(NumericalPoint(spatialDimension, 1.0), amplitude)
  , covarianceFactor_()
{
  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill(spatialDimension);
  // Compute the Cholesky factor of the spatial covariance
  CovarianceMatrix covariance(spatialCovariance_);
  if (nuggetFactor_ > 0.0)
    for (UnsignedInteger i = 0; i < covariance.getDimension(); ++i) covariance(i, i) *= (1.0 + nuggetFactor_);
  covarianceFactor_ = covariance.computeCholesky(false);
}

/* Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension,
                                           const NumericalPoint & amplitude,
                                           const CorrelationMatrix & correlation)
  : StationaryCovarianceModel(NumericalPoint(spatialDimension, 1.0), amplitude, correlation)
  , covarianceFactor_()
{
  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill(spatialDimension);
  // Compute the Cholesky factor of the spatial covariance
  CovarianceMatrix covariance(spatialCovariance_);
  if (nuggetFactor_ > 0.0)
    for (UnsignedInteger i = 0; i < covariance.getDimension(); ++i) covariance(i, i) *= (1.0 + nuggetFactor_);
  covarianceFactor_ = covariance.computeCholesky(false);
}

  /* Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension,
                                           const CovarianceMatrix & spatialCovariance)
  : StationaryCovarianceModel(NumericalPoint(spatialDimension, 1.0), spatialCovariance)
  , covarianceFactor_()
{
  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill(spatialDimension);
  // Compute the Cholesky factor of the spatial covariance
  CovarianceMatrix covariance(spatialCovariance_);
  if (nuggetFactor_ > 0.0)
    for (UnsignedInteger i = 0; i < covariance.getDimension(); ++i) covariance(i, i) *= (1.0 + nuggetFactor_);
  covarianceFactor_ = covariance.computeCholesky(false);
}

/* Virtual constructor */
DiracCovarianceModel * DiracCovarianceModel::clone() const
{
  return new DiracCovarianceModel(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix DiracCovarianceModel::operator() (const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::operator(), the point tau has dimension=" << tau.getDimension() << ", expected dimension=" << spatialDimension_;
  // If tau.norm1 is zero we compute the covariance matrix
  // Otherwise the returned value is 0
  if (tau.norm() == 0)
    {
      if (nuggetFactor_ == 0.0) return spatialCovariance_;
      CovarianceMatrix covariance(spatialCovariance_);
      for (UnsignedInteger i = 0; i < dimension_; ++i) covariance(i, i) *= (1.0 + nuggetFactor_);
      return covariance;
    }
  return CovarianceMatrix(SquareMatrix(dimension_).getImplementation());
}

// The following structure helps to compute the full covariance matrix
struct DiracCovarianceModelDiscretizePolicy
{
  const NumericalSample & input_;
  CovarianceMatrix & output_;
  const CovarianceMatrix & covariance_;
  const UnsignedInteger dimension_;

  DiracCovarianceModelDiscretizePolicy(const NumericalSample & input,
                                       CovarianceMatrix & output,
                                       const CovarianceMatrix & covariance)
    : input_(input)
    , output_(output)
    , covariance_(covariance)
    , dimension_(covariance.getDimension())
  {
    // Nothing to do
  }

  inline void operator()(const TBB::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger indexBlock = index * dimension_;
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        for (UnsignedInteger i = 0; i < dimension_; ++i)
          output_(indexBlock + i, indexBlock + j) = covariance_(i, j);
    }
  }
}; /* end struct DiracCovarianceModelDiscretizePolicy */

CovarianceMatrix DiracCovarianceModel::discretize(const NumericalSample & vertices) const
{
  if (vertices.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input spatial dimension=" << spatialDimension_;

  if (vertices.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a size 0";

  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * dimension_;
  CovarianceMatrix covarianceMatrix(fullSize);

  const DiracCovarianceModelDiscretizePolicy policy( vertices, covarianceMatrix, operator()(NumericalPoint(spatialDimension_, 0.0)) );
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, size, policy );

  return covarianceMatrix;
}

// The following structure helps to compute the full covariance matrix
struct DiracCovarianceModelDiscretizeAndFactorizePolicy
{
  const NumericalSample & input_;
  TriangularMatrix & output_;
  const DiracCovarianceModel & model_;
  const UnsignedInteger dimension_;

  DiracCovarianceModelDiscretizeAndFactorizePolicy(const NumericalSample & input,
      TriangularMatrix & output,
      const DiracCovarianceModel & model)
    : input_(input)
    , output_(output)
    , model_(model)
    , dimension_(model.getDimension())
  {
    // Nothing to do
  }

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger indexBlock = index * dimension_;
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        for (UnsignedInteger i = j; i < dimension_; ++i)
          output_(indexBlock + i, indexBlock + j) = model_.covarianceFactor_(i, j);
    }
  }
}; /* end struct DiracCovarianceModelDiscretizeAndFactorizePolicy */

TriangularMatrix DiracCovarianceModel::discretizeAndFactorize(const NumericalSample & vertices) const
{
  if (vertices.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input spatial dimension=" << spatialDimension_;

  if (vertices.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a size 0";

  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * dimension_;
  TriangularMatrix covarianceFactor(fullSize);

  const DiracCovarianceModelDiscretizeAndFactorizePolicy policy( vertices, covarianceFactor, *this );
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, size, policy );

  return covarianceFactor;
}

NumericalSample DiracCovarianceModel::discretizeRow(const NumericalSample & vertices,
    const UnsignedInteger p) const
{
  if (vertices.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input dimension=" << spatialDimension_;
  if (vertices.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the given sample has a size 0";
  if (p >= vertices.getSize())
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the index p should be lower or equal to " << vertices.getSize() - 1
                                         << ", here, p=" << p;

  const UnsignedInteger size = vertices.getSize();
  NumericalSample result(size * dimension_, dimension_);
  for(UnsignedInteger j = 0; j < dimension_; ++j)
    for(UnsignedInteger i = j; i < dimension_; ++i)
      result[p * dimension_ + i][j] = spatialCovariance_(i, j);
  return result;
}

// discretize with use of HMatrix
HMatrix DiracCovarianceModel::discretizeHMatrix(const NumericalSample & vertices,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
#ifdef OPENTURNS_HAVE_HMAT
  HMatrixFactory hmatrixFactory;
  const NumericalScalar assemblyEpsilon = parameters.getAssemblyEpsilon();
  const NumericalScalar recompressionEpsilon = parameters.getRecompressionEpsilon();

  HMatrix covarianceHMatrix = hmatrixFactory.build(vertices, dimension_, true);
  // Set assembly & recompression epsilon
  covarianceHMatrix.getImplementation()->setKey("assembly-epsilon", OSS() << assemblyEpsilon);
  covarianceHMatrix.getImplementation()->setKey("recompression-epsilon", OSS() << recompressionEpsilon);
  // Update covariance matrix
  // Compute the covariance
  DiracAssemblyFunction dirac(operator()(NumericalPoint(spatialDimension_, 0.0)));
  covarianceHMatrix.assemble(dirac, 'L');
  return covarianceHMatrix;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

/* Gradient */
Matrix DiracCovarianceModel::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  // Gradient should be checked
  Matrix gradient(spatialDimension_, dimension_ * dimension_);
  return gradient;
}

/* Nugget factor accessor */
void DiracCovarianceModel::setNuggetFactor(const NumericalScalar nuggetFactor)
{
  if (!(nuggetFactor == nuggetFactor_))
    {
      CovarianceModelImplementation::setNuggetFactor(nuggetFactor);
      CovarianceMatrix covariance(spatialCovariance_);
      if (nuggetFactor > 0.0)
	for (UnsignedInteger i = 0; i < covariance.getDimension(); ++i)
	  covariance(i, i) *= (1.0 + nuggetFactor);
      covarianceFactor_ = covariance.computeCholesky(false);
    }
}

/* Parameters accessor */
void DiracCovarianceModel::setParameter(const NumericalPoint & parameters)
{
  if (parameters.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setParameter, parameters should be of size " << dimension_ << ", here, parameters dimension = " << parameters.getDimension();
  setAmplitude(parameters);
}

NumericalPoint DiracCovarianceModel::getParameter() const
{
  return amplitude_;
}

Description DiracCovarianceModel::getParameterDescription() const
{
  Description description(0);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    description.add(OSS() << "amplitude_" << j);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      description.add(OSS() << "R_" << i << "_" << j);
  return description;
}

/* Amplitude accessor */
void DiracCovarianceModel::setAmplitude(const NumericalPoint & amplitude)
{
  if (amplitude.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setAmplitude, amplitude vector should be of size " << dimension_
                                         << ", got dimension=" << amplitude.getDimension();
  if (!(amplitude == amplitude_))
    {
      // Check positivity of amplitude
      for (UnsignedInteger i = 0; i < dimension_; ++i)
	{
	  if (amplitude[i] <= 0)
	    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setAmplitude, the component " << i << " of the amplitude=" << amplitude[i] << " is not positive.";
	}
      // Update the Cholesky factor
      for (UnsignedInteger i = 0; i < dimension_; ++i)
	{
	  const NumericalScalar rho = std::sqrt(amplitude[i] / amplitude_[i]);
	  for (UnsignedInteger j = 0; j <= i; ++j)
	    covarianceFactor_(i, j) *= rho; 
	} // i
      amplitude_ = amplitude;
    } // amplitude != amplitude_
}

void DiracCovarianceModel::setSpatialCorrelation(const CorrelationMatrix & correlation)
{
  CovarianceModelImplementation::setSpatialCorrelation(correlation);
  CovarianceMatrix covariance(spatialCovariance_);
  if (nuggetFactor_ > 0.0)
    for (UnsignedInteger i = 0; i < covariance.getDimension(); ++i)
      covariance(i, i) *= (1.0 + nuggetFactor_);
  covarianceFactor_ = covariance.computeCholesky(false);
}

/* Is it a stationary model ? */
Bool DiracCovarianceModel::isStationary() const
{
  return true;
}

/* String converter */
String DiracCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << DiracCovarianceModel::GetClassName()
      << ", input dimension=" << spatialDimension_
      << ", amplitude=" << amplitude_
      << ", spatialCorelation=" << getSpatialCorrelation();
   return oss;
}

/* String converter */
String DiracCovarianceModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << DiracCovarianceModel::GetClassName();
  oss << "(amplitude=" << getAmplitude();
  if (!isDiagonal_)
    oss << ", spatial correlation=\n" << getSpatialCorrelation().__str__(offset);
  else
    oss << ", no spatial correlation";
  oss << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void DiracCovarianceModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute("covarianceFactor_", covarianceFactor_);
}

/* Method load() reloads the object from the StorageManager */
void DiracCovarianceModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute("covarianceFactor_", covarianceFactor_);
}

END_NAMESPACE_OPENTURNS
