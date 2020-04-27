//                                               -*- C++ -*-
/**
 *  @brief The class Sample implements blank free samples
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
#include <iomanip>
#include <fstream>
#include "openturns/Sample.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Sample)

/* Factory of Sample from CSV file */
Sample Sample::ImportFromCSVFile(const FileName & fileName,
                                 const String & csvSeparator)
{
  Sample sample(SampleImplementation::BuildFromCSVFile(fileName, csvSeparator));
  return sample;
}

/* Factory of Sample from Text file */
Sample Sample::ImportFromTextFile(const FileName & fileName,
                                  const String & separator,
                                  const UnsignedInteger skippedLines)
{
  Sample sample(SampleImplementation::BuildFromTextFile(fileName, separator, skippedLines));
  return sample;
}

/* Save to CSV file */
void Sample::exportToCSVFile(const FileName & filename,
                             const String & csvSeparator) const
{
  getImplementation()->exportToCSVFile(filename, csvSeparator);
}


/* Store a sample in a temporary text file, one realization by line. Returns the file name. */
String Sample::storeToTemporaryFile() const
{
  return getImplementation()->storeToTemporaryFile();
}

/* Export a sample as a matrix, one row by realization, in a format suitable to exchange with R. */
String Sample::streamToRFormat() const
{
  return getImplementation()->streamToRFormat();
}

/* Default constructor */
Sample::Sample()
  : TypedInterfaceObject<SampleImplementation>(new SampleImplementation(0, 1))
{
  // Nothing to do
}

/* Constructor with size and dimension */
Sample::Sample(const UnsignedInteger size,
               const UnsignedInteger dim)
  : TypedInterfaceObject<SampleImplementation>(new SampleImplementation(size, dim))
{
  // Nothing to do
}

/* Constructor from implementation */
Sample::Sample(const SampleImplementation & implementation)
  : TypedInterfaceObject<SampleImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
Sample::Sample(const Implementation & implementation)
  : TypedInterfaceObject<SampleImplementation>(implementation)
{
  // Nothing to do
}

/* Constructor from a Point (all elements are equal to the Point) */
Sample::Sample(const UnsignedInteger size,
               const Point & point)
  : TypedInterfaceObject<SampleImplementation>(new SampleImplementation(size, point))
{
  // Nothing to do
}

/* Partial copy constructor */
Sample::Sample(const Sample & other,
               const UnsignedInteger first,
               const UnsignedInteger last)
  : TypedInterfaceObject<SampleImplementation>(new SampleImplementation(*other.getImplementation(), other.getImplementation()->begin() + first, other.getImplementation()->begin() + last))
{
  // Nothing to do
}

/* Constructor from a collection of Point */
Sample::Sample(const Collection<Point> & coll)
  : TypedInterfaceObject<SampleImplementation>(new SampleImplementation(coll))
{
  // Nothing to do
}

/* Constructor from a collection of Indices */
Sample::Sample(const Collection<Indices> & coll)
  : TypedInterfaceObject<SampleImplementation>(new SampleImplementation(coll))
{
  // Nothing to do
}

/* Comparison operator */
Bool Sample::operator ==(const Sample & other) const
{
  return *getImplementation() == *(other.getImplementation());
}

/* Comparison operator */
Bool Sample::operator !=(const Sample & other) const
{
  return !operator==(other);
}


NSI_point Sample::operator [] (const UnsignedInteger index)
{
#ifdef DEBUG_BOUNDCHECKING
  // No copyOnWrite() as the at() method already does it
  return this->at(index);
#else
  copyOnWrite();
  return (*getImplementation())[index];
#endif /* DEBUG_BOUNDCHECKING */
}

NSI_const_point Sample::operator [] (const UnsignedInteger index) const
{
#ifdef DEBUG_BOUNDCHECKING
  return this->at(index);
#else
  return (*getImplementation())[index];
#endif /* DEBUG_BOUNDCHECKING */
}


Scalar & Sample::operator () (const UnsignedInteger i,
                              const UnsignedInteger j)
{
#ifdef DEBUG_BOUNDCHECKING
  // No copyOnWrite() as the at() method already does it
  return this->at(i, j);
#else
  copyOnWrite();
  return (*getImplementation())(i, j);
#endif /* DEBUG_BOUNDCHECKING */
}

const Scalar & Sample::operator () (const UnsignedInteger i,
                                    const UnsignedInteger j) const
{
#ifdef DEBUG_BOUNDCHECKING
  return this->at(i, j);
#else
  return (*getImplementation())(i, j);
#endif /* DEBUG_BOUNDCHECKING */
}


NSI_point Sample::at (const UnsignedInteger index)
{
  if (index >= getSize()) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  copyOnWrite();
  return (*getImplementation())[index];
}

NSI_const_point Sample::at (const UnsignedInteger index) const
{
  if (index >= getSize()) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  return (*getImplementation())[index];
}

Scalar & Sample::at (const UnsignedInteger i,
                     const UnsignedInteger j)
{
  if (i >= getSize()) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than size (" << getSize() << ")";
  if (j >= getDimension()) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than dimension (" << getDimension() << ")";
  copyOnWrite();
  return (*getImplementation())(i, j);
}

const Scalar & Sample::at (const UnsignedInteger i,
                           const UnsignedInteger j) const
{
  if (i >= getSize()) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than size (" << getSize() << ")";
  if (j >= getDimension()) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than dimension (" << getDimension() << ")";
  return (*getImplementation())(i, j);
}

const Scalar * Sample::data() const
{
  return getImplementation()->data();
}

UnsignedInteger Sample::elementSize() const
{
  return getImplementation()->elementSize();
}

/* Whether the list contains the value val */
Bool Sample::contains(const Point & val) const
{
  return getImplementation()->contains(val);
}

/* String converter */
String Sample::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

String Sample::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}


/* Description accessor */
void Sample::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}



/* Description accessor */
Description Sample::getDescription() const
{
  return getImplementation()->getDescription();
}


/* Dimension accessor */
UnsignedInteger Sample::getDimension() const
{
  return getImplementation()->getDimension();
}


/* Size accessor */
UnsignedInteger Sample::getSize() const
{
  return getImplementation()->getSize();
}

/* Maximum accessor */
Point Sample::getMax() const
{
  return getImplementation()->getMax();
}

/* Minimum accessor */
Point Sample::getMin() const
{
  return getImplementation()->getMin();
}


/* Method add() appends an element to the collection */
void Sample::add(const Point & point)
{
  if ( (getSize() > 0) && (getDimension() != point.getDimension()) )
    throw InvalidArgumentException(HERE)
        << "Point has invalid dimension (dim=" << point.getDimension()
        << ") for sample (dim=" << getDimension() << ")";

  copyOnWrite();
  getImplementation()->add(point);
}

/* Method add() appends another sample to the collection */
void Sample::add(const Sample & sample)
{
  if ( (getSize() > 0) && (getDimension() != sample.getDimension()) )
    throw InvalidArgumentException(HERE)
        << "Sample has invalid dimension (dim=" << sample.getDimension()
        << ") for sample (dim=" << getDimension() << ")";

  copyOnWrite();
  getImplementation()->add(*sample.getImplementation());
}

/* Stack another sample to the current sample */
void Sample::stack(const Sample & sample)
{
  copyOnWrite();
  getImplementation()->stack(*sample.getImplementation());
}

/* Return the internal linear storage */
Point Sample::asPoint() const
{
  if (getDimension() != 1) throw NotDefinedException(HERE) << "Error: the conversion to Point is defined only for a sample of dimension=1, here dimension=" << getDimension();
  return getImplementation()->getData();
}

void Sample::erase(const UnsignedInteger first,
                   const UnsignedInteger last)
{
  copyOnWrite();
  getImplementation()->erase(first, last);
}


void Sample::erase(const UnsignedInteger index)
{
  copyOnWrite();
  getImplementation()->erase(index, index + 1);
}

void Sample::erase(SampleImplementation::iterator first, SampleImplementation::iterator last)
{
  copyOnWrite();
  getImplementation()->erase(first, last);
}

void Sample::clear()
{
  copyOnWrite();
  getImplementation()->clear();
}

/*
 * Method split() trunk the sample before the index passed as argument
 * and returns the remainder as new sample. This method tries its best not for doubling
 * memory usage.
 */
Sample Sample::split(const UnsignedInteger index)
{
  copyOnWrite();
  // We first save the size of the original sample
  UnsignedInteger theSize = getSize();

  // We first check that the index is in the sample's range
  if (index > theSize) throw OutOfBoundException(HERE) << "Index over size. Index=" << index << " size=" << theSize;
  // Quick check for easy cases
  if (index == theSize) return Sample(0, getDimension());
  if (index >= theSize / 2)   // Strategy 1.
  {
    // We take two iterators :
    //  * one on the first element of the returned sample (at the i-th element)
    //  * one at the end of the returned sample
    SampleImplementation::iterator first = getImplementation()->begin() + index;
    SampleImplementation::iterator last  = getImplementation()->end();

    // We create a new sample by copy-constructing it from the two iterators
    Sample newSample(new SampleImplementation(*getImplementation(), first, last));

    // We clear the copied elements of the original sample (*this)
    erase(first, last);

    // We return the new sample
    return newSample;

  }
  else     // Strategy 2.
  {
    // We take two iterators :
    //  * one at the beginning of the original sample
    //  * one before the beginning of the returned sample
    SampleImplementation::iterator first = getImplementation()->begin();
    SampleImplementation::iterator last  = getImplementation()->begin() + index;

    // We create a new sample by copy-constructing it from the two iterators
    Sample newSample(new SampleImplementation(*getImplementation(), first, last));

    // We clear the copied elements of the original sample (*this)
    erase(first, last);

    // We swap the two samples
    this->swap(newSample);

    // We return the new sample
    return newSample;
  }
}

/*
 * Method computeMean() gives the mean of the sample, based on the formula
 * mean = sum of the elements in the sample / size of the sample
 */
Point Sample::computeMean() const
{
  return getImplementation()->computeMean();
}

/*
 * Method computeCovariance() gives the covariance of the sample
 */
CovarianceMatrix Sample::computeCovariance() const
{
  return getImplementation()->computeCovariance();
}

/*
 * Method computeStandardDeviation() gives the standard deviation of the sample
 */
TriangularMatrix Sample::computeStandardDeviation() const
{
  return getImplementation()->computeStandardDeviation();
}

/*
 * Method computeStandardDeviationPerComponent() gives the standard deviation of each component of the sample
 */
Point Sample::computeStandardDeviationPerComponent() const
{
  return getImplementation()->computeStandardDeviationPerComponent();
}

/*
 * Method computePearsonCorrelation() gives the Pearson correlation matrix of the sample
 */
CorrelationMatrix Sample::computeLinearCorrelation() const
{
  return getImplementation()->computeLinearCorrelation();
}

CorrelationMatrix Sample::computePearsonCorrelation() const
{
  return getImplementation()->computePearsonCorrelation();
}

/*
 * Method computeSpearmanCorrelation() gives the Spearman correlation matrix of the sample
 */
CorrelationMatrix Sample::computeSpearmanCorrelation() const
{
  return getImplementation()->computeSpearmanCorrelation();
}

/*
 * Method computeKendallTau() gives the Kendall tau matrix of the sample
 */
CorrelationMatrix Sample::computeKendallTau() const
{
  return getImplementation()->computeKendallTau();
}

/*
 * Method computeRange gives the range of the sample (by component)
 */
Point Sample::computeRange() const
{
  return getImplementation()->computeRange();
}

/*
 * Method computeMedian() gives the median of the sample (by component)
 */
Point Sample::computeMedian() const
{
  return getImplementation()->computeMedian();
}

/*
 * Method computeVariance() gives the variance of the sample (by component)
 */
Point Sample::computeVariance() const
{
  return getImplementation()->computeVariance();
}

/*
 * Method computeSkewness() gives the skewness of the sample (by component)
 */
Point Sample::computeSkewness() const
{
  return getImplementation()->computeSkewness();
}

/*
 * Method computeKurtosis() gives the kurtosis of the sample (by component)
 */
Point Sample::computeKurtosis() const
{
  return getImplementation()->computeKurtosis();
}

/*
 * Gives the centered moment of order k of the sample (by component)
 */
Point Sample::computeCenteredMoment(const UnsignedInteger k) const
{
  return getImplementation()->computeCenteredMoment(k);
}

/*
 * Gives the raw moment of order k of the sample (by component)
 */
Point Sample::computeRawMoment(const UnsignedInteger k) const
{
  return getImplementation()->computeRawMoment(k);
}

/*
 * Method computeQuantilePerComponent() gives the quantile per component of the sample
 */
Point Sample::computeQuantilePerComponent(const Scalar prob) const
{
  return getImplementation()->computeQuantilePerComponent(prob);
}

Sample Sample::computeQuantilePerComponent(const Point & prob) const
{
  return getImplementation()->computeQuantilePerComponent(prob);
}

/*
 * Method computeQuantile() gives the N-dimension quantile of the sample
 */
Point Sample::computeQuantile(const Scalar prob) const
{
  return getImplementation()->computeQuantile(prob);
}

Sample Sample::computeQuantile(const Point & prob) const
{
  return getImplementation()->computeQuantile(prob);
}

/*
 * Get the empirical CDF of the sample
 */
Scalar Sample::computeEmpiricalCDF(const Point & point,
                                   const Bool tail) const
{
  return getImplementation()->computeEmpiricalCDF(point, tail);
}

/*
 * Get the position of a point in the sample.
 * Returns size if the point does not belong to the sample.
 */
UnsignedInteger Sample::find(const Point & point) const
{
  return std::find(getImplementation()->begin(),  getImplementation()->end(), point) - getImplementation()->begin();
}

/*
 * Translate realizations in-place
 */
Sample & Sample::operator += (const Scalar translation)
{
  copyOnWrite();
  if (translation != 0.0) getImplementation()->operator +=(Point(getDimension(), translation));
  return *this;
}

Sample & Sample::operator += (const Point & translation)
{
  copyOnWrite();
  getImplementation()->operator +=(translation);
  return *this;
}

Sample & Sample::operator += (const Sample & translation)
{
  copyOnWrite();
  getImplementation()->operator +=(*translation.getImplementation());
  return *this;
}

Sample & Sample::operator -= (const Scalar translation)
{
  copyOnWrite();
  if (translation != 0.0) getImplementation()->operator -=(Point(getDimension(), translation));
  return *this;
}

Sample & Sample::operator -= (const Point & translation)
{
  copyOnWrite();
  getImplementation()->operator -=(translation);
  return *this;
}

Sample & Sample::operator -= (const Sample & translation)
{
  copyOnWrite();
  getImplementation()->operator -=(*translation.getImplementation());
  return *this;
}

Sample Sample::operator + (const Scalar translation) const
{
  const Sample sample(getImplementation()->operator + (translation));
  return sample;
}

Sample Sample::operator + (const Point & translation) const
{
  const Sample sample(getImplementation()->operator + (translation));
  return sample;
}

Sample Sample::operator + (const Sample & translation) const
{
  const Sample sample(getImplementation()->operator + (*translation.getImplementation()));
  return sample;
}

Sample Sample::operator - (const Scalar translation) const
{
  const Sample sample(getImplementation()->operator - (translation));
  return sample;
}

Sample Sample::operator - (const Point & translation) const
{
  const Sample sample(getImplementation()->operator - (translation));
  return sample;
}

Sample Sample::operator - (const Sample & translation) const
{
  const Sample sample(getImplementation()->operator - (*translation.getImplementation()));
  return sample;
}

/*
 * Scale realizations componentwise in-place
 */
Sample & Sample::operator *= (const Scalar scaling)
{
  copyOnWrite();
  getImplementation()->operator *=(scaling);
  return *this;
}

Sample & Sample::operator *= (const Point & scaling)
{
  copyOnWrite();
  getImplementation()->operator *=(scaling);
  return *this;
}

Sample & Sample::operator /= (const Scalar scaling)
{
  copyOnWrite();
  getImplementation()->operator /=(scaling);
  return *this;
}

Sample & Sample::operator /= (const Point & scaling)
{
  copyOnWrite();
  getImplementation()->operator /=(scaling);
  return *this;
}

Sample Sample::operator * (const Scalar scaling) const
{
  const Sample sample(getImplementation()->operator * (scaling));
  return sample;
}

Sample Sample::operator * (const Point & scaling) const
{
  const Sample sample(getImplementation()->operator * (scaling));
  return sample;
}

Sample Sample::operator / (const Scalar scaling) const
{
  const Sample sample(getImplementation()->operator / (scaling));
  return sample;
}

Sample Sample::operator / (const Point & scaling) const
{
  const Sample sample(getImplementation()->operator / (scaling));
  return sample;
}

/* Ranked sample */
Sample Sample::rank() const
{
  return getImplementation()->rank();
}

/* Ranked component */
Sample Sample::rank(const UnsignedInteger index) const
{
  return getImplementation()->rank(index);
}

/* Sorted sample */
Sample Sample::sort() const
{
  return getImplementation()->sort();
}

void Sample::sortInPlace()
{
  copyOnWrite();
  getImplementation()->sortInPlace();
}

/* Sorted component */
Sample Sample::sort(const UnsignedInteger index) const
{
  return getImplementation()->sort(index);
}

/* Sorted component */
Sample Sample::sortAccordingToAComponent(const UnsignedInteger index) const
{
  return getImplementation()->sortAccordingToAComponent(index);
}

void Sample::sortAccordingToAComponentInPlace(const UnsignedInteger index)
{
  copyOnWrite();
  getImplementation()->sortAccordingToAComponentInPlace(index);
}

/* Sort and remove duplicated points */
Sample Sample::sortUnique() const
{
  return getImplementation()->sortUnique();
}

void Sample::sortUniqueInPlace()
{
  copyOnWrite();
  getImplementation()->sortUniqueInPlace();
}

/* Get the i-th marginal sample */
Sample Sample::getMarginal(const UnsignedInteger index) const
{
  return getImplementation()->getMarginal(index);
}

/* Get the marginal sample by indices */
Sample Sample::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* Get the marginal sample by identifiers */
Sample Sample::getMarginal(const Description & description) const
{
  return getImplementation()->getMarginal(description);
}

/* Select points as a sample */
Sample Sample::select(const UnsignedIntegerCollection & indices) const
{
  return getImplementation()->select(indices);
}

/* Returns a pointer to the underlying implementation object */
Sample::ImplementationAsPersistentObject Sample::getImplementationAsPersistentObject() const
{
  return getImplementation();
}

/* Sets the pointer to the underlying implementation object */
void Sample::setImplementationAsPersistentObject(const ImplementationAsPersistentObject & obj)
{
  getImplementation().assign(obj);
}

/* Product operator */
Sample operator *(const Scalar scalar,
                  const Sample & sample)
{
  return sample.getImplementation()->operator*(scalar);
}


END_NAMESPACE_OPENTURNS
