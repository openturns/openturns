//                                               -*- C++ -*-
/**
 *  @brief The class that implements randomMixtures
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
#ifndef OPENTURNS_RANDOMMIXTURE_HXX
#define OPENTURNS_RANDOMMIXTURE_HXX

#include "openturns/Distribution.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SphereUniformNorm.hxx"
#include "openturns/Indices.hxx"
#include "openturns/FFT.hxx"
#include "openturns/GaussKronrod.hxx"
#include <limits>       // std::numeric_limits

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RandomMixture
 *
 * The class describes the probabilistic concept of RandomMixture.
 */
class OT_API RandomMixture
  : public DistributionImplementation
{
  CLASSNAME
public:

  typedef Collection<Distribution>               DistributionCollection;
  typedef PersistentCollection<Distribution>     DistributionPersistentCollection;
  typedef PersistentCollection<Complex> ComplexPersistentCollection;
  typedef Collection<DistributionFactory>        DistributionFactoryCollection;

  /** Default constructor */
  RandomMixture();

  /** Parameter constructor - 1D */
  explicit RandomMixture(const DistributionCollection & coll,
                         const Scalar constant = 0.0);

  /** Parameter constructor - 1D */
  RandomMixture(const DistributionCollection & coll,
                const Point & weights,
                const Scalar constant = 0.0);

  /** Parameter constructor - nD */
  RandomMixture(const DistributionCollection & coll,
                const Matrix & weights,
                const Point & constant);

  /** Parameter constructor - nD */
  RandomMixture(const DistributionCollection & coll,
                const Matrix & weights);

  /** Parameter constructor - nD */
  RandomMixture(const DistributionCollection & coll,
                const Sample & weights,
                const Point & constant);

  /** Parameter constructor - nD */
  RandomMixture(const DistributionCollection & coll,
                const Sample & weights);

  /** Comparison operator */
  Bool operator ==(const RandomMixture & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Distribution collection accessor */
  const DistributionCollection & getDistributionCollection() const;

  /** FFT algorithm accessor */
  FFT getFFTAlgorithm() const;
  void setFFTAlgorithm(const FFT & fft);

  /** Constant accessor */
  void setConstant(const Point & constant);
  Point getConstant() const;



  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual RandomMixture * clone() const;

  /** Get one realization of the RandomMixture */
  Point getRealization() const;

  /** Get a sample of the RandomMixture */
  Sample getSample(const UnsignedInteger size) const;

protected:
  virtual Sample getSampleByQMC(const UnsignedInteger size) const;
public:

  /** Get the DDF of the RandomMixture */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the RandomMixture */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

#ifndef SWIG
  /** Compute the PDF over a regular grid */
  Sample computePDF(const Scalar xMin,
                    const Scalar xMax,
                    const UnsignedInteger pointNumber,
                    Sample & grid) const;

  /* Compute the PDF of over a regular grid */
  Sample computePDF(const Point & xMin,
                    const Point & xMax,
                    const Indices & pointNumber,
                    Sample & grid) const;
#endif

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const;

protected:

private:

  /** Quantile computation for dimension=1 */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Compute the characteristic function of 1D distributions by difference to a reference Normal distribution with the same mean and the same standard deviation in a regular pattern with cache */
  Complex computeDeltaCharacteristicFunction(const UnsignedInteger index) const;

  /** Compute the characteristic function of nD distributions by difference to a reference Normal distribution with the same mean and the same covariance */
  friend struct AddPDFOn1DGridPolicy;
  friend struct AddPDFOn2DGridPolicy;
  friend struct AddPDFOn3DGridPolicy;
  Complex computeDeltaCharacteristicFunction(const Point & x) const;

  /** Update cache of the characteristic function */
  void updateCacheDeltaCharacteristicFunction(const Sample & points) const;

  /** Contribution to computePDF on a 1D grid */
  void addPDFOn1DGrid(const Indices & pointNumber, const Point & h, const Point & tau, Sample & result) const;

  /** Contribution to computePDF on a 2D grid */
  void addPDFOn2DGrid(const Indices & pointNumber, const Point & h, const Point & tau, Sample & result) const;

  /** Contribution to computePDF on a 3D grid */
  void addPDFOn3DGrid(const Indices & pointNumber, const Point & h, const Point & tau, Sample & result) const;

public:
  /** Get the CDF of the RandomMixture */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;
  using DistributionImplementation::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const;

#ifndef SWIG
  /** Compute the CDF over a regular grid */
  Sample computeCDF(const Scalar xMin,
                    const Scalar xMax,
                    const UnsignedInteger pointNumber,
                    Sample & grid) const;
#endif

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Compute the quantile over a regular grid */
  using DistributionImplementation::computeQuantile;
  Sample computeQuantile(const Scalar qMin,
                         const Scalar qMax,
                         const UnsignedInteger pointNumber,
                         const Bool tail = false) const;

  /** Get the minimum volume level set containing a given probability of the distribution */
  virtual LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  using DistributionImplementation::computeCharacteristicFunction;
  Complex computeCharacteristicFunction(const Scalar x) const;
  Complex computeCharacteristicFunction(const Point & x) const;
  using DistributionImplementation::computeLogCharacteristicFunction;
  Complex computeLogCharacteristicFunction(const Scalar x) const;
  Complex computeLogCharacteristicFunction(const Point & x) const;

  /** Get the PDF gradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const;

  /** Get the CDF gradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

protected:
  void setDistributionCollectionAndWeights(const DistributionCollection & coll,
      const Matrix & weights,
      const Bool simplifyAtoms);

public:
  /** Weights distribution accessor */
  Matrix getWeights() const;

  /** Get a positon indicator for a 1D distribution */
  Scalar getPositionIndicator() const;

  /** Get a dispersion indicator for a 1D distribution */
  Scalar getDispersionIndicator() const;

  /** BlockMin accessor */
  void setBlockMin(const UnsignedInteger blockMin);
  UnsignedInteger getBlockMin() const;

  /** BlockMax accessor */
  void setBlockMax(const UnsignedInteger blockMax);
  UnsignedInteger getBlockMax() const;

  /** MaxSize accessor */
  void setMaxSize(const UnsignedInteger maxSize);
  UnsignedInteger getMaxSize() const;

  /** Alpha accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Beta accessor */
  void setBeta(const Scalar beta);
  Scalar getBeta() const;

  /** Reference bandwidth accessor */
  void setReferenceBandwidth(const Point & bandwidth);
  Point getReferenceBandwidth() const;

  /** PDF epsilon accessor. For other distributions, it is a read-only attribute. */
  void setPDFPrecision(const Scalar pdfPrecision);

  /** CDF epsilon accessor. For other distributions, it is a read-only attribute. */
  void setCDFPrecision(const Scalar cdfPrecision);

  /** Project a RandomMixture distribution over a collection of DistributionFactory by using sampling and Kolmogorov distance. */
  DistributionCollection project(const DistributionFactoryCollection & factoryCollection,
                                 Point & kolmogorovNormOut,
                                 const UnsignedInteger size = ResourceMap::GetAsUnsignedInteger( "RandomMixture-ProjectionDefaultSize" )) const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** Check if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  class KolmogorovProjection
  {
  public:
    /** Constructor from a distribution and a data set */
    KolmogorovProjection(const Sample & dataX,
                         const Sample & dataY,
                         const DistributionFactory & factory):
      dataX_(dataX),
      dataY_(dataY),
      factory_(factory) {};

    /** Compute the Kolmogorov distance based on the given data, for a given parameter set */
    Point computeNorm(const Point & parameters) const
    {
      Scalar norm = 0.0;
      try
      {
        const Distribution candidate(factory_.build(PointCollection(1, parameters)));
        for (UnsignedInteger i = 0; i < dataX_.getSize(); ++i)
          norm += std::pow(candidate.computeCDF(dataX_(i, 0)) - dataY_(i, 0), 2);
        return Point(1, norm);
      }
      catch(...)
      {
        return Point(1, SpecFunc::MaxScalar);
      }
    }

    /** factory accessor */
    void setDistributionFactory(const DistributionFactory & factory)
    {
      factory_ = factory;
    }
  private:
    Sample dataX_;
    Sample dataY_;
    DistributionFactory factory_;
  };

protected:
  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Get the mean of a randomMixture */
  void computeMean() const;

  /** Get the covariance of a randomMixture */
  void computeCovariance() const;

private:
  /** Compute the left-hand sum in Poisson's summation formula for the equivalent normal */
  Scalar computeEquivalentNormalPDFSum(const Scalar x) const;
  Scalar computeEquivalentNormalCDFSum(const Scalar s, const Scalar t) const;

  friend struct EquivalentNormalPDFSumPolicy;
  Scalar computeEquivalentNormalPDFSum(const Point & y, const Point & gridStep,
                                       UnsignedInteger imax, UnsignedInteger & levelMax) const;
public:
  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

private:
  /** Compute the position indicator */
  void computePositionIndicator() const;

  /** Compute the dispersion indicator */
  void computeDispersionIndicator() const;

  /** Compute the reference bandwidth. It is defined as the maximum bandwidth
      that allow a precise computation of the PDF over the range
      [positionIndicator_ +/- beta * dispersionIndicator_] */
  void computeReferenceBandwidth();

  /** Compute the equivalent normal distribution, i.e. with the same mean and
      the same standard deviation */
  void computeEquivalentNormal();

  /** The collection of distribution of the randomMixture */
  DistributionPersistentCollection distributionCollection_;

  /** The constant term of the mixture */
  Point constant_;

  /** The Weight matrix */
  Matrix weights_;

  /** inverse weight matrix if defined */
  Matrix inverseWeights_;

  /** Determinant of inverse weights */
  Scalar detWeightsInverse_;

  /** FFT algorithm */
  FFT fftAlgorithm_;

  /** The RandomMixture is analytic if size of collection = dimension */
  Bool isAnalytical_;

  /** Position indicator */
  mutable Scalar positionIndicator_;
  mutable Bool isAlreadyComputedPositionIndicator_;

  /** Dispersion indicator */
  mutable Scalar dispersionIndicator_;
  mutable Bool isAlreadyComputedDispersionIndicator_;

  /** Minimum number of blocks to consider for PDF and CDF computation */
  UnsignedInteger blockMin_;

  /** Maximum number of blocks to consider for PDF and CDF computation */
  UnsignedInteger blockMax_;

  /** Reference bandwidth */
  Point referenceBandwidth_;

  /** Reference bandwidth factor */
  Scalar referenceBandwidthFactor_;

  /** Maximum size of the cache for the CharacteristicFunction values */
  UnsignedInteger maxSize_;

  /** Index of the top of the cache */
  mutable UnsignedInteger storedSize_;

  /** Cache for the characteristic function values */
  mutable ComplexPersistentCollection characteristicValuesCache_;

  /** A priori range of PDF and CDF argument expressed in dispersionIndicator units */
  Scalar alpha_;

  /** Distance from the boundary of the a priori range at which the PDF is negligible */
  Scalar beta_;

  /** Requested precision for PDF computation */
  mutable Scalar pdfPrecision_;

  /** Requested precision for CDF computation */
  Scalar cdfPrecision_;

  /** Normal distribution with the same mean and standard deviation than the RandomMixture */
  Normal equivalentNormal_;

  /** Helper object to retrieve points on a regular grid */
  SphereUniformNorm gridMesher_;

  /** Integration algorithm */
  GaussKronrod algo_;

}; /* class RandomMixture */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMMIXTURE_HXX */
