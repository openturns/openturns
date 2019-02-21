//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all RandomMixtures
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <algorithm>

#include "openturns/RandomMixture.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Function.hxx"
#include "openturns/Log.hxx"
#include "openturns/Triangular.hxx"
#include "openturns/Trapezoidal.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Exponential.hxx"
#include "openturns/Gamma.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/SmoothedUniform.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/Bernoulli.hxx"
#include "openturns/Binomial.hxx"
#include "openturns/Poisson.hxx"
#include "openturns/ComplexTensor.hxx"
#include "openturns/FFT.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/TBB.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<Distribution>)
static const Factory<PersistentCollection<Distribution> > Factory_PersistentCollection_Distribution;


typedef Collection<Distribution> DistributionCollection;
typedef Collection<Point> PointCollection;
typedef Collection<Complex> ComplexCollection;


CLASSNAMEINIT(RandomMixture)

static const Factory<RandomMixture> Factory_RandomMixture;

/* Default constructor */
RandomMixture::RandomMixture()
  : DistributionImplementation()
  , distributionCollection_()
  , constant_(Point(1))
  , weights_()
  , inverseWeights_()
  , detWeightsInverse_()
  , fftAlgorithm_()
  , isAnalytical_(false)
  , blockMin_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMin" ))
  , blockMax_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMax" ))
  , maxSize_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultMaxSize"  ))
  , storedSize_(0)
  , characteristicValuesCache_(0)
  , alpha_(ResourceMap::GetAsScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  setDimension(1);
  DistributionCollection coll(1);
  setDistributionCollectionAndWeights(coll, Matrix(1, coll.getSize(), Point(coll.getSize(), 1.0)), ResourceMap::GetAsBool("RandomMixture-SimplifyAtoms"));
  cdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultCDFEpsilon");
  pdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultPDFEpsilon");
}


/* Default constructor */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const Scalar constant)
  : DistributionImplementation()
  , distributionCollection_()
  , constant_(Point(1, constant))
  , weights_()
  , inverseWeights_()
  , detWeightsInverse_()
  , fftAlgorithm_()
  , isAnalytical_(false)
  , blockMin_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMin" ))
  , blockMax_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMax" ))
  , maxSize_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultMaxSize"  ))
  , storedSize_(0)
  , characteristicValuesCache_(0)
  , alpha_(ResourceMap::GetAsScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  setDimension(1);
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollectionAndWeights(coll, Matrix(1, coll.getSize(), Point(coll.getSize(), 1.0)), ResourceMap::GetAsBool("RandomMixture-SimplifyAtoms"));
  cdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultCDFEpsilon");
  pdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultPDFEpsilon");
}

/* Default constructor */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const Point & weights,
                             const Scalar constant)
  : DistributionImplementation()
  , distributionCollection_()
  , constant_(Point(1, constant))
  , weights_()
  , inverseWeights_()
  , detWeightsInverse_()
  , fftAlgorithm_()
  , isAnalytical_(false)
  , blockMin_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMin" ))
  , blockMax_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMax" ))
  , maxSize_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultMaxSize"  ))
  , storedSize_(0)
  , characteristicValuesCache_(0)
  , alpha_(ResourceMap::GetAsScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  setDimension(1);
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  // the setDistributionCollection() method that do it for us.
  if (weights.getDimension() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weights collection must have the same size as the distribution collection";
  // This call set also the range.
  setDistributionCollectionAndWeights(coll, Matrix(1, coll.getSize(), weights), ResourceMap::GetAsBool("RandomMixture-SimplifyAtoms"));
  cdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultCDFEpsilon");
  pdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultPDFEpsilon");
}

/* Parameter constructor - nD */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const Matrix & weights,
                             const Point constant)
  : DistributionImplementation()
  , distributionCollection_()
  , constant_(constant)
  , weights_()
  , inverseWeights_()
  , detWeightsInverse_()
  , fftAlgorithm_()
  , isAnalytical_(false)
  , blockMin_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMin" ))
  , blockMax_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMax" ))
  , maxSize_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultMaxSize"  ))
  , storedSize_(0)
  , characteristicValuesCache_(0)
  , alpha_(ResourceMap::GetAsScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  if (constant.getSize() > 3) throw InvalidDimensionException(HERE) << "RandomMixture only possible for dimension 1,2 or 3";
  setDimension(constant.getSize());
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  // the setDistributionCollection() method that do it for us.
  if (weights.getNbColumns() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight matrix must have the same column numbers as the distribution collection's size";
  if (weights.getNbRows() != constant.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight matrix must have the same row numbers as the distribution dimension";
  // This call set also the range.
  setDistributionCollectionAndWeights(coll, weights, ResourceMap::GetAsBool("RandomMixture-SimplifyAtoms"));
  cdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultCDFEpsilon");
  pdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultPDFEpsilon");
}

/* Parameter constructor - nD */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const Matrix & weights)
  : DistributionImplementation()
  , distributionCollection_()
  , constant_()
  , weights_()
  , inverseWeights_()
  , detWeightsInverse_()
  , fftAlgorithm_()
  , isAnalytical_(false)
  , blockMin_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMin" ))
  , blockMax_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMax" ))
  , maxSize_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultMaxSize"  ))
  , storedSize_(0)
  , characteristicValuesCache_(0)
  , alpha_(ResourceMap::GetAsScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  const UnsignedInteger dimension = weights.getNbRows();
  if (dimension > 3) throw InvalidDimensionException(HERE) << "RandomMixture only possible for dimension 1,2 or 3";
  constant_ = Point(dimension, 0.0);
  setDimension(dimension);
  if (weights.getNbColumns() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight matrix must have the same column numbers as the distribution collection's size";
  setDistributionCollectionAndWeights(coll, weights, ResourceMap::GetAsBool("RandomMixture-SimplifyAtoms"));
  cdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultCDFEpsilon");
  pdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultPDFEpsilon");
}

/* Parameter constructor - nD */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const Sample & weights,
                             const Point constant)
  : DistributionImplementation()
  , distributionCollection_()
  , constant_(constant)
  , weights_()
  , inverseWeights_()
  , detWeightsInverse_()
  , fftAlgorithm_()
  , isAnalytical_(false)
  , blockMin_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMin" ))
  , blockMax_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMax" ))
  , maxSize_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultMaxSize"  ))
  , storedSize_(0)
  , characteristicValuesCache_(0)
  , alpha_(ResourceMap::GetAsScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  const UnsignedInteger dimension = constant.getSize();
  if (dimension > 3) throw InvalidDimensionException(HERE) << "RandomMixture only possible for dimension 1,2 or 3";
  setDimension(dimension);
  if (weights.getSize() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight sample must have the same size as the distribution collection's size";
  if (weights.getDimension() != constant.getDimension()) throw InvalidArgumentException(HERE) << "Error: the weight sample must have the same dimension as the distribution dimension";
  setDistributionCollectionAndWeights(coll, Matrix(weights.getDimension(), weights.getSize(), weights.getImplementation()->getData()), ResourceMap::GetAsBool("RandomMixture-SimplifyAtoms"));
  cdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultCDFEpsilon");
  pdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultPDFEpsilon");
}

/* Parameter constructor - nD */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const Sample & weights)
  : DistributionImplementation()
  , distributionCollection_()
  , weights_()
  , inverseWeights_()
  , detWeightsInverse_()
  , fftAlgorithm_()
  , isAnalytical_(false)
  , blockMin_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMin" ))
  , blockMax_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultBlockMax" ))
  , maxSize_(ResourceMap::GetAsUnsignedInteger( "RandomMixture-DefaultMaxSize"  ))
  , storedSize_(0)
  , characteristicValuesCache_(0)
  , alpha_(ResourceMap::GetAsScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  const UnsignedInteger dimension = weights.getDimension();
  if (dimension > 3) throw InvalidDimensionException(HERE) << "RandomMixture only possible for dimension 1,2 or 3";
  constant_ = Point(dimension, 0.0);
  setDimension(dimension);
  if (weights.getSize() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight sample must have the same size as the distribution collection's size";
  setDistributionCollectionAndWeights(coll, Matrix(weights.getDimension(), weights.getSize(), weights.getImplementation()->getData()), ResourceMap::GetAsBool("RandomMixture-SimplifyAtoms"));
  cdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultCDFEpsilon");
  pdfEpsilon_ = ResourceMap::GetAsScalar("RandomMixture-DefaultPDFEpsilon");
}

/* Compute the numerical range of the distribution given the parameters values */
void RandomMixture::computeRange()
{
  const UnsignedInteger size = distributionCollection_.getSize();
  // First, compute the *exact* range. It will be used to clip the asymptotic range if Poisson's formula is used (ie the collection has a size greater than the dimension)
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1 && size == 1)
  {
    const Scalar w = weights_(0, 0);
    const Scalar c = constant_[0];
    const Scalar a = distributionCollection_[0].getRange().getLowerBound()[0];
    const Scalar b = distributionCollection_[0].getRange().getUpperBound()[0];
    const Interval::BoolCollection aFinite = distributionCollection_[0].getRange().getFiniteLowerBound();
    const Interval::BoolCollection bFinite = distributionCollection_[0].getRange().getFiniteUpperBound();
    if (w > 0.0)
    {
      setRange(Interval(Point(1, c + w * a), Point(1, c + w * b), aFinite, bFinite));
      return;
    }
    setRange(Interval(Point(1, c + w * b), Point(1, c + w * a), bFinite, aFinite));
    return;
  }
  Interval::BoolCollection finiteLowerBound(dimension);
  Interval::BoolCollection finiteUpperBound(dimension);
  Point lowerBound(getDimension());
  Point upperBound(getDimension());
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    Interval range(constant_[j], constant_[j]);
    for (UnsignedInteger i = 0; i < size; ++i)
      range += distributionCollection_[i].getRange() * weights_(j, i);
    lowerBound[j] = range.getLowerBound()[0];
    upperBound[j] = range.getUpperBound()[0];
    finiteLowerBound[j] = range.getFiniteLowerBound()[0];
    finiteUpperBound[j] = range.getFiniteUpperBound()[0];
  }
  const Interval range(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
  if (size <= dimension)
  {
    setRange(range);
    return;
  } // Analytical case
  if (dimension == 1)
  {
    const Point m(1, getPositionIndicator());
    const Point s(1, getDispersionIndicator());
    setRange(range.intersect(Interval(m - s * beta_, m + s * beta_)));
  } // dimension == 1
  else
  {
    Point m(constant_);
    Point s(getDimension(), 0.0);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      for(UnsignedInteger i = 0; i < size; ++i)
      {
        m[j] += weights_(j, i) * distributionCollection_[i].getPositionIndicator();
        s[j] += std::pow(weights_(j, i) * distributionCollection_[i].getDispersionIndicator(), 2.0);
      }
    }
    for (UnsignedInteger j = 0; j < dimension; ++j) s[j] = std::sqrt(s[j]);
    setRange(range.intersect(Interval(m - s * beta_, m + s * beta_)));
  } // dimension > 1
}

/* Comparison operator */
Bool RandomMixture::operator ==(const RandomMixture & other) const
{
  if (this == &other) return true;
  return (distributionCollection_ == other.distributionCollection_) && (constant_ == other.constant_);
}

Bool RandomMixture::equals(const DistributionImplementation & other) const
{
  const RandomMixture* p_other = dynamic_cast<const RandomMixture*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String RandomMixture::__repr__() const
{
  OSS oss(true);
  oss << "class=" << RandomMixture::GetClassName()
      << " name=" << getName()
      << " distribution collection=" << distributionCollection_
      << " weights =" << weights_
      << " constant=" << constant_;
  return oss;
}

/* String converter */
String RandomMixture::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  const UnsignedInteger size = distributionCollection_.getSize();
  if (dimension_ > 1) oss << "\n";
  // Print marginal by marginal
  for (UnsignedInteger marginal = 0; marginal < dimension_; ++ marginal)
  {
    // If marginal > 0, alignement
    if (constant_[marginal] != 0.0) oss << constant_[marginal];
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar w = weights_(marginal, i);
      if ((constant_[marginal] != 0.0) || (i > 0))
      {
        if (w > 0.0) oss << " + ";
        else oss << " - ";
      }
      else if (w < 0.0) oss << "-";
      const String coeff(OSS() << std::abs(w));
      if (coeff != "1") oss << std::abs(w) << " * ";
      oss << distributionCollection_[i];
    }
    // skip to new line
    if (dimension_ > 1) oss << Os::GetEndOfLine() << offset;
  }
  oss << ")";
  return oss;
}

/* Weights distribution accessor */
Matrix RandomMixture::getWeights() const
{
  return weights_;
}


/* Distribution collection accessor */
void RandomMixture::setDistributionCollectionAndWeights(const DistributionCollection & coll,
    const Matrix & weights,
    const Bool simplifyAtoms)
{
  weights_ = weights;
  // Size will be updated during the several treatments of the collection
  UnsignedInteger size = coll.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a RandomMixture based on an empty distribution collection.";
  // No simplification in the analytical case
  const UnsignedInteger dimension = getDimension();
  if ((size == dimension) && !simplifyAtoms)
  {
    isAnalytical_ = true;
    if (dimension == 1)
    {
      detWeightsInverse_ = 1.0 / weights_(0, 0);
      inverseWeights_ = SquareMatrix(1);
      inverseWeights_(0, 0) = detWeightsInverse_;
    }
    else
    {
      inverseWeights_ = weights_.solveLinearSystem(IdentityMatrix(dimension));
      detWeightsInverse_ = inverseWeights_.getImplementation().get()->computeDeterminant();
    }
    setParallel(coll[0].getImplementation()->isParallel());
    distributionCollection_ = coll;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
    // No need to precompute Mean, Covariance, PositionIndicator, DispersionIndicator, ReferenceBandwidth, EquivalentNormal
    return;
  }
  // In 1D case, collection's size might change
  // When reducing collection to 1, computations become faster
  // First, flatten all the RandomMixture atoms
  DistributionCollection atomCandidates(0);
  // The weights are stored as a collection of scalars, to be read by blocks of size dimension.
  Sample weightCandidates(0, dimension);
  LOGDEBUG("Flatten RandomMixture atoms in the current RandomMixture");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Distribution atom(coll[i]);
    const Matrix w = weights_.getColumn(i);
    // Skip atoms with null coefficient
    if (w.computeGram()(0, 0) == 0.0) continue;
    const String atomKind(atom.getImplementation()->getClassName());
    if (atom.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: a RandomMixture cannot be built from a collection of distributions of dimension not equal to 1, here distribution " << i << " has a dimension=" << atom.getDimension();
    if (atomKind == "RandomMixture" || atomKind == "SmoothedUniform")
    {
      // Here we know that the atom is 1D, so we merge a 1D RandomMixture
      // Get the weight of the atom
      // Cast the atom into a RandomMixture
      const RandomMixture * mixture(dynamic_cast< const RandomMixture * >(atom.getImplementation().get()));
      // Aggregate the constant
      constant_ += w * mixture->constant_;
      // Aggregate the weights
      const Matrix localWeights(w * mixture->weights_);
      SampleImplementation localWeightsAsSample(localWeights.getNbColumns(), dimension);
      localWeightsAsSample.setData(*localWeights.getImplementation());
      weightCandidates.add(localWeightsAsSample);
      // Aggregate the atoms
      atomCandidates.add(mixture->getDistributionCollection());
    } // atom is a RandomMixture
    else if (atomKind == "TruncatedDistribution")
    {
      const TruncatedDistribution * truncatedDistribution(dynamic_cast< const TruncatedDistribution * >(atom.getImplementation().get()));
      weightCandidates.add(*w.getImplementation());
      atomCandidates.add(truncatedDistribution->getSimplifiedVersion());
    }
    else
    {
      weightCandidates.add(*w.getImplementation());
      atomCandidates.add(atom);
    } // atom is not a RandomMixture
  } // Flatten the atoms of RandomMixture type
  // Update the size
  size = atomCandidates.getSize();
  if (simplifyAtoms)
  {
    // Second, split the atoms between the discrete ones, the continuous ones and the others
    // The Dirac atoms are optimized during this step
    DistributionCollection continuousAtoms(0);
    Sample continuousWeights(0, dimension);
    DistributionCollection discreteAtoms(0);
    Sample discreteWeights(0, dimension);
    DistributionCollection otherAtoms(0);
    Sample otherWeights(0, dimension);
    LOGDEBUG("Sort the atoms into continuous, discrete and others. Also merge Dirac atoms into the constant.");
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Distribution atom(atomCandidates[i]);
      if (atom.getImplementation()->getClassName() == "Dirac")
      {
        constant_ += Point(weightCandidates[i]) * atom.getParameter()[0];
      }
      else if (atom.isContinuous())
      {
        continuousAtoms.add(atom);
        continuousWeights.add(weightCandidates[i]);
      }
      else if (atom.isDiscrete())
      {
        discreteAtoms.add(atom);
        discreteWeights.add(weightCandidates[i]);
      }
      else
      {
        otherAtoms.add(atom);
        otherWeights.add(weightCandidates[i]);
      }
    } // Split atoms and optimize Dirac
    LOGDEBUG(OSS() << "Continuous atoms=" << continuousAtoms.__str__() << ", discrete atoms=" << discreteAtoms.__str__() << ", other atoms=" << otherAtoms.__str__());
    // Third, merge the atoms as much as possible. Most of the optimizations assume a 1D RandomMixture.
    //
    // In the case of a nD RandomMixture with n>1:
    //
    // + The discrete atoms can be merged into a unique discrete nD UserDefined with a support of reasonable size
    // + There is no continuous atom, or a unique continuous atom, or all the continuous atoms are Normal
    // + There is no 'other' atom.
    // Depending on the continuous atoms, one get:
    // + A multivariate UserDefined if no continuous atom
    // + A multivariate Mixture if a unique continuous atom or only continuous Normal atoms (merged into a unique multivariate Normal)
    // -> all these special cases lead to an analytical expression of the RandomMixture
    // -> these simplifications will be implemented in a second time
    //
    // In the case of a 1D mixture:
    //
    // Continuous optimizations:
    // + The Uniform atoms can be merged two by two into a Trapezoidal or Triangular
    // + The Exponential, ChiSquare and Gamma atoms can be merged into a Gamma atom if there scale parameters are all equal after standardization (so we must group these atoms by scale*weight parameter). The possible translation has to be accumulated into the constant.
    // + The Normal atoms can be merged into a unique Normal
    // + If it remains a Uniform atom and a Normal atom, they can be merged into a SmoothedUniform atom
    //
    // Discrete optimizations:
    // + The Bernoulli and Binomial atoms can be merged into a unique Binomial as soon as they share the same value for p and the same weight
    // + The Poisson atoms can be merged into a unique Poisson as soon as they share the same weight
    // + Poisson atoms with opposite weights could be merged into a Skellam atom but it is not clear if it is worth the effort...
    // + The Discrete atoms can be grouped into Discrete atoms of larger support, if these merged atoms have a support of reasonable size.
    //
    // Mixed optimizations:
    //
    // + A continuous atom can be merged with a discrete atom to form a Mixture. This simplification can be done for each pair (continuous,discrete). It is not clear if some pairings are to prefer to others.
    distributionCollection_ = DistributionCollection(0);
    Sample reducedWeights(0, dimension);
    if (dimension == 1)
    {
      // Optimization of continuous atoms
      Bool hasNormalAtom = false;
      Scalar aggregatedMean = 0.0;
      Scalar aggregatedVariance = 0.0;
      Bool hasPendingUniform = false;
      Uniform pendingUniform;
      // This map will store all the Exponential, ChiSquare and Gamma atoms
      // For each value of lambda*weight it stores the cummulated k parameter
      std::map<Scalar, Scalar> gammaMap;
      for (UnsignedInteger i = 0; i < continuousAtoms.getSize(); ++i)
      {
        const Scalar w = continuousWeights(0, i);
        const Distribution atom(continuousAtoms[i]);
        const String atomKind(atom.getImplementation()->getClassName());
        // Knowledge-based optimization
        // First for 1D atoms
        if (atomKind == "Uniform")
        {
          const Scalar low = atom.getRange().getLowerBound()[0];
          const Scalar high = atom.getRange().getUpperBound()[0];
          Scalar a0 = w * low;
          Scalar b0 = w * high;
          if (a0 > b0) std::swap(a0, b0);
          // If there is already a Uniform atom, merge it into a symmetrical Trapezoidal
          if (hasPendingUniform)
          {
            const Scalar a1 = pendingUniform.getA();
            const Scalar b1 = pendingUniform.getB();
            const Scalar alpha = a1 + a0;
            const Scalar delta = b1 + b0;
            const Scalar halfWidth = 0.5 * std::abs((b1 - a1) - (b0 - a0));
            const Scalar center = 0.5 * (alpha + delta);
            // A proper Trapezoidal
            if (halfWidth > 0.0) distributionCollection_.add(Trapezoidal(alpha, center - halfWidth, center + halfWidth, delta));
            // A degenerated Trapezoidal, ie a Triangular
            else distributionCollection_.add(Triangular(alpha, center, delta));
            // Add a unit weight as its initial weight has been merged into the parameters
            reducedWeights.add(Point(1, 1.0));
            hasPendingUniform = false;
          } // hasPendingUniform
          else
          {
            pendingUniform = Uniform(a0, b0);
            hasPendingUniform = true;
          } // !hasPendingUniform
        } // atom is a Uniform
        else if (atomKind == "Normal")
        {
          hasNormalAtom = true;
          aggregatedMean += w * atom.getMean()[0];
          aggregatedVariance += w * w * atom.getCovariance()(0, 0);
        } // atom is a Normal
        else if (atomKind == "Exponential")
        {
          const Point parameters(atom.getParameter());
          const Scalar key = parameters[0] / w;
          std::map<Scalar, Scalar>::iterator it(gammaMap.find(key));
          // New Gamma
          if (it == gammaMap.end()) gammaMap[key] = 1.0;
          // Already known Gamma, update the shape parameter
          else gammaMap[key] += 1.0;
          // In any case update the constant
          constant_ += Point(1, parameters[1]) * w;
        } // atom is Exponential
        else if (atomKind == "Gamma")
        {
          const Point parameters(atom.getParameter());
          const Scalar key = parameters[1] / w;
          std::map<Scalar, Scalar>::iterator it(gammaMap.find(key));
          // New Gamma
          if (it == gammaMap.end()) gammaMap[key] = parameters[0];
          // Already known Gamma, update the shape parameter
          else gammaMap[key] += parameters[0];
          // In any case update the constant
          constant_ += Point(1, parameters[2]) * w;
        } // atom is Gamma
        else if (atomKind == "ChiSquare")
        {
          const Point parameters(atom.getParameter());
          const Scalar key = 0.5 / w;
          std::map<Scalar, Scalar>::iterator it(gammaMap.find(key));
          // New Gamma
          if (it == gammaMap.end()) gammaMap[key] = 0.5 * parameters[0];
          // Already known Gamma, update the shape parameter
          else gammaMap[key] += 0.5 * parameters[0];
        } // atom is ChiSquare
        else
        {
          distributionCollection_.add(atom);
          reducedWeights.add(Point(1, w));
        } // no simplification known
      } // Loop over continuous atoms
      // Set the aggregated normal if any. Note that this atom absorbs the constant.
      if (hasNormalAtom)
      {
        if (hasPendingUniform)
        {
          distributionCollection_.add(SmoothedUniform(pendingUniform.getA() + aggregatedMean + constant_[0], pendingUniform.getB() + aggregatedMean + constant_[0], std::sqrt(aggregatedVariance)));
          constant_[0] = 0.0;
          // Add a unit weight as its initial weight has been merged into the parameters
          reducedWeights.add(Point(1, 1.0));
          // No more pending uniform
          hasPendingUniform = false;
        } // hasPendingNormal && hasPendingUniform
        else
        {
          distributionCollection_.add(Normal(aggregatedMean + constant_[0], std::sqrt(aggregatedVariance)));
          constant_[0] = 0.0;
          // Add a unit weight as its initial weight has been merged into the parameters
          reducedWeights.add(Point(1, 1.0));
        } // hasPendingNormal && !hasPendingUniform
      } // hasNormalAtom
      // Set the pending Uniform if any. Note that this atom absorbs the constant if not yet absorbed.
      if (hasPendingUniform)
      {
        if (constant_[0] != 0.0)
        {
          pendingUniform = Uniform(pendingUniform.getA() + constant_[0], pendingUniform.getB() + constant_[0]);
          constant_[0] = 0.0;
        }
        distributionCollection_.add(pendingUniform);
        // Add a unit weight as its initial weight has been merged into the parameters
        reducedWeights.add(Point(1, 1.0));
      } // hasPendingUniform
      // Add the aggregated Gamma if any
      while (!gammaMap.empty())
      {
        const Scalar lambda = gammaMap.begin()->first;
        const Scalar k = gammaMap.begin()->second;
        if (k == 1.0) distributionCollection_.add(Exponential(std::abs(lambda)));
        else distributionCollection_.add(Gamma(k, std::abs(lambda)));
        reducedWeights.add(Point(1, lambda > 0.0 ? 1.0 : -1.0));
        gammaMap.erase(gammaMap.begin());
      } // while Gamma atoms to insert
      // Remember the index of the first non-continuous atom in order to
      const UnsignedInteger firstNonContinuousAtom = distributionCollection_.getSize();
      LOGDEBUG(OSS() << "After simplification of continuous atoms, distributionCollection_=" << distributionCollection_.__str__());
      // Optimization of discrete atoms
      // This map will store all the Poisson atoms
      // For each value of weight it stores the cummulated theta parameter
      std::map<Scalar, Scalar> poissonMap;
      // This map will store all the Bernoulli and Binomial atoms
      // For each value of (p, weight) it stores the cummulated n parameter
      std::map<Point, UnsignedInteger> binomialMap;
      for (UnsignedInteger i = 0; i < discreteAtoms.getSize(); ++i)
      {
        const Scalar w = discreteWeights(0, i);
        const Distribution atom(discreteAtoms[i]);
        const String atomKind(atom.getImplementation()->getClassName());
        if (atomKind == "Poisson")
        {
          const Point parameters(atom.getParameter());
          const Scalar key = w;
          std::map<Scalar, Scalar>::iterator it(poissonMap.find(key));
          // New Poisson
          if (it == poissonMap.end()) poissonMap[key] = parameters[0];
          // Already known Poisson, update the count parameter
          else poissonMap[key] += parameters[0];
        } // atom is Bernoulli
        else if (atomKind == "Bernoulli")
        {
          const Point parameters(atom.getParameter());
          Point key(2);
          key[0] = parameters[0];
          key[1] = w;
          std::map<Point, UnsignedInteger>::iterator it(binomialMap.find(key));
          // New Binomial
          if (it == binomialMap.end()) binomialMap[key] = 1;
          // Already known Binomial, update the count parameter
          else binomialMap[key] += 1;
        } // atom is Bernoulli
        else if (atomKind == "Binomial")
        {
          const Point parameters(atom.getParameter());
          Point key(2);
          key[0] = parameters[1];
          key[1] = w;
          std::map<Point, UnsignedInteger>::iterator it(binomialMap.find(key));
          // New Binomial
          if (it == binomialMap.end()) binomialMap[key] = static_cast<UnsignedInteger>(parameters[0]);
          // Already known Binomial, update the count parameter
          else binomialMap[key] += static_cast<UnsignedInteger>(parameters[0]);
        } // atom is Binomial
        else
        {
          distributionCollection_.add(atom);
          reducedWeights.add(Point(1, w));
        }
      } // discreteAtoms
      // Add the aggregated Poisson if any
      while (!poissonMap.empty())
      {
        const Scalar w = poissonMap.begin()->first;
        const Scalar theta = poissonMap.begin()->second;
        distributionCollection_.add(Poisson(theta));
        reducedWeights.add(Point(1, w));
        poissonMap.erase(poissonMap.begin());
      } // while Poisson atoms to insert
      // Add the aggregated Binomial if any
      while (!binomialMap.empty())
      {
        const Scalar p = binomialMap.begin()->first[0];
        const Scalar w = binomialMap.begin()->first[1];
        const UnsignedInteger n = binomialMap.begin()->second;
        if (n == 1) distributionCollection_.add(Bernoulli(p));
        else distributionCollection_.add(Binomial(n, p));
        reducedWeights.add(Point(1, w));
        binomialMap.erase(binomialMap.begin());
      } // while Binomial atoms to insert
      LOGDEBUG(OSS() << "After simplification of discrete atoms, distributionCollection_=" << distributionCollection_.__str__());
      // Now merge the discrete atoms by groups of reasonably sized support
      // if there is at least 2 discrete atoms
      const UnsignedInteger maxSupportSize(ResourceMap::GetAsUnsignedInteger("RandomMixture-MaximumSupportSize"));
      UnsignedInteger firstOtherAtom = distributionCollection_.getSize();
      // No aggregation if maxSupportSize==0 or if only one discrete atom
      if (firstOtherAtom > firstNonContinuousAtom + 1 && maxSupportSize > 0)
      {

        UnsignedInteger indexAggregated = firstNonContinuousAtom;
        UnsignedInteger firstDiscreteIndex = firstNonContinuousAtom;
        Distribution firstDiscrete(distributionCollection_[firstDiscreteIndex]);
        Sample aggregatedSupport(firstDiscrete.getSupport() * reducedWeights[firstDiscreteIndex]);
        Point aggregatedProbabilities(firstDiscrete.getProbabilities());
        UnsignedInteger aggregatedSupportSize = aggregatedSupport.getSize();
        for (UnsignedInteger secondDiscreteIndex = firstNonContinuousAtom + 1; secondDiscreteIndex < firstOtherAtom; ++secondDiscreteIndex)
        {
          const Distribution secondDiscrete(distributionCollection_[secondDiscreteIndex]);
          const Sample secondSupport(secondDiscrete.getSupport() * reducedWeights[secondDiscreteIndex]);
          const Point secondProbabilities(secondDiscrete.getProbabilities());
          const UnsignedInteger secondSupportSize = secondSupport.getSize();
          const UnsignedInteger newAggregatedSupportSize = aggregatedSupportSize * secondSupportSize;
          // If the next discrete may lead to a too large support, store the current aggregated discrete atom and go to the next group
          if (newAggregatedSupportSize > maxSupportSize)
          {
            // If several discrete atoms have been merged store the aggregated
            // atom at the place occuped by the first discrete atom
            if (secondDiscreteIndex > firstDiscreteIndex + 1)
            {
              distributionCollection_[indexAggregated] = UserDefined(aggregatedSupport, aggregatedProbabilities);
              reducedWeights[indexAggregated] = Point(1, 1.0);
            }
            else
              distributionCollection_[indexAggregated] = firstDiscrete;
            ++indexAggregated;
            firstDiscreteIndex = secondDiscreteIndex;
            firstDiscrete = secondDiscrete;
            aggregatedSupport = secondSupport;
            aggregatedProbabilities = secondProbabilities;
            aggregatedSupportSize = secondSupportSize;
          } // If the aggregated discrete atom is large enough
          else
          {
            Sample newAggregatedSupportAndProbabilities(newAggregatedSupportSize, 2);
            UnsignedInteger k = 0;
            for (UnsignedInteger firstIndex = 0; firstIndex < aggregatedSupportSize; ++firstIndex)
            {
              const Scalar xI = aggregatedSupport(firstIndex, 0);
              const Scalar pI = aggregatedProbabilities[firstIndex];
              for (UnsignedInteger secondIndex = 0; secondIndex < secondSupportSize; ++secondIndex)
              {
                const Scalar xJ = secondSupport(secondIndex, 0);
                const Scalar pJ = secondProbabilities[secondIndex];
                newAggregatedSupportAndProbabilities(k, 0) = xI + xJ;
                newAggregatedSupportAndProbabilities(k, 1) = pI * pJ;
                ++k;
              } // secondIndex
            } // firstIndex
            // Merge the identical points in the support
            // First, sort the new aggregated data according to the support points
            newAggregatedSupportAndProbabilities = newAggregatedSupportAndProbabilities.sortAccordingToAComponent(0);
            // Second, filter out the duplicates in the point space and aggregate the values in the probability space
            aggregatedSupport = Sample(1, Point(1, newAggregatedSupportAndProbabilities(0, 0)));
            aggregatedProbabilities = Point(1, newAggregatedSupportAndProbabilities(0, 1));
            k = 0;
            for (UnsignedInteger index = 1; index < newAggregatedSupportSize; ++index)
            {
              // If the current point is equal to the last one aggregate the probabilities
              if (newAggregatedSupportAndProbabilities[index][0] == aggregatedSupport(k, 0))
              {
                aggregatedProbabilities[k] += newAggregatedSupportAndProbabilities(index, 1);
              } // current point equals to the previous one
              else
              {
                ++k;
                aggregatedSupport.add(Point(1, newAggregatedSupportAndProbabilities(index, 0)));
                aggregatedProbabilities.add(newAggregatedSupportAndProbabilities(index, 1));
              } // current point is different from the previous one
            } // Loop over the new aggregated support
            aggregatedSupportSize = aggregatedSupport.getSize();
          } // Merge the second atom into the aggregated atom
        } // Loop over the discrete atoms
        // If there is still something to merge
        // It can be:
        // + an aggregated atom with small support (detected because firstDiscreteIndex < firstOtherAtom - 1
        // + a single atom (the second one, but now equals to the first one) (detected because firstDiscreteIndex == firstOtherAtom - 1)
        if (firstDiscreteIndex == firstOtherAtom - 1)
          distributionCollection_[indexAggregated] = firstDiscrete;
        else
        {
          distributionCollection_[indexAggregated] = UserDefined(aggregatedSupport, aggregatedProbabilities);
          reducedWeights[indexAggregated] = Point(1, 1.0);
        }
        // To identify the first discrete atom to remove
        ++indexAggregated;
        // Now remove the discrete atoms that have been merged from the list of distributions
        distributionCollection_.erase(distributionCollection_.begin() + indexAggregated, distributionCollection_.end());
        reducedWeights.erase(indexAggregated, reducedWeights.getSize());
        firstOtherAtom = distributionCollection_.getSize();
      } // If there are discrete atoms to merge

      // Then perform the continuous/discrete simplification using mixtures
      // There must be continuous atoms and discrete ones
      if (firstNonContinuousAtom > 0 && firstNonContinuousAtom != firstOtherAtom)
      {
        const SignedInteger firstContinuous = 0;
        const SignedInteger firstDiscrete = firstNonContinuousAtom;
        SignedInteger currentContinuous = firstNonContinuousAtom - 1;
        SignedInteger currentDiscrete = firstOtherAtom - 1;
        while (currentContinuous >= firstContinuous && currentDiscrete >= firstDiscrete)
        {
          const Distribution continuousAtom(distributionCollection_[currentContinuous]);
          const Scalar continuousWeight = reducedWeights(currentContinuous, 0);
          Distribution discreteAtom(distributionCollection_[currentDiscrete]);
          Scalar discreteWeight = reducedWeights(currentDiscrete, 0);
          const Sample support(discreteAtom.getSupport());
          DistributionCollection mixtureAtoms;
          for (UnsignedInteger i = 0; i < support.getSize(); ++i)
            mixtureAtoms.add(RandomMixture(DistributionCollection(1, continuousAtom), Point(1, continuousWeight), support(i, 0) * discreteWeight));
          const Point probabilities(discreteAtom.getProbabilities());
          // Replace the current continuous atom by the Mixture
          distributionCollection_[currentContinuous] = Mixture(mixtureAtoms, probabilities);
          // Remove the current discrete atom
          distributionCollection_.erase(distributionCollection_.begin() + currentDiscrete);
          reducedWeights.erase(currentDiscrete);
          --currentContinuous;
          --currentDiscrete;
        } // loop over (continuous, discrete) pairs
      } // continuous and discrete atoms to merge?
      // No simplification for other atoms
      distributionCollection_.add(otherAtoms);
      reducedWeights.add(otherWeights);
    } // dimension == 1
    else
    {
      distributionCollection_.add(continuousAtoms);
      reducedWeights.add(continuousWeights);
      distributionCollection_.add(discreteAtoms);
      reducedWeights.add(discreteWeights);
      distributionCollection_.add(otherAtoms);
      reducedWeights.add(otherWeights);
    } // dimension > 1
    // Store the weights in a Matrix format
    weights_ = Matrix(reducedWeights.getDimension(), reducedWeights.getSize(), reducedWeights.getImplementation()->getData());
  } // simplify atoms=true
  else
  {
    distributionCollection_ = atomCandidates;
    // Store the weights in a Matrix format
    weights_ = Matrix(weightCandidates.getDimension(), weightCandidates.getSize(), weightCandidates.getImplementation()->getData());
  } // simplify atoms=false
  // Special case: distributionCollection_ is empty because all the atoms were Dirac distributions, so they have all been merged into the constant. As we need at least one atom for the algorithms to work we convert the constant back into a unique Dirac distribution. This case can occur only in dimension 1
  if (distributionCollection_.getSize() == 0)
  {
    distributionCollection_.add(Dirac(constant_));
    weights_ = Matrix(1, 1);
    weights_(0, 0) = 1.0;
    constant_[0] = 0.0;
  }

  // We cannot use parallelism if we have more than one atom due to the characteristic function cache
  if (distributionCollection_.getSize() > 1) setParallel(false);
  else setParallel(distributionCollection_[0].getImplementation()->isParallel());
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeMean();
  computeCovariance();
  computePositionIndicator();
  computeDispersionIndicator();
  computeRange();
  computeReferenceBandwidth();
  computeEquivalentNormal();
  // In 1D case, collection's size might change
  // When reducing collection to 1, computations become faster
  if (distributionCollection_.getSize() == dimension)
  {
    inverseWeights_ = weights_.solveLinearSystem(IdentityMatrix(dimension));
    isAnalytical_ = true;
    detWeightsInverse_ = inverseWeights_.getImplementation().get()->computeDeterminant();
  }
}

/* Constant accessor */
void RandomMixture::setConstant(const Point & constant)
{
  if (constant != constant_)
  {
    if (constant.getSize() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the constant term must have the same dimension as the distribution";
    constant_ = constant;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on the constant
    computeRange();
  }
}

Point RandomMixture::getConstant() const
{
  return constant_;
}

/* Distribution collection accessor */
const DistributionCollection & RandomMixture::getDistributionCollection() const
{
  return distributionCollection_;
}

/* FFT algorithm accessor */
FFT RandomMixture::getFFTAlgorithm() const
{
  return fftAlgorithm_;
}

/* FFT algorithm accessor */
void RandomMixture::setFFTAlgorithm(const FFT & fft)
{
  fftAlgorithm_ = fft;
}


/* Virtual constructor */
RandomMixture * RandomMixture::clone() const
{
  return new RandomMixture(*this);
}

/* Get one realization of the RandomMixture */
Point RandomMixture::getRealization() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  Point realization(size);
  for (UnsignedInteger i = 0; i < size; ++i) realization[i] = distributionCollection_[i].getRealization()[0];
  return weights_ * realization + constant_;
}

/* Get a Sample of the RandomMixture */
Sample RandomMixture::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger atomSize = distributionCollection_.getSize();
  Sample sample(atomSize, size);
  for (UnsignedInteger i = 0; i < atomSize; ++i)
  {
    sample[i] = distributionCollection_[i].getSample(size).asPoint();
  }
  return weights_.getImplementation()->genSampleProd(sample, true, true, 'R') + constant_;
}

Sample RandomMixture::getSampleByQMC(const UnsignedInteger size) const
{
  const UnsignedInteger atomSize = distributionCollection_.getSize();
  Sample sample(atomSize, size);
  const Point u(SobolSequence(1).generate(size).getImplementation()->getData());
  for (UnsignedInteger i = 0; i < atomSize; ++i)
  {
    sample[i] = distributionCollection_[i].computeQuantile(u).asPoint();
  }
  return weights_.getImplementation()->genSampleProd(sample, true, true, 'R') + constant_;
}

/* Get the DDF of the RandomMixture */
Point RandomMixture::computeDDF(const Point & point) const
{
  return DistributionImplementation::computeDDF(point);
}

/* Wrapper for the convolution in the 1D case with 2 atoms */
struct RandomMixture2AtomsWrapper
{
  RandomMixture2AtomsWrapper(const Scalar alpha1,
                             const Scalar alpha2,
                             const Distribution & atom1,
                             const Distribution & atom2,
                             const Scalar z0)
    : alpha1_(alpha1)
    , alpha2_(alpha2)
    , atom1_(atom1)
    , atom2_(atom2)
    , z0_(z0)
  {
    // Nothing to do
  }
  // Z = alpha0 + alpha1 X1 + alpha2 X2
  Point convolutionPDFKernel(const Point & point) const
  {
    const Scalar t = point[0];
    const Scalar pdf = atom1_.computePDF(t);
    if (pdf == 0.0) return Point(1, 0.0);
    return Point(1, pdf * atom2_.computePDF((z0_ - alpha1_ * t) / alpha2_));
  }

  Point convolutionCDFKernel(const Point & point) const
  {
    const Scalar t = point[0];
    const Scalar pdf = atom1_.computePDF(t);
    if (pdf == 0.0) return Point(1, 0.0);
    return Point(1, pdf * atom2_.computeCDF((z0_ - alpha1_ * t) / alpha2_));
  }

  Point convolutionComplementaryCDFKernel(const Point & point) const
  {
    const Scalar t = point[0];
    const Scalar pdf = atom1_.computePDF(t);
    if (pdf == 0.0) return Point(1, 0.0);
    return Point(1, pdf * atom2_.computeComplementaryCDF((z0_ - alpha1_ * t) / alpha2_));
  }

  const Scalar alpha1_;
  const Scalar alpha2_;
  const Distribution & atom1_;
  const Distribution & atom2_;
  const Scalar z0_;
};

/* Get the PDF of the RandomMixture. It uses the Poisson inversion formula as described in the reference:
   "Abate, J. and Whitt, W. (1992). The Fourier-series method for inverting
   transforms of probability distributions. Queueing Systems 10, 5--88., 1992",
   formula 5.5.
   We use an incremental update of the trigonometric functions and reduce the complex arithmetic to a real
   arithmetic for performance purpose.

   Here, we recall the Poisson summation formula:
   \sum_{k\in Z}p(x+2k\pi/h) = h/2\pi\sum_{j\in Z}\phi(jh)\exp(-Ihjx)
   We can rewrite this formula as:
   \sum_{k\in Z}p(x+2k\pi/h) = h/2\pi\sum_{j\in Z}\left[\phi(jh) - \psi(jh)\right]\exp(-Ihjx) + \sum_{k\in Z}q(x+2k\pi/h),
   where q is the PDF and \psi the characteristic function of the normal distribution with the same mean and
   the same variance as the mixture. Take h such as p(x+2k\pi/h) << p(x) for k\neq 0, then:
   p(x) \simeq h/2\pi\sum_{j\in Z}\left[\phi(jh) - \psi(jh)\right]\exp(-Ihjx) + \sum_{k\in Z}q(x+2k\pi/h).
   The second sum \sum_{k\in Z}q(x+2k\pi/h) will be approximated using only few terms, as the condition on h will almost
   gives q(x+2k\pi/h) << q(x) for k\neq 0. Call this sum Q(x, h), and define \delta as delta(t) = \phi(t) - \psi(t).
   We unroll the complex arithmetic for performance purpose:
   p(x) \simeq h/2\pi\sum_{j\neq 0}\delta(jh)\exp(-Ihjx) + Q(x, h) as \delta(0) = 0
   \simeq h/\pi\sum_{j>0} Re(\delta(jh)) * cos(jhx) + Im(\delta(jh)) * sin(jhx) + Q(x, h)
*/
Scalar RandomMixture::computePDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();

  if (isAnalytical_)
  {
    // compute analytically the pdf
    const Point u(point - constant_);
    const Point Qu(inverseWeights_ * u);
    // Scaling factor is 1 for discrete PDF and inverse of the Jacobian for continuous PDF
    Scalar value = (isDiscrete() ? 1.0 : std::abs(detWeightsInverse_));
    for (UnsignedInteger j = 0; j < dimension_; ++j) value *= distributionCollection_[j].computePDF(Qu[j]);
    return value;
  } // isAnalytical_

  // Check range
  // We check that point is in range, excepted bounds
  // In bounds, value is 0.0
  const Interval range(getRange());
  const Point lowerBound(range.getLowerBound());
  const Point upperBound(range.getUpperBound());
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    if ((point[j] <= lowerBound[j]) || (point[j] >= upperBound[j])) return 0.0;
  // Check for exotic situations not yet implemented by the class
  if (!isContinuous() && distributionCollection_.getSize() > 1) throw NotYetImplementedException(HERE) << "Error: no algorithm is currently available for the non-continuous case with more than one atom.";
  // Special case for 1D distributions with exactly 2 continuous atoms
  if ((dimension_ == 1) && (distributionCollection_.getSize() == 2) && distributionCollection_[0].isContinuous() && distributionCollection_[1].isContinuous())
  {
    // Get the parameters of the random mixture
    const Scalar z0 = point[0] - constant_[0];
    const Scalar alpha1 = weights_(0, 0);
    const Scalar alpha2 = weights_(0, 1);
    // Get the bounds of the atoms
    const Scalar a = distributionCollection_[0].getRange().getLowerBound()[0];
    const Scalar b = distributionCollection_[0].getRange().getUpperBound()[0];
    const Scalar c = distributionCollection_[1].getRange().getLowerBound()[0];
    const Scalar d = distributionCollection_[1].getRange().getUpperBound()[0];
    // Compute the bounds of the convolution
    Scalar lower = -1.0;
    Scalar upper = -1.0;
    Scalar uc = (z0 - alpha2 * c) / alpha1;
    Scalar ud = (z0 - alpha2 * d) / alpha1;
    if ((alpha1 > 0) == (alpha2 > 0))
    {
      lower = std::max(a, ud);
      upper = std::min(b, uc);
    }
    else
    {
      lower = std::max(a, uc);
      upper = std::min(b, ud);
    }
    GaussKronrod algo;
    const RandomMixture2AtomsWrapper convolutionKernelWrapper(alpha1, alpha2, distributionCollection_[0], distributionCollection_[1], z0);
    const Function convolutionKernel(bindMethod<RandomMixture2AtomsWrapper, Point, Point>(convolutionKernelWrapper, &RandomMixture2AtomsWrapper::convolutionPDFKernel, 1, 1));
    return algo.integrate(convolutionKernel, Interval(lower, upper))[0] / std::abs(alpha2);
  }

  LOGDEBUG(OSS() << "Equivalent normal=" << equivalentNormal_);
  // We unroll the complex arithmetic and we perform incremental update in order to improve the performances
  Point two_pi_on_h(dimension_);
  for (UnsignedInteger k = 0; k < dimension_; ++k) two_pi_on_h[k] = 2.0 * M_PI / referenceBandwidth_[k];
  UnsignedInteger levelMax = 0;
  Scalar value = computeEquivalentNormalPDFSum(point, two_pi_on_h, 0, levelMax);

  UnsignedInteger k = 1;
  const Scalar precision = pdfPrecision_;
  const UnsignedInteger kmin = 1 << blockMin_;
  const UnsignedInteger kmax = 1 << blockMax_;
  // hX is only useful in 1D
  Scalar hX = referenceBandwidth_[0] * point[0];
  Scalar error = 2.0 * precision;
  LOGDEBUG(OSS() << std::setprecision(20) << "h=" << referenceBandwidth_ << ", equivalent normal pdf sum=" << value << ", k=" << k << ", precision=" << precision << ", kmin=" << kmin << ", kmax=" << kmax << ", error=" << error);
  while ( (k < kmin) || ( (k < kmax) && (error > precision) ) )
  {
    Scalar sumContributions = 0.0;
    error = 0.0;
    for (UnsignedInteger m = k; m < 2 * k; ++m)
    {
      if (dimension_ == 1)
      {
        const Scalar sinMHX = std::sin(m * hX);
        const Scalar cosMHX = std::cos(m * hX);
        const Complex deltaValue(computeDeltaCharacteristicFunction(m));
        const Scalar contribution = (deltaValue.real() * cosMHX + deltaValue.imag() * sinMHX);
        LOGDEBUG(OSS() << "m=" << m << ", delta=" << deltaValue << ", contribution=" << contribution);
        sumContributions += contribution;
        error += std::abs(contribution);
      } // dimension_ == 1
      else
      {
        Sample skinPoints(gridMesher_.getPoints(m));
        const UnsignedInteger fromIndex = gridMesher_.getOffsetLevel(m);
        const UnsignedInteger lastIndex = gridMesher_.getOffsetLevel(m + 1) - 1;
        if (lastIndex <= maxSize_)
        {
          if (lastIndex > storedSize_)
            updateCacheDeltaCharacteristicFunction(skinPoints);
          // Level is now entirely on cache
          for (UnsignedInteger i = 0; i < skinPoints.getSize(); ++i)
          {
            const Complex deltaValue(characteristicValuesCache_[fromIndex + i - 1]);
            hX = 0.0;
            for (UnsignedInteger j = 0; j < dimension_; ++j) hX += skinPoints(i, j) * point[j];
            const Scalar sinHX = std::sin(hX);
            const Scalar cosHX = std::cos(hX);
            const Scalar contribution = deltaValue.real() * cosHX + deltaValue.imag() * sinHX;
            error += std::abs(contribution);
            sumContributions += contribution;
            LOGDEBUG(OSS() << "m=" << m << ", delta=" << deltaValue << ", contribution=" << contribution << ", error=" << error);
          } // skinPoints
        } // lastIndex <= maxSize_
        else
        {
          Point pti(dimension_);
          for (UnsignedInteger i = 0; i < skinPoints.getSize(); ++i)
          {
            hX = 0.0;
            for (UnsignedInteger j = 0; j < dimension_; ++j)
            {
              pti[j] = skinPoints(i, j);
              hX += skinPoints(i, j) * point[j];
            }
            const Complex deltaValue(computeDeltaCharacteristicFunction(pti));
            const Scalar sinHX = std::sin(hX);
            const Scalar cosHX = std::cos(hX);
            const Scalar contribution = deltaValue.real() * cosHX + deltaValue.imag() * sinHX;
            error += std::abs(contribution);
            sumContributions += contribution;
            LOGDEBUG(OSS() << "m=" << m << ", delta=" << deltaValue << ", contribution=" << contribution << ", error=" << error);
          } // skinPoints
        } // lastIndex > maxSize_
      } // dimension > 1
    }
    error *= referenceBandwidthFactor_;
    sumContributions *= referenceBandwidthFactor_;
    if (gridMesher_.isSymmetric())
    {
      error *= 2.0;
      sumContributions *= 2.0;
    }
    value += sumContributions;
    k *= 2;
  } // while
  // For very low level of PDF, the computed value can be slightly negative. Round it up to zero.
  if (value < 0.0) value = 0.0;
  return value;
}

/*  Compute the PDF of 1D distributions over a regular grid. The precision is reduced as this method is for drawing purpose only. */
Sample RandomMixture::computePDF(const Scalar xMin,
                                 const Scalar xMax,
                                 const UnsignedInteger pointNumber,
                                 Sample & grid) const
{
  if (getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: this method is available only for 1D distribution";
  return computePDF(Point(1, xMin), Point(1, xMax), Indices(1, pointNumber), grid);
}

struct EquivalentNormalPDFSumPolicy
{
  const RandomMixture & mixture_;
  const Sample & grid_;
  const Point & two_b_sigma_;
  const UnsignedInteger levelMax_;
  Collection<Scalar> & output_;

  EquivalentNormalPDFSumPolicy(const RandomMixture & mixture,
                               const Sample & grid,
                               const Point & two_b_sigma,
                               const UnsignedInteger levelMax,
                               Collection<Scalar> & output)
    : mixture_(mixture)
    , grid_(grid)
    , two_b_sigma_(two_b_sigma)
    , levelMax_(levelMax)
    , output_(output)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    UnsignedInteger fakeLevelMax = 0;
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      output_[i] = mixture_.computeEquivalentNormalPDFSum(grid_[i], two_b_sigma_, levelMax_, fakeLevelMax);
    }
  }
}; /* end struct EquivalentNormalPDFSumPolicy */

/* Compute the PDF of nD distributions over a regular grid */
Sample RandomMixture::computePDF(const Point & xMin,
                                 const Point & xMax,
                                 const Indices & pointNumber,
                                 Sample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: xMin & xMax should be different to define a grid";
  if (isAnalytical_ && (dimension_ == 1)) return DistributionImplementation::computePDF(xMin, xMax, pointNumber, grid);
  IndicesCollection indices(Tuples(pointNumber).generate());

  if (dimension_ < 1 || dimension_ > 3) throw InvalidArgumentException(HERE) << "Error: dimension must be 1, 2 or 3; here dimension=" << dimension_;
  // Special case for 1D distributions with exactly 2 atoms
  if ((dimension_ == 1) && (distributionCollection_.getSize() == 2))
  {
    const Scalar a = xMin[0];
    const Scalar b = xMax[0];
    const UnsignedInteger n = pointNumber[0];
    grid = Sample(n, 1);
    Sample pdf(n, 1);
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      const Scalar x = a + i * (b - a) / (n - 1);
      grid(i, 0) = x;
      pdf(i, 0) = computePDF(x);
    }
    return pdf;
  } // dimension == 1 && size == 2
  const Point mu(getMean());
  const Interval bounds(xMin, xMax);
  //if (!bounds.contains(mu)) throw InvalidArgumentException(HERE) << "Error: requested interval does not contain mean=" << mu;

  const Point sigma(getStandardDeviation());
  UnsignedInteger b = 0;
  for(UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar dx = std::max(mu[i] - xMin[i], xMax[i] - mu[i]);
    b = std::max(b, static_cast<UnsignedInteger>(std::ceil(dx / sigma[i])));
  }
  const Point b_sigma(b * sigma);
  const Point two_b_sigma(2.0 * b_sigma);

  Point h(dimension_);
  Point tau(dimension_);
  for(UnsignedInteger i = 0; i < dimension_; ++i)
  {
    h[i] = M_PI / b_sigma[i];
    tau[i] = mu[i] / b_sigma[i];
  }
  const UnsignedInteger size = indices.getSize();
  grid = Sample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      grid(i, j) = mu[j] + ((2.0 * indices(i, j) + 1.0) / pointNumber[j] - 1.0) * b_sigma[j];

  LOGWARN(OSS() << "Warning! Grid is modified: xMin=" << grid[0] << " xMax=" << grid[size - 1] << " instead of xMin=" << xMin << ", xMax=" << xMax);

  Sample result(size, 1);
  // Special case when the distribution is analytical
  if (isAnalytical_) return computePDF(grid);
  UnsignedInteger levelMax = 0;
  // Compute Gaussian sum pdf
  // First compute levelMax on mu, to speed up calls to computeEquivalentNormalPDFSum
  (void) computeEquivalentNormalPDFSum(mu, two_b_sigma, 0, levelMax);

  Collection<Scalar> output(size);
  const  EquivalentNormalPDFSumPolicy policyGrid(*this, grid, two_b_sigma, levelMax, output);
  TBB::ParallelFor( 0, size, policyGrid);

  result.getImplementation()->setData(output);

  // Methods below will call computeDeltaCharacteristicFunction() on different threads
  // if using TBB, which in turn calls equivalentNormal_.computeCharacteristicFunction()
  // and then equivalentNormal_.getCovariance().  But covariance is lazily evaluated.
  // We must ensure that it is computed before entering TBB multithreaded section.
  (void) equivalentNormal_.getCovariance();

  switch(dimension_)
  {
    case 1:
      addPDFOn1DGrid(pointNumber, h, tau, result);
      break;
    case 2:
      addPDFOn2DGrid(pointNumber, h, tau, result);
      break;
    case 3:
      addPDFOn3DGrid(pointNumber, h, tau, result);
      break;
  }
  for (UnsignedInteger j = 0; j < size; ++j)
  {
    result(j, 0) = std::max(0.0, result(j, 0));
  }
  return result;
}

struct AddPDFOn1DGridPolicy
{
  const RandomMixture & mixture_;
  const Point & xPoints_;
  Collection<Complex> & output_;

  AddPDFOn1DGridPolicy(const RandomMixture & mixture,
                       const Point & xPoints,
                       Collection<Complex> & output)
    : mixture_(mixture)
    , xPoints_(xPoints)
    , output_(output)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    Point x(1);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      x[0] = xPoints_[i];
      output_[i] = mixture_.computeDeltaCharacteristicFunction(x);
    }
  }
}; /* end struct AddPDFOn1DGridPolicy */

void RandomMixture::addPDFOn1DGrid(const Indices & pointNumber, const Point & h, const Point & tau, Sample & result) const
{
  if (pointNumber.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: the given indices must have dimension=1, here dimension=" << pointNumber.getSize();

  const UnsignedInteger N = pointNumber[0];
  Collection<Complex> fx(N);
  Collection<Complex> z_exp(N);
  const Complex cOne(0.0, 1.0);
  // Grid points
  Point xPlus(N);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    xPlus[i] = (i + 1) * h[0];
    fx[i] = std::exp(- M_PI * cOne * (tau[0] - 1.0 + 1.0 / N) * (1.0 + i));
    z_exp[i] = std::exp(- 2.0 * M_PI * cOne * static_cast<Scalar>(i) / static_cast<Scalar>(N));
  }

  // FFT 1D
  Collection<Complex> yk(N);
  // 1) compute \Sigma_+
  const  AddPDFOn1DGridPolicy policyGridPP(*this, xPlus, yk);
  TBB::ParallelFor( 0, N, policyGridPP);
  for (UnsignedInteger j = 0; j < N; ++j)
    yk[j] *= fx[j];

  Collection<Complex> sigma_plus(fftAlgorithm_.transform(yk));

  for (UnsignedInteger j = 0; j < N; ++j)
    sigma_plus[j] *= z_exp[j];

  // 2) compute \Sigma_-
  Collection<Complex> ykc(N);
  for (UnsignedInteger j = 0; j < N; ++j)
    ykc[j] = std::conj(yk[N - 1 - j]);

  Collection<Complex> sigma_minus(fftAlgorithm_.transform(ykc));

  const Scalar scaling = h[0] / (2.0 * M_PI);
  for (UnsignedInteger j = 0; j < N; ++j)
  {
    result(j, 0) += scaling * std::real( sigma_plus[j] + sigma_minus[j] );
  }
}

struct AddPDFOn2DGridPolicy
{
  const RandomMixture & mixture_;
  const Point & xPoints_;
  const Point & yPoints_;
  const UnsignedInteger nx_;
  const UnsignedInteger ny_;
  Collection<Complex> & output_;

  AddPDFOn2DGridPolicy(const RandomMixture & mixture,
                       const Point & xPoints,
                       const Point & yPoints,
                       Collection<Complex> & output)
    : mixture_(mixture)
    , xPoints_(xPoints)
    , yPoints_(yPoints)
    , nx_(xPoints.getDimension())
    , ny_(yPoints.getDimension())
    , output_(output)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    Point x(2);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger jj = i / nx_;
      const UnsignedInteger ii = i - jj * nx_;
      x[0] = xPoints_[ii];
      x[1] = yPoints_[jj];
      output_[i] = mixture_.computeDeltaCharacteristicFunction(x);
    }
  }
}; /* end struct AddPDFOn2DGridPolicy */

void RandomMixture::addPDFOn2DGrid(const Indices & pointNumber, const Point & h, const Point & tau, Sample & result) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: the given indices must have dimension=2, here dimension=" << pointNumber.getSize();

  const UnsignedInteger Nx = pointNumber[0];
  const UnsignedInteger Ny = pointNumber[1];
  Collection<Complex> fx(Nx);
  Collection<Complex> fy(Ny);
  Collection<Complex> z_exp_mx(Nx);
  Collection<Complex> z_exp_my(Ny);
  const Complex cOne(0.0, 1.0);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    fx[i] = std::exp(- M_PI * cOne * (tau[0] - 1.0 + 1.0 / Nx) * (1.0 + i));
    z_exp_mx[i] = std::exp(- 2.0 * M_PI * cOne * static_cast<Scalar>(i) / static_cast<Scalar>(Nx));
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    fy[j] = std::exp(- M_PI * cOne * (tau[1] - 1.0 + 1.0 / Ny) * (1.0 + j));
    z_exp_my[j] = std::exp(- 2.0 * M_PI * cOne * static_cast<Scalar>(j) / static_cast<Scalar>(Ny));
  }
  Point xPlus(Nx);
  Point xMinus(Nx);
  Point yPlus(Ny);
  Point yMinus(Ny);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    xPlus[i] = (i + 1) * h[0];
    xMinus[i] = (static_cast<Scalar>(i) - Nx) * h[0];
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    yPlus[j] = (j + 1) * h[1];
    yMinus[j] = (static_cast<Scalar>(j) - Ny) * h[1];
  }
  ComplexMatrix yk(Nx, Ny);
  // 1) compute \Sigma_++
  const  AddPDFOn2DGridPolicy policyGridPP(*this, xPlus, yPlus, *(yk.getImplementation().get()));
  TBB::ParallelFor( 0, Nx * Ny, policyGridPP);
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      yk(i, j) *= fx[i] * fy[j];

  ComplexMatrix sigma_plus_plus(fftAlgorithm_.transform2D(yk));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_plus_plus(i, j) *= z_exp_mx[i] * z_exp_my[j];

  // 2) compute \Sigma_--
  ComplexMatrix ykc(Nx, Ny);
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      ykc(i, j) = std::conj(yk(Nx - 1 - i, Ny - 1 - j));
  ComplexMatrix sigma_minus_minus(fftAlgorithm_.transform2D(ykc));

  // 3) compute \Sigma_+-
  const  AddPDFOn2DGridPolicy policyGridPM(*this, xPlus, yMinus, *(yk.getImplementation().get()));
  TBB::ParallelFor( 0, Nx * Ny, policyGridPM);
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      yk(i, j) *= fx[i] * std::conj(fy[Ny - 1 - j]);

  ComplexMatrix sigma_plus_minus(fftAlgorithm_.transform2D(yk));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_plus_minus(i, j) *= z_exp_mx[i];

  // 4) compute \Sigma_-+
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      ykc(i, j) = std::conj(yk(Nx - 1 - i, Ny - 1 - j));

  ComplexMatrix sigma_minus_plus(fftAlgorithm_.transform2D(ykc));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_minus_plus(i, j) *= z_exp_my[j];

  // 5) compute \Sigma_+0
  ComplexCollection yk0(Nx);
  Point x(2);
  x[1] = 0.0;
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    x[0] = (i + 1) * h[0];
    yk0[i] = computeDeltaCharacteristicFunction(x) * fx[i];
  }
  ComplexCollection sigma_plus_0(fftAlgorithm_.transform(yk0));
  for (UnsignedInteger i = 0; i < Nx; ++i)
    sigma_plus_0[i] *= z_exp_mx[i];

  // 6) compute \Sigma_-0
  ComplexCollection yk0c(Nx);
  for (UnsignedInteger i = 0; i < Nx; ++i)
    yk0c[i] = std::conj(yk0[Nx - 1 - i]);
  ComplexCollection sigma_minus_0(fftAlgorithm_.transform(yk0c));

  // 7) compute \Sigma_0+
  if (Nx != Ny)
  {
    yk0.resize(Ny);
    yk0c.resize(Ny);
  }
  x[0] = 0.0;
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    x[1] = (j + 1) * h[1];
    yk0[j] = computeDeltaCharacteristicFunction(x) * fy[j];
  }
  ComplexCollection sigma_0_plus(fftAlgorithm_.transform(yk0));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    sigma_0_plus[j] *= z_exp_my[j];

  // 8) compute \Sigma_0-
  for (UnsignedInteger j = 0; j < Ny; ++j)
    yk0c[j] = std::conj(yk0[Ny - 1 - j]);
  ComplexCollection sigma_0_minus(fftAlgorithm_.transform(yk0c));

  UnsignedInteger counter = 0;
  const Scalar scaling = (h[0] * h[1]) / (4.0 * M_PI * M_PI);
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    for (UnsignedInteger i = 0; i < Nx; ++i, ++counter)
    {
      result(counter, 0) += scaling * std::real(
                              sigma_plus_plus(i, j)   + sigma_minus_minus(i, j) +
                              sigma_plus_minus(i, j)  + sigma_minus_plus(i, j)  +
                              sigma_plus_0[i]  + sigma_minus_0[i] +
                              sigma_0_plus[j]  + sigma_0_minus[j]
                            );
    }
  }
}

struct AddPDFOn3DGridPolicy
{
  const RandomMixture & mixture_;
  const Point & xPoints_;
  const Point & yPoints_;
  const Point & zPoints_;
  const UnsignedInteger nx_;
  const UnsignedInteger ny_;
  const UnsignedInteger nz_;
  Collection<Complex> & output_;

  AddPDFOn3DGridPolicy(const RandomMixture & mixture,
                       const Point & xPoints,
                       const Point & yPoints,
                       const Point & zPoints,
                       Collection<Complex> & output)
    : mixture_(mixture)
    , xPoints_(xPoints)
    , yPoints_(yPoints)
    , zPoints_(zPoints)
    , nx_(xPoints.getDimension())
    , ny_(yPoints.getDimension())
    , nz_(zPoints.getDimension())
    , output_(output)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    Point x(3);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger kk = i / nx_ / ny_;
      const UnsignedInteger jj = ( i - kk * nx_ * ny_ ) / nx_;
      const UnsignedInteger ii = i - kk * nx_ * ny_ - jj * nx_;
      x[0] = xPoints_[ii];
      x[1] = yPoints_[jj];
      x[2] = zPoints_[kk];
      output_[i] = mixture_.computeDeltaCharacteristicFunction(x);
    }
  }
}; /* end struct AddPDFOn3DGridPolicy */

void RandomMixture::addPDFOn3DGrid(const Indices & pointNumber, const Point & h, const Point & tau, Sample & result) const
{
  if (pointNumber.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: the given indices must have dimension=3, here dimension=" << pointNumber.getSize();

  const UnsignedInteger Nx = pointNumber[0];
  const UnsignedInteger Ny = pointNumber[1];
  const UnsignedInteger Nz = pointNumber[2];
  Collection<Complex> fx(Nx);
  Collection<Complex> fy(Ny);
  Collection<Complex> fz(Nz);
  Collection<Complex> z_exp_mx(Nx);
  Collection<Complex> z_exp_my(Ny);
  Collection<Complex> z_exp_mz(Nz);
  const Complex cOne(0.0, 1.0);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    fx[i] = std::exp(- M_PI * cOne * (tau[0] - 1.0 + 1.0 / Nx) * (1.0 + i));
    z_exp_mx[i] = std::exp(- 2.0 * M_PI * cOne * static_cast<Scalar>(i) / static_cast<Scalar>(Nx));
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    fy[j] = std::exp(- M_PI * cOne * (tau[1] - 1.0 + 1.0 / Ny) * (1.0 + j));
    z_exp_my[j] = std::exp(- 2.0 * M_PI * cOne * static_cast<Scalar>(j) / static_cast<Scalar>(Ny));
  }
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    fz[k] = std::exp(- M_PI * cOne * (tau[2] - 1.0 + 1.0 / Nz) * (1.0 + k));
    z_exp_mz[k] = std::exp(- 2.0 * M_PI * cOne * static_cast<Scalar>(k) / static_cast<Scalar>(Nz));
  }
  Point xPlus(Nx);
  Point xMinus(Nx);
  Point yPlus(Ny);
  Point yMinus(Ny);
  Point zPlus(Nz);
  Point zMinus(Nz);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    xPlus[i] = (i + 1) * h[0];
    xMinus[i] = (static_cast<Scalar>(i) - Nx) * h[0];
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    yPlus[j] = (j + 1) * h[1];
    yMinus[j] = (static_cast<Scalar>(j) - Ny) * h[1];
  }
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    zPlus[k] = (k + 1) * h[2];
    zMinus[k] = (static_cast<Scalar>(k) - Nz) * h[2];
  }
  ComplexTensor yk(Nx, Ny, Nz);
  const  AddPDFOn3DGridPolicy policyGridPPP(*this, xPlus, yPlus, zPlus, *(yk.getImplementation().get()));
  TBB::ParallelFor( 0, Nx * Ny * Nz, policyGridPPP);
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        yk(i, j, k) *= fx[i] * fy[j] * fz[k];

  // 1) compute \Sigma_+++
  ComplexTensor sigma_plus_plus_plus(fftAlgorithm_.transform3D(yk));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        sigma_plus_plus_plus(i, j, k) *= z_exp_mx[i] * z_exp_my[j] * z_exp_mz[k];
  // 2) compute \Sigma_---
  ComplexTensor ykc(Nx, Ny, Nz);
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        ykc(i, j, k) = std::conj(yk(Nx - 1 - i, Ny - 1 - j, Nz - 1 - k));
  ComplexTensor sigma_minus_minus_minus(fftAlgorithm_.transform3D(ykc));

  // 3) compute \Sigma_++-
  const  AddPDFOn3DGridPolicy policyGridPPM(*this, xPlus, yPlus, zMinus, *(yk.getImplementation().get()));
  TBB::ParallelFor( 0, Nx * Ny * Nz, policyGridPPM);
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        yk(i, j, k) *= fx[i] * fy[j] * std::conj(fz[Nz - 1 - k]);

  ComplexTensor sigma_plus_plus_minus(fftAlgorithm_.transform3D(yk));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        sigma_plus_plus_minus(i, j, k) *= z_exp_mx[i] * z_exp_my[j];

  // 4) compute \Sigma_--+
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        ykc(i, j, k) = std::conj(yk(Nx - 1 - i, Ny - 1 - j, Nz - 1 - k));

  ComplexTensor sigma_minus_minus_plus(fftAlgorithm_.transform3D(ykc));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        sigma_minus_minus_plus(i, j, k) *= z_exp_mz[k];

  // 5) compute \Sigma_+-+
  const  AddPDFOn3DGridPolicy policyGridPMP(*this, xPlus, yMinus, zPlus, *(yk.getImplementation().get()));
  TBB::ParallelFor( 0, Nx * Ny * Nz, policyGridPMP);
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        yk(i, j, k) *= fx[i] * std::conj(fy[Ny - 1 - j]) * fz[k];

  ComplexTensor sigma_plus_minus_plus(fftAlgorithm_.transform3D(yk));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        sigma_plus_minus_plus(i, j, k) *= z_exp_mx[i] * z_exp_mz[k];

  // 6) compute \Sigma_-+-
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        ykc(i, j, k) = std::conj(yk(Nx - 1 - i, Ny - 1 - j, Nz - 1 - k));

  ComplexTensor sigma_minus_plus_minus(fftAlgorithm_.transform3D(ykc));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        sigma_minus_plus_minus(i, j, k) *= z_exp_my[j];

  // 7) compute \Sigma_+--
  const  AddPDFOn3DGridPolicy policyGridPMM(*this, xPlus, yMinus, zMinus, *(yk.getImplementation().get()));
  TBB::ParallelFor( 0, Nx * Ny * Nz, policyGridPMM);
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        yk(i, j, k) *= fx[i] * std::conj(fy[Ny - 1 - j]) * std::conj(fz[Nz - 1 - k]);

  ComplexTensor sigma_plus_minus_minus(fftAlgorithm_.transform3D(yk));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        sigma_plus_minus_minus(i, j, k) *= z_exp_mx[i];

  // 8) compute \Sigma_-++
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        ykc(i, j, k) = std::conj(yk(Nx - 1 - i, Ny - 1 - j, Nz - 1 - k));

  ComplexTensor sigma_minus_plus_plus(fftAlgorithm_.transform3D(ykc));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      for (UnsignedInteger i = 0; i < Nx; ++i)
        sigma_minus_plus_plus(i, j, k) *= z_exp_my[j] * z_exp_mz[k];

  // 9) compute \Sigma_++0
  ComplexMatrix yk0(Nx, Ny);
  Point x(3);
  x[2] = 0.0;
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    x[1] = (j + 1) * h[1];
    for (UnsignedInteger i = 0; i < Nx; ++i)
    {
      x[0] = (i + 1) * h[0];
      yk0(i, j) = computeDeltaCharacteristicFunction(x) * fx[i] * fy[j];
    }
  }
  ComplexMatrix sigma_plus_plus_0(fftAlgorithm_.transform2D(yk0));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_plus_plus_0(i, j) *= z_exp_mx[i] * z_exp_my[j];

  // 10) compute \Sigma_--0
  ComplexMatrix yk0c(Nx, Ny);
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      yk0c(i, j) = std::conj(yk0(Nx - 1 - i, Ny - 1 - j));
  ComplexMatrix sigma_minus_minus_0(fftAlgorithm_.transform2D(yk0c));

  // 11) compute \Sigma_0++
  if (Nx != Ny || Ny != Nz)
  {
    yk0 = ComplexMatrix(Ny, Nz);
    yk0c = ComplexMatrix(Ny, Nz);
  }
  x[0] = 0.0;
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    x[2] = (k + 1) * h[2];
    for (UnsignedInteger j = 0; j < Ny; ++j)
    {
      x[1] = (j + 1) * h[1];
      yk0(j, k) = computeDeltaCharacteristicFunction(x) * fy[j] * fz[k];
    }
  }
  ComplexMatrix sigma_0_plus_plus(fftAlgorithm_.transform2D(yk0));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      sigma_0_plus_plus(j, k) *= z_exp_my[j] * z_exp_mz[k];

  // 12) compute \Sigma_0--
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      yk0c(j, k) = std::conj(yk0(Ny - 1 - j, Nz - 1 - k));
  ComplexMatrix sigma_0_minus_minus(fftAlgorithm_.transform2D(yk0c));

  // 13) compute \Sigma_+0+
  if (Nx != Ny)
  {
    yk0 = ComplexMatrix(Nx, Nz);
    yk0c = ComplexMatrix(Nx, Nz);
  }
  x[1] = 0.0;
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    x[2] = (k + 1) * h[2];
    for (UnsignedInteger i = 0; i < Nx; ++i)
    {
      x[0] = (i + 1) * h[0];
      yk0(i, k) = computeDeltaCharacteristicFunction(x) * fx[i] * fz[k];
    }
  }
  ComplexMatrix sigma_plus_0_plus(fftAlgorithm_.transform2D(yk0));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_plus_0_plus(i, k) *= z_exp_mx[i] * z_exp_mz[k];

  // 14) compute \Sigma_-0-
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      yk0c(i, k) = std::conj(yk0(Nx - 1 - i, Nz - 1 - k));
  ComplexMatrix sigma_minus_0_minus(fftAlgorithm_.transform2D(yk0c));

  // 15) compute \Sigma_+-0
  if (Ny != Nz)
  {
    yk0 = ComplexMatrix(Nx, Ny);
    yk0c = ComplexMatrix(Nx, Ny);
  }
  x[2] = 0.0;
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    x[1] = (static_cast<Scalar>(j) - Ny) * h[1];
    for (UnsignedInteger i = 0; i < Nx; ++i)
    {
      x[0] = (i + 1) * h[0];
      yk0(i, j) = computeDeltaCharacteristicFunction(x) * fx[i] * std::conj(fy[Ny - 1 - j]);
    }
  }
  ComplexMatrix sigma_plus_minus_0(fftAlgorithm_.transform2D(yk0));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_plus_minus_0(i, j) *= z_exp_mx[i];

  // 16) compute \Sigma_-+0
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      yk0c(i, j) = std::conj(yk0(Nx - 1 - i, Ny - 1 - j));
  ComplexMatrix sigma_minus_plus_0(fftAlgorithm_.transform2D(yk0c));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_minus_plus_0(i, j) *= z_exp_my[j];

  // 17) compute \Sigma_+0-
  if (Nz != Ny)
  {
    yk0 = ComplexMatrix(Nx, Nz);
    yk0c = ComplexMatrix(Nx, Nz);
  }
  x[1] = 0.0;
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    x[2] = (static_cast<Scalar>(k) - Nz) * h[2];
    for (UnsignedInteger i = 0; i < Nx; ++i)
    {
      x[0] = (i + 1) * h[0];
      yk0(i, k) = computeDeltaCharacteristicFunction(x) * fx[i] * std::conj(fz[Nz - 1 - k]);
    }
  }
  ComplexMatrix sigma_plus_0_minus(fftAlgorithm_.transform2D(yk0));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_plus_0_minus(i, k) *= z_exp_mx[i];

  // 18) compute \Sigma_-0+
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      yk0c(i, k) = std::conj(yk0(Nx - 1 - i, Nz - 1 - k));
  ComplexMatrix sigma_minus_0_plus(fftAlgorithm_.transform2D(yk0c));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger i = 0; i < Nx; ++i)
      sigma_minus_0_plus(i, k) *= z_exp_mz[k];

  // 19) compute \Sigma_0+-
  if (Nx != Ny)
  {
    yk0 = ComplexMatrix(Ny, Nz);
    yk0c = ComplexMatrix(Ny, Nz);
  }
  x[0] = 0.0;
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    x[2] = (static_cast<Scalar>(k) - Nz) * h[2];
    for (UnsignedInteger j = 0; j < Ny; ++j)
    {
      x[1] = (j + 1) * h[1];
      yk0(j, k) = computeDeltaCharacteristicFunction(x) * fy[j] * std::conj(fz[Nz - 1 - k]);
    }
  }
  ComplexMatrix sigma_0_plus_minus(fftAlgorithm_.transform2D(yk0));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      sigma_0_plus_minus(j, k) *= z_exp_my[j];

  // 20) compute \Sigma_0-+
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      yk0c(j, k) = std::conj(yk0(Ny - 1 - j, Nz - 1 - k));
  ComplexMatrix sigma_0_minus_plus(fftAlgorithm_.transform2D(yk0c));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    for (UnsignedInteger j = 0; j < Ny; ++j)
      sigma_0_minus_plus(j, k) *= z_exp_mz[k];

  // 21) compute \Sigma_+00
  Collection<Complex> yk00(Nx);
  x[1] = 0.0;
  x[2] = 0.0;
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    x[0] = (i + 1) * h[0];
    yk00[i] = computeDeltaCharacteristicFunction(x) * fx[i];
  }
  Collection<Complex> sigma_plus_0_0(fftAlgorithm_.transform(yk00));
  for (UnsignedInteger i = 0; i < Nx; ++i)
    sigma_plus_0_0[i] *= z_exp_mx[i];

  // 22) compute \Sigma_-00
  Collection<Complex> yk00c(Nx);
  for (UnsignedInteger i = 0; i < Nx; ++i)
    yk00c[i] = std::conj(yk00[Nx - 1 - i]);
  Collection<Complex> sigma_minus_0_0(fftAlgorithm_.transform(yk00c));

  // 23) compute \Sigma_0+0
  if (Nx != Ny)
  {
    yk00.resize(Ny);
    yk00c.resize(Ny);
  }
  x[0] = 0.0;
  x[2] = 0.0;
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    x[1] = (j + 1) * h[1];
    yk00[j] = computeDeltaCharacteristicFunction(x) * fy[j];
  }
  Collection<Complex> sigma_0_plus_0(fftAlgorithm_.transform(yk00));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    sigma_0_plus_0[j] *= z_exp_my[j];

  // 24) compute \Sigma_0-0
  for (UnsignedInteger j = 0; j < Ny; ++j)
    yk00c[j] = std::conj(yk00[Ny - 1 - j]);
  Collection<Complex> sigma_0_minus_0(fftAlgorithm_.transform(yk00c));

  // 25) compute \Sigma_00+
  if (Ny != Nz)
  {
    yk00.resize(Nz);
    yk00c.resize(Nz);
  }
  x[0] = 0.0;
  x[1] = 0.0;
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    x[2] = (k + 1) * h[2];
    yk00[k] = computeDeltaCharacteristicFunction(x) * fz[k];
  }
  Collection<Complex> sigma_0_0_plus(fftAlgorithm_.transform(yk00));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    sigma_0_0_plus[k] *= z_exp_mz[k];

  // 26) compute \Sigma_00-
  for (UnsignedInteger k = 0; k < Nz; ++k)
    yk00c[k] = std::conj(yk00[Nz - 1 - k]);
  Collection<Complex> sigma_0_0_minus(fftAlgorithm_.transform(yk00c));

  UnsignedInteger counter = 0;
  const Scalar scaling = (h[0] * h[1] * h[2]) / (8.0 * M_PI * M_PI * M_PI);
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    for (UnsignedInteger j = 0; j < Ny; ++j)
    {
      for (UnsignedInteger i = 0; i < Nx; ++i, ++counter)
      {
        result(counter, 0) += scaling * std::real(
                                sigma_plus_plus_plus(i, j, k)   + sigma_minus_minus_minus(i, j, k) +
                                sigma_plus_plus_minus(i, j, k)  + sigma_minus_minus_plus(i, j, k)  +
                                sigma_plus_minus_plus(i, j, k)  + sigma_minus_plus_minus(i, j, k)  +
                                sigma_plus_minus_minus(i, j, k) + sigma_minus_plus_plus(i, j, k)   +
                                sigma_plus_plus_0(i, j)  + sigma_minus_minus_0(i, j) +
                                sigma_plus_minus_0(i, j) + sigma_minus_plus_0(i, j)  +
                                sigma_plus_0_plus(i, k)  + sigma_minus_0_minus(i, k) +
                                sigma_plus_0_minus(i, k) + sigma_minus_0_plus(i, k)  +
                                sigma_0_plus_plus(j, k)  + sigma_0_minus_minus(j, k) +
                                sigma_0_plus_minus(j, k) + sigma_0_minus_plus(j, k)  +
                                sigma_0_0_plus[k] + sigma_0_0_minus[k] +
                                sigma_0_plus_0[j] + sigma_0_minus_0[j] +
                                sigma_plus_0_0[i] + sigma_minus_0_0[i]
                              );
      }
    }
  }
}

/* Get the CDF of the RandomMixture */
Scalar RandomMixture::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension())
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  if (point.getDimension() != 1) return computeProbability(Interval(getRange().getLowerBound(), point));

  const Scalar x = point[0];
  // Special case for combination containing only one contributor Y = alpha * X + beta
  // for alpha > 0.0:
  // P(Y < y) = P(X < (y - beta) / alpha) = CDF_X((y - beta) / alpha)
  // for alpha < 0.0:
  // P(Y < y) = P(X > (y - beta) / alpha) = 1.0 - CDF_X((y - beta) / alpha)
  if (isAnalytical_ && (dimension_ == 1))
  {
    const Scalar alpha = weights_(0, 0);
    const Scalar cdf = alpha > 0.0 ? distributionCollection_[0].computeCDF((x - constant_[0]) / alpha) : distributionCollection_[0].computeComplementaryCDF((x - constant_[0]) / alpha);
    return cdf;
  }
  // Check range
  const Interval range(getRange());
  const Scalar lowerBound = range.getLowerBound()[0];
  const Scalar upperBound = range.getUpperBound()[0];
  if (x <= lowerBound) return 0.0;
  if (x >= upperBound) return 1.0;
  if (!isContinuous() && distributionCollection_.getSize() > 1) throw NotYetImplementedException(HERE) << "Error: no algorithm is currently available for the non-continuous case with more than one atom.";
  // Special case for 1D distributions with exactly 2 continuous atoms
  if ((dimension_ == 1) && (distributionCollection_.getSize() == 2) && distributionCollection_[0].isContinuous() && distributionCollection_[1].isContinuous())
  {
    // Z = alpha0 + alpha1 X1 + alpha2 X2
    // F(z) = P(Z < z) = P(alpha1 X1 + alpha2 X2 < z0) with z0 = z - alpha0
    // If alpha2>0:
    // F(z) = \int_R F_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1
    // Else:
    // F(z) = \int_R Fbar_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1
    // Get the parameters of the random mixture
    const Scalar z0 = x - constant_[0];
    const Scalar alpha1 = weights_(0, 0);
    const Scalar alpha2 = weights_(0, 1);
    const RandomMixture2AtomsWrapper convolutionKernelWrapper(alpha1, alpha2, distributionCollection_[0], distributionCollection_[1], z0);
    GaussKronrod algo;
    // Get the bounds of the atoms
    const Scalar a = distributionCollection_[0].getRange().getLowerBound()[0];
    const Scalar b = distributionCollection_[0].getRange().getUpperBound()[0];
    const Scalar c = distributionCollection_[1].getRange().getLowerBound()[0];
    const Scalar d = distributionCollection_[1].getRange().getUpperBound()[0];
    // If alpha2 > 0
    if (alpha2 > 0.0)
    {
      // F(z) = \int_R F_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1
      const Function convolutionKernel(bindMethod<RandomMixture2AtomsWrapper, Point, Point>(convolutionKernelWrapper, &RandomMixture2AtomsWrapper::convolutionCDFKernel, 1, 1));
      // bounds:
      // x1 >= a otherwise p_X1 == 0
      // x1 <= b otherwise p_X1 == 0
      // (z0 - alpha1 x1) / alpha2 >= c otherwise F_X2 == 0
      // (z0 - alpha1 x1) / alpha2 <= d otherwise F_X2 == 1
      if (alpha1 > 0.0)
      {
        // (z0 - alpha1 x1) / alpha2 >= c <=> x1 <= (z0 - alpha2 * c) / alpha1 = beta
        // (z0 - alpha1 x1) / alpha2 <= d <=> x1 >= (z0 - alpha2 * d) / alpha1 = alpha
        const Scalar alpha = (z0 - alpha2 * d) / alpha1;
        const Scalar beta  = (z0 - alpha2 * c) / alpha1;
        const Scalar lower = std::max(a, alpha);
        const Scalar upper = std::min(b, beta);
        Scalar cdf = 0.0;
        if (lower < upper) cdf = algo.integrate(convolutionKernel, Interval(lower, upper))[0];
        // Take into account a possible missing tail:
        // \int_a^alpha F_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1 = F_X1(alpha)
        if (lower > a) cdf += distributionCollection_[0].computeCDF(alpha);
        return cdf;
      }
      // Here alpha1 < 0
      // (z0 - alpha1 x1) / alpha2 >= c <=> x1 >= (z0 - alpha2 * c) / alpha1 = alpha
      // (z0 - alpha1 x1) / alpha2 <= d <=> x1 <= (z0 - alpha2 * d) / alpha1 = beta
      const Scalar alpha = (z0 - alpha2 * c) / alpha1;
      const Scalar beta  = (z0 - alpha2 * d) / alpha1;
      const Scalar lower = std::max(a, alpha);
      const Scalar upper = std::min(b, beta);
      Scalar cdf = 0.0;
      if (lower < upper) cdf = algo.integrate(convolutionKernel, Interval(lower, upper))[0];
      // Take into account a possible missing tail:
      // \int_beta^b F_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1 = Fbar_X1(beta)
      if (upper < b) cdf += distributionCollection_[0].computeComplementaryCDF(beta);
      return cdf;
    } // alpha2 > 0
    // F(z) = \int_R Fbar_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1
    const Function convolutionKernel(bindMethod<RandomMixture2AtomsWrapper, Point, Point>(convolutionKernelWrapper, &RandomMixture2AtomsWrapper::convolutionComplementaryCDFKernel, 1, 1));
    // bounds:
    // x1 >= a otherwise p_X1 == 0
    // x1 <= b otherwise p_X1 == 0
    // (z0 - alpha1 x1) / alpha2 >= c otherwise Fbar_X2 == 1
    // (z0 - alpha1 x1) / alpha2 <= d otherwise Fbar_X2 == 0
    if (alpha1 > 0.0)
    {
      // (z0 - alpha1 x1) / alpha2 >= c <=> x1 >= (z0 - alpha2 * c) / alpha1 = alpha
      // (z0 - alpha1 x1) / alpha2 <= d <=> x1 <= (z0 - alpha2 * d) / alpha1 = beta
      const Scalar alpha = (z0 - alpha2 * c) / alpha1;
      const Scalar beta  = (z0 - alpha2 * d) / alpha1;
      const Scalar lower = std::max(a, alpha);
      const Scalar upper = std::min(b, beta);
      Scalar cdf = 0.0;
      if (lower < upper) cdf = algo.integrate(convolutionKernel, Interval(lower, upper))[0];
      // Take into account a possible missing tail:
      // \int_beta^b Fbar_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1 = Fbar_X1(beta)
      if (lower > a) cdf += distributionCollection_[0].computeCDF(alpha);
      return cdf;
    }
    // Here alpha1 < 0
    // (z0 - alpha1 x1) / alpha2 >= c <=> x1 <= (z0 - alpha2 * c) / alpha1 = beta
    // (z0 - alpha1 x1) / alpha2 <= d <=> x1 >= (z0 - alpha2 * d) / alpha1 = alpha
    const Scalar alpha = (z0 - alpha2 * d) / alpha1;
    const Scalar beta  = (z0 - alpha2 * c) / alpha1;
    const Scalar lower = std::max(a, alpha);
    const Scalar upper = std::min(b, beta);
    Scalar cdf = 0.0;
    if (lower < upper) cdf = algo.integrate(convolutionKernel, Interval(lower, upper))[0];
    // Take into account a possible missing tail:
    // \int_a^alpha Fbar_X2((z0 - alpha1 x1) / alpha2)p_X1(x1)dx1 = F_X1(alpha)
    if (upper < b) cdf += distributionCollection_[0].computeComplementaryCDF(beta);
    return cdf;
  } // dimension_ == 1 && size == 2

  // Here we call computeProbability with a ]-inf, x] interval
  const Scalar cdf = computeProbability(Interval(Point(1, lowerBound), point, getRange().getFiniteLowerBound(), Interval::BoolCollection(1, true)));
  if (cdf < 0.5) return cdf;
  // and if the cdf value is less than 1/2, it was better to use the complementary CDF
  else return 1.0 - computeProbability(Interval(point, Point(1, upperBound), Interval::BoolCollection(1, true), getRange().getFiniteUpperBound()));
}

Scalar RandomMixture::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != getDimension())
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  if (getDimension() > 1) return DistributionImplementation::computeComplementaryCDF(point);
  const Scalar x = point[0];
  // Special case for combination containing only one contributor Y = alpha * X + beta
  // for alpha > 0.0:
  // P(Y < y) = P(X < (y - beta) / alpha) = CDF_X((y - beta) / alpha)
  // for alpha < 0.0:
  // P(Y < y) = P(X > (y - beta) / alpha) = 1.0 - CDF_X((y - beta) / alpha)
  if (isAnalytical_)
  {
    const Scalar alpha = weights_(0, 0);
    if (alpha > 0.0) return distributionCollection_[0].computeComplementaryCDF((x - constant_[0]) / alpha);
    // If alpha < 0.0, compute the CDF
    return distributionCollection_[0].computeCDF((x - constant_[0]) / alpha);
  }
  // Check range
  const Interval range(getRange());
  const Scalar lowerBound = range.getLowerBound()[0];
  const Scalar upperBound = range.getUpperBound()[0];
  if (x <= lowerBound) return 1.0;
  if (x >= upperBound) return 0.0;
  // Here we call computeProbability with a [x, +inf[ interval
  // Here we call computeProbability with a ]-inf, x] interval
  const Scalar complementaryCDF = computeProbability(Interval(point, Point(1, upperBound), Interval::BoolCollection(1, true), getRange().getFiniteUpperBound()));
  if (complementaryCDF < 0.5) return complementaryCDF;
  // and if the cdf value is less than 1/2, it was better to use the complementary CDF
  else return 1.0 - computeProbability(Interval(Point(1, lowerBound), point, getRange().getFiniteLowerBound(), Interval::BoolCollection(1, true)));
}

/*  Compute the CDF of 1D distributions over a regular grid. The precision is reduced as this method is for drawing purpose only. */
Sample RandomMixture::computeCDF(const Scalar xMin,
                                 const Scalar xMax,
                                 const UnsignedInteger pointNumber,
                                 Sample & grid) const
{
  return DistributionImplementation::computeCDF(xMin, xMax, pointNumber, grid);
}

/* Get the probability content of an interval. It uses the Poisson inversion formula as described in the reference:
   "Abate, J. and Whitt, W. (1992). The Fourier-series method for inverting
   transforms of probability distributions. Queueing Systems 10, 5--88., 1992",
   formula 5.14.
   We use an incremental update of the trigonometric functions and reduce the complex arithmetic to a real
   arithmetic for performance purpose.
*/
Scalar RandomMixture::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  if (interval.isNumericallyEmpty()) return 0.0;
  // Use direct convolution for two continuous atoms of dimension 1
  if ((dimension_ == 1) && (distributionCollection_.getSize() == 2) && distributionCollection_[0].isContinuous() && distributionCollection_[1].isContinuous())
  {
    const Scalar lower = interval.getLowerBound()[0];
    const Scalar upper = interval.getUpperBound()[0];
    const Scalar cdfLower = computeCDF(lower);
    const Scalar cdfUpper = computeCDF(upper);
    return std::min(1.0, std::max(0.0, cdfUpper - cdfLower));
  }
  if ((dimension != 1) || (distributionCollection_.getSize() >= ResourceMap::GetAsUnsignedInteger( "RandomMixture-SmallSize" )))
  {
    const Scalar oldPDFPrecision = pdfPrecision_;
    pdfPrecision_ = std::pow(SpecFunc::ScalarEpsilon, 2.0 / (3.0 * dimension_));
    const UnsignedInteger n1 = ResourceMap::GetAsUnsignedInteger("RandomMixture-MarginalIntegrationNodesNumber");
    const UnsignedInteger N = ResourceMap::GetAsUnsignedInteger("RandomMixture-MaximumIntegrationNodesNumber");
    const UnsignedInteger n2 = static_cast<UnsignedInteger>(round(std::pow(N, 1.0 / dimension_)));
    const UnsignedInteger marginalSize = SpecFunc::NextPowerOfTwo(std::min(n1, n2));
    setIntegrationNodesNumber(marginalSize);
    const Scalar probability = DistributionImplementation::computeProbability(interval);
    pdfPrecision_ = oldPDFPrecision;
    return probability;
#ifdef TRAPEZE
    // For now, use a simple mid-point rule
    const Point xMin(interval.getLowerBound());
    const Point xMax(interval.getUpperBound());
    Sample gridX;
    const Indices discretization(dimension, marginalSize);
    const Sample samplePDF(computePDF(xMin, xMax, discretization, gridX));
    // Compute the elementary volume
    const UnsignedInteger otherCornerIndex = static_cast<UnsignedInteger>(round((std::pow(marginalSize, dimension) - 1) / (marginalSize - 1)));
    const Interval elementaryInterval(gridX[0], gridX[otherCornerIndex]);
    const Scalar elementaryVolume = elementaryInterval.getVolume();
    Scalar cdf = 0.0;
    for (UnsignedInteger i = 0; i < gridX.getSize(); ++i)
      if (interval.contains(gridX[i])) cdf += samplePDF(i, 0) * elementaryVolume;
    return cdf;
#endif
  }
  // Special case for combination containing only one contributor
  if (isAnalytical_ && (dimension_ == 1))
  {
    const Scalar lower = interval.getLowerBound()[0];
    const Scalar upper = interval.getUpperBound()[0];
    const Scalar alpha = weights_(0, 0);
    // Negative alpha, swap upper and lower bound flags
    if (alpha < 0.0) return distributionCollection_[0].computeProbability(Interval((upper - constant_[0]) / alpha, (lower - constant_[0]) / alpha));
    return distributionCollection_[0].computeProbability(Interval((lower - constant_[0]) / alpha, (upper - constant_[0]) / alpha));
  } // isAnalytical
  const Interval clippedInterval(getRange().intersect(interval));
  // Quick return if there is no mass in the clipped interval
  if (clippedInterval.isNumericallyEmpty()) return 0.0;
  const Bool finiteLowerBound = clippedInterval.getFiniteLowerBound()[0] == 1;
  const Bool finiteUpperBound = clippedInterval.getFiniteUpperBound()[0] == 1;
  // Quick return for integral over the whole real line
  if (!finiteLowerBound && !finiteUpperBound) return 1.0;
  const Scalar lowerBound = clippedInterval.getLowerBound()[0];
  const Scalar upperBound = clippedInterval.getUpperBound()[0];
  // Small size case: use Fourier series
  const Scalar precision = cdfPrecision_;
  Scalar error = 2.0 * precision;
  const Scalar a = referenceBandwidth_[0] * lowerBound;
  const Scalar b = referenceBandwidth_[0] * upperBound;
  const Scalar factor = referenceBandwidth_[0] / M_PI;
  Scalar value = computeEquivalentNormalCDFSum(lowerBound, upperBound);
  UnsignedInteger k = 1;
  const UnsignedInteger kmin = 1 << blockMin_;
  const UnsignedInteger kmax = 1 << blockMax_;
  while ( (k < kmax) && (error > std::max(precision, std::abs(precision * value)) || k < kmin) )
  {
    error = 0.0;
    for (UnsignedInteger m = k; m < 2 * k; ++m)
    {
      Scalar sinMHLower = std::sin(m * a);
      Scalar cosMHLower = std::cos(m * a);
      Scalar sinMHUpper = std::sin(m * b);
      Scalar cosMHUpper = std::cos(m * b);
      const Complex deltaValue(computeDeltaCharacteristicFunction(m));
      const Scalar contribution = factor * (deltaValue.real() * (sinMHUpper - sinMHLower) + deltaValue.imag() * (cosMHLower - cosMHUpper)) / (m * referenceBandwidth_[0]);
      value += contribution;
      error += std::abs(contribution);
    }
    k *= 2;
  }
  // For extrem values of the argument, the computed value can be slightly outside of [0,1]. Truncate it.
  return (value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value));
}

/*  Compute the quantile over a regular grid */
Sample RandomMixture::computeQuantile(const Scalar qMin,
                                      const Scalar qMax,
                                      const UnsignedInteger pointNumber,
                                      const Bool tail) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot compute the quantile over a regular 1D grid if the dimension is > 1";
  Sample result(pointNumber, 2);
  Scalar q = (tail ? 1.0 - qMax : qMin);
  const Scalar step = (qMax - qMin) / Scalar(pointNumber - 1.0);
  for (UnsignedInteger i = 0; i < pointNumber; ++i)
  {
    result(i, 0) = q;
    result(i, 1) = computeQuantile(q)[0];
    q += step;
  }
  return result;
}

/* Quantile computation for dimension=1 */
Scalar RandomMixture::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions"; 

  // Special case for random mixture with only 1 atom: Y = alpha * X + beta
  // find Yq such that P(Y < Yq) = q
  // i.e. for alpha > 0
  // P(X < (Yq - beta) / alpha) = q -> Yq = alpha * Xq + beta where P(X < Xq) = q
  // and for alpha < 0
  // P(X > (Yq - beta) / alpha) = q i.e. P(X < (Yq - beta) / alpha) = r with r = 1-q -> Yq = alpha * Xr + beta
  if (isAnalytical_)
  {
    const Scalar alpha = weights_(0, 0);
    // tail && alpha > 0: 1-p
    // tail && alpha <= 0: p
    // !tail && alpha > 0: p
    // !tail && alpha <= 0: 1-p
    // ->1-p iff tail != (alpha <= 0) (xor)
    const Scalar q = distributionCollection_[0].computeQuantile(prob, tail != (alpha <= 0.0))[0];
    return q * alpha + constant_[0];
  }
  if (isContinuous())
  {
    // General continuous case
    // Try a Newton method to benefit from the additive nature of Poisson's summation formula:
    // F(x_n+dx_n)~F(x_n)+p(x_n)dx_n
    // so F(x_n+dx_n)=q gives dx_n = (q - F(x_n)) / p(x_n)
    // and at the next step we have to compute F(x_n+dx_n), p(x_n+dx_n)
    // but F(x_n+dx_n)=F(x_n)+P(X\in[x_n,x_n+dx_n])
    const Scalar q = (tail ? 1.0 - prob : prob);
    Scalar x = equivalentNormal_.computeQuantile(q)[0];
    Scalar sigma = equivalentNormal_.getStandardDeviation()[0];
    Scalar epsilon = cdfEpsilon_ * sigma;
    Scalar dx = sigma;
    Scalar cdf = computeCDF(x);
    const Bool twoAtoms = distributionCollection_.getSize() == 2;
    for (UnsignedInteger i = 0; i < 16 && std::abs(dx) > epsilon; ++i)
    {
      const Scalar pdf = computePDF(x);
      dx = (q - cdf) / pdf;
      // Depending on the size of the mixture, use computeCDF (size == 2) or computeProbability (size > 2)
      if (twoAtoms) cdf = computeCDF(x);
      else
      {
        const Scalar dcdf = (dx > 0.0 ? computeProbability(Interval(x, x + dx)) : computeProbability(Interval(x + dx, x)));
        cdf += (dx > 0.0 ? dcdf : -dcdf);
      }
      x += dx;
    }
    // Has the Newton iteration converged?
    if (std::abs(dx) <= epsilon) return x;
  }
  // If no convergence of Newton's iteration of if non continuous and non analytical
  return DistributionImplementation::computeScalarQuantile(prob, tail);
}

/** Get the minimum volume level set containing a given probability of the distribution */
LevelSet RandomMixture::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob,
    Scalar & threshold) const
{
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  // As we are in 1D and as the function defining the composite distribution can have complex variations,
  // we use an improved sampling method to compute the quantile of the -logPDF(X) distribution
  const UnsignedInteger size = SpecFunc::NextPowerOfTwo(ResourceMap::GetAsUnsignedInteger("Distribution-MinimumVolumeLevelSetSamplingSize"));
  const Sample minusLogPDFSample(computeLogPDF(getSampleByQMC(size)) * Point(1, -1.0));
  const Scalar minusLogPDFThreshold = minusLogPDFSample.computeQuantile(prob)[0];
  threshold = std::exp(-minusLogPDFThreshold);

  return LevelSet(minimumVolumeLevelSetFunction, minusLogPDFThreshold);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex RandomMixture::computeCharacteristicFunction(const Scalar x) const
{
  if (x == 0.0) return 1.0;
  return std::exp(computeLogCharacteristicFunction(x));
}

Complex RandomMixture::computeCharacteristicFunction(const Point & x) const
{
  // The characteristic function is given by the following formula:
  // \phi(y) = \prod_{j=1}^{d} (exp(i * y_j * constant_j) * \prod_{k=1}^{n} \phi_{X_k}((M^t y)_k))
  // compute M^t * y
  return std::exp(computeLogCharacteristicFunction(x));
}

Complex RandomMixture::computeLogCharacteristicFunction(const Scalar x) const
{
  if (x == 0.0) return 0.0;
  Complex logCfValue(0.0, constant_[0] * x);
  const UnsignedInteger size = distributionCollection_.getSize();
  const Scalar smallScalar = 0.5 * std::log(SpecFunc::MinScalar);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    logCfValue += distributionCollection_[i].computeLogCharacteristicFunction(weights_(0, i) * x);
    // Early exit for null value
    if (logCfValue.real() < smallScalar) break;
  } /* end for */
  return logCfValue;
}

Complex RandomMixture::computeLogCharacteristicFunction(const Point & x) const
{
  // The log-characteristic function is given by:
  // log(\phi(x)) = \sum_{j=1}^{d} ((i * y_j * constant_j) + \sum_{k=1}^{n} log(\phi_{X_k})((M^t x)_k))
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  const Scalar smallScalar = 0.5 * std::log(SpecFunc::MinScalar);
  // 1) compute the deterministic term
  Complex logCfValue;
  for (UnsignedInteger i = 0; i < dimension; ++i) logCfValue += Complex(0.0, x[i] * constant_[i]);
  // 2) compute the random part
  // The variables are independent
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // compute M^t * x
    Scalar wtx = 0.;
    for (UnsignedInteger j = 0; j < dimension; ++j) wtx += weights_(j, i) * x[j];
    logCfValue += distributionCollection_[i].computeLogCharacteristicFunction(wtx);
    if (logCfValue.real() < smallScalar) break;
  }
  return logCfValue;
}

/* Compute a value of the characteristic function on a prescribed discretization. As the value associated with index == 0 is known, it is not stored so for index > 0, the corresponding value is at position index-1 */
Complex RandomMixture::computeDeltaCharacteristicFunction(const UnsignedInteger index) const
{
  if (index == 0) return 0.0;
  // The cached values are computed and stored in an ascending order without hole: this function is always called on a sequence starting from 0 to n-1
  // Usual case first: the index is within the already computed values
  if (index <= storedSize_) return characteristicValuesCache_[index - 1];
  // If the index is higher than the maximum allowed storage
  if (index > maxSize_)
  {
    LOGINFO(OSS() << "Cache exceeded in RandomMixture::computeDeltaCharacteristicFunction, consider increasing maxSize_ to " << index);
    const Scalar x = index * referenceBandwidth_[0];
    const Complex logCF(computeLogCharacteristicFunction(x));
    const Complex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
    const Complex deltaLog(logCF - logNormalCF);
    Complex value;
    if (std::abs(deltaLog) < 1.0e-5) value = std::exp(logNormalCF) * (deltaLog * (1.0 + deltaLog * (0.5 + deltaLog / 6.0)));
    else value = std::exp(logCF) - std::exp(logNormalCF);
    LOGDEBUG(OSS() << "ih=" << x << ", logCF=" << logCF << ", CF=" << std::exp(logCF) << ", logNormalCF=" << logNormalCF << ", NormalCF=" << std::exp(logNormalCF) << ", value=" << value);
    return value;
  }
  // Here, the index has not been computed so far, fill-in the gap
  if (index > storedSize_)
  {
    for (UnsignedInteger i = storedSize_ + 1; i <= index; ++i)
    {
      const Scalar x = i * referenceBandwidth_[0];
      const Complex logCF(computeLogCharacteristicFunction(x));
      const Complex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
      const Complex deltaLog(logCF - logNormalCF);
      Complex value;
      if (std::abs(deltaLog) < 1.0e-5) value = std::exp(logNormalCF) * (deltaLog * (1.0 + deltaLog * (0.5 + deltaLog / 6.0)));
      else value = std::exp(logCF) - std::exp(logNormalCF);
      LOGDEBUG(OSS() << "ih=" << x << ", logCF=" << logCF << ", CF=" << std::exp(logCF) << ", logNormalCF=" << logNormalCF << ", NormalCF=" << std::exp(logNormalCF) << ", value=" << value);
      characteristicValuesCache_.add(value);
    }
    storedSize_ = index;
    return characteristicValuesCache_[storedSize_ - 1];
  }
  // Should never go there
  throw InvalidArgumentException(HERE) << "Error: trying to access to a cached characteristic value in an incorrect pattern.";
}

/* Compute the characteristic function of nD distributions by difference to a reference Normal distribution with the same mean and the same covariance */
Complex RandomMixture::computeDeltaCharacteristicFunction(const Point & x) const
{
  // Direct application on a point ==> useful for computation on grid
  const Complex logCF(computeLogCharacteristicFunction(x));
  const Complex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
  const Complex deltaLog(logCF - logNormalCF);
  if (std::abs(deltaLog) < 1.0e-5) return std::exp(logNormalCF) * (deltaLog * (1.0 + deltaLog * (0.5 + deltaLog / 6.0)));
  else return std::exp(logCF) - std::exp(logNormalCF);
}

/* Update cache */
void RandomMixture::updateCacheDeltaCharacteristicFunction(const Sample & points) const
{
  for(UnsignedInteger i = 0; i < points.getSize(); ++i)
  {
    Point x(points[i]);
    // Computation of CF - NormalCF
    // Here we check if it is possible to reduce calculation
    // We reduce CF - NormalCF to NormalCF * (CF/NormalCF -1), which rewrites
    // as exp(logNormalCF) * (exp(deltaLog) - 1), with deltaLog=logCF - logNormalCF
    // We use a 3rd order Taylor expansion of exp(deltaLog) - 1 if |deltaLog| <= 1e-5
    const Complex logCF(computeLogCharacteristicFunction(x));
    const Complex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
    const Complex deltaLog(logCF - logNormalCF);
    Complex value;
    if (std::abs(deltaLog) < 1.0e-5) value = std::exp(logNormalCF) * (deltaLog * (1.0 + deltaLog * (0.5 + deltaLog / 6.0)));
    else value = std::exp(logCF) - std::exp(logNormalCF);
    LOGDEBUG(OSS() << "ih=" << x << ", logCF=" << logCF << ", CF=" << std::exp(logCF) << ", logNormalCF=" << logNormalCF << ", NormalCF=" << std::exp(logNormalCF) << ", value=" << value);
    characteristicValuesCache_.add(value);
    ++storedSize_;
  }
}

/* Get the PDF gradient of the distribution */
Point RandomMixture::computePDFGradient(const Point & point) const
{
  if (isAnalytical_ && (dimension_ == 1))
  {
    const Scalar alpha = weights_(0, 0);
    Scalar factor = (isDiscrete() ? 1.0 : 1.0 / std::abs(alpha));
    return distributionCollection_[0].computePDFGradient((point - constant_) / alpha) * factor;
  } // isAnalytical_
  return DistributionImplementation::computePDFGradient(point);
}

/* Get the CDF gradient of the distribution */
Point RandomMixture::computeCDFGradient(const Point & point) const
{
  if (isAnalytical_ && (dimension_ == 1))
  {
    const Scalar alpha = weights_(0, 0);
    if (alpha > 0.0) return distributionCollection_[0].computeCDFGradient((point - constant_) / alpha);
    // If alpha < 0.0, compute the complementary CDF
    return distributionCollection_[0].computeCDFGradient((point - constant_) / alpha) * (-1.0);
  } // isAnalytical_
  return DistributionImplementation::computePDFGradient(point);
}

/* Compute the mean of the RandomMixture */
void RandomMixture::computeMean() const
{
  mean_ = constant_;
  const UnsignedInteger size = distributionCollection_.getSize();
  Point mu(size);
  for(UnsignedInteger i = 0; i < size; ++i)
    mu[i] = distributionCollection_[i].getMean()[0];
  mean_ += weights_ * mu;
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the RandomMixture */
void RandomMixture::computeCovariance() const
{
  // Compute the covariance of the mixture.
  // This method is private. Use the getCovariance to get the covariance value.
  // The covariance is given by
  //   Cov(Y) = weight * Cov(X) * weight^t
  // As Cov(X) is diagonal:
  //   Cov(Y)_{i,j} = \sum_{k=1}^n weights_{i,k} weights_{j,k} Cov(X_k, X_k)
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      Scalar covariance = 0.0;
      for (UnsignedInteger k = 0; k < size; ++k)
      {
        covariance += weights_(i, k) * weights_(j, k) * distributionCollection_[k].getCovariance().operator()(0, 0);
      }
      covariance_(i, j) = covariance;
    }
  }
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard deviation of the RandomMixture */
Point RandomMixture::getStandardDeviation() const
{
  // It looks like the default implementation but
  // it is not in the multivariate case. Even if
  // it looks wasteful to compute the whole covariance
  // matrix only for its diagonal, it is not so wasteful
  // as random mixtures are limited to dimension <= 3,
  // and it is much more efficient than using getCenteredMoment()
  // because the covariance is a simple algebra based on
  // coefficients and atoms covariance.
  const UnsignedInteger dimension = getDimension();
  Point sigma(dimension, 0.0);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    sigma[i] = std::sqrt(getCovariance().operator()(i, i));
  return sigma;
}

/* Get the skewness of the RandomMixture */
Point RandomMixture::getSkewness() const
{
  Point skewness(getDimension(), 0.0);
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger j = 0; j < getDimension(); ++j)
  {
    Scalar variance = 0.0;
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar wi = weights_(j, i);
      const Scalar wi2 = wi * wi;
      const Scalar vi = distributionCollection_[i].getCovariance().operator()(0, 0);
      variance += wi2 * vi;
      skewness[j] += wi2 * wi * distributionCollection_[i].getSkewness()[0] * std::pow(vi, 1.5);
    } /* end for */
    skewness[j] *= std::pow(variance, -1.5);
  }
  return skewness;
}

/* Get the kurtosis of the RandomMixture */
Point RandomMixture::getKurtosis() const
{
  Point kurtosis(getDimension(), 0.0);
  const UnsignedInteger size = distributionCollection_.getSize();
  Point v(size);
  Point w2(size);
  for (UnsignedInteger d = 0; d < getDimension(); ++d)
  {
    Scalar variance = 0.0;
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar wi = weights_(d, i);
      const Scalar wi2 = wi * wi;
      w2[i] = wi2;
      const Scalar vi = distributionCollection_[i].getCovariance().operator()(0, 0);
      v[i] = vi;
      variance += wi2 * vi;
      kurtosis[d] += wi2 * wi2 * distributionCollection_[i].getKurtosis()[0] * vi * vi;
      for (UnsignedInteger j = 0; j < i; ++j) kurtosis[d] += 6.0 * wi2 * w2[j] * vi * v[j];
    } /* end for */
    kurtosis[d] /= variance * variance;
  }
  return kurtosis;
}

/* Parameters value and description accessor */
RandomMixture::PointWithDescriptionCollection RandomMixture::getParametersCollection() const
{
  // Assume that the weights are not part of the parameters
  const UnsignedInteger size = distributionCollection_.getSize();
  PointWithDescriptionCollection parameters(1);
  Description parametersDescription;
  // Form a big Point from the parameters of each atom
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const String prefix(distributionCollection_[i].getName());
    const PointWithDescription atomParameters(distributionCollection_[i].getParametersCollection()[0]);
    const Description atomDescription(atomParameters.getDescription());
    const UnsignedInteger atomParameterDimension = atomParameters.getDimension();
    // Add the current atom parameters
    for (UnsignedInteger j = 0; j < atomParameterDimension; ++j)
    {
      parameters[0].add(atomParameters[j]);
      parametersDescription.add(OSS() << prefix << "_" << atomDescription[j]);
    }
  }
  parameters[0].setDescription(parametersDescription);
  parameters[0].setName(getName());
  return parameters;
} // getParametersCollection

/* Parameters value and description accessor */
Point RandomMixture::getParameter() const
{
  // Assume that the weights are not part of the parameters
  const UnsignedInteger size = distributionCollection_.getSize();
  Point parameter;
  // Form a big Point from the parameters of each atom
  for (UnsignedInteger i = 0; i < size; ++i)
    parameter.add(distributionCollection_[i].getParameter());
  return parameter;
} // getParameter

void RandomMixture::setParameter(const Point & parameter)
{
  if (parameter.getSize() != getParameter().getSize()) throw InvalidArgumentException(HERE) << "Error: expected " << getParameter().getSize() << " values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = RandomMixture(distributionCollection_, weights_, constant_);
  setWeight(w);
} // setParameter

/* Parameters value and description accessor */
Description RandomMixture::getParameterDescription() const
{
  // Assume that the weights are not part of the parameters
  const UnsignedInteger size = distributionCollection_.getSize();
  Description parameterDescription;
  // Form a big Point from the parameters of each atom
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const String prefix(distributionCollection_[i].getName());
    const Description atomParameterDescription(distributionCollection_[i].getParameterDescription());
    const UnsignedInteger atomParameterSize = atomParameterDescription.getSize();
    // Add the current atom parameters
    for (UnsignedInteger j = 0; j < atomParameterSize; ++j)
      parameterDescription.add(OSS() << prefix << "_" << atomParameterDescription[j]);
  }
  return parameterDescription;
} // getParameterDescription

/* Get a positon indicator for a 1D distribution */
Scalar RandomMixture::getPositionIndicator() const
{
  if (!isAlreadyComputedPositionIndicator_) computePositionIndicator();
  return positionIndicator_;
}

/* Compute a positon indicator for a 1D distribution */
void RandomMixture::computePositionIndicator() const
{
  if (getDimension() == 1)
  {
    positionIndicator_ = constant_[0];
    const UnsignedInteger size = distributionCollection_.getSize();
    // Assume an additive behaviour of the position indicator. It is true for the mean value, and almost true for the median of moderatly skewed distributions
    for(UnsignedInteger i = 0; i < size; ++i) positionIndicator_ += weights_(0, i) * distributionCollection_[i].getPositionIndicator();
    isAlreadyComputedPositionIndicator_ = true;
  }
}


/* Get a dispersion indicator for a 1D distribution */
Scalar RandomMixture::getDispersionIndicator() const
{
  if (!isAlreadyComputedDispersionIndicator_) computeDispersionIndicator();
  return dispersionIndicator_;
}

/* Compute a dispersion indicator for a 1D distribution */
void RandomMixture::computeDispersionIndicator() const
{
  if (getDimension() == 1)
  {
    dispersionIndicator_ = 0.0;
    const UnsignedInteger size = distributionCollection_.getSize();
    // Assume a quadratic additive behaviour of the dispersion indicator. It is true for the standard deviation value, and almost true for the interquartile of moderatly skewed distributions
    for(UnsignedInteger i = 0; i < size; ++i) dispersionIndicator_ += std::pow(weights_(0, i) * distributionCollection_[i].getDispersionIndicator(), 2.0);
    dispersionIndicator_ = std::sqrt(dispersionIndicator_);
    isAlreadyComputedDispersionIndicator_ = true;
  }
}

/* BlockMin accessor */
void RandomMixture::setBlockMin(const UnsignedInteger blockMin)
{
  blockMin_ = blockMin;
}

UnsignedInteger RandomMixture::getBlockMin() const
{
  return blockMin_;
}

/* BlockMax accessor */
void RandomMixture::setBlockMax(const UnsignedInteger blockMax)
{
  blockMax_ = blockMax;
}

UnsignedInteger RandomMixture::getBlockMax() const
{
  return blockMax_;
}

/* MaxSize accessor */
void RandomMixture::setMaxSize(const UnsignedInteger maxSize)
{
  maxSize_ = maxSize;
  // The cache must grow progresively, so;
  // + if maxSize >= storedSize, we keep the current cache as it is
  // + if maxSize < storedSize, we reduce the cache and update the storedSize
  if (maxSize_ < storedSize_)
  {
    characteristicValuesCache_.resize(maxSize);
    storedSize_ = maxSize;
  }
}

UnsignedInteger RandomMixture::getMaxSize() const
{
  return maxSize_;
}

/* Alpha accessor */
void RandomMixture::setAlpha(const Scalar alpha)
{
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Error: the alpha parameter must be strictly positive";
  alpha_ = alpha;
  computeRange();
  computeReferenceBandwidth();
}

Scalar RandomMixture::getAlpha() const
{
  return alpha_;
}

void RandomMixture::setBeta(const Scalar beta)
{
  beta_ = beta;
  computeRange();
  computeReferenceBandwidth();
}

Scalar RandomMixture::getBeta() const
{
  return beta_;
}

/* Reference bandwidth accessor */
void RandomMixture::setReferenceBandwidth(const Point & bandwidth)
{
  referenceBandwidth_ = bandwidth;
  // Reset the cached values
  storedSize_ = 0;
  characteristicValuesCache_ = ComplexPersistentCollection(0);
}

Point RandomMixture::getReferenceBandwidth() const
{
  return referenceBandwidth_;
}

/* PDF precision accessor. For other distributions, it is a read-only attribute. */
void RandomMixture::setPDFPrecision(const Scalar pdfPrecision)
{
  pdfPrecision_ = pdfPrecision;
}

/* CDF precision accessor. For other distributions, it is a read-only attribute. */
void RandomMixture::setCDFPrecision(const Scalar cdfPrecision)
{
  cdfPrecision_ = cdfPrecision;
}

/* Compute the reference bandwidth. It is defined as the largest bandwidth
   that allow a precise computation of the PDF over the range
   [positionIndicator_ +/- beta * dispersionIndicator_] */
void RandomMixture::computeReferenceBandwidth()
{
  referenceBandwidth_ = Point(getDimension(), 0.0);
  Bool isFinite = true;
  for (UnsignedInteger k = 0; k < getDimension(); ++k)
  {
    referenceBandwidth_[k] = 2.0 * M_PI / (getRange().getUpperBound()[k] - getRange().getLowerBound()[k]);
    isFinite &= (getRange().getFiniteLowerBound()[k] && getRange().getFiniteUpperBound()[k]);
  }
  // Shrink a little bit the bandwidth if the range is finite
  if (isFinite) referenceBandwidth_ *= 0.5;

  // Compute the reference bandwidth factor
  referenceBandwidthFactor_ = 1.0;
  for (UnsignedInteger k = 0; k < getDimension(); ++k) referenceBandwidthFactor_ *= (referenceBandwidth_[k] / (2.0 * M_PI));

  // Compute grid helper object
  gridMesher_ = SphereUniformNorm::GetFromGridSteps(referenceBandwidth_, true);

  // Reset the cached values
  storedSize_ = 0;
  characteristicValuesCache_ = ComplexPersistentCollection(0);
}

/* Compute the equivalent normal distribution, i.e. with the same mean and
   the same standard deviation */
void RandomMixture::computeEquivalentNormal()
{
  if (distributionCollection_.getSize() > 0) equivalentNormal_ = Normal(getMean(), getCovariance());
  else equivalentNormal_ = Normal();
}

/* Compute the left-hand sum in Poisson's summation formula for the equivalent normal */
Scalar RandomMixture::computeEquivalentNormalPDFSum(const Scalar x) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "RandomMixture::computeEquivalentNormalPDFSum(Scalar) is only possible for dimension 1";

  Scalar value = equivalentNormal_.computePDF(x);
  UnsignedInteger i = 0;
  Scalar delta = 0.0;
  do
  {
    ++i;
    const Scalar step = 2.0 * M_PI * i / referenceBandwidth_[0];
    delta = equivalentNormal_.computePDF(x + step) + equivalentNormal_.computePDF(x - step);
    value += delta;
  }
  while (delta > 0.0 * value);
  return value;
}

Scalar RandomMixture::computeEquivalentNormalPDFSum(const Point & y, const Point & gridStep,
    UnsignedInteger imax, UnsignedInteger & levelMax) const
{
  /*
    Compute the left-hand sum in Poisson's summation formula for the equivalent normal.
    The goal is to compute:
    \sum_{i \in \mathbb{Z}^d} q(y + i * h) with :
    y = (y_1,...,y_d) point on which the pdf is requested
    q = the density function of the distribution computed by computeEquivalentNormal
    h = (h_1,...,h_d) the reference bandwidth
    i*h = (i_1 * h_1,...,i_d * h_d)
    The sum above is rewritten as:
    \sum_{s \in \mathbb{N}} \sum_{x such as \norm{x-y}_\infinity=s} q(x)
    We start with s=0 and at each iteration, we add the points which are exactly at
    distance s with norm L^\infinity.
    If s>0, there are (2s+1)^d - (2s-1)^d points to add at iteration s.
    The evaluation of the gaussian density at these points are added into the current sum.
    The summation halts when the added value at iteration s is negligible relative to
    the current density value.
  */
  if (gridStep.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: invalid grid dimension";
  if (dimension_ == 1)
  {
    const Scalar x = y[0];
    Scalar value = equivalentNormal_.computePDF(x);
    UnsignedInteger i = 0;
    Scalar delta = 0.0;
    do
    {
      ++i;
      const Scalar step = i * gridStep[0];
      delta = equivalentNormal_.computePDF(x + step) + equivalentNormal_.computePDF(x - step);
      value += delta;
    }
    while (delta > 0.0 * value);
    return value;
  }

  // We cannot use gridMesher_; we need another instance, which does not use caching.
  // We force symmetry to improve performance.
  const SphereUniformNorm grid(SphereUniformNorm::GetFromGridSteps(gridStep, true));

  Scalar gaussian_pdf = equivalentNormal_.computePDF(y);
  Scalar delta = std::max(1.0, gaussian_pdf);
  const Scalar epsilon = pdfPrecision_;

  // If imax is zero, we want to store in levelMax the first level which does not improve accuracy.
  // If non zero, this means that a previous call had already computed levelMax, and levelMax
  // must not change.
  levelMax = imax;
  Point skin1(dimension_);
  Point skin2(dimension_);
  for (UnsignedInteger i = 1; (imax == 0 || i < imax) && (delta > gaussian_pdf * epsilon); ++i)
  {
    const Sample skinPoints(grid.getPoints(i));

    if (!imax) levelMax = i;
    const Scalar numberOfPoints = skinPoints.getSize();
    delta = 0.0;
    for (UnsignedInteger j = 0; j < numberOfPoints; ++j)
    {
      for (UnsignedInteger d = 0; d < dimension_; ++d)
      {
        skin1[d] = y[d] + skinPoints(j, d);
        skin2[d] = y[d] - skinPoints(j, d);
      }
      delta += equivalentNormal_.computePDF(skin1) + equivalentNormal_.computePDF(skin2);
    }
    gaussian_pdf += delta;
  }
  return gaussian_pdf;
}

/* Compute the left-hand sum in Poisson's summation formula for the equivalent normal */
Scalar RandomMixture::computeEquivalentNormalCDFSum(const Scalar s,
    const Scalar t) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "RandomMixture::computeEquivalentNormalCDFSum(Scalar) is only possible for dimension 1";

  Scalar value = equivalentNormal_.computeProbability(Interval(s, t));
  UnsignedInteger i = 0;
  Scalar delta = 0.0;
  do
  {
    ++i;
    const Scalar step = 2.0 * M_PI * i / referenceBandwidth_[0];
    delta = (equivalentNormal_.computeCDF(t + step) - equivalentNormal_.computeCDF(s + step)) + (equivalentNormal_.computeCDF(t - step) - equivalentNormal_.computeCDF(s - step));
    value += delta;
  }
  while (delta > 0.0 * value);
  return value;
}

struct RandomMixturePair
{
  Scalar norm_;
  Distribution distribution_;
  RandomMixturePair(): norm_(0.0), distribution_() {}
  RandomMixturePair(const Scalar norm, const Distribution & distribution): norm_(norm), distribution_(distribution) {}

  Bool operator < (const RandomMixturePair & other) const
  {
    return norm_ < other.norm_;
  }
};

typedef Collection<RandomMixturePair> RandomMixturePairCollection;

/** Project a RandomMixture over a Collection of DistributionFactory by using a regular sampling and Kolmogorov distance. */
DistributionCollection RandomMixture::project(const DistributionFactoryCollection & factoryCollection,
    Point & kolmogorovNorm,
    const UnsignedInteger size) const
{
  if (getDimension() != 1) throw NotDefinedException(HERE) << "Error: cannot project random mixtures of dimension>1.";
  const UnsignedInteger factorySize = factoryCollection.getSize();
  RandomMixturePairCollection result(0);
  const Scalar mean = getMean()[0];
  const Scalar sigma = getStandardDeviation()[0];
  // Sample the quantile function uniformly over [mean +/- alpha * sigma]
  const Scalar qMin = computeCDF(mean - alpha_ * sigma);
  const Scalar qMax = computeCDF(mean + alpha_ * sigma);
  const Sample dataX(computeQuantile(qMin, qMax, size).getMarginal(1));
  // Loop over the factories
  for (UnsignedInteger i = 0; i < factorySize; ++i)
  {
    DistributionFactory factory(factoryCollection[i]);
    Distribution candidate;
    try
    {
      candidate = factory.build(dataX);
      LOGINFO(OSS() << "candidate " << i << " for the projection=" << candidate);
    }
    catch(...)
    {
      LOGWARN(OSS() << "Estimation failed for the factory " << factory.getImplementation()->getClassName() << ". It is removed from the set of factories.");
    }
    Scalar kolmogorov = 0.0;
    for (UnsignedInteger j = 0; j < size; ++j)
      kolmogorov = std::max(kolmogorov, std::abs(candidate.computeCDF(dataX(j, 0)) - (qMin + j * (qMax - qMin) / (size - 1.0))));
    result.add(RandomMixturePair(kolmogorov, candidate));
  }
  // Sort the results
  const UnsignedInteger resultSize = result.getSize();
  std::stable_sort(result.begin(), result.end());
  // Extract the results
  DistributionCollection distributionCollection(resultSize);
  kolmogorovNorm = Point(resultSize);
  for (UnsignedInteger i = 0; i < resultSize; ++i)
  {
    distributionCollection[i] = result[i].distribution_;
    kolmogorovNorm[i] = result[i].norm_;
  }
  return distributionCollection;
}

/* Get the i-th marginal distribution */
Distribution RandomMixture::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  RandomMixture::Implementation marginal(new RandomMixture(distributionCollection_, weights_.getRow(i), Point(1, constant_[i])));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution RandomMixture::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  const UnsignedInteger outputDimension = indices.getSize();
  const UnsignedInteger size = distributionCollection_.getSize();
  Matrix marginalWeights(outputDimension, size);
  Point marginalConstant(outputDimension);
  Description description(getDescription());
  Description marginalDescription(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    marginalConstant[i] = constant_[index_i];
    const Matrix row(weights_.getRow(index_i));
    for (UnsignedInteger j = 0; j < outputDimension; ++j) marginalWeights(i, j) = row(0, j);
    marginalDescription[i] = description[index_i];
  }
  RandomMixture::Implementation marginal(new RandomMixture(distributionCollection_, marginalWeights, marginalConstant));
  marginal->setDescription(marginalDescription);
  return marginal;
} // getMarginal(Indices)

/* Tell if the distribution has independent copula */
Bool RandomMixture::hasIndependentCopula() const
{
  return (getDimension() == 1);
}

/* Tell if the distribution has elliptical copula */
Bool RandomMixture::hasEllipticalCopula() const
{
  return (getDimension() == 1);
}

/* Check if the distribution is elliptical */
Bool RandomMixture::isElliptical() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  // Case of a Dirac distribution
  if (size == 0) return true;
  for (UnsignedInteger i = 0; i < size; ++i)
    if (!distributionCollection_[i].isElliptical()) return false;
  return true;
}

/* Check if the distribution is continuous */
Bool RandomMixture::isContinuous() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (distributionCollection_[i].isContinuous()) return true;
  return false;
}

/* Check if the distribution is discrete */
Bool RandomMixture::isDiscrete() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Tell if the distribution is integer valued */
Bool RandomMixture::isIntegral() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Check if the contributor is discrete
    if (!distributionCollection_[i].isDiscrete()) return false;
    // Check if all the weights are integer
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if (weights_(j, i) != round(weights_(j, i))) return false;
  }
  return true;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample RandomMixture::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  if (!isDiscrete()) throw NotDefinedException(HERE) << "Error: the support is defined only for discrete distributions.";
  const UnsignedInteger size = distributionCollection_.getSize();
  const UnsignedInteger dimension = getDimension();
  // The computation of the support is available only if there is one atom
  // otherwise the computePDF() and computeCDF() methods are not implemented anyway
  Sample support(0, dimension);
  Sample supportCandidates;
  if (dimension == 1)
    supportCandidates = distributionCollection_[0].getSupport() * Point(*weights_.getColumn(0).getImplementation()) + constant_;
  else
  {
    const Sample support0 = distributionCollection_[0].getSupport();
    const Point scaling(*weights_.getColumn(0).getImplementation());
    supportCandidates = Sample(support0.getSize(), dimension);
    for (UnsignedInteger i = 0; i < support0.getSize(); ++i)
      supportCandidates[i] = scaling * support0(i, 0) + constant_;
  } // dimension > 1
  for (UnsignedInteger indexNext = 1; indexNext < size; ++indexNext)
  {
    Sample nextSupport;
    if (dimension == 1)
      nextSupport = distributionCollection_[indexNext].getSupport() * Point(*weights_.getColumn(indexNext).getImplementation());
    else
    {
      const Sample supportNext = distributionCollection_[indexNext].getSupport();
      const Point scaling(*weights_.getColumn(indexNext).getImplementation());
      nextSupport = Sample(supportNext.getSize(), dimension);
      for (UnsignedInteger i = 0; i < supportNext.getSize(); ++i)
        nextSupport[i] = scaling * supportNext(i, 0) + constant_;
    } // dimension > 1
    const UnsignedInteger supportCandidatesSize = supportCandidates.getSize();
    const UnsignedInteger nextSupportSize = nextSupport.getSize();
    Sample newSupportCandidate(supportCandidatesSize * nextSupportSize, dimension);
    UnsignedInteger k = 0;
    for (UnsignedInteger indexCandidates = 0; indexCandidates < supportCandidatesSize; ++indexCandidates)
    {
      const Point xI(supportCandidates[indexCandidates]);
      for (UnsignedInteger indexNext2 = 0; indexNext2 < nextSupportSize; ++indexNext2)
      {
        const Point xJ(nextSupport[indexNext2]);
        newSupportCandidate[k] = xI + xJ;
        ++k;
      } // indexNext2
    } // indexCandidates
    // Remove duplicates
    supportCandidates = newSupportCandidate.sortUnique();
  } // loop over the other atoms
  for (UnsignedInteger i = 0; i < supportCandidates.getSize(); ++i)
  {
    const Point candidate(supportCandidates[i]);
    if (interval.contains(candidate)) support.add(candidate);
  }
  return support;
}

/* Method save() stores the object through the StorageManager */
void RandomMixture::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distributionCollection_", distributionCollection_  );
  adv.saveAttribute( "constant_", constant_ );
  adv.saveAttribute( "weights_", weights_ );
  adv.saveAttribute( "positionIndicator_", positionIndicator_ );
  adv.saveAttribute( "isAlreadyComputedPositionIndicator_", isAlreadyComputedPositionIndicator_ );
  adv.saveAttribute( "dispersionIndicator_", dispersionIndicator_ );
  adv.saveAttribute( "isAlreadyComputedDispersionIndicator_", isAlreadyComputedDispersionIndicator_ );
  adv.saveAttribute( "blockMin_", blockMin_ );
  adv.saveAttribute( "blockMax_", blockMax_ );
  adv.saveAttribute( "referenceBandwidth_", referenceBandwidth_ );
  adv.saveAttribute( "referenceBandwidthFactor_", referenceBandwidthFactor_ );
  adv.saveAttribute( "maxSize_", maxSize_ );
  adv.saveAttribute( "storedSize_", storedSize_ );
  adv.saveAttribute( "characteristicValuesCache_", characteristicValuesCache_ );
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "pdfPrecision_", pdfPrecision_ );
  adv.saveAttribute( "cdfPrecision_", cdfPrecision_ );
  adv.saveAttribute( "inverseWeights_", inverseWeights_ );
  adv.saveAttribute( "detWeightsInverse_", detWeightsInverse_ );
  adv.saveAttribute( "fftAlgorithm_", fftAlgorithm_ );
  adv.saveAttribute( "isAnalytical_", isAnalytical_ );
} // save

/* Method load() reloads the object from the StorageManager */
void RandomMixture::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distributionCollection_", distributionCollection_  );
  adv.loadAttribute( "constant_", constant_ );
  adv.loadAttribute( "weights_", weights_ );
  adv.loadAttribute( "positionIndicator_", positionIndicator_ );
  adv.loadAttribute( "isAlreadyComputedPositionIndicator_", isAlreadyComputedPositionIndicator_ );
  adv.loadAttribute( "dispersionIndicator_", dispersionIndicator_ );
  adv.loadAttribute( "isAlreadyComputedDispersionIndicator_", isAlreadyComputedDispersionIndicator_ );
  adv.loadAttribute( "blockMin_", blockMin_ );
  adv.loadAttribute( "blockMax_", blockMax_ );
  adv.loadAttribute( "referenceBandwidth_", referenceBandwidth_ );
  adv.loadAttribute( "referenceBandwidthFactor_", referenceBandwidthFactor_ );
  adv.loadAttribute( "maxSize_", maxSize_ );
  adv.loadAttribute( "storedSize_", storedSize_ );
  adv.loadAttribute( "characteristicValuesCache_", characteristicValuesCache_ );
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "pdfPrecision_", pdfPrecision_ );
  adv.loadAttribute( "cdfPrecision_", cdfPrecision_ );
  adv.loadAttribute( "inverseWeights_", inverseWeights_ );
  adv.loadAttribute( "detWeightsInverse_", detWeightsInverse_ );
  adv.loadAttribute( "fftAlgorithm_", fftAlgorithm_ );
  adv.loadAttribute( "isAnalytical_", isAnalytical_ );
  computePositionIndicator();
  computeDispersionIndicator();
  computeRange();
  computeReferenceBandwidth();
  computeEquivalentNormal();
} // load

END_NAMESPACE_OPENTURNS
