//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all Mixtures
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include <cmath>
#include <set>
#include "openturns/Mixture.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Mixture)

static const Factory<Mixture> Factory_Mixture;

/* Default constructor */
Mixture::Mixture()
  : DistributionImplementation()
  , distributionCollection_(1)
  , weightsDistribution_(UserDefined())
  , pdfApproximationCDF_()
  , cdfApproximation_()
  , pdfApproximationCCDF_()
  , ccdfApproximation_()
  , useApproximatePDFCDF_(false)
{
  setName("Mixture");
  setParallel(true);
  // Set an empty range
  setDistributionCollection(DistributionCollection(1));
}

/* Parameters constructor */
Mixture::Mixture(const DistributionCollection & coll)
  : DistributionImplementation()
  , distributionCollection_()
  , weightsDistribution_()
  , pdfApproximationCDF_()
  , cdfApproximation_()
  , pdfApproximationCCDF_()
  , ccdfApproximation_()
  , useApproximatePDFCDF_(false)
{
  setName("Mixture");
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollection( coll );
  if ((getDimension() == 1) && (coll.getSize() >= ResourceMap::GetAsUnsignedInteger("Mixture-SmallSize")) && (coll.getSize() < ResourceMap::GetAsUnsignedInteger("Mixture-LargeSize")))
  {
    // Here we use the implementation provided by the DistributionImplementation class instead of the ContinuousDistribution class in order to use both the PDF and the CDF
    Collection<PiecewiseHermiteEvaluation> interpolation(DistributionImplementation::interpolatePDFCDF(ResourceMap::GetAsUnsignedInteger("Mixture-PDFCDFDiscretization")));
    pdfApproximationCDF_ = interpolation[0];
    cdfApproximation_ = interpolation[1];
    pdfApproximationCCDF_ = interpolation[2];
    ccdfApproximation_ = interpolation[3];
    useApproximatePDFCDF_ = true;
  }
}

/* Parameters constructor */
Mixture::Mixture(const DistributionCollection & coll,
                 const Point & weights)
  : DistributionImplementation()
  , distributionCollection_()
  , weightsDistribution_()
  , pdfApproximationCDF_()
  , cdfApproximation_()
  , pdfApproximationCCDF_()
  , ccdfApproximation_()
  , useApproximatePDFCDF_(false)
{
  setName("Mixture");
  if (coll.getSize() != weights.getSize()) throw InvalidArgumentException(HERE) << "Error: the weight size " << weights.getSize() << " must be equal to the distribution collection size " << coll.getSize();
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollectionWithWeights( coll, weights );
  if ((getDimension() == 1) && (coll.getSize() >= ResourceMap::GetAsUnsignedInteger("Mixture-SmallSize")) && (coll.getSize() < ResourceMap::GetAsUnsignedInteger("Mixture-LargeSize")))
  {
    // Here we use the implementation provided by the DistributionImplementation class instead of the ContinuousDistribution class in order to use both the PDF and the CDF
    Collection<PiecewiseHermiteEvaluation> interpolation(DistributionImplementation::interpolatePDFCDF(ResourceMap::GetAsUnsignedInteger("Mixture-PDFCDFDiscretization")));
    pdfApproximationCDF_ = interpolation[0];
    cdfApproximation_ = interpolation[1];
    pdfApproximationCCDF_ = interpolation[2];
    ccdfApproximation_ = interpolation[3];
    useApproximatePDFCDF_ = true;
  }
}

/* Comparison operator */
Bool Mixture::operator ==(const Mixture & other) const
{
  if (this == &other) return true;
  return distributionCollection_ == other.distributionCollection_;
}

Bool Mixture::equals(const DistributionImplementation & other) const
{
  const Mixture* p_other = dynamic_cast<const Mixture*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Mixture::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Mixture::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " distributionCollection=" << distributionCollection_
      << " weightsDistribution=" << weightsDistribution_;
  return oss;
}

String Mixture::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << separator << "(w = " << distributionCollection_[i].getWeight() << ", d = " << distributionCollection_[i] << ")";
    separator = ", ";
  }
  oss << ")";
  return oss;
}

/* Weights accessor */
Point Mixture::getWeights() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  Point weights(size);
  for (UnsignedInteger i = 0; i < size; ++i) weights[i] = distributionCollection_[i].getWeight();
  return weights;
}

void Mixture::setWeights(const Point & weights)
{
  const DistributionCollection coll(distributionCollection_);
  setDistributionCollectionWithWeights( coll, weights);
}


/* Compute the numerical range of the distribution given the parameters values */
void Mixture::computeRange()
{
  const UnsignedInteger size = distributionCollection_.getSize();
  if (size == 0) return;
  Interval range(distributionCollection_[0].getRange());
  for (UnsignedInteger i = 1; i < size; ++i) range = range.join(distributionCollection_[i].getRange());
  setRange(range);
}

/* Weights distribution accessor */
void Mixture::setWeightsDistribution(const UserDefined & weightsDistribution)
{
  weightsDistribution_ = weightsDistribution;
}

UserDefined Mixture::getWeightsDistribution() const
{
  return weightsDistribution_;
}


/* Distribution collection accessor */
void Mixture::setDistributionCollection(const DistributionCollection & coll)
{
  const UnsignedInteger size = coll.getSize();
  Point weights(size);
  for (UnsignedInteger i = 0; i < size; ++i) weights[i] = coll[i].getWeight();
  setDistributionCollectionWithWeights(coll, weights);
}

void Mixture::setDistributionCollectionWithWeights(const DistributionCollection & coll,
    const Point & weights)
{
  // Not const because the collection will be simplified and its size reduced
  UnsignedInteger size = coll.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Mixture based on an empty distribution collection.";
  if (weights.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the number of weights=" << weights.getSize() << " is different from the number of distributions=" << size << ".";
  Scalar maximumWeight = weights[0];
  Scalar weightSum = maximumWeight;
  UnsignedInteger dimension = coll[0].getDimension();
  // First loop, check the atoms dimensions and the weigths values
  for(UnsignedInteger i = 1; i < size; ++i)
  {
    if (dimension != coll[i].getDimension())
      // We throw an exception because the collection has distributions of different sizes
      throw InvalidArgumentException(HERE) << "Collection of distributions has distributions of different dimensions";
    Scalar w = weights[i];
    if (!(w >= 0.0)) throw InvalidArgumentException(HERE) << "Distribution " << i << " has a negative weight, w=" << w;
    if (w > maximumWeight) maximumWeight = w;
    weightSum += w;
  } /* end for */
  const Scalar smallWeight = ResourceMap::GetAsScalar("Mixture-SmallWeight");
  if (weightSum < smallWeight)
    // We throw an exception because the collection of distributions has only distributions with small weight: they cannot be renormalized
    throw InvalidArgumentException(HERE) << "Collection of distributions has atoms with too small total weight=" << weightSum << " for a threshold equal to Mixture-SmallWeight=" << smallWeight;
  // Second loop, keep only the atoms with a significant weight and update the sum
  weightSum = 0.0;
  distributionCollection_ = DistributionCollection(0);
  isCopula_ = true;
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar w = weights[i];
    if (w < smallWeight * maximumWeight)
    {
      LOGINFO(OSS() << "Warning! The distribution number " << i << " has a too small weight=" << w << " for a relative threshold equal to Mixture-SmallWeight=" << smallWeight << " with respect to the maximum weight=" << maximumWeight << ". It is removed from the collection.");
    }
    else
    {
      // Set the original weight into the collection as it will be reused from here in the normalization step
      Distribution atom(coll[i]);
      atom.setWeight(w);
      distributionCollection_.add(atom);
      weightSum += w;
      isCopula_ = isCopula_ && atom.isCopula();
    }
  }

  // Update the size of the collection as null-weighted distributions could have been dismissed
  size = distributionCollection_.getSize();

  // We set the member with the collection passed as argument and we renormalize it in place
  Sample x(size, 1);
  Point p(size);
  Bool parallel = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar normalizedWeight = distributionCollection_[i].getWeight() / weightSum;
    distributionCollection_[i].setWeight(normalizedWeight);
    x[i][0] = i;
    p[i] = normalizedWeight;
    parallel = parallel && distributionCollection_[i].getImplementation()->isParallel();
  } /* end for */
  setParallel(parallel);
  setWeightsDistribution(UserDefined(x, p));
  setDimension(dimension);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}


/* Distribution collection accessor */
const Mixture::DistributionCollection & Mixture::getDistributionCollection() const
{
  return distributionCollection_;
}

/* Virtual constructor */
Mixture * Mixture::clone() const
{
  return new Mixture(*this);
}


/* Get one realization of the Mixture */
Point Mixture::getRealization() const
{
  // Select the atom following the weightsDistribution
  const UnsignedInteger index = static_cast<UnsignedInteger>(round(weightsDistribution_.getRealization()[0]));
  return distributionCollection_[index].getRealization();
}


/* Get the DDF of the Mixture */
Point Mixture::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point ddfValue(dimension, 0.0);
  if (!getRange().numericallyContains(point)) return ddfValue;
  const UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) ddfValue += distributionCollection_[i].getWeight() * distributionCollection_[i].computeDDF(point);
  return ddfValue;
}

/* Get the PDF of the Mixture */
Scalar Mixture::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (useApproximatePDFCDF_)
  {
    if (point[0] < getMean()[0]) return pdfApproximationCDF_.derivate(point)[0];
    else return pdfApproximationCCDF_.derivate(point)[0];
  }
  Scalar pdfValue = 0.0;
  const UnsignedInteger size = distributionCollection_.getSize();
  if (!getRange().numericallyContains(point)) return pdfValue;
  for(UnsignedInteger i = 0; i < size; ++i) pdfValue += distributionCollection_[i].getWeight() * distributionCollection_[i].computePDF(point);
  return pdfValue;
}

/* Get the CDF of the Mixture */
Scalar Mixture::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (useApproximatePDFCDF_)
  {
    if (point[0] < getMean()[0]) return cdfApproximation_(point)[0];
    else return 1.0 - ccdfApproximation_(point)[0];
  }
  Scalar cdfValue = 0.0;
  const UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) cdfValue += distributionCollection_[i].getWeight() * distributionCollection_[i].computeCDF(point);
  return cdfValue;
}

/* Get the survival function of the Mixture */
Scalar Mixture::computeSurvivalFunction(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Scalar survivalValue = 0.0;
  const UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) survivalValue += distributionCollection_[i].getWeight() * distributionCollection_[i].computeSurvivalFunction(point);
  return survivalValue;
}

/* Compute the probability content of an interval */
Scalar Mixture::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  const Interval reducedInterval(interval.intersect(getRange()));
  // If the interval is empty
  if (reducedInterval.isNumericallyEmpty()) return 0.0;

  // If the interval is the range
  if (reducedInterval == getRange()) return 1.0;

  Scalar probability = 0.0;
  const UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) probability += distributionCollection_[i].getWeight() * distributionCollection_[i].computeProbability(reducedInterval);
  return probability;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Mixture::computeCharacteristicFunction(const Scalar x) const
{
  Complex cfValue(0.0);
  UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) cfValue += distributionCollection_[i].getWeight() * distributionCollection_[i].computeCharacteristicFunction(x);
  return cfValue;
}

/* Get the PDF gradient of the distribution */
Point Mixture::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point pdfGradientValue;
  const UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) pdfGradientValue += distributionCollection_[i].getWeight() * distributionCollection_[i].computePDFGradient(point);
  return pdfGradientValue;
}

/* Get the CDF gradient of the distribution */
Point Mixture::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point cdfGradientValue(getDimension(), 0.0);
  const UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) cdfGradientValue += distributionCollection_[i].getWeight() * distributionCollection_[i].computeCDFGradient(point);
  return cdfGradientValue;
}

/* Get the i-th marginal distribution */
Mixture::Implementation Mixture::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  DistributionCollection collection;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger index = 0; index < size; ++index)
  {
    collection.add(distributionCollection_[index].getMarginal(i));
    collection[index].setWeight(distributionCollection_[index].getWeight());
  }
  Mixture* marginal(new Mixture(collection));
  marginal->isCopula_ = isCopula_;
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Mixture::Implementation Mixture::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  DistributionCollection collection;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger index = 0; index < size; ++index)
  {
    collection.add(distributionCollection_[index].getMarginal(indices));
    collection[index].setWeight(distributionCollection_[index].getWeight());
  }
  const UnsignedInteger outputDimension = indices.getSize();
  Description description(getDescription());
  Description marginalDescription(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    marginalDescription[i] = description[index_i];
  }
  Mixture* marginal(new Mixture(collection));
  marginal->isCopula_ = isCopula_;
  marginal->setDescription(marginalDescription);
  return marginal;
}

/* Compute the mean of the Mixture */
void Mixture::computeMean() const
{
  mean_ = Point(getDimension(), 0.0);
  const UnsignedInteger size = distributionCollection_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) mean_ += distributionCollection_[i].getWeight() * distributionCollection_[i].getMean();
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the Mixture */
void Mixture::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  // To insure a zero initialization
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) covariance_(i, i) = 0.0;
  const UnsignedInteger size = distributionCollection_.getSize();
  // First, compute E(X.X^t)
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar weightI = distributionCollection_[i].getWeight();
    const CovarianceMatrix covarianceI(distributionCollection_[i].getCovariance());
    const Point meanI(distributionCollection_[i].getMean());
    for(UnsignedInteger row = 0; row < dimension; ++row)
      for(UnsignedInteger column = 0; column <= row; ++column)
        covariance_(row, column) += weightI * (covarianceI(row, column) + meanI[row] * meanI[column]);
  } /* end for */
  // Then, substract E(X).E(X)^t
  const Point mean(getMean());
  for(UnsignedInteger row = 0; row < dimension; ++row)
    for(UnsignedInteger column = 0; column <= row; ++column)
      covariance_(row, column) -= mean[row] * mean[column];
  isAlreadyComputedCovariance_ = true;
}

/** Parameters value and description accessor */
Mixture::PointWithDescriptionCollection Mixture::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  // Special case for dimension=1
  if (dimension == 1)
  {
    PointWithDescriptionCollection parameters(1);
    Description description;
    // Form a big Point from the parameters of each atom and its weight
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const PointWithDescription atomParameters(distributionCollection_[i].getParametersCollection()[0]);
      const Description atomDescription(atomParameters.getDescription());
      const UnsignedInteger atomParameterDimension = atomParameters.getDimension();
      // Add the current atom parameters
      parameters[0].add(distributionCollection_[i].getWeight());
      description.add(OSS() << "w_" << i);
      for (UnsignedInteger j = 0; j < atomParameterDimension; ++j)
      {
        parameters[0].add(atomParameters[j]);
        description.add(atomDescription[j]);
      }
    }
    parameters[0].setDescription(description);
    parameters[0].setName(getName());
    return parameters;
  }
  // General case
  PointWithDescriptionCollection parameters(size + 1);
  Description description;
  // First put the marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < size; ++marginalIndex)
  {
    // Each marginal distribution must output a collection of parameters of size 1, even if it contains an empty Point
    const PointWithDescriptionCollection marginalParameters(distributionCollection_[marginalIndex].getParametersCollection());
    PointWithDescription point(marginalParameters[0]);
    point.setName(distributionCollection_[marginalIndex].getName());
    parameters[marginalIndex] = point;
  } // marginalIndex

  // Form a big Point from the dependence parameters of each atom
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const PointWithDescription atomDependenceParameters(distributionCollection_[i].getParametersCollection()[dimension]);
    const Description atomDescription(atomDependenceParameters.getDescription());
    const UnsignedInteger atomParameterDimension = atomDependenceParameters.getDimension();
    const String prefix(OSS() << "atom_" << i << "_");
    // Add the current atom dependence parameters
    for (UnsignedInteger j = 0; j < atomParameterDimension; j++)
    {
      parameters[size].add(atomDependenceParameters[j]);
      description.add(prefix + atomDescription[j]);
    }
  }
  parameters[size].setDescription(description);
  parameters[size].setName("dependence");
  return parameters;
} // getParametersCollection

/* Check if the distribution is elliptical */
Bool Mixture::isElliptical() const
{
  // If there is only one atom
  if (distributionCollection_.getSize() == 1) return distributionCollection_[0].isElliptical();
  return false;
}

/* Check if the distribution is continuous */
Bool Mixture::isContinuous() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isContinuous()) return false;
  return true;
}

/* Check if the distribution is discrete */
Bool Mixture::isDiscrete() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Check if the distribution is integral */
Bool Mixture::isIntegral() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isIntegral()) return false;
  return true;
}

/* Tell if the distribution has elliptical copula */
Bool Mixture::hasEllipticalCopula() const
{
  // In 1D, all the distributions have an elliptical copula
  if (getDimension() == 1) return true;
  // If there is only one atom, the mixture has the same properties as this atom
  if (distributionCollection_.getSize() == 1) return distributionCollection_[0].getImplementation()->hasEllipticalCopula();
  // General case
  return false;
}

/* Tell if the distribution has independent copula */
Bool Mixture::hasIndependentCopula() const
{
  // In 1D, all the distributions have an independent copula
  if (getDimension() == 1) return true;
  // If there is only one atom, the mixture has the same properties as this atom
  if (distributionCollection_.getSize() == 1) return distributionCollection_[0].getImplementation()->hasIndependentCopula();
  // General case
  return false;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Mixture::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  // Fill a std::set with the points in the support of each atom in order to automatically remove duplicates
  std::set<Point> setSupport;
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    const Sample atomSupport(distributionCollection_[i].getSupport(interval));
    for (UnsignedInteger j = 0; j < atomSupport.getSize(); ++j) setSupport.insert(atomSupport[j]);
  }
  Sample support(0, getDimension());
  for (std::set<Point>::iterator i = setSupport.begin(); i != setSupport.end(); ++i)
    support.add(*i);
  return support;
}

/* Get the PDF singularities inside of the range - 1D only */
Point Mixture::getSingularities() const
{
  if (getDimension() > 1) throw InternalException(HERE) << "Error: getSingularities() is defined for 1D distributions only";
  Point singularities(0);
  // Aggregate all the singularities of the atoms including the bounds of the range
  // as it can be singularities within the mixture range
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    singularities.add(distributionCollection_[i].getRange().getLowerBound()[0]);
    singularities.add(distributionCollection_[i].getSingularities());
    singularities.add(distributionCollection_[i].getRange().getUpperBound()[0]);
  }
  // The singularities of a distribution have to be strictly included into the
  // range of the distribution. As the range of a mixture is the bounding box
  // of the ranges of the atoms, the bounds of the mixture range are within
  // the bounds of the atoms ranges and after the sorting and removing of
  // duplicates they are the current first and last elements of singularities.
  // We use a 3-steps approach:
  // 1) Sort the values to put the lower bound at the first position and the upper
  //    bound at the last position
  std::sort(singularities.begin(), singularities.end());
  // 2) Remove the duplicates and the upper bound in a unique pass
  singularities.erase(std::unique(singularities.begin(), singularities.end()) - 1, singularities.end());
  // 3) Remove the lower bound
  singularities.erase(singularities.begin(), singularities.begin() + 1);
  return singularities;
}

/* Method save() stores the object through the StorageManager */
void Mixture::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distributionCollection_", distributionCollection_ );
  adv.saveAttribute( "weightsDistribution_", weightsDistribution_ );
  adv.saveAttribute( "pdfApproximationCDF_", pdfApproximationCDF_ );
  adv.saveAttribute( "cdfApproximation_", cdfApproximation_ );
  adv.saveAttribute( "pdfApproximationCCDF_", pdfApproximationCCDF_ );
  adv.saveAttribute( "ccdfApproximation_", ccdfApproximation_ );
  adv.saveAttribute( "useApproximatePDFCDF_", useApproximatePDFCDF_ );
}

/* Method load() reloads the object from the StorageManager */
void Mixture::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distributionCollection_", distributionCollection_ );
  adv.loadAttribute( "weightsDistribution_", weightsDistribution_ );
  adv.loadAttribute( "pdfApproximationCDF_", pdfApproximationCDF_ );
  adv.loadAttribute( "cdfApproximation_", cdfApproximation_ );
  adv.loadAttribute( "pdfApproximationCCDF_", pdfApproximationCCDF_ );
  adv.loadAttribute( "ccdfApproximation_", ccdfApproximation_ );
  adv.loadAttribute( "useApproximatePDFCDF_", useApproximatePDFCDF_ );
  // To compute the range
  setDistributionCollection(DistributionCollection(distributionCollection_));
}

END_NAMESPACE_OPENTURNS
