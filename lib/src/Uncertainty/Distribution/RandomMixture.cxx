//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all RandomMixtures
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <algorithm>

#include "RandomMixture.hxx"
#include "SpecFunc.hxx"
#include "PersistentObjectFactory.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "Interval.hxx"
#include "Tuples.hxx"
#include "NumericalMathFunction.hxx"
#include "Log.hxx"
#include "Triangular.hxx"
#include "Trapezoidal.hxx"
#include "Uniform.hxx"
#include "ComplexTensor.hxx"
#include "FFT.hxx"
#include "GaussKronrod.hxx"
#include "TBB.hxx"
#include "OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<Distribution>);
static const Factory<PersistentCollection<Distribution> > RegisteredFactory3;


typedef Collection<Distribution> DistributionCollection;
typedef Collection<NumericalPoint> NumericalPointCollection;
typedef Collection<NumericalComplex> NumericalComplexCollection;


CLASSNAMEINIT(RandomMixture);

static const Factory<RandomMixture> RegisteredFactory;

/* Default constructor */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const NumericalScalar constant)
  : DistributionImplementation()
  , distributionCollection_()
  , constant_(NumericalPoint(1, constant))
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
  , alpha_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  setDimension(1);
  weights_ = Matrix(1, coll.getSize());
  for (UnsignedInteger k = 0; k < coll.getSize(); ++k) weights_(0, k) = 1.0;
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollection( coll );
}

/* Default constructor */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const NumericalPoint & weights,
                             const NumericalScalar constant)
  : DistributionImplementation()
  , distributionCollection_()
  , constant_(NumericalPoint(1, constant))
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
  , alpha_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  setDimension(1);
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  // the setDistributionCollection() method that do it for us.
  if (weights.getDimension() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weights collection must have the same size as the distribution collection";
  const UnsignedInteger size(coll.getSize());
  weights_ = Matrix(1, weights.getSize());
  for (UnsignedInteger i = 0; i < size; ++i) weights_(0, i) = weights[i];
  // This call set also the range.
  setDistributionCollection(coll);
}

/* Parameter constructor - nD */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const Matrix & weights,
                             const NumericalPoint constant)
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
  , alpha_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon" ))
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
  weights_ = weights;
  setDistributionCollection(coll);
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
  , alpha_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  const UnsignedInteger dimension(weights.getNbRows());
  if (dimension > 3) throw InvalidDimensionException(HERE) << "RandomMixture only possible for dimension 1,2 or 3";
  constant_ = NumericalPoint(dimension, 0.0);
  setDimension(dimension);
  if (weights.getNbColumns() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight matrix must have the same column numbers as the distribution collection's size";
  weights_ = weights;
  setDistributionCollection(coll);
}

/* Parameter constructor - nD */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const NumericalSample & weights,
                             const NumericalPoint constant)
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
  , alpha_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  const UnsignedInteger dimension(constant.getSize());
  if (dimension > 3) throw InvalidDimensionException(HERE) << "RandomMixture only possible for dimension 1,2 or 3";
  setDimension(dimension);
  if (weights.getDimension() != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight matrix must have the same column numbers as the distribution collection's size";
  if (weights.getSize() != constant.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight matrix must have the same row numbers as the distribution dimension";
  weights_ = Matrix(weights.getDimension(), weights.getSize(), weights.getImplementation()->getData()).transpose();
  setDistributionCollection(coll);
}

/* Parameter constructor - nD */
RandomMixture::RandomMixture(const DistributionCollection & coll,
                             const NumericalSample & weights)
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
  , alpha_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultAlpha" ))
  , beta_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultBeta" ))
  , pdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultPDFEpsilon" ))
  , cdfPrecision_(ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon" ))
  , equivalentNormal_()
{
  setName("RandomMixture");
  const UnsignedInteger dimension(weights.getDimension());
  if (dimension > 3) throw InvalidDimensionException(HERE) << "RandomMixture only possible for dimension 1,2 or 3";
  constant_ = NumericalPoint(dimension, 0.0);
  setDimension(dimension);
  if (dimension != coll.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight matrix must have the same column numbers as the distribution collection's size";
  weights_ = Matrix(weights.getDimension(), weights.getSize(), weights.getImplementation()->getData()).transpose();
  setDistributionCollection(coll);
}
/* Compute the numerical range of the distribution given the parameters values */
void RandomMixture::computeRange()
{
  const UnsignedInteger size(distributionCollection_.getSize());
  Interval::BoolCollection finiteLowerBound(getDimension());
  Interval::BoolCollection finiteUpperBound(getDimension());
  NumericalPoint lowerBound(getDimension());
  NumericalPoint upperBound(getDimension());
  for (UnsignedInteger j = 0; j < getDimension(); ++j)
  {
    Interval range(constant_[j], constant_[j]);
    for (UnsignedInteger i = 0; i < size; ++i)
      range += distributionCollection_[i].getRange() * weights_(j, i);
    lowerBound[j] = range.getLowerBound()[0];
    upperBound[j] = range.getUpperBound()[0];
    finiteLowerBound[j] = range.getFiniteLowerBound()[0];
    finiteUpperBound[j] = range.getFiniteUpperBound()[0];
  }
  Interval range(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
  if (getDimension() == 1)
  {
    const NumericalPoint m(1, getPositionIndicator());
    const NumericalPoint s(1, getDispersionIndicator());
    setRange(range.intersect(Interval(m - s * beta_, m + s * beta_)));
  } // dimension == 1
  else
  {
    NumericalPoint m(constant_);
    NumericalPoint s(getDimension(), 0.0);
    for (UnsignedInteger j = 0; j < getDimension(); ++j)
    {
      for(UnsignedInteger i = 0; i < size; ++i)
      {
        m[j] += weights_(j, i) * distributionCollection_[i].getPositionIndicator();
        s[j] += std::pow(weights_(j, i) * distributionCollection_[i].getDispersionIndicator(), 2.0);
      }
    }
    for (UnsignedInteger j = 0; j < getDimension(); ++j) s[j] = std::sqrt(s[j]);
    setRange(range.intersect(Interval(getMean() - getStandardDeviation() * beta_, getMean() + getStandardDeviation() * beta_)));
  } // dimension > 1
}

/* Comparison operator */
Bool RandomMixture::operator ==(const RandomMixture & other) const
{
  if (this == &other) return true;
  return (distributionCollection_ == other.distributionCollection_) && (constant_ == other.constant_);
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
  oss << offset << getClassName() << "(";
  const UnsignedInteger size(distributionCollection_.getSize());
  if (dimension_ > 1) oss << "\n";
  // Print marginal by marginal
  for (UnsignedInteger marginal = 0; marginal < dimension_; ++ marginal)
  {
    // If marginal > 0, alignement
    if (constant_[marginal] != 0.0) oss << constant_[marginal];
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const NumericalScalar w(weights_(marginal, i));
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
    if (dimension_ > 1) oss << "\n";
  }
  oss << ")";
  return oss;
}

/* Weights distribution accessor */
void RandomMixture::setWeights(const Matrix & weights)
{
  weights_ = weights;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computePositionIndicator();
  computeDispersionIndicator();
  computeRange();
  computeReferenceBandwidth();
  computeEquivalentNormal();
}

Matrix RandomMixture::getWeights() const
{
  return weights_;
}


/* Distribution collection accessor */
void RandomMixture::setDistributionCollection(const DistributionCollection & coll)
{
  const UnsignedInteger size(coll.getSize());
  const UnsignedInteger dimension(getDimension());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a RandomMixture based on an empty distribution collection.";
  Bool hasNormalAtom(false);
  NumericalScalar aggregatedMean(0.0);
  NumericalScalar aggregatedVariance(0.0);
  Bool hasPendingUniform(false);
  Uniform pendingUniform;
  distributionCollection_ = DistributionCollection(0);
  NumericalSample weights(0, dimension);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    if (coll[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: a RandomMixture cannot be built from a collection of distributions of dimension not equal to 1";
    // Knowledge-based optimization
    if (dimension == 1)
    {
      if (coll[i].getImplementation()->getClassName() == "Uniform")
      {
        const NumericalScalar w(weights_(0, i));
        const NumericalScalar low(coll[i].getRange().getLowerBound()[0]);
        const NumericalScalar high(coll[i].getRange().getUpperBound()[0]);
        NumericalScalar a0(w * low);
        NumericalScalar b0(w * high);
        if (a0 > b0) std::swap(a0, b0);
        // If there is already a uniform, merge it into a symmetrical trapezoidal distribution
        if (hasPendingUniform)
        {
          const NumericalScalar a1(pendingUniform.getA());
          const NumericalScalar b1(pendingUniform.getB());
          const NumericalScalar alpha(a1 + a0);
          const NumericalScalar delta(b1 + b0);
          const NumericalScalar halfWidth(0.5 * std::abs((b1 - a1) - (b0 - a0)));
          const NumericalScalar center(0.5 * (alpha + delta));
          if (halfWidth > 0.0) distributionCollection_.add(Trapezoidal(alpha, center - halfWidth, center + halfWidth, delta));
          else distributionCollection_.add(Triangular(alpha, center, delta));
          // Add weight
          weights.add(NumericalPoint(1, 1.0));
          hasPendingUniform = false;
        }
        else
        {
          pendingUniform = Uniform(a0, b0);
          hasPendingUniform = true;
        }
      } // atom is a Uniform
      else if (coll[i].getImplementation()->getClassName() == "Normal")
      {
        hasNormalAtom = true;
        const NumericalScalar w(weights_(0, i));
        aggregatedMean += w * coll[i].getMean()[0];
        aggregatedVariance += w * w * coll[i].getCovariance()(0, 0);
      } // atom is a Normal
      else if (coll[i].getImplementation()->getClassName() == "Dirac")
      {
        const NumericalScalar w(weights_(0, i));
        constant_[0] += w * coll[i].getSupport()[0][0];
      } // atom is a Dirac
      else if (coll[i].getImplementation()->getClassName() == "RandomMixture")
      {
        const NumericalScalar w(weights_(0, i));
        const RandomMixture * mixture(static_cast< const RandomMixture * >(coll[i].getImplementation().get()));
        // As the random mixture atom has already been built, all its Dirac components have been merged into its constant and all its
        // Normal components have been merged into a single component in the last place, if any.
        // First, merge the constant
        constant_[0] += w * mixture->constant_[0];
        // Second, merge the potential Normal component
        UnsignedInteger atomSize(mixture->distributionCollection_.getSize());
        const Matrix localWeights(mixture->getWeights() * w);
        if (mixture->distributionCollection_[atomSize - 1].getImplementation()->getClassName() == "Normal")
        {
          hasNormalAtom = true;
          aggregatedMean += w * mixture->distributionCollection_[atomSize - 1].getMean()[0];
          aggregatedVariance += w * w * mixture->distributionCollection_[atomSize - 1].getCovariance()(0, 0);
          --atomSize;
        }
        // Third, merge the remaining components
        for (UnsignedInteger j = 0; j < atomSize; ++j)
        {
          const Distribution atom(mixture->distributionCollection_[j]);
          if (atom.getImplementation()->getClassName() == "Uniform")
          {
            const NumericalScalar wj(localWeights(0, j));
            const NumericalScalar low(coll[i].getRange().getLowerBound()[0]);
            const NumericalScalar high(coll[i].getRange().getUpperBound()[0]);
            NumericalScalar a0(wj * low);
            NumericalScalar b0(wj * high);
            if (a0 > b0) std::swap(a0, b0);
            // If there is already a uniform, merge it into a symmetrical trapezoidal distribution
            if (hasPendingUniform)
            {
              const NumericalScalar a1(pendingUniform.getA());
              const NumericalScalar b1(pendingUniform.getB());
              const NumericalScalar alpha(a1 + a0);
              const NumericalScalar delta(b1 + b0);
              const NumericalScalar halfWidth(0.5 * std::abs((b1 - a1) - (b0 - a0)));
              const NumericalScalar center(0.5 * (alpha + delta));
              if (halfWidth > 0.0) distributionCollection_.add(Trapezoidal(alpha, center - halfWidth, center + halfWidth, delta));
              else distributionCollection_.add(Triangular(alpha, center, delta));
              // Add weight
              weights.add(NumericalPoint(1, 1.0));
              hasPendingUniform = false;
            }
            else
            {
              pendingUniform = Uniform(a0, b0);
              hasPendingUniform = true;
            }
          } // atom is a Uniform
          else
          {
            distributionCollection_.add(atom);
            weights.add(NumericalPoint(1, localWeights(0, j)));
          }
        }
      } // atom is a RandomMixture
      else
      {
        distributionCollection_.add(coll[i]);
        weights.add(NumericalPoint(1, weights_(0, i)));
      } // not Uniform, Normal, Dirac, RandomMixture
    }
    else
    {
      // In nD, there is currently no aggregation
      // Weight matrix is unchanged
      distributionCollection_.add(coll[i]);
    } //multivariate case
  }// i loopfor

  // Set the aggregated normal as the last atom
  if (hasNormalAtom)
  {
    distributionCollection_.add(Normal(aggregatedMean + constant_[0], std::sqrt(aggregatedVariance)));
    constant_[0] = 0.0;
    weights.add(NumericalPoint(1, 1.0));
  }
  if (hasPendingUniform)
  {
    if (constant_[0] != 0.0)
    {
      pendingUniform = Uniform(pendingUniform.getA() + constant_[0], pendingUniform.getB() + constant_[0]);
      constant_[0] = 0.0;
    }
    distributionCollection_.add(pendingUniform);
    weights.add(NumericalPoint(1, 1.0));
  }

  if (dimension == 1) setWeights(Matrix(1, distributionCollection_.getSize() , weights.getImplementation()->getData()));

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
void RandomMixture::setConstant(const NumericalPoint & constant)
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

NumericalPoint RandomMixture::getConstant() const
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
NumericalPoint RandomMixture::getRealization() const
{
  const UnsignedInteger size(distributionCollection_.getSize());
  NumericalPoint realization(size);
  for (UnsignedInteger i = 0; i < size; ++i) realization[i] = distributionCollection_[i].getRealization()[0];
  return weights_ * realization + constant_;
}


/* Get the DDF of the RandomMixture */
NumericalPoint RandomMixture::computeDDF(const NumericalPoint & point) const
{
  return DistributionImplementation::computeDDF(point);
}

/* Wrapper for the convolution in the 1D case with 2 atoms */
struct RandomMixture2AtomsWrapper
{
  RandomMixture2AtomsWrapper(const NumericalScalar alpha1,
                             const NumericalScalar alpha2,
                             const Distribution & atom1,
                             const Distribution & atom2,
                             const NumericalScalar z0)
    : alpha1_(alpha1)
    , alpha2_(alpha2)
    , atom1_(atom1)
    , atom2_(atom2)
    , z0_(z0)
  {
    // Nothing to do
  }
  // Z = alpha0 + alpha1 X1 + alpha2 X2
  // F(z) = P(Z < z) = P(alpha1 X1 + alpha2 X2 < z - alpha0)
  NumericalPoint convolutionPDFKernel(const NumericalPoint & point) const
  {
    const NumericalScalar t(point[0]);
    const NumericalScalar res(atom1_.computePDF(t) * atom2_.computePDF((z0_ - alpha1_ * t) / alpha2_));
    return NumericalPoint(1, res);
  }

  NumericalPoint convolutionCDFKernel(const NumericalPoint & point) const
  {
    const NumericalScalar t(point[0]);
    const NumericalScalar pdf1(atom1_.computePDF(t));
    const NumericalScalar s((z0_ - alpha1_ * t) / alpha2_);
    const NumericalScalar cdf2(atom2_.computeCDF(s));
    const NumericalScalar res(pdf1 * cdf2);
    return NumericalPoint(1, res);
  }

  NumericalPoint convolutionCCDFKernel(const NumericalPoint & point) const
  {
    const NumericalScalar t(point[0]);
    const NumericalScalar res(atom1_.computePDF(t) * atom2_.computeComplementaryCDF((z0_ - alpha1_ * t) / alpha2_));
    return NumericalPoint(1, res);
  }

  const NumericalScalar alpha1_;
  const NumericalScalar alpha2_;
  const Distribution & atom1_;
  const Distribution & atom2_;
  const NumericalScalar z0_;
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
NumericalScalar RandomMixture::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();

  if (isAnalytical_)
  {
    // compute analytically the pdf
    const NumericalPoint u(point - constant_);
    const NumericalPoint Qu(inverseWeights_ * u);
    NumericalScalar value(std::abs(detWeightsInverse_));
    for (UnsignedInteger j = 0; j < dimension_; ++j) value *= distributionCollection_[j].computePDF(Qu[j]);
    return value;
  } // isAnalytical_

  // Check range
  // We check that point is in range, excepted bounds
  // In bounds, value is 0.0
  const Interval range(getRange());
  const NumericalPoint lowerBound(range.getLowerBound());
  const NumericalPoint upperBound(range.getUpperBound());
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    if ((point[j] <= lowerBound[j]) || (point[j] >= upperBound[j])) return 0.0;
  // Special case for 1D distributions with exactly 2 atoms
  if ((dimension_ == 1) && (distributionCollection_.getSize() == 2))
  {
    // Get the parameters of the random mixture
    const NumericalScalar z0(point[0] - constant_[0]);
    const NumericalScalar alpha1(weights_(0, 0));
    const NumericalScalar alpha2(weights_(0, 1));
    // Get the bounds of the atoms
    const NumericalScalar a(distributionCollection_[0].getRange().getLowerBound()[0]);
    const NumericalScalar b(distributionCollection_[0].getRange().getUpperBound()[0]);
    const NumericalScalar c(distributionCollection_[1].getRange().getLowerBound()[0]);
    const NumericalScalar d(distributionCollection_[1].getRange().getUpperBound()[0]);
    // Compute the bounds of the convolution
    NumericalScalar lower;
    NumericalScalar upper;
    NumericalScalar uc((z0 - alpha2 * c) / alpha1);
    NumericalScalar ud((z0 - alpha2 * d) / alpha1);
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
    const NumericalMathFunction convolutionKernel(bindMethod<RandomMixture2AtomsWrapper, NumericalPoint, NumericalPoint>(convolutionKernelWrapper, &RandomMixture2AtomsWrapper::convolutionPDFKernel, 1, 1));
    return algo.integrate(convolutionKernel, Interval(lower, upper), pdfEpsilon_)[0] / std::abs(alpha2);
  }

  LOGDEBUG(OSS() << "Equivalent normal=" << equivalentNormal_);
  // We unroll the complex arithmetic and we perform incremental update in order to improve the performances
  NumericalPoint two_pi_on_h(dimension_);
  for (UnsignedInteger k = 0; k < dimension_; ++k) two_pi_on_h[k] = 2.0 * M_PI / referenceBandwidth_[k];
  UnsignedInteger levelMax(0);
  NumericalScalar value(computeEquivalentNormalPDFSum(point, two_pi_on_h, 0, levelMax));

  UnsignedInteger k(1);
  const NumericalScalar precision(pdfPrecision_);
  const UnsignedInteger kmin(1 << blockMin_);
  const UnsignedInteger kmax(1 << blockMax_);
  // hX is only useful in 1D
  NumericalScalar hX = referenceBandwidth_[0] * point[0];
  NumericalScalar error(2.0 * precision);
  LOGDEBUG(OSS() << std::setprecision(20) << "h=" << referenceBandwidth_ << ", equivalent normal pdf sum=" << value << ", k=" << k << ", precision=" << precision << ", kmin=" << kmin << ", kmax=" << kmax << ", error=" << error);
  while ( (k < kmin) || ( (k < kmax) && (error > precision) ) )
  {
    NumericalScalar sumContributions(0.0);
    error = 0.0;
    for (UnsignedInteger m = k; m < 2 * k; ++m)
    {
      if (dimension_ == 1)
      {
        const NumericalScalar sinMHX(std::sin(m * hX));
        const NumericalScalar cosMHX(std::cos(m * hX));
        const NumericalComplex deltaValue(computeDeltaCharacteristicFunction(m));
        const NumericalScalar contribution((deltaValue.real() * cosMHX + deltaValue.imag() * sinMHX));
        LOGDEBUG(OSS() << "m=" << m << ", delta=" << deltaValue << ", contribution=" << contribution);
        sumContributions += contribution;
        error += std::abs(contribution);
      } // dimension_ == 1
      else
      {
        NumericalSample skinPoints(gridMesher_.getPoints(m));
        const UnsignedInteger fromIndex(gridMesher_.getOffsetLevel(m));
        const UnsignedInteger lastIndex(gridMesher_.getOffsetLevel(m + 1) - 1);
        if (lastIndex <= maxSize_)
        {
          if (lastIndex > storedSize_)
            updateCacheDeltaCharacteristicFunction(skinPoints);
          // Level is now entirely on cache
          for (UnsignedInteger i = 0; i < skinPoints.getSize(); ++i)
          {
            const NumericalComplex deltaValue(characteristicValuesCache_[fromIndex + i - 1]);
            hX = 0.0;
            for (UnsignedInteger j = 0; j < dimension_; ++j) hX += skinPoints[i][j] * point[j];
            const NumericalScalar sinHX(std::sin(hX));
            const NumericalScalar cosHX(std::cos(hX));
            const NumericalScalar contribution(deltaValue.real() * cosHX + deltaValue.imag() * sinHX);
            error += std::abs(contribution);
            sumContributions += contribution;
            LOGDEBUG(OSS() << "m=" << m << ", delta=" << deltaValue << ", contribution=" << contribution << ", error=" << error);
          } // skinPoints
        } // lastIndex <= maxSize_
        else
        {
          NumericalPoint pti(dimension_);
          for (UnsignedInteger i = 0; i < skinPoints.getSize(); ++i)
          {
            hX = 0.0;
            for (UnsignedInteger j = 0; j < dimension_; ++j)
            {
              pti[j] = skinPoints[i][j];
              hX += skinPoints[i][j] * point[j];
            }
            const NumericalComplex deltaValue(computeDeltaCharacteristicFunction(pti));
            const NumericalScalar sinHX(std::sin(hX));
            const NumericalScalar cosHX(std::cos(hX));
            const NumericalScalar contribution(deltaValue.real() * cosHX + deltaValue.imag() * sinHX);
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
  pdfEpsilon_ = error;
  return value;
}

/*  Compute the PDF of 1D distributions over a regular grid. The precision is reduced as this method is for drawing purpose only. */
NumericalSample RandomMixture::computePDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    NumericalSample & grid) const
{
  if (getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: this method is available only for 1D distribution";
  return computePDF(NumericalPoint(1, xMin), NumericalPoint(1, xMax), Indices(1, pointNumber), grid);
}

struct EquivalentNormalPDFSumPolicy
{
  const RandomMixture & mixture_;
  const NumericalSample & grid_;
  const NumericalPoint & two_b_sigma_;
  const UnsignedInteger levelMax_;
  Collection<NumericalScalar> & output_;

  EquivalentNormalPDFSumPolicy(const RandomMixture & mixture,
                               const NumericalSample & grid,
                               const NumericalPoint & two_b_sigma,
                               const UnsignedInteger levelMax,
                               Collection<NumericalScalar> & output)
    : mixture_(mixture)
    , grid_(grid)
    , two_b_sigma_(two_b_sigma)
    , levelMax_(levelMax)
    , output_(output)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    UnsignedInteger fakeLevelMax;
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      output_[i] = mixture_.computeEquivalentNormalPDFSum(grid_[i], two_b_sigma_, levelMax_, fakeLevelMax);
    }
  }
}; /* end struct EquivalentNormalPDFSumPolicy */

/* Compute the PDF of nD distributions over a regular grid */
NumericalSample RandomMixture::computePDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    NumericalSample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: xMin & xMax should be different to define a grid";
  if (isAnalytical_ && (dimension_ == 1)) return DistributionImplementation::computePDF(xMin, xMax, pointNumber, grid);
  Tuples::IndicesCollection indices(Tuples(pointNumber).generate());

  if (dimension_ < 1 || dimension_ > 3) throw InvalidArgumentException(HERE) << "Error: dimension must be 1, 2 or 3; here dimension=" << dimension_;
  // Special case for 1D distributions with exactly 2 atoms
  if ((dimension_ == 1) && (distributionCollection_.getSize() == 2))
  {
    const NumericalScalar a(xMin[0]);
    const NumericalScalar b(xMax[0]);
    const UnsignedInteger n(pointNumber[0]);
    grid = NumericalSample(n, 1);
    NumericalSample pdf(n, 1);
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      const NumericalScalar x(a + i * (b - a) / (n - 1));
      grid[i][0] = x;
      pdf[i][0] = computePDF(x);
    }
    return pdf;
  } // dimension == 1 && size == 2
  const NumericalPoint mu(getMean());
  const Interval bounds(xMin, xMax);
  //if (!bounds.contains(mu)) throw InvalidArgumentException(HERE) << "Error: requested interval does not contain mean=" << mu;

  const NumericalPoint sigma(getStandardDeviation());
  UnsignedInteger b(0);
  for(UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const NumericalScalar dx(std::max(mu[i] - xMin[i], xMax[i] - mu[i]));
    b = std::max(b, static_cast<UnsignedInteger>(std::ceil(dx / sigma[i])));
  }
  const NumericalPoint b_sigma(b * sigma);
  const NumericalPoint two_b_sigma(2.0 * b_sigma);

  NumericalPoint h(dimension_);
  NumericalPoint tau(dimension_);
  for(UnsignedInteger i = 0; i < dimension_; ++i)
  {
    h[i] = M_PI / b_sigma[i];
    tau[i] = mu[i] / b_sigma[i];
  }
  const UnsignedInteger size(indices.getSize());
  grid = NumericalSample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      grid[i][j] = mu[j] + ((2.0 * indices[i][j] + 1.0) / pointNumber[j] - 1.0) * b_sigma[j];

  LOGWARN(OSS() << "Warning! Grid is modified: xMin=" << grid[0] << " xMax=" << grid[size - 1] << " instead of xMin=" << xMin << ", xMax=" << xMax);

  NumericalSample result(size, 1);
  if (isAnalytical_)
  {
    // compute analytically the pdf
    for (UnsignedInteger j = 0; j < size; ++j) result[j][0] = computePDF(grid[j]);
    return result;
  }
  UnsignedInteger levelMax(0);
  // Compute Gaussian sum pdf
  // First compute levelMax on mu, to speed up calls to computeEquivalentNormalPDFSum
  (void) computeEquivalentNormalPDFSum(mu, two_b_sigma, 0, levelMax);

  Collection<NumericalScalar> output(size);
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
    result[j][0] = std::max(0.0, result[j][0]);
  }
  return result;
}

struct AddPDFOn1DGridPolicy
{
  const RandomMixture & mixture_;
  const NumericalPoint & xPoints_;
  Collection<NumericalComplex> & output_;

  AddPDFOn1DGridPolicy(const RandomMixture & mixture,
                       const NumericalPoint & xPoints,
                       Collection<NumericalComplex> & output)
    : mixture_(mixture)
    , xPoints_(xPoints)
    , output_(output)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    NumericalPoint x(1);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      x[0] = xPoints_[i];
      output_[i] = mixture_.computeDeltaCharacteristicFunction(x);
    }
  }
}; /* end struct AddPDFOn1DGridPolicy */

void RandomMixture::addPDFOn1DGrid(const Indices & pointNumber, const NumericalPoint & h, const NumericalPoint & tau, NumericalSample & result) const
{
  if (pointNumber.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: the given indices must have dimension=1, here dimension=" << pointNumber.getSize();

  const UnsignedInteger N(pointNumber[0]);
  Collection<NumericalComplex> fx(N);
  Collection<NumericalComplex> z_exp(N);
  const NumericalComplex cOne(0.0, 1.0);
  // Grid points
  NumericalPoint xPlus(N);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    xPlus[i] = (i + 1) * h[0];
    fx[i] = std::exp(- M_PI * cOne * (tau[0] - 1.0 + 1.0 / N) * (1.0 + i));
    z_exp[i] = std::exp(- 2.0 * M_PI * cOne * static_cast<NumericalScalar>(i) / static_cast<NumericalScalar>(N));
  }

  // FFT 1D
  Collection<NumericalComplex> yk(N);
  // 1) compute \Sigma_+
  const  AddPDFOn1DGridPolicy policyGridPP(*this, xPlus, yk);
  TBB::ParallelFor( 0, N, policyGridPP);
  for (UnsignedInteger j = 0; j < N; ++j)
    yk[j] *= fx[j];

  Collection<NumericalComplex> sigma_plus(fftAlgorithm_.transform(yk));

  for (UnsignedInteger j = 0; j < N; ++j)
    sigma_plus[j] *= z_exp[j];

  // 2) compute \Sigma_-
  Collection<NumericalComplex> ykc(N);
  for (UnsignedInteger j = 0; j < N; ++j)
    ykc[j] = std::conj(yk[N - 1 - j]);

  Collection<NumericalComplex> sigma_minus(fftAlgorithm_.transform(ykc));

  const NumericalScalar scaling(h[0] / (2.0 * M_PI));
  for (UnsignedInteger j = 0; j < N; ++j)
  {
    result[j][0] += scaling * std::real( sigma_plus[j] + sigma_minus[j] );
  }
}

struct AddPDFOn2DGridPolicy
{
  const RandomMixture & mixture_;
  const NumericalPoint & xPoints_;
  const NumericalPoint & yPoints_;
  const UnsignedInteger nx_;
  const UnsignedInteger ny_;
  Collection<NumericalComplex> & output_;

  AddPDFOn2DGridPolicy(const RandomMixture & mixture,
                       const NumericalPoint & xPoints,
                       const NumericalPoint & yPoints,
                       Collection<NumericalComplex> & output)
    : mixture_(mixture)
    , xPoints_(xPoints)
    , yPoints_(yPoints)
    , nx_(xPoints.getDimension())
    , ny_(yPoints.getDimension())
    , output_(output)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    NumericalPoint x(2);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger jj( i / nx_ );
      const UnsignedInteger ii( i - jj * nx_ );
      x[0] = xPoints_[ii];
      x[1] = yPoints_[jj];
      output_[i] = mixture_.computeDeltaCharacteristicFunction(x);
    }
  }
}; /* end struct AddPDFOn2DGridPolicy */

void RandomMixture::addPDFOn2DGrid(const Indices & pointNumber, const NumericalPoint & h, const NumericalPoint & tau, NumericalSample & result) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: the given indices must have dimension=2, here dimension=" << pointNumber.getSize();

  const UnsignedInteger Nx(pointNumber[0]);
  const UnsignedInteger Ny(pointNumber[1]);
  Collection<NumericalComplex> fx(Nx);
  Collection<NumericalComplex> fy(Ny);
  Collection<NumericalComplex> z_exp_mx(Nx);
  Collection<NumericalComplex> z_exp_my(Ny);
  const NumericalComplex cOne(0.0, 1.0);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    fx[i] = std::exp(- M_PI * cOne * (tau[0] - 1.0 + 1.0 / Nx) * (1.0 + i));
    z_exp_mx[i] = std::exp(- 2.0 * M_PI * cOne * static_cast<NumericalScalar>(i) / static_cast<NumericalScalar>(Nx));
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    fy[j] = std::exp(- M_PI * cOne * (tau[1] - 1.0 + 1.0 / Ny) * (1.0 + j));
    z_exp_my[j] = std::exp(- 2.0 * M_PI * cOne * static_cast<NumericalScalar>(j) / static_cast<NumericalScalar>(Ny));
  }
  NumericalPoint xPlus(Nx);
  NumericalPoint xMinus(Nx);
  NumericalPoint yPlus(Ny);
  NumericalPoint yMinus(Ny);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    xPlus[i] = (i + 1) * h[0];
    xMinus[i] = (static_cast<NumericalScalar>(i) - Nx) * h[0];
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    yPlus[j] = (j + 1) * h[1];
    yMinus[j] = (static_cast<NumericalScalar>(j) - Ny) * h[1];
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
  NumericalComplexCollection yk0(Nx);
  NumericalPoint x(2);
  x[1] = 0.0;
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    x[0] = (i + 1) * h[0];
    yk0[i] = computeDeltaCharacteristicFunction(x) * fx[i];
  }
  NumericalComplexCollection sigma_plus_0(fftAlgorithm_.transform(yk0));
  for (UnsignedInteger i = 0; i < Nx; ++i)
    sigma_plus_0[i] *= z_exp_mx[i];

  // 6) compute \Sigma_-0
  NumericalComplexCollection yk0c(Nx);
  for (UnsignedInteger i = 0; i < Nx; ++i)
    yk0c[i] = std::conj(yk0[Nx - 1 - i]);
  NumericalComplexCollection sigma_minus_0(fftAlgorithm_.transform(yk0c));

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
  NumericalComplexCollection sigma_0_plus(fftAlgorithm_.transform(yk0));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    sigma_0_plus[j] *= z_exp_my[j];

  // 8) compute \Sigma_0-
  for (UnsignedInteger j = 0; j < Ny; ++j)
    yk0c[j] = std::conj(yk0[Ny - 1 - j]);
  NumericalComplexCollection sigma_0_minus(fftAlgorithm_.transform(yk0c));

  UnsignedInteger counter = 0;
  const NumericalScalar scaling((h[0] * h[1]) / (4.0 * M_PI * M_PI));
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    for (UnsignedInteger i = 0; i < Nx; ++i, ++counter)
    {
      result[counter][0] += scaling * std::real(
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
  const NumericalPoint & xPoints_;
  const NumericalPoint & yPoints_;
  const NumericalPoint & zPoints_;
  const UnsignedInteger nx_;
  const UnsignedInteger ny_;
  const UnsignedInteger nz_;
  Collection<NumericalComplex> & output_;

  AddPDFOn3DGridPolicy(const RandomMixture & mixture,
                       const NumericalPoint & xPoints,
                       const NumericalPoint & yPoints,
                       const NumericalPoint & zPoints,
                       Collection<NumericalComplex> & output)
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
    NumericalPoint x(3);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger kk( i / nx_ / ny_ );
      const UnsignedInteger jj( ( i - kk * nx_ * ny_ ) / nx_ );
      const UnsignedInteger ii( i - kk * nx_ * ny_ - jj * nx_ );
      x[0] = xPoints_[ii];
      x[1] = yPoints_[jj];
      x[2] = zPoints_[kk];
      output_[i] = mixture_.computeDeltaCharacteristicFunction(x);
    }
  }
}; /* end struct AddPDFOn3DGridPolicy */

void RandomMixture::addPDFOn3DGrid(const Indices & pointNumber, const NumericalPoint & h, const NumericalPoint & tau, NumericalSample & result) const
{
  if (pointNumber.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: the given indices must have dimension=3, here dimension=" << pointNumber.getSize();

  const UnsignedInteger Nx(pointNumber[0]);
  const UnsignedInteger Ny(pointNumber[1]);
  const UnsignedInteger Nz(pointNumber[2]);
  Collection<NumericalComplex> fx(Nx);
  Collection<NumericalComplex> fy(Ny);
  Collection<NumericalComplex> fz(Nz);
  Collection<NumericalComplex> z_exp_mx(Nx);
  Collection<NumericalComplex> z_exp_my(Ny);
  Collection<NumericalComplex> z_exp_mz(Nz);
  const NumericalComplex cOne(0.0, 1.0);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    fx[i] = std::exp(- M_PI * cOne * (tau[0] - 1.0 + 1.0 / Nx) * (1.0 + i));
    z_exp_mx[i] = std::exp(- 2.0 * M_PI * cOne * static_cast<NumericalScalar>(i) / static_cast<NumericalScalar>(Nx));
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    fy[j] = std::exp(- M_PI * cOne * (tau[1] - 1.0 + 1.0 / Ny) * (1.0 + j));
    z_exp_my[j] = std::exp(- 2.0 * M_PI * cOne * static_cast<NumericalScalar>(j) / static_cast<NumericalScalar>(Ny));
  }
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    fz[k] = std::exp(- M_PI * cOne * (tau[2] - 1.0 + 1.0 / Nz) * (1.0 + k));
    z_exp_mz[k] = std::exp(- 2.0 * M_PI * cOne * static_cast<NumericalScalar>(k) / static_cast<NumericalScalar>(Nz));
  }
  NumericalPoint xPlus(Nx);
  NumericalPoint xMinus(Nx);
  NumericalPoint yPlus(Ny);
  NumericalPoint yMinus(Ny);
  NumericalPoint zPlus(Nz);
  NumericalPoint zMinus(Nz);
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    xPlus[i] = (i + 1) * h[0];
    xMinus[i] = (static_cast<NumericalScalar>(i) - Nx) * h[0];
  }
  for (UnsignedInteger j = 0; j < Ny; ++j)
  {
    yPlus[j] = (j + 1) * h[1];
    yMinus[j] = (static_cast<NumericalScalar>(j) - Ny) * h[1];
  }
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    zPlus[k] = (k + 1) * h[2];
    zMinus[k] = (static_cast<NumericalScalar>(k) - Nz) * h[2];
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
  NumericalPoint x(3);
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
    x[1] = (static_cast<NumericalScalar>(j) - Ny) * h[1];
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
    x[2] = (static_cast<NumericalScalar>(k) - Nz) * h[2];
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
    x[2] = (static_cast<NumericalScalar>(k) - Nz) * h[2];
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
  Collection<NumericalComplex> yk00(Nx);
  x[1] = 0.0;
  x[2] = 0.0;
  for (UnsignedInteger i = 0; i < Nx; ++i)
  {
    x[0] = (i + 1) * h[0];
    yk00[i] = computeDeltaCharacteristicFunction(x) * fx[i];
  }
  Collection<NumericalComplex> sigma_plus_0_0(fftAlgorithm_.transform(yk00));
  for (UnsignedInteger i = 0; i < Nx; ++i)
    sigma_plus_0_0[i] *= z_exp_mx[i];

  // 22) compute \Sigma_-00
  Collection<NumericalComplex> yk00c(Nx);
  for (UnsignedInteger i = 0; i < Nx; ++i)
    yk00c[i] = std::conj(yk00[Nx - 1 - i]);
  Collection<NumericalComplex> sigma_minus_0_0(fftAlgorithm_.transform(yk00c));

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
  Collection<NumericalComplex> sigma_0_plus_0(fftAlgorithm_.transform(yk00));
  for (UnsignedInteger j = 0; j < Ny; ++j)
    sigma_0_plus_0[j] *= z_exp_my[j];

  // 24) compute \Sigma_0-0
  for (UnsignedInteger j = 0; j < Ny; ++j)
    yk00c[j] = std::conj(yk00[Ny - 1 - j]);
  Collection<NumericalComplex> sigma_0_minus_0(fftAlgorithm_.transform(yk00c));

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
  Collection<NumericalComplex> sigma_0_0_plus(fftAlgorithm_.transform(yk00));
  for (UnsignedInteger k = 0; k < Nz; ++k)
    sigma_0_0_plus[k] *= z_exp_mz[k];

  // 26) compute \Sigma_00-
  for (UnsignedInteger k = 0; k < Nz; ++k)
    yk00c[k] = std::conj(yk00[Nz - 1 - k]);
  Collection<NumericalComplex> sigma_0_0_minus(fftAlgorithm_.transform(yk00c));

  UnsignedInteger counter = 0;
  const NumericalScalar scaling((h[0] * h[1] * h[2]) / (8.0 * M_PI * M_PI * M_PI));
  for (UnsignedInteger k = 0; k < Nz; ++k)
  {
    for (UnsignedInteger j = 0; j < Ny; ++j)
    {
      for (UnsignedInteger i = 0; i < Nx; ++i, ++counter)
      {
        result[counter][0] += scaling * std::real(
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
NumericalScalar RandomMixture::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension())
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  if (point.getDimension() != 1) return computeProbability(Interval(getRange().getLowerBound(), point));

  const NumericalScalar x(point[0]);
  // Special case for combination containing only one contributor Y = alpha * X + beta
  // for alpha > 0.0:
  // P(Y < y) = P(X < (y - beta) / alpha) = CDF_X((y - beta) / alpha)
  // for alpha < 0.0:
  // P(Y < y) = P(X > (y - beta) / alpha) = 1.0 - CDF_X((y - beta) / alpha)
  if (isAnalytical_)
  {
    const NumericalScalar alpha(weights_(0, 0));
    if (alpha > 0.0) return distributionCollection_[0].computeCDF((x - constant_[0]) / alpha);
    // If alpha < 0.0, compute the complementary CDF
    return distributionCollection_[0].computeComplementaryCDF((x - constant_[0]) / alpha);
  }
  // Check range
  const Interval range(getRange());
  const NumericalScalar lowerBound(range.getLowerBound()[0]);
  const NumericalScalar upperBound(range.getUpperBound()[0]);
  if (x <= lowerBound) return 0.0;
  if (x >= upperBound) return 1.0;
  // Special case for 1D distributions with exactly 2 atoms
  if ((dimension_ == 1) && (distributionCollection_.getSize() == 2))
  {
    // Get the parameters of the random mixture
    const NumericalScalar z0(x - constant_[0]);
    const NumericalScalar alpha1(weights_(0, 0));
    const NumericalScalar alpha2(weights_(0, 1));
    // Get the bounds of the atoms
    const NumericalScalar a(distributionCollection_[0].getRange().getLowerBound()[0]);
    const NumericalScalar b(distributionCollection_[0].getRange().getUpperBound()[0]);
    const NumericalScalar c(distributionCollection_[1].getRange().getLowerBound()[0]);
    const NumericalScalar d(distributionCollection_[1].getRange().getUpperBound()[0]);
    // Compute the bounds of the convolution
    NumericalScalar lower;
    NumericalScalar upper;
    NumericalScalar uc((z0 - alpha2 * c) / alpha1);
    NumericalScalar ud((z0 - alpha2 * d) / alpha1);
    const RandomMixture2AtomsWrapper convolutionKernelWrapper(alpha1, alpha2, distributionCollection_[0], distributionCollection_[1], z0);
    GaussKronrod algo;
    if (alpha2 > 0)
    {
      const NumericalMathFunction convolutionKernel(bindMethod<RandomMixture2AtomsWrapper, NumericalPoint, NumericalPoint>(convolutionKernelWrapper, &RandomMixture2AtomsWrapper::convolutionCDFKernel, 1, 1));
      if (alpha1 > 0)
      {
        lower = std::max(a, ud);
        upper = std::min(b, uc);
        NumericalScalar cdf(algo.integrate(convolutionKernel, Interval(lower, upper), cdfEpsilon_)[0]);
        if (ud > a) cdf += distributionCollection_[0].computeCDF(ud);
        return cdf;
      } // alpha1 > 0
      else
      {
        lower = std::max(a, uc);
        upper = std::min(b, ud);
        NumericalScalar cdf(algo.integrate(convolutionKernel, Interval(lower, upper), cdfEpsilon_)[0]);
        if (uc > a) cdf += distributionCollection_[0].computeCDF(uc);
        return cdf;
      } // alpha1 < 0
    } // alpha2 > 0
    else
    {
      const NumericalMathFunction convolutionKernel(bindMethod<RandomMixture2AtomsWrapper, NumericalPoint, NumericalPoint>(convolutionKernelWrapper, &RandomMixture2AtomsWrapper::convolutionCCDFKernel, 1, 1));
      if (alpha1 > 0)
      {
        lower = std::max(a, uc);
        upper = std::min(b, ud);
        NumericalScalar cdf(algo.integrate(convolutionKernel, Interval(lower, upper), cdfEpsilon_)[0]);
        if (uc > a) cdf += distributionCollection_[0].computeCDF(uc);
        return cdf;
      } // alpha1 > 0
      else
      {
        lower = std::max(a, ud);
        upper = std::min(b, uc);
        NumericalScalar cdf(algo.integrate(convolutionKernel, Interval(lower, upper), cdfEpsilon_)[0]);
        if (ud > a) cdf += distributionCollection_[0].computeCDF(ud);
        return cdf;
      } // alpha1 < 0
    } // alpha2 < 0
  } // dimension_ == 1 && size == 2

  // Here we call computeProbability with a ]-inf, x] interval
  const NumericalScalar cdf(computeProbability(Interval(NumericalPoint(1, lowerBound), point, getRange().getFiniteLowerBound(), Interval::BoolCollection(1, true))));
  if (cdf < 0.5) return cdf;
  // and if the cdf value is less than 1/2, it was better to use the complementary CDF
  else return 1.0 - computeProbability(Interval(point, NumericalPoint(1, upperBound), Interval::BoolCollection(1, true), getRange().getFiniteUpperBound()));
}

NumericalScalar RandomMixture::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension())
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  if (getDimension() > 1) return DistributionImplementation::computeComplementaryCDF(point);
  const NumericalScalar x(point[0]);
  // Special case for combination containing only one contributor Y = alpha * X + beta
  // for alpha > 0.0:
  // P(Y < y) = P(X < (y - beta) / alpha) = CDF_X((y - beta) / alpha)
  // for alpha < 0.0:
  // P(Y < y) = P(X > (y - beta) / alpha) = 1.0 - CDF_X((y - beta) / alpha)
  if (isAnalytical_)
  {
    const NumericalScalar alpha(weights_(0, 0));
    if (alpha > 0.0) return distributionCollection_[0].computeComplementaryCDF((x - constant_[0]) / alpha);
    // If alpha < 0.0, compute the CDF
    return distributionCollection_[0].computeCDF((x - constant_[0]) / alpha);
  }
  // Check range
  const Interval range(getRange());
  const NumericalScalar lowerBound(range.getLowerBound()[0]);
  const NumericalScalar upperBound(range.getUpperBound()[0]);
  if (x <= lowerBound) return 1.0;
  if (x >= upperBound) return 0.0;
  // Here we call computeProbability with a [x, +inf[ interval
  // Here we call computeProbability with a ]-inf, x] interval
  const NumericalScalar complementaryCDF(computeProbability(Interval(point, NumericalPoint(1, upperBound), Interval::BoolCollection(1, true), getRange().getFiniteUpperBound())));
  if (complementaryCDF < 0.5) return complementaryCDF;
  // and if the cdf value is less than 1/2, it was better to use the complementary CDF
  else return 1.0 - computeProbability(Interval(NumericalPoint(1, lowerBound), point, getRange().getFiniteLowerBound(), Interval::BoolCollection(1, true)));
}

/*  Compute the CDF of 1D distributions over a regular grid. The precision is reduced as this method is for drawing purpose only. */
NumericalSample RandomMixture::computeCDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    NumericalSample & grid) const
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
NumericalScalar RandomMixture::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension(getDimension());
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  if (interval.isNumericallyEmpty()) return 0.0;
  if ((dimension != 1) || (distributionCollection_.getSize() >= ResourceMap::GetAsUnsignedInteger( "RandomMixture-SmallSize" )))
  {
    const NumericalScalar oldPDFPrecision(pdfPrecision_);
    pdfPrecision_ = std::pow(SpecFunc::NumericalScalarEpsilon, 2.0 / (3.0 * dimension_));
    const UnsignedInteger n1(ResourceMap::GetAsUnsignedInteger("RandomMixture-MarginalIntegrationNodesNumber"));
    const UnsignedInteger N(ResourceMap::GetAsUnsignedInteger("RandomMixture-MaximumIntegrationNodesNumber"));
    const UnsignedInteger n2(static_cast<UnsignedInteger>(round(std::pow(N, 1.0 / dimension_))));
    const UnsignedInteger marginalSize(SpecFunc::NextPowerOfTwo(std::min(n1, n2)));
    setIntegrationNodesNumber(marginalSize);
    const NumericalScalar probability(DistributionImplementation::computeProbability(interval));
    pdfPrecision_ = oldPDFPrecision;
    return probability;
#ifdef TRAPEZE
    // For now, use a simple mid-point rule
    const NumericalPoint xMin(interval.getLowerBound());
    const NumericalPoint xMax(interval.getUpperBound());
    NumericalSample gridX;
    const Indices discretization(dimension, marginalSize);
    const NumericalSample samplePDF(computePDF(xMin, xMax, discretization, gridX));
    // Compute the elementary volume
    const UnsignedInteger otherCornerIndex(static_cast<UnsignedInteger>(round((std::pow(marginalSize, dimension) - 1) / (marginalSize - 1))));
    const Interval elementaryInterval(gridX[0], gridX[otherCornerIndex]);
    const NumericalScalar elementaryVolume(elementaryInterval.getVolume());
    NumericalScalar cdf(0.0);
    for (UnsignedInteger i = 0; i < gridX.getSize(); ++i)
      if (interval.contains(gridX[i])) cdf += samplePDF[i][0] * elementaryVolume;
    return cdf;
#endif
  }
  // Special case for combination containing only one contributor
  if (isAnalytical_)
  {
    cdfEpsilon_ = ResourceMap::GetAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon" );
    const NumericalScalar lower(interval.getLowerBound()[0]);
    const NumericalScalar upper(interval.getUpperBound()[0]);
    const NumericalScalar weight(getWeight());
    // Negative weight, swap upper and lower bound flags
    if (weight < 0.0)
    {
      const Interval adjustedInterval(NumericalPoint(1, (upper - constant_[0]) / weight), NumericalPoint(1, (lower - constant_[0]) / weight), interval.getFiniteUpperBound(), interval.getFiniteUpperBound());
      return distributionCollection_[0].computeProbability(adjustedInterval.intersect(getRange()));
    }
    else
    {
      const Interval adjustedInterval(NumericalPoint(1, (lower - constant_[0]) / weight), NumericalPoint(1, (upper - constant_[0]) / weight), interval.getFiniteUpperBound(), interval.getFiniteUpperBound());
      return distributionCollection_[0].computeProbability(interval.intersect(getRange()));
    }
  }
  const Interval clippedInterval(getRange().intersect(interval));
  // Quick return if there is no mass in the clipped interval
  if (clippedInterval.isNumericallyEmpty()) return 0.0;
  const Bool finiteLowerBound(clippedInterval.getFiniteLowerBound()[0]);
  const Bool finiteUpperBound(clippedInterval.getFiniteUpperBound()[0]);
  // Quick return for integral over the whole real line
  if (!finiteLowerBound && !finiteUpperBound) return 1.0;
  const NumericalScalar lowerBound(clippedInterval.getLowerBound()[0]);
  const NumericalScalar upperBound(clippedInterval.getUpperBound()[0]);
  // Small size case: use Fourier series
  const NumericalScalar precision(cdfPrecision_);
  NumericalScalar error(2.0 * precision);
  const NumericalScalar a(referenceBandwidth_[0] * lowerBound);
  const NumericalScalar b(referenceBandwidth_[0] * upperBound);
  const NumericalScalar factor(referenceBandwidth_[0] / M_PI);
  NumericalScalar value(computeEquivalentNormalCDFSum(lowerBound, upperBound));
  UnsignedInteger k(1);
  const UnsignedInteger kmin(1 << blockMin_);
  const UnsignedInteger kmax(1 << blockMax_);
  while ( (k < kmax) && (error > std::max(precision, std::abs(precision * value)) || k < kmin) )
  {
    error = 0.0;
    for (UnsignedInteger m = k; m < 2 * k; ++m)
    {
      NumericalScalar sinMHLower;
      NumericalScalar cosMHLower;
      sinMHLower = std::sin(m * a);
      cosMHLower = std::cos(m * a);
      NumericalScalar sinMHUpper;
      NumericalScalar cosMHUpper;
      sinMHUpper = std::sin(m * b);
      cosMHUpper = std::cos(m * b);
      const NumericalComplex deltaValue(computeDeltaCharacteristicFunction(m));
      const NumericalScalar contribution(factor * (deltaValue.real() * (sinMHUpper - sinMHLower) + deltaValue.imag() * (cosMHLower - cosMHUpper)) / (m * referenceBandwidth_[0]));
      value += contribution;
      error += std::abs(contribution);
    }
    k *= 2;
  }
  cdfEpsilon_ = error;
  // For extrem values of the argument, the computed value can be slightly outside of [0,1]. Truncate it.
  return (value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value));
}

/*  Compute the quantile over a regular grid */
NumericalSample RandomMixture::computeQuantile(const NumericalScalar qMin,
    const NumericalScalar qMax,
    const UnsignedInteger pointNumber,
    const Bool tail) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot compute the quantile over a regular 1D grid if the dimension is > 1";
  NumericalSample result(pointNumber, 2);
  NumericalScalar q(qMin);
  const NumericalScalar step((qMax - qMin) / NumericalScalar(pointNumber - 1.0));
  for (UnsignedInteger i = 0; i < pointNumber; ++i)
  {
    result[i][0] = q;
    result[i][1] = computeQuantile(q)[0];
    q += step;
  }
  return result;
}

/* Quantile computation for dimension=1 */
NumericalScalar RandomMixture::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  // Special case for random mixture with only 1 atom: Y = alpha * X + beta
  // find Yq such that P(Y < Yq) = q
  // i.e. for alpha > 0
  // P(X < (Yq - beta) / alpha) = q -> Yq = alpha * Xq + beta where P(X < Xq) = q
  // and for alpha < 0
  // P(X > (Yq - beta) / alpha) = q i.e. P(X < (Yq - beta) / alpha) = r with r = 1-q -> Yq = alpha * Xr + beta
  if (isAnalytical_)
  {
    const NumericalScalar alpha(weights_(0, 0));
    return distributionCollection_[0].computeQuantile((alpha > 0.0) ? (prob) : (1.0 - prob))[0] * alpha + constant_[0];
  }
  // General case
  return DistributionImplementation::computeScalarQuantile(prob, tail);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex RandomMixture::computeCharacteristicFunction(const NumericalScalar x) const
{
  if (x == 0.0) return 1.0;
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex RandomMixture::computeCharacteristicFunction(const NumericalPoint & x) const
{
  // The characteristic function is given by the following formula:
  // \phi(y) = \prod_{j=1}^{d} (exp(i * y_j * constant_j) * \prod_{k=1}^{n} \phi_{X_k}((M^t y)_k))
  // compute M^t * y
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex RandomMixture::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  if (x == 0.0) return 0.0;
  NumericalComplex logCfValue(0.0, constant_[0] * x);
  const UnsignedInteger size(distributionCollection_.getSize());
  const NumericalScalar smallScalar(0.5 * std::log(SpecFunc::MinNumericalScalar));
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    logCfValue += distributionCollection_[i].computeLogCharacteristicFunction(weights_(0, i) * x);
    // Early exit for null value
    if (logCfValue.real() < smallScalar) break;
  } /* end for */
  return logCfValue;
}

NumericalComplex RandomMixture::computeLogCharacteristicFunction(const NumericalPoint & x) const
{
  // The log-characteristic function is given by:
  // log(\phi(x)) = \sum_{j=1}^{d} ((i * y_j * constant_j) + \sum_{k=1}^{n} log(\phi_{X_k})((M^t x)_k))
  const UnsignedInteger dimension(getDimension());
  const UnsignedInteger size(distributionCollection_.getSize());
  const NumericalScalar smallScalar(0.5 * std::log(SpecFunc::MinNumericalScalar));
  // 1) compute the deterministic term
  NumericalComplex logCfValue;
  for (UnsignedInteger i = 0; i < dimension; ++i) logCfValue += NumericalComplex(0.0, x[i] * constant_[i]);
  // 2) compute the random part
  // The variables are independent
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // compute M^t * x
    NumericalScalar wtx(0.);
    for (UnsignedInteger j = 0; j < dimension; ++j) wtx += weights_(j, i) * x[j];
    logCfValue += distributionCollection_[i].computeLogCharacteristicFunction(wtx);
    if (logCfValue.real() < smallScalar) break;
  }
  return logCfValue;
}

/* Compute a value of the characteristic function on a prescribed discretization. As the value associated with index == 0 is known, it is not stored so for index > 0, the corresponding value is at position index-1 */
NumericalComplex RandomMixture::computeDeltaCharacteristicFunction(const UnsignedInteger index) const
{
  LOGDEBUG(OSS() << "In RandomMixture::computeDeltaCharacteristicFunction, index=" << index << ", h=" << referenceBandwidth_.__str__());
  if (index == 0) return 0.0;
  // The cached values are computed and stored in an ascending order without hole: this function is always called on a sequence starting from 0 to n-1
  // Usual case first: the index is within the already computed values
  if (index <= storedSize_) return characteristicValuesCache_[index - 1];
  // If the index is higher than the maximum allowed storage
  if (index > maxSize_)
  {
    LOGINFO(OSS() << "Cache exceeded in RandomMixture::computeDeltaCharacteristicFunction, consider increasing maxSize_ to " << index);
    const NumericalScalar x(index * referenceBandwidth_[0]);
    const NumericalComplex logCF(computeLogCharacteristicFunction(x));
    const NumericalComplex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
    const NumericalComplex deltaLog(logCF - logNormalCF);
    NumericalComplex value;
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
      const NumericalScalar x(i * referenceBandwidth_[0]);
      const NumericalComplex logCF(computeLogCharacteristicFunction(x));
      const NumericalComplex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
      const NumericalComplex deltaLog(logCF - logNormalCF);
      NumericalComplex value;
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
NumericalComplex RandomMixture::computeDeltaCharacteristicFunction(const NumericalPoint & x) const
{
  // Direct application on a point ==> useful for computation on grid
  const NumericalComplex logCF(computeLogCharacteristicFunction(x));
  const NumericalComplex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
  const NumericalComplex deltaLog(logCF - logNormalCF);
  if (std::abs(deltaLog) < 1.0e-5) return std::exp(logNormalCF) * (deltaLog * (1.0 + deltaLog * (0.5 + deltaLog / 6.0)));
  else return std::exp(logCF) - std::exp(logNormalCF);
}

/* Update cache */
void RandomMixture::updateCacheDeltaCharacteristicFunction(const NumericalSample & points) const
{
  NumericalPoint x(dimension_);
  for(UnsignedInteger i = 0; i < points.getSize(); ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j) x[j] = points[i][j];
    // Computation of CF - NormalCF
    // Here we check if it is possible to reduce calculation
    // We reduce CF - NormalCF to NormalCF * (CF/NormalCF -1), which rewrites
    // as exp(logNormalCF) * (exp(deltaLog) - 1), with deltaLog=logCF - logNormalCF
    // We use a 3rd order Taylor expansion of exp(deltaLog) - 1 if |deltaLog| <= 1e-5
    const NumericalComplex logCF(computeLogCharacteristicFunction(x));
    const NumericalComplex logNormalCF(equivalentNormal_.computeLogCharacteristicFunction(x));
    const NumericalComplex deltaLog(logCF - logNormalCF);
    NumericalComplex value;
    if (std::abs(deltaLog) < 1.0e-5) value = std::exp(logNormalCF) * (deltaLog * (1.0 + deltaLog * (0.5 + deltaLog / 6.0)));
    else value = std::exp(logCF) - std::exp(logNormalCF);
    LOGDEBUG(OSS() << "ih=" << x << ", logCF=" << logCF << ", CF=" << std::exp(logCF) << ", logNormalCF=" << logNormalCF << ", NormalCF=" << std::exp(logNormalCF) << ", value=" << value);
    characteristicValuesCache_.add(value);
    ++storedSize_;
  }
}

/* Get the PDF gradient of the distribution */
NumericalPoint RandomMixture::computePDFGradient(const NumericalPoint & point) const
{
  return DistributionImplementation::computePDFGradient(point);
}

/* Get the CDF gradient of the distribution */
NumericalPoint RandomMixture::computeCDFGradient(const NumericalPoint & point) const
{
  return DistributionImplementation::computeCDFGradient(point);
}

/* Compute the mean of the RandomMixture */
void RandomMixture::computeMean() const
{
  mean_ = constant_;
  const UnsignedInteger size(distributionCollection_.getSize());
  NumericalPoint mu(size, 0.0);
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
  const UnsignedInteger dimension(getDimension());
  covariance_ = CovarianceMatrix(dimension);
  const UnsignedInteger size(distributionCollection_.getSize());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      NumericalScalar covariance(0.0);
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
NumericalPoint RandomMixture::getStandardDeviation() const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPoint sigma(dimension, 0.0);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    sigma[i] = std::sqrt(getCovariance().operator()(i, i));
  return sigma;
}

/* Get the skewness of the RandomMixture */
NumericalPoint RandomMixture::getSkewness() const
{
  NumericalPoint skewness(getDimension(), 0.0);
  const UnsignedInteger size(distributionCollection_.getSize());
  for (UnsignedInteger j = 0; j < getDimension(); ++j)
  {
    NumericalScalar variance(0.0);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      const NumericalScalar wi(weights_(j, i));
      const NumericalScalar wi2(wi * wi);
      const NumericalScalar vi(distributionCollection_[i].getCovariance().operator()(0, 0));
      variance += wi2 * vi;
      skewness[j] += wi2 * wi * distributionCollection_[i].getSkewness()[0] * std::pow(vi, 1.5);
    } /* end for */
    skewness[j] *= std::pow(variance, -1.5);
  }
  return skewness;
}

/* Get the kurtosis of the RandomMixture */
NumericalPoint RandomMixture::getKurtosis() const
{
  NumericalPoint kurtosis(getDimension(), 0.0);
  const UnsignedInteger size(distributionCollection_.getSize());
  NumericalPoint v(size);
  NumericalPoint w2(size);
  for (UnsignedInteger d = 0; d < getDimension(); ++d)
  {
    NumericalScalar variance(0.0);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      const NumericalScalar wi(weights_(d, i));
      const NumericalScalar wi2(wi * wi);
      w2[i] = wi2;
      const NumericalScalar vi(distributionCollection_[i].getCovariance().operator()(0, 0));
      v[i] = vi;
      variance += wi2 * vi;
      kurtosis[d] += wi2 * wi2 * distributionCollection_[i].getKurtosis()[0] * vi * vi;
      for (UnsignedInteger j = 0; j < i; ++j) kurtosis[d] += 6.0 * wi2 * w2[j] * vi * v[j];
    } /* end for */
    kurtosis[d] /= variance * variance;
  }
  return kurtosis;
}

/** Parameters value and description accessor */
RandomMixture::NumericalPointWithDescriptionCollection RandomMixture::getParametersCollection() const
{
  // TODO: Take into account Weights!
  const UnsignedInteger size(distributionCollection_.getSize());
  NumericalPointWithDescriptionCollection parameters(1);
  Description parametersDescription;
  // Form a big NumericalPoint from the parameters of each atom
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const String prefix(distributionCollection_[i].getName());
    const NumericalPointWithDescription atomParameters(distributionCollection_[i].getParametersCollection()[0]);
    const Description atomDescription(atomParameters.getDescription());
    const UnsignedInteger atomParameterDimension(atomParameters.getDimension());
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

/* Get a positon indicator for a 1D distribution */
NumericalScalar RandomMixture::getPositionIndicator() const
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
    const UnsignedInteger size(distributionCollection_.getSize());
    // Assume an additive behaviour of the position indicator. It is true for the mean value, and almost true for the median of moderatly skewed distributions
    for(UnsignedInteger i = 0; i < size; ++i) positionIndicator_ += weights_(0, i) * distributionCollection_[i].getPositionIndicator();
    isAlreadyComputedPositionIndicator_ = true;
  }
}


/* Get a dispersion indicator for a 1D distribution */
NumericalScalar RandomMixture::getDispersionIndicator() const
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
    const UnsignedInteger size(distributionCollection_.getSize());
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
void RandomMixture::setAlpha(const NumericalScalar alpha)
{
  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "Error: the alpha parameter must be strictly positive";
  alpha_ = alpha;
  computeRange();
  computeReferenceBandwidth();
}

NumericalScalar RandomMixture::getAlpha() const
{
  return alpha_;
}

void RandomMixture::setBeta(const NumericalScalar beta)
{
  beta_ = beta;
  computeRange();
  computeReferenceBandwidth();
}

NumericalScalar RandomMixture::getBeta() const
{
  return beta_;
}

/* Reference bandwidth accessor */
void RandomMixture::setReferenceBandwidth(const NumericalPoint & bandwidth)
{
  referenceBandwidth_ = bandwidth;
  // Reset the cached values
  storedSize_ = 0;
  characteristicValuesCache_ = NumericalComplexPersistentCollection(0);
}

NumericalPoint RandomMixture::getReferenceBandwidth() const
{
  return referenceBandwidth_;
}

/* PDF precision accessor. For other distributions, it is a read-only attribute. */
void RandomMixture::setPDFPrecision(const NumericalScalar pdfPrecision)
{
  pdfPrecision_ = pdfPrecision;
}

/* CDF precision accessor. For other distributions, it is a read-only attribute. */
void RandomMixture::setCDFPrecision(const NumericalScalar cdfPrecision)
{
  cdfPrecision_ = cdfPrecision;
}

/* Compute the reference bandwidth. It is defined as the largest bandwidth
   that allow a precise computation of the PDF over the range
   [positionIndicator_ +/- beta * dispersionIndicator_] */
void RandomMixture::computeReferenceBandwidth()
{
  referenceBandwidth_ = NumericalPoint(getDimension(), 0.0);
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
  characteristicValuesCache_ = NumericalComplexPersistentCollection(0);
}

/* Compute the equivalent normal distribution, i.e. with the same mean and
   the same standard deviation */
void RandomMixture::computeEquivalentNormal()
{
  if (distributionCollection_.getSize() > 0) equivalentNormal_ = Normal(getMean(), getCovariance());
  else equivalentNormal_ = Normal();
}

/* Compute the left-hand sum in Poisson's summation formula for the equivalent normal */
NumericalScalar RandomMixture::computeEquivalentNormalPDFSum(const NumericalScalar x) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "RandomMixture::computeEquivalentNormalPDFSum(NumericalScalar) is only possible for dimension 1";

  NumericalScalar value(equivalentNormal_.computePDF(x));
  UnsignedInteger i(0);
  NumericalScalar delta(0.0);
  do
  {
    ++i;
    const NumericalScalar step(2.0 * M_PI * i / referenceBandwidth_[0]);
    delta = equivalentNormal_.computePDF(x + step) + equivalentNormal_.computePDF(x - step);
    value += delta;
  }
  while (delta > 0.0 * value);
  return value;
}

NumericalScalar RandomMixture::computeEquivalentNormalPDFSum(const NumericalPoint & y, const NumericalPoint & gridStep,
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
    const NumericalScalar x(y[0]);
    NumericalScalar value(equivalentNormal_.computePDF(x));
    UnsignedInteger i(0);
    NumericalScalar delta(0.0);
    do
    {
      ++i;
      const NumericalScalar step(i * gridStep[0]);
      delta = equivalentNormal_.computePDF(x + step) + equivalentNormal_.computePDF(x - step);
      value += delta;
    }
    while (delta > 0.0 * value);
    return value;
  }

  // We cannot use gridMesher_; we need another instance, which does not use caching.
  // We force symmetry to improve performance.
  const SphereUniformNorm grid(SphereUniformNorm::GetFromGridSteps(gridStep, true));

  NumericalScalar gaussian_pdf(equivalentNormal_.computePDF(y));
  NumericalScalar delta(std::max(1.0, gaussian_pdf));
  const NumericalScalar epsilon(pdfPrecision_);

  // If imax is zero, we want to store in levelMax the first level which does not improve accuracy.
  // If non zero, this means that a previous call had already computed levelMax, and levelMax
  // must not change.
  levelMax = imax;
  NumericalPoint skin1(dimension_);
  NumericalPoint skin2(dimension_);
  for (UnsignedInteger i = 1; (imax == 0 || i < imax) && (delta > gaussian_pdf * epsilon); ++i)
  {
    const NumericalSample skinPoints(grid.getPoints(i));

    if (!imax) levelMax = i;
    const NumericalScalar numberOfPoints(skinPoints.getSize());
    delta = 0.0;
    for (UnsignedInteger j = 0; j < numberOfPoints; ++j)
    {
      for (UnsignedInteger d = 0; d < dimension_; ++d)
      {
        skin1[d] = y[d] + skinPoints[j][d];
        skin2[d] = y[d] - skinPoints[j][d];
      }
      delta += equivalentNormal_.computePDF(skin1) + equivalentNormal_.computePDF(skin2);
    }
    gaussian_pdf += delta;
  }
  return gaussian_pdf;
}

/* Compute the left-hand sum in Poisson's summation formula for the equivalent normal */
NumericalScalar RandomMixture::computeEquivalentNormalCDFSum(const NumericalScalar s,
    const NumericalScalar t) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "RandomMixture::computeEquivalentNormalCDFSum(NumericalScalar) is only possible for dimension 1";

  NumericalScalar value(equivalentNormal_.computeProbability(Interval(s, t)));
  UnsignedInteger i(0);
  NumericalScalar delta(0.0);
  do
  {
    ++i;
    const NumericalScalar step(2.0 * M_PI * i / referenceBandwidth_[0]);
    delta = (equivalentNormal_.computeCDF(t + step) - equivalentNormal_.computeCDF(s + step)) + (equivalentNormal_.computeCDF(t - step) - equivalentNormal_.computeCDF(s - step));
    value += delta;
  }
  while (delta > 0.0 * value);
  return value;
}

struct RandomMixturePair
{
  NumericalScalar norm_;
  Distribution distribution_;
  RandomMixturePair(): norm_(0.0), distribution_() {}
  RandomMixturePair(const NumericalScalar norm, const Distribution & distribution): norm_(norm), distribution_(distribution) {}

  Bool operator < (const RandomMixturePair & other) const
  {
    return norm_ < other.norm_;
  }
};

typedef Collection<RandomMixturePair> RandomMixturePairCollection;

/** Project a RandomMixture over a Collection of DistributionFactory by using a regular sampling and Kolmogorov distance. */
DistributionCollection RandomMixture::project(const DistributionFactoryCollection & factoryCollection,
    NumericalPoint & kolmogorovNorm,
    const UnsignedInteger size) const
{
  if (getDimension() != 1) throw NotDefinedException(HERE) << "Error: cannot project random mixtures of dimension>1.";
  const UnsignedInteger factorySize(factoryCollection.getSize());
  RandomMixturePairCollection result(0);
  const NumericalScalar mean(getMean()[0]);
  const NumericalScalar sigma(getStandardDeviation()[0]);
  // Sample the quantile function uniformly over [mean +/- alpha * sigma]
  const NumericalScalar qMin(computeCDF(mean - alpha_ * sigma));
  const NumericalScalar qMax(computeCDF(mean + alpha_ * sigma));
  const NumericalSample dataX(computeQuantile(qMin, qMax, size).getMarginal(1));
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
    NumericalScalar kolmogorov(0.0);
    for (UnsignedInteger j = 0; j < size; ++j)
      kolmogorov = std::max(kolmogorov, std::abs(candidate.computeCDF(dataX[j][0]) - (qMin + j * (qMax - qMin) / (size - 1.0))));
    result.add(RandomMixturePair(kolmogorov, candidate));
  }
  // Sort the results
  const UnsignedInteger resultSize(result.getSize());
  std::stable_sort(result.begin(), result.end());
  // Extract the results
  DistributionCollection distributionCollection(resultSize);
  kolmogorovNorm = NumericalPoint(resultSize);
  for (UnsignedInteger i = 0; i < resultSize; ++i)
  {
    distributionCollection[i] = result[i].distribution_;
    kolmogorovNorm[i] = result[i].norm_;
  }
  return distributionCollection;
}

/* Get the i-th marginal distribution */
RandomMixture::Implementation RandomMixture::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension(getDimension());
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  return RandomMixture(distributionCollection_, weights_.getRow(i), NumericalPoint(1, constant_[i])).clone();
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
RandomMixture::Implementation RandomMixture::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and  must be different";
  if (dimension == 1) return clone();
  const UnsignedInteger outputDimension(indices.getSize());
  const UnsignedInteger size(distributionCollection_.getSize());
  Matrix marginalWeights(outputDimension, size);
  NumericalPoint marginalConstant(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger fromI(indices[i]);
    marginalConstant[i] = constant_[fromI];
    const Matrix row(weights_.getRow(fromI));
    for (UnsignedInteger j = 0; j < outputDimension; ++j) marginalWeights(i, j) = row(0, j);
  }
  return RandomMixture(distributionCollection_, marginalWeights, marginalConstant).clone();
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
  const UnsignedInteger size(distributionCollection_.getSize());
  // Case of a Dirac distribution
  if (size == 0) return true;
  if (!distributionCollection_[0].isElliptical()) return false;
  Distribution standardDistribution(distributionCollection_[0].getStandardDistribution());
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (!distributionCollection_[i].isElliptical()) return false;
    if (distributionCollection_[i].getStandardDistribution() != standardDistribution) return false;
  }
  return true;
}

/* Check if the distribution is continuous */
Bool RandomMixture::isContinuous() const
{
  const UnsignedInteger size(distributionCollection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
    if (distributionCollection_[i].isContinuous()) return true;
  return false;
}

/* Check if the distribution is discrete */
Bool RandomMixture::isDiscrete() const
{
  const UnsignedInteger size(distributionCollection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
    if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Tell if the distribution is integer valued */
Bool RandomMixture::isIntegral() const
{
  const UnsignedInteger size(distributionCollection_.getSize());
  const UnsignedInteger dimension(getDimension());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Check if the contributor is discrete
    if (!distributionCollection_[i].isDiscrete()) return false;
    // Check if all the weights are integer
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if (weights_(i, j) != round(weights_(i, j))) return false;
  }
  return true;
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
  adv.saveAttribute( "inverseWeights_", inverseWeights_ );
  adv.saveAttribute( "detWeightsInverse_", detWeightsInverse_ );
  adv.saveAttribute( "fftAlgorithm_", fftAlgorithm_ );
  adv.saveAttribute( "isAnalytical_", isAnalytical_ );
  computePositionIndicator();
  computeDispersionIndicator();
  computeRange();
  computeReferenceBandwidth();
  computeEquivalentNormal();
} // load

END_NAMESPACE_OPENTURNS
