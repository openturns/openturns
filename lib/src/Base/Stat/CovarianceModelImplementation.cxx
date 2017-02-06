//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/HMatrixFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModelImplementation
 */

CLASSNAMEINIT(CovarianceModelImplementation);

static const Factory<CovarianceModelImplementation> Factory_CovarianceModelImplementation;

/* Dimension-based constructor */
CovarianceModelImplementation::CovarianceModelImplementation(const UnsignedInteger spatialDimension)
  : PersistentObject()
  , scale_(spatialDimension, 1.0)
  , spatialDimension_(spatialDimension)
  , amplitude_(1, 1.0)
  , dimension_(1)
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(spatialDimension_ + dimension_)
{
  activeParameter_.fill();
  updateSpatialCovariance();
}

/** Standard constructor with scale and amplitude scale parameter parameter */
CovarianceModelImplementation::CovarianceModelImplementation(const NumericalPoint & scale,
    const NumericalPoint & amplitude)
  : PersistentObject()
  , scale_(0)
  , spatialDimension_(scale.getDimension())
  , amplitude_(0)
  , dimension_(amplitude.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(spatialDimension_ + (dimension_ * (dimension_ + 1)) / 2)
{
  setAmplitude(amplitude);
  setScale(scale);
  activeParameter_.fill();
  updateSpatialCovariance();
}

/** Standard constructor with scale, amplitude and spatial correlation parameter parameter */
CovarianceModelImplementation::CovarianceModelImplementation(const NumericalPoint & scale,
    const NumericalPoint & amplitude,
    const CorrelationMatrix & spatialCorrelation)
  : PersistentObject()
  , scale_(0)
  , spatialDimension_(scale.getDimension())
  , amplitude_(0)
  , dimension_(amplitude.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(spatialDimension_ + (dimension_ * (dimension_ + 1)) / 2)
{
  setAmplitude(amplitude);
  setScale(scale);
  activeParameter_.fill();
  if (spatialCorrelation.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::CovarianceModelImplementation, the given spatial correlation has a dimension different from the scales and amplitudes.";
  setSpatialCorrelation(spatialCorrelation);
}

/** Standard constructor with scale and spatial covariance parameter parameter */
CovarianceModelImplementation::CovarianceModelImplementation(const NumericalPoint & scale,
    const CovarianceMatrix & spatialCovariance)
  : PersistentObject()
  , scale_(0)
  , spatialDimension_(scale.getDimension())
  , amplitude_(0)
  , dimension_(spatialCovariance.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(spatialDimension_ + (dimension_ * (dimension_ + 1)) / 2)
{
  // spatialCovariance
  spatialCovariance_ = spatialCovariance;
  NumericalPoint amplitude(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) amplitude[i] = sqrt(spatialCovariance(i, i));
  // Check that the amplitudes are valid
  setAmplitude(amplitude);
  // Convert the spatial covariance into a spatial correlation
  if (!spatialCovariance.isDiagonal())
  {
    spatialCorrelation_ = CorrelationMatrix(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      for (UnsignedInteger j = 0; j < i; ++j)
        spatialCorrelation_(i, j) = spatialCovariance(i, j) / (amplitude[i] * amplitude[j]);
  } // !isDiagonal
  setScale(scale);
  activeParameter_.fill();
}

/* Virtual constructor */
CovarianceModelImplementation * CovarianceModelImplementation::clone() const
{
  return new CovarianceModelImplementation(*this);
}

/* Dimension accessors */
UnsignedInteger CovarianceModelImplementation::getDimension() const
{
  return dimension_;
}

UnsignedInteger CovarianceModelImplementation::getSpatialDimension() const
{
  return spatialDimension_;
}

CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalScalar s,
    const NumericalScalar t) const
{
  return operator() (NumericalPoint(1, s), NumericalPoint(1, t));
}

CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  const NumericalScalar rho = computeStandardRepresentative(s, t);
  return CovarianceMatrix((spatialCovariance_ * rho).getImplementation());
}

// compute standard representative computes the term \rho(s, t)
NumericalScalar CovarianceModelImplementation::computeStandardRepresentative(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeStandardRepresentative(const NumericalPoint & s, const NumericalPoint & t) const";
}

NumericalScalar CovarianceModelImplementation::computeStandardRepresentative(const NumericalScalar & s,
    const NumericalScalar & t) const
{
  return computeStandardRepresentative(NumericalPoint(1, s), NumericalPoint(1, t));
}

NumericalScalar CovarianceModelImplementation::computeStandardRepresentative(const NumericalPoint & tau) const
{
  return computeStandardRepresentative(NumericalPoint(dimension_), tau);
}

NumericalScalar CovarianceModelImplementation::computeStandardRepresentative(const NumericalScalar & tau) const
{
  return computeStandardRepresentative(NumericalPoint(1, tau));
}


NumericalScalar CovarianceModelImplementation::computeAsScalar (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (dimension_ != 1) throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << dimension_ << ", expected dimension=1.";
  return (*this)(s, t)(0, 0);
}

NumericalScalar CovarianceModelImplementation::computeAsScalar(const NumericalScalar s,
    const NumericalScalar t) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar(const NumericalScalar s, const NumericalScalar t) const";
}

/* Computation of the covariance function */
CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalScalar tau) const
{
  return operator() (NumericalPoint(1, tau));
}

CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalPoint & tau) const
{
  return operator() (NumericalPoint(tau.getDimension()), tau);
}

NumericalScalar CovarianceModelImplementation::computeAsScalar (const NumericalPoint & tau) const
{
  if (dimension_ != 1) throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << dimension_ << ", expected dimension=1.";
  return (*this)(tau)(0, 0);
}

NumericalScalar CovarianceModelImplementation::computeAsScalar(const NumericalScalar tau) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar(const NumericalScalar tau) const";
}

/* Gradient */
Matrix CovarianceModelImplementation::partialGradient (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  Matrix gradient(spatialDimension_, dimension_ * dimension_);
  CovarianceMatrix covarianceST(operator()(s, t));
  // Convert result into MatrixImplementation to symmetrize & get the collection
  MatrixImplementation covarianceSTImplementation(*covarianceST.getImplementation());
  covarianceSTImplementation.symmetrize();
  const NumericalPoint centralValue(covarianceSTImplementation);
  const NumericalScalar epsilon = std::sqrt(SpecFunc::NumericalScalarEpsilon);
  // Loop over the shifted points
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i)
  {
    NumericalPoint currentPoint(s);
    currentPoint[i] += epsilon;
    CovarianceMatrix localCovariance(operator()(currentPoint, t));
    MatrixImplementation localCovarianceImplementation(*localCovariance.getImplementation());
    localCovarianceImplementation.symmetrize();
    const NumericalPoint currentValue(localCovarianceImplementation);
    for (UnsignedInteger j = 0; j < centralValue.getDimension(); ++j)
      gradient(i, j) = (currentValue[j] - centralValue[j]) / epsilon;
  }
  return gradient;
}

/* Gradient */
Matrix CovarianceModelImplementation::parameterGradient(const NumericalPoint & s,
                                                        const NumericalPoint & t) const
{
  const NumericalPoint parameter(getParameter());
  const UnsignedInteger size = parameter.getSize();
  const NumericalScalar epsilon = std::sqrt(SpecFunc::NumericalScalarEpsilon);
  Matrix gradient(size, (dimension_ * (dimension_ + 1)) / 2);
  CovarianceMatrix covRef = operator()(s, t);
  Pointer<CovarianceModelImplementation> p_implementation(clone());
  for (UnsignedInteger k = 0; k < size; ++ k)
  {
    NumericalPoint parameterP(parameter);
    parameterP[k] += epsilon;
    p_implementation->setParameter(parameterP);
    CovarianceMatrix covP = p_implementation->operator()(s, t);
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < dimension_; ++ j)
    {
      for (UnsignedInteger i = 0; i <= j; ++ i)
      {
        gradient(k, index) = (covP(i, j) - covRef(i, j)) / epsilon;
        ++ index;
      }
    }
  }
  return gradient;
}

/* Discretize the covariance function on a given TimeGrid/Mesh */
CovarianceMatrix CovarianceModelImplementation::discretize(const RegularGrid & timeGrid) const
{
  return discretize(timeGrid.getVertices());
}

struct CovarianceModelDiscretizePolicy
{
  const NumericalSample & input_;
  CovarianceMatrix & output_;
  const CovarianceModelImplementation & model_;
  const UnsignedInteger dimension_;

  CovarianceModelDiscretizePolicy(const NumericalSample & input,
                                  CovarianceMatrix & output,
                                  const CovarianceModelImplementation & model)
    : input_(input)
    , output_(output)
    , model_(model)
    , dimension_(model.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger jLocal = static_cast< UnsignedInteger >(sqrt(2.0 * i + 0.25) - 0.5);
      const UnsignedInteger jBase = jLocal * dimension_;
      const UnsignedInteger iLocal = i - (jLocal * (jLocal + 1)) / 2;
      const UnsignedInteger iBase = iLocal * dimension_;
      const CovarianceMatrix localCovariance(model_(input_[iLocal], input_[jLocal]));
      for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
          output_(iBase + ii, jBase + jj) = localCovariance(ii, jj);
    }
  }

}; /* end struct CovarianceModelDiscretizePolicy */


CovarianceMatrix CovarianceModelImplementation::discretize(const NumericalSample & vertices) const
{
  if (vertices.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << spatialDimension_;
  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * dimension_;
  CovarianceMatrix covarianceMatrix(fullSize);
  const CovarianceModelDiscretizePolicy policy( vertices, covarianceMatrix, *this );
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, size * (size + 1) / 2, policy );
  return covarianceMatrix;
}

CovarianceMatrix CovarianceModelImplementation::discretize(const Mesh & mesh) const
{
  return discretize(mesh.getVertices());
}

/** Discretize and factorize the covariance function on a given TimeGrid/Mesh */
TriangularMatrix CovarianceModelImplementation::discretizeAndFactorize(const RegularGrid & timeGrid) const
{
  return discretizeAndFactorize(timeGrid.getVertices());
}

TriangularMatrix CovarianceModelImplementation::discretizeAndFactorize(const Mesh & mesh) const
{
  return discretizeAndFactorize(mesh.getVertices());
}

TriangularMatrix CovarianceModelImplementation::discretizeAndFactorize(const NumericalSample & vertices) const
{
  // We suppose that covariance matrix is symmetric positive definite
  // We do not catch InternalException
  // Incremeant nugget factor to make matrix positive definite
  CovarianceMatrix covariance = discretize(vertices);
  TriangularMatrix choleskyFactor = covariance.computeCholesky();
  return choleskyFactor;
}


struct CovarianceModelScalarDiscretizeRowPolicy
{
  const NumericalSample & input_;
  const NumericalPoint p_;
  NumericalSample & output_;
  const CovarianceModelImplementation & model_;

  CovarianceModelScalarDiscretizeRowPolicy(const NumericalSample & input,
      const UnsignedInteger p,
      NumericalSample & output,
      const CovarianceModelImplementation & model)
    : input_(input)
    , p_(input[p])
    , output_(output)
    , model_(model)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i][0] = model_.computeAsScalar(p_, input_[i]);
  }

}; /* end struct CovarianceModelScalarDiscretizeRowPolicy */

struct CovarianceModelDiscretizeRowPolicy
{
  const NumericalSample & input_;
  const NumericalPoint p_;
  NumericalSample & output_;
  const CovarianceModelImplementation & model_;
  const UnsignedInteger dimension_;

  CovarianceModelDiscretizeRowPolicy(const NumericalSample & input,
                                     const UnsignedInteger p,
                                     NumericalSample & output,
                                     const CovarianceModelImplementation & model)
    : input_(input)
    , p_(input[p])
    , output_(output)
    , model_(model)
    , dimension_(model.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const MatrixImplementation localCovariance(*(model_(p_, input_[i]).getImplementation()));
      const UnsignedInteger base = i * dimension_;
      for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
          output_[base + ii][jj] = localCovariance(ii, jj);
    }
  }

}; /* end struct CovarianceModelDiscretizeRowPolicy */

NumericalSample CovarianceModelImplementation::discretizeRow(const NumericalSample & vertices,
    const UnsignedInteger p) const
{
  if (vertices.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << spatialDimension_;
  const UnsignedInteger size = vertices.getSize();
  NumericalSample result(size * dimension_, dimension_);
  if (dimension_ == 1)
  {
    const CovarianceModelScalarDiscretizeRowPolicy policy( vertices, p, result, *this );
    TBB::ParallelFor( 0, size, policy );
  }
  else
  {
    const CovarianceModelDiscretizeRowPolicy policy( vertices, p, result, *this );
    TBB::ParallelFor( 0, size, policy );
  }
  return result;
}

/** Discretize the covariance function on a given TimeGrid/Mesh using HMatrix */
HMatrix CovarianceModelImplementation::discretizeHMatrix(const RegularGrid & timeGrid,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
  return discretizeHMatrix(timeGrid.getVertices(), nuggetFactor, parameters);
}

HMatrix CovarianceModelImplementation::discretizeHMatrix(const Mesh & mesh,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
  return discretizeHMatrix(mesh.getVertices(), nuggetFactor, parameters);
}

HMatrix CovarianceModelImplementation::discretizeHMatrix(const NumericalSample & vertices,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
#ifdef OPENTURNS_HAVE_HMAT
  HMatrixFactory hmatrixFactory;
  HMatrix covarianceHMatrix = hmatrixFactory.build(vertices, dimension_, true, parameters);
  if (dimension_ == 1)
  {
    CovarianceAssemblyFunction simple(*this, vertices, nuggetFactor);
    covarianceHMatrix.assemble(simple, 'L');
  }
  else
  {
    CovarianceBlockAssemblyFunction block(*this, vertices, nuggetFactor);
    covarianceHMatrix.assemble(block, 'L');
  }
  return covarianceHMatrix;
#else
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::discretizeHMatrix, OpenTURNS had been compiled without HMat support";
#endif
}

/** Discretize and factorize the covariance function on a given TimeGrid/Mesh using HMatrix */
HMatrix CovarianceModelImplementation::discretizeAndFactorizeHMatrix(const RegularGrid & timeGrid,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
  return discretizeAndFactorizeHMatrix(timeGrid.getVertices(), nuggetFactor, parameters);
}

HMatrix CovarianceModelImplementation::discretizeAndFactorizeHMatrix(const Mesh & mesh,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
  return discretizeAndFactorizeHMatrix(mesh.getVertices(), nuggetFactor, parameters);
}

HMatrix CovarianceModelImplementation::discretizeAndFactorizeHMatrix(const NumericalSample & vertices,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
  // We suppose that covariance matrix is symmetric positive definite
  // We do not catch InternalException
  // Incremeant nugget factor to make matrix positive definite
  // Maybe parameters need to be adapted.
  HMatrix covarianceFactor = discretizeHMatrix(vertices, nuggetFactor, parameters);
  covarianceFactor.factorize("LLt");
  return covarianceFactor;
}

/* Amplitude accessor */
NumericalPoint CovarianceModelImplementation::getAmplitude() const
{
  return amplitude_;
}

void CovarianceModelImplementation::setAmplitude(const NumericalPoint & amplitude)
{
  if (amplitude.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setAmplitude: the given amplitude has a dimension=" << amplitude.getDimension() << " different from the dimension=" << dimension_;
  for (UnsignedInteger index = 0; index < dimension_; ++index)
    if (amplitude[index] <= 0)
      throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setAmplitude, the component " << index << " of amplitude is non positive" ;
  amplitude_ = amplitude;
  updateSpatialCovariance();
}

/* Scale accessor */
NumericalPoint CovarianceModelImplementation::getScale() const
{
  return scale_;
}

void CovarianceModelImplementation::setScale(const NumericalPoint & scale)
{
  if (scale.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setScale: the given scale has a dimension=" << scale.getDimension() << " different from the input dimension=" << spatialDimension_;
  for (UnsignedInteger index = 0; index < spatialDimension_; ++index)
    if (scale[index] <= 0)
      throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setScale: the component " << index << " of scale is non positive" ;
  scale_ = scale;
}

/* Spatial correlation accessor */
CorrelationMatrix CovarianceModelImplementation::getSpatialCorrelation() const
{
  if (!isDiagonal_) return spatialCorrelation_;
  return CorrelationMatrix(dimension_);
}

void CovarianceModelImplementation::setSpatialCorrelation(const CorrelationMatrix & spatialCorrelation)
{
  spatialCorrelation_ = spatialCorrelation;
  isDiagonal_ = spatialCorrelation_.isDiagonal();
  updateSpatialCovariance();
}

void CovarianceModelImplementation::updateSpatialCovariance()
{
  spatialCovariance_ = CovarianceMatrix(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    spatialCovariance_(j, j) = amplitude_[j] * amplitude_[j];
    if (!isDiagonal_)
    {
      for (UnsignedInteger i = j + 1; i < dimension_; ++i)
        spatialCovariance_(i, j) = spatialCorrelation_(i , j) * amplitude_[i] * amplitude_[j];
    }
  }
}

/* Nugget factor accessor */
NumericalScalar CovarianceModelImplementation::getNuggetFactor() const
{
  return nuggetFactor_;
}

void CovarianceModelImplementation::setNuggetFactor(const NumericalScalar nuggetFactor)
{
  if (nuggetFactor < 0.0) throw InvalidArgumentException(HERE) << "Error: the nugget factor=" << nuggetFactor << " is negative";
  nuggetFactor_ = nuggetFactor;
}

void CovarianceModelImplementation::setFullParameter(const NumericalPoint & parameter)
{
  // Here we manage only the generic parameters
  // First the scale parameter
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < spatialDimension_; ++ i)
    {
      scale_[i] = parameter[index];
      ++ index;
    }
  // Second the amplitude parameter
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
    {
      amplitude_[i] = parameter[index];
      ++ index;
    }
  // Third the spatial correation parameter, only the lower triangle
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
    for (UnsignedInteger j = 0; j < i; ++ j)
      {
        spatialCorrelation_(i, j) = parameter[index];
        ++ index;
      }
  isDiagonal_ = spatialCorrelation_.isDiagonal();
  updateSpatialCovariance();
}


NumericalPoint CovarianceModelImplementation::getFullParameter() const
{
  // Here we manage only the generic parameters
  // First the scale parameter
  NumericalPoint parameter(getScale());
  // Second the amplitude parameter
  parameter.add(getAmplitude());
  // Third the spatial correation parameter, only the lower triangle
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
    for (UnsignedInteger j = 0; j < i; ++ j)
      parameter.add(spatialCorrelation_(i, j));
  return parameter;
}

Description CovarianceModelImplementation::getFullParameterDescription() const
{
  // Here we manage only the generic parameters
  // First the scale parameter
  Description description(0);
  // First the scale parameter
  for (UnsignedInteger j = 0; j < spatialDimension_; ++j)
    description.add(OSS() << "scale_" << j);
  // Second the amplitude parameter
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    description.add(OSS() << "amplitude_" << j);
  // Third the spatial correlation parameter, only the lower triangle
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      description.add(OSS() << "R_" << i << "_" << j);

  return description;
}

/* Indices of the active parameters */
void CovarianceModelImplementation::setActiveParameter(const Indices & active)
{
  if (!active.isIncreasing()) throw InvalidArgumentException(HERE) << "Error: the active parameter indices must be given in increasing order, here active=" << active;
  activeParameter_ = active;
}

Indices CovarianceModelImplementation::getActiveParameter() const
{
  return activeParameter_;
}


void CovarianceModelImplementation::setParameter(const NumericalPoint & parameter)
{
  const UnsignedInteger activeSize = activeParameter_.getSize();
  if (activeSize == 0) return;
  // Brute-force approach: everything is updated
  NumericalPoint fullParameter(getFullParameter());
  for (UnsignedInteger i = 0; i < activeSize; ++i)
    fullParameter[activeParameter_[i]] = parameter[i];
  setFullParameter(fullParameter);
}


NumericalPoint CovarianceModelImplementation::getParameter() const
{
  // For now we use a brute-force approach: build all the parameters then extract the relevant ones
  NumericalPoint result(0);
  const UnsignedInteger activeSize = activeParameter_.getSize();
  // Quick return if no parameter
  if (activeSize == 0) return result;
  const NumericalPoint parameter(getFullParameter());
  if (activeSize == parameter.getSize()) return parameter;
  // Then filter only the active parameters
  for (UnsignedInteger i = 0; i < activeSize; ++ i)
    result.add(parameter[activeParameter_[i]]);
  return result;
}

Description CovarianceModelImplementation::getParameterDescription() const
{
  // For now we use a brute-force approach: build all the parameters then extract the relevant ones
  Description result(0);
  const UnsignedInteger activeSize = activeParameter_.getSize();
  // Quick return if no parameter
  if (activeSize == 0) return result;
  const Description description(getFullParameterDescription());
  if (activeSize == description.getSize()) return description;
  // Then filter only the active parameters
  for (UnsignedInteger i = 0; i < activeSize; ++i)
    result.add(description[activeParameter_[i]]);
  return result;
}


/* Is it a stationary model ? */
Bool CovarianceModelImplementation::isStationary() const
{
  return false;
}

/* Is it a diagonal model ? */
Bool CovarianceModelImplementation::isDiagonal() const
{
  return isDiagonal_;
}

/* Marginal accessor */
CovarianceModelImplementation::Implementation CovarianceModelImplementation::getMarginal(const UnsignedInteger index) const
{
  if (index >= dimension_) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << dimension_;
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::getMarginal(const UnsignedInteger index) const";
  return clone();
}

/* String converter */
String CovarianceModelImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class= " << CovarianceModelImplementation::GetClassName();
  return oss;
}

/* String converter */
String CovarianceModelImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class= " << CovarianceModelImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CovarianceModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("scale_", scale_);
  adv.saveAttribute("spatialDimension_", spatialDimension_);
  adv.saveAttribute("amplitude_", amplitude_);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("spatialCorrelation_", spatialCorrelation_);
  adv.saveAttribute("spatialCovariance_", spatialCovariance_);
  adv.saveAttribute("isDiagonal_", isDiagonal_);
  adv.saveAttribute("nuggetFactor_", nuggetFactor_);
  adv.saveAttribute("activeParameter_", activeParameter_);
}

/* Method load() reloads the object from the StorageManager */
void CovarianceModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("scale_", scale_);
  adv.loadAttribute("spatialDimension_", spatialDimension_);
  adv.loadAttribute("amplitude_", amplitude_);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("spatialCorrelation_", spatialCorrelation_);
  adv.loadAttribute("spatialCovariance_", spatialCovariance_);
  adv.loadAttribute("isDiagonal_", isDiagonal_);
  adv.loadAttribute("nuggetFactor_", nuggetFactor_);
  adv.loadAttribute("activeParameter_", activeParameter_);
}

END_NAMESPACE_OPENTURNS
