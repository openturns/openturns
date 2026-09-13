//                                               -*- C++ -*-
/**
 *  @brief ProcessSampleImplementation Class
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ProcessSampleImplementation.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/Drawable.hxx"
#include "openturns/NonStationaryCovarianceModelFactory.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<ProcessSampleImplementation>)

TEMPLATE_CLASSNAMEINIT(PersistentCollection<Field>)

static const Factory<PersistentCollection<Field> > Factory_PersistentCollection_Field;

CLASSNAMEINIT(ProcessSampleImplementation)

static const Factory<ProcessSampleImplementation> Factory_ProcessSampleImplementation;

ProcessSampleImplementation::ProcessSampleImplementation()
  : PersistentObject()
  , mesh_()
  , size_(0)
  , dimension_(0)
  , verticesNumber_(0)
  , flatData_()
{
  // Nothing to do
}

/* Default constructor */
ProcessSampleImplementation::ProcessSampleImplementation(const UnsignedInteger size,
    const Field & field)
  : PersistentObject()
  , mesh_(field.getMesh())
  , size_(size)
  , dimension_(field.getOutputDimension())
  , verticesNumber_(mesh_.getVerticesNumber())
  , flatData_()
{
  const Sample values(field.getValues());
  description_ = values.getDescription();
  const UnsignedInteger blockSize = verticesNumber_ * dimension_;
  flatData_.resize(size_ * blockSize);
  for (UnsignedInteger k = 0; k < size_; ++k)
    std::copy(values.getImplementation()->data_begin(), values.getImplementation()->data_end(), flatData_.data() + k * blockSize);
}

ProcessSampleImplementation::ProcessSampleImplementation(const Mesh & mesh,
    const UnsignedInteger size,
    const UnsignedInteger dimension)
  : PersistentObject()
  , mesh_(mesh)
  , size_(size)
  , dimension_(dimension)
  , verticesNumber_(mesh.getVerticesNumber())
  , flatData_(size * verticesNumber_ * dimension)
{
  // Nothing to do
}

ProcessSampleImplementation::ProcessSampleImplementation(const Mesh & mesh,
    const SampleCollection & collection)
  : PersistentObject()
  , mesh_(mesh)
  , size_(0)
  , dimension_(0)
  , verticesNumber_(mesh.getVerticesNumber())
  , flatData_()
{
  for (UnsignedInteger k = 0; k < collection.getSize(); ++k) add(collection[k]);
}

/* Virtual constructor */
ProcessSampleImplementation * ProcessSampleImplementation::clone() const
{
  return new ProcessSampleImplementation(*this);
}

/* String converter */
String ProcessSampleImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ProcessSampleImplementation::GetClassName()
      << " mesh=" << mesh_
      << " values=[" << [&]() {
           String str;
           for (UnsignedInteger i = 0; i < getSize(); ++i) str += ((i > 0) ? "," : "") + getField(i).__repr__();
           return str;
         }() << "]";
  return oss;
}

String ProcessSampleImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "[";
  String separator("");
  for (UnsignedInteger i = 0; i < getSize(); ++i, separator = "\n") oss << separator << offset << "field " << i << ":" << "\n" << getField(i).__str__(offset);
  oss << "]";
  return oss;
}

void ProcessSampleImplementation::erase(const UnsignedInteger first, const UnsignedInteger last)
{
  if (!(first <= last && last <= getSize())) throw OutOfBoundException(HERE) << "Error: cannot erase rows outside of the process sample";
  const UnsignedInteger blockSize = verticesNumber_ * dimension_;
  flatData_.erase(first * blockSize, last * blockSize);
  size_ -= last - first;
}

void ProcessSampleImplementation::clear()
{
  size_ = 0;
  dimension_ = 0;
  description_ = Description();
  flatData_ = DataContainer();
}

/* Partial copy constructor */
void ProcessSampleImplementation::add(const Field & field)
{
  if (getSize() == 0)
  {
    mesh_ = field.getMesh();
    verticesNumber_ = mesh_.getVerticesNumber();
    dimension_ = field.getOutputDimension();
  }
  else if ((dimension_ != field.getOutputDimension()) || !(mesh_ == field.getMesh()))
    throw InvalidArgumentException(HERE) << "Error: could not add the field. Either its dimension or its mesh are incompatible.";
  add(field.getValues());
}

void ProcessSampleImplementation::add(const Sample & values)
{
  if (values.getSize() != verticesNumber_) throw InvalidArgumentException(HERE) << "Error: could not add the values. Their size=" << values.getSize() << " does not match the number of vertices=" << verticesNumber_ << " of the mesh.";
  if (!(getSize() == 0 || dimension_ == values.getDimension())) throw InvalidArgumentException(HERE) << "Error: could not add the values. Their dimension=" << values.getDimension() << " does not match the process sample dimension=" << dimension_;
  if (getSize() == 0) dimension_ = values.getDimension();
  if (description_.getSize() == 0)
  {
    const Description defaultDescription(Description::BuildDefault(values.getDimension(), "v"));
    if (values.getDescription().getSize() > 0 && values.getDescription() != defaultDescription) description_ = values.getDescription();
  }
  const UnsignedInteger oldSize = flatData_.getSize();
  const UnsignedInteger blockSize = values.getSize() * dimension_;
  flatData_.resize(oldSize + blockSize);
  std::copy(values.getImplementation()->data_begin(), values.getImplementation()->data_end(), flatData_.data() + oldSize);
  ++size_;
}


/* Operators accessors */
Field ProcessSampleImplementation::getField(const UnsignedInteger index) const
{
  if (!(index < getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << getSize() - 1;
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  const UnsignedInteger dimension = getDimension();
  Sample values(verticesNumber, dimension);
  std::copy(flatData_.data() + blockOffset(index), flatData_.data() + blockOffset(index) + verticesNumber * dimension, values.getImplementation()->data_begin());
  const Description defaultDescription(Description::BuildDefault(dimension, "v"));
  if (description_.getSize() > 0 && description_ != defaultDescription) values.setDescription(description_);
  return Field(mesh_, values);
}


Field ProcessSampleImplementation::getFieldView(const UnsignedInteger index) const
{
  if (!(index < getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << getSize() - 1;
  // zero-copy view over realization k: (verticesNumber, dimension) row-major
  Pointer<FieldImplementation> p_impl(new FieldImplementation(mesh_, dimension_));
  p_impl->values_ = Sample(*SampleImplementation::FromDataContainerView(
      DataContainer(const_cast<Scalar *>(flatData_.data()) + blockOffset(index), verticesNumber_, dimension_, DataContainer::ROW_MAJOR)).getImplementation());
  if (description_.getSize() > 0)
  {
    const Description defaultDescription(Description::BuildDefault(dimension_, "v"));
    if (description_ != defaultDescription) p_impl->values_.setDescription(description_);
  }
  return Field(p_impl);
}

void ProcessSampleImplementation::setField(const UnsignedInteger index,
                                           const Field & field)
{
  if (!(index < getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << getSize() - 1;
  if (field.getOutputDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a field of dimension=" << getDimension() << ", got a field of dimension=" << field.getOutputDimension();
  const Sample values(field.getValues());
  if (description_.getSize() == 0)
  {
    const Description defaultDescription(Description::BuildDefault(values.getDimension(), "v"));
    if (values.getDescription().getSize() > 0 && values.getDescription() != defaultDescription) description_ = values.getDescription();
  }
  std::copy(values.getImplementation()->data_begin(), values.getImplementation()->data_end(), flatData_.data() + blockOffset(index));
}

void ProcessSampleImplementation::setField(const UnsignedInteger index,
                                           const Sample & values)
{
  if (!(index < getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << getSize() - 1;
  if (values.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a field of dimension=" << dimension_ << ", got a field of dimension=" << values.getDimension();
  if (values.getSize() != verticesNumber_) throw InvalidArgumentException(HERE) << "Error: expected a field of size=" << verticesNumber_ << ", got a field of size=" << values.getSize();
  if (description_.getSize() == 0)
  {
    const Description defaultDescription(Description::BuildDefault(values.getDimension(), "v"));
    if (values.getDescription().getSize() > 0 && values.getDescription() != defaultDescription) description_ = values.getDescription();
  }
  std::copy(values.getImplementation()->data_begin(), values.getImplementation()->data_end(), flatData_.data() + blockOffset(index));
}

void ProcessSampleImplementation::setField(const Field & field,
    const UnsignedInteger index)
{
  LOGWARN("ProcessSample.setField(Field, int) is deprecated, use ProcessSample.setField(int, Field)");
  setField(index, field);
}

/* Time grid accessors */
RegularGrid ProcessSampleImplementation::getTimeGrid() const
{
  return RegularGrid(mesh_);
}

/* Mesh accessors */
Mesh ProcessSampleImplementation::getMesh() const
{
  return mesh_;
}

/* Dimension accessors */
UnsignedInteger ProcessSampleImplementation::getDimension() const
{
  if (size_ == 0 && verticesNumber_ > 0) return 0;
  return dimension_;
}

/* Dimension accessors */
UnsignedInteger ProcessSampleImplementation::getSize() const
{
  return size_;
}

/* Mean accessor */
Field ProcessSampleImplementation::computeMean() const
{
  const UnsignedInteger size = getSize();
  if (!(size > 0)) throw InternalException(HERE) << "Error: cannot compute the mean of an empty sample.";
  // Each realization block is contiguous, so accumulation is a sequence
  // of unit-stride axpy operations over the whole block
  Sample meanValues(getField(0).getValues());
  int n = static_cast<int>(verticesNumber_ * dimension_);
  int one = 1;
  double alpha = 1.0;
  for (UnsignedInteger k = 1; k < size; ++k)
    daxpy_(&n, &alpha, const_cast<double *>(flatData_.data() + blockOffset(k)), &one, meanValues.getImplementation()->data_begin(), &one);
  meanValues *= Point(dimension_, 1.0 / size);
  return Field(mesh_, meanValues);
}


/*
 * Gives the range of the sample (by component)
 */
Field ProcessSampleImplementation::computeRange() const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeRange();
  return Field(mesh_, values);
}

/*
 * Gives the median of the sample (by component)
 */
Field ProcessSampleImplementation::computeMedian() const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeMedian();
  return Field(mesh_, values);
}

/*
 * Gives the variance of the sample (by component)
 */
Field ProcessSampleImplementation::computeVariance() const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeVariance();
  return Field(mesh_, values);
}

/*
 * Gives the skewness of the sample (by component)
 */
Field ProcessSampleImplementation::computeSkewness() const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeSkewness();
  return Field(mesh_, values);
}

/*
 * Gives the kurtosis of the sample (by component)
 */
Field ProcessSampleImplementation::computeKurtosis() const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeKurtosis();
  return Field(mesh_, values);
}

/*
 * Gives the central moment of order k of the sample (by component)
 */
Field ProcessSampleImplementation::computeCentralMoment(const UnsignedInteger k) const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeCentralMoment(k);
  return Field(mesh_, values);
}

/*
 * Gives the raw moment of order k of the sample (by component)
 */
Field ProcessSampleImplementation::computeRawMoment(const UnsignedInteger k) const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeRawMoment(k);
  return Field(mesh_, values);
}

/*
 * Get the empirical CDF of the sample
 */
Field ProcessSampleImplementation::computeEmpiricalCDF(const Point & point,
    const Bool tail) const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, 1);
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values(i, 0) = getSampleAtVertex(i).computeEmpiricalCDF(point, tail);
  return Field(mesh_, values);
}

/* Maximum accessor */
Field ProcessSampleImplementation::getMax() const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).getMax();
  return Field(mesh_, values);
}

/* Minimum accessor */
Field ProcessSampleImplementation::getMin() const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).getMin();
  return Field(mesh_, values);
}

/* Compute the sample of spatial means of each field */
Sample ProcessSampleImplementation::computeTemporalMean() const
{
  if (!mesh_.isRegular() || (mesh_.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the temporal mean is defined only when the mesh is regular and of dimension 1.";
  return computeSpatialMean();
}

/* Compute the sample of spatial means of each field */
Sample ProcessSampleImplementation::computeSpatialMean() const
{
  Sample result(getSize(), dimension_);
  for (UnsignedInteger k = 0; k < getSize(); ++k)
  {
    Scalar sum = 0.0;
    const Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      for (UnsignedInteger d = 0; d < dimension_; ++d)
        sum += block[v * dimension_ + d];
    for (UnsignedInteger d = 0; d < dimension_; ++d) result(k, d) = sum / verticesNumber_;
  }
  return result;
}

/* Standard deviation accessor */
Field ProcessSampleImplementation::computeStandardDeviation() const
{
  const UnsignedInteger size = getSize();
  if (size == 0) return Field();
  if (size == 1) return Field(mesh_, getDimension());
  Sample meanValues(computeMean().getValues());
  Sample stdValues(verticesNumber_, dimension_);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      for (UnsignedInteger d = 0; d < dimension_; ++d)
      {
        const Scalar slack = block[v * dimension_ + d] - meanValues(v, d);
        stdValues(v, d) += slack * slack;
      }
  }
  stdValues *= Point(dimension_, 1.0 / (size - 1.0));
  for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
    for (UnsignedInteger d = 0; d < dimension_; ++d)
      stdValues(v, d) = std::sqrt(stdValues(v, d));
  return Field(mesh_, stdValues);
}


struct ComputeQuantilePerComponentPolicy
{
  Point & contiguous_;
  SampleImplementation & output_;
  const UnsignedInteger size_;
  const UnsignedInteger index_;
  const Scalar alpha_;
  const Scalar beta_;

  ComputeQuantilePerComponentPolicy( Point & contiguous,
                                     SampleImplementation & output,
                                     UnsignedInteger size,
                                     UnsignedInteger index,
                                     Scalar beta)
    : contiguous_(contiguous)
    , output_(output)
    , size_(size)
    , index_(index)
    , alpha_(1.0 - beta)
    , beta_(beta)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    Point::iterator it = contiguous_.begin() + r.begin() * size_;
    SampleImplementation::data_iterator resultIt = output_.data_begin() + r.begin();
    if (beta_ == 0)
    {
      // We use a special case here to avoid using an indefinite value if index is the last element
      for (UnsignedInteger i = r.begin(); i != r.end(); ++i, ++resultIt, it += size_)
      {
        // Find index-th element
        std::nth_element(it, it + index_, it + size_);
        *resultIt = *(it + index_);
      }
    }
    else if (2 * index_ > size_)
    {
      for (UnsignedInteger i = r.begin(); i != r.end(); ++i, ++resultIt, it += size_)
      {
        // Find index-th and (index+1)-th elements
        std::nth_element(it, it + index_, it + size_);
        std::nth_element(it + index_, it + index_ + 1, it + size_);
        // Interpolation between the two adjacent empirical quantiles
        *resultIt = alpha_ * (*(it + index_)) + beta_ * (*(it + index_ + 1));
      }
    }
    else
    {
      for (UnsignedInteger i = r.begin(); i != r.end(); ++i, ++resultIt, it += size_)
      {
        // Find index-th and (index+1)-th elements
        std::nth_element(it, it + index_ + 1, it + size_);
        std::nth_element(it, it + index_, it + index_ + 1);
        // Interpolation between the two adjacent empirical quantiles
        *resultIt = alpha_ * (*(it + index_)) + beta_ * (*(it + index_ + 1));
      }
    }
  }
}; /* end struct ComputeQuantilePerComponentPolicy */

/*
 * Method computeQuantilePerComponent() gives the quantile per component of the sample
 */
Field ProcessSampleImplementation::computeQuantilePerComponent(const Scalar prob) const
{
  const UnsignedInteger size = getSize();
  if (size == 0) return Field();
  if (size == 1) return getField(0);
  const UnsignedInteger dimension = dimension_;
  const UnsignedInteger length = verticesNumber_;
  const UnsignedInteger sampleSize = dimension * length;

  // Store and transpose values in a contiguous buffer
  Point contiguous(size * sampleSize);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const Scalar * data_begin = flatData_.data() + blockOffset(k);
    for (UnsignedInteger ij = 0; ij < sampleSize; ++ij)
      contiguous[ij * size + k] = *(data_begin + ij);
  }

  // Special case for extremum cases
  const Scalar scalarIndex = prob * size - 0.5;
  UnsignedInteger index = static_cast<UnsignedInteger>( floor( scalarIndex) );
  Scalar beta = scalarIndex - index;
  if (scalarIndex >= size - 1)
  {
    beta = 0.0;
    index = size - 1;
  }
  else if (scalarIndex <= 0.0)
  {
    beta = 0.0;
    // Ensure that index does not overflow
    index = 0;
  }

  SampleImplementation result(length, dimension);
  const ComputeQuantilePerComponentPolicy policy( contiguous, result, size, index, beta);
  TBBImplementation::ParallelFor( 0, sampleSize, policy );
  return Field(mesh_, result);
}

ProcessSampleImplementation ProcessSampleImplementation::computeQuantilePerComponent(const Point & prob) const
{
  const UnsignedInteger size = getSize();
  if (size == 0) return ProcessSampleImplementation();
  if (size == 1) return *this;

  // Check that prob is inside bounds
  const UnsignedInteger probSize = prob.getSize();
  for (UnsignedInteger p = 0; p < probSize; ++p)
    if (!(prob[p] >= 0.0) || !(prob[p] <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";

  const UnsignedInteger dimension = dimension_;
  const UnsignedInteger length = verticesNumber_;
  const UnsignedInteger sampleSize = dimension * length;

  // Store and transpose values in a contiguous buffer
  Point contiguous(size * sampleSize);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const Scalar * data_begin = flatData_.data() + blockOffset(k);
    for (UnsignedInteger ij = 0; ij < sampleSize; ++ij)
      contiguous[ij * size + k] = *(data_begin + ij);
  }

  SampleImplementation quantile(probSize, dimension);
  ProcessSampleImplementation result(mesh_, 0, dimension);
  SampleImplementation output(length, dimension);
  output.setDescription(Description::BuildDefault(dimension, "q"));
  for (UnsignedInteger p = 0; p < probSize; ++p)
  {
    const Scalar scalarIndex = prob[p] * size - 0.5;
    UnsignedInteger index = static_cast<UnsignedInteger>( floor( scalarIndex) );
    Scalar beta = scalarIndex - index;
    // Special case for extremum cases
    if (scalarIndex >= size - 1)
    {
      beta = 0.0;
      index = size - 1;
    }
    else if (scalarIndex <= 0.0)
    {
      beta = 0.0;
      // Ensure that index does not overflow
      index = 0;
    }
    const ComputeQuantilePerComponentPolicy policy( contiguous, output, size, index, beta);
    TBBImplementation::ParallelFor( 0, sampleSize, policy );
    result.add(output);
  }
  return result;
}

/* Extract the sample of values at the given vertex index */
Sample ProcessSampleImplementation::getSampleAtVertex(const UnsignedInteger index) const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  if (index >= verticesNumber)
    throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than vertices number (" << verticesNumber << ")";
  const UnsignedInteger size = getSize();
  Sample result(size, dimension_);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const Scalar * block = flatData_.data() + blockOffset(k) + index * dimension_;
    for (UnsignedInteger d = 0; d < dimension_; ++d)
      result.getImplementation()->data_begin()[k * dimension_ + d] = block[d];
  }
  return result;
}

/* Get the i-th marginal process sample */
ProcessSampleImplementation ProcessSampleImplementation::getMarginal(const UnsignedInteger index) const
{
  if (!(index < dimension_)) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than dimension (" << dimension_ << ")";
  ProcessSampleImplementation result(mesh_, getSize(), 1);
  for (UnsignedInteger k = 0; k < getSize(); ++k)
  {
    const Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      result.flatData_[k * verticesNumber_ + v] = block[v * dimension_ + index];
  }
  return result;
}

/* Get the marginal field corresponding to indices dimensions */
ProcessSampleImplementation ProcessSampleImplementation::getMarginal(const Indices & indices) const
{
  const UnsignedInteger marginalDimension = indices.getSize();
  ProcessSampleImplementation result(mesh_, getSize(), marginalDimension);
  for (UnsignedInteger k = 0; k < getSize(); ++k)
  {
    const Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      for (UnsignedInteger j = 0; j < marginalDimension; ++j)
        result.flatData_[(k * verticesNumber_ + v) * marginalDimension + j] = block[v * dimension_ + indices[j]];
  }
  return result;
}

/* Draw a marginal */
Graph ProcessSampleImplementation::drawMarginal(const UnsignedInteger index,
    const Bool interpolate) const
{
  if (mesh_.getDimension() != 1) throw NotDefinedException(HERE) << "Error: cannot draw a marginal sample if the mesh is of dimension greater than one. Here dimension=" << mesh_.getDimension();
  if (!(index < getDimension())) throw InvalidArgumentException(HERE) << "Error : indice should be in {0,...," << getDimension() - 1 << "}";

  // Discretization of the x axis
  const String title(OSS() << getName() << " - " << index << " marginal" );
  Graph graph(title, "Time", "Values");
  graph.setLegendPosition("topright");
  const UnsignedInteger size = getSize();
  const Description colors(Drawable::BuildDefaultPalette(size));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Drawable drawable(getField(i).drawMarginal(index, interpolate).getDrawable(0));
    drawable.setColor(colors[i]);
    graph.add(drawable);
  }
  return graph;
}

/* Draw all marginals */
GridLayout ProcessSampleImplementation::draw(const Bool interpolate) const
{
  const UnsignedInteger outputDimension = getDimension();
  GridLayout grid(outputDimension, 1);
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
  {
    const Graph graph(drawMarginal(i, interpolate));
    grid.setGraph(i, 0, graph);
  }
  return grid;
}


class ProcessSampleCorrelationEvaluation : public EvaluationImplementation
{
public:
  ProcessSampleCorrelationEvaluation(const CovarianceModel & covarianceModel, const UnsignedInteger i, const UnsignedInteger j)
    : covarianceModel_(covarianceModel), i_(i), j_(j) {}

  ProcessSampleCorrelationEvaluation * clone() const override
  {
    return new ProcessSampleCorrelationEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 2;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator() (const Point & inP) const override
  {
    const Scalar s = inP[0];
    const Scalar t = inP[1];
    const Scalar covST = covarianceModel_(s, t).operator()(i_, j_);
    const Scalar covSS = covarianceModel_(s, s).operator()(i_, j_);
    const Scalar covTT = covarianceModel_(t, t).operator()(i_, j_);
    Point result(1);
    const Scalar den = std::sqrt(std::max(0.0, covSS * covTT));
    if (den > 0.0)
      result[0] = covST / den;
    return result;
  }

private:
  CovarianceModel covarianceModel_;
  UnsignedInteger i_ = 0;
  UnsignedInteger j_ = 0;
};


/* Draw correlation between 2 marginals */
Graph ProcessSampleImplementation::drawMarginalCorrelation(const UnsignedInteger i,
    const UnsignedInteger j) const
{
  if (getMesh().getDimension() != 1)
    throw InvalidArgumentException(HERE) << "drawMarginalCorrelation only supports 1-d domains";
  const UnsignedInteger dimension = getDimension();
  if (!(i < dimension) || !(j < dimension))
    throw InvalidArgumentException(HERE) << "Invalid indices: (" << i << ", " << j << "), dimension is " << dimension;
  ProcessSample processSampleCentered(*this);
  processSampleCentered -= computeMean().getValues();
  const CovarianceModel covariance(NonStationaryCovarianceModelFactory().build(processSampleCentered, true));
  const Function correlationFunction(new ProcessSampleCorrelationEvaluation(covariance, i, j));
  const Point dateMin(2, getMesh().getLowerBound()[0]);
  const Point dateMax(2, getMesh().getUpperBound()[0]);
  Graph graph(correlationFunction.draw(dateMin, dateMax));
  graph.setLegendPosition("bottomright");
  graph.setXTitle("s");
  graph.setYTitle("t");
  graph.setTitle(OSS() << "Empirical correlation of marginals " << i << ", " << j);
  return graph;
}


/* Draw correlation between all marginals */
GridLayout ProcessSampleImplementation::drawCorrelation() const
{
  const UnsignedInteger outputDimension = getDimension();
  GridLayout grid(outputDimension, outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
    for (UnsignedInteger j = 0; j < outputDimension; ++ j)
    {
      Graph graph(drawMarginalCorrelation(i, j));
      graph.setTitle("");
      graph.setXTitle((i == outputDimension - 1 ) ? OSS() << "marginal " << j << ", s" : OSS() << "");
      graph.setYTitle((j == 0) ? OSS() << "marginal " << i << ", t" : OSS() << "");
      grid.setGraph(i, j, graph);
    }
  grid.setTitle("Empirical correlation of marginals");
  return grid;
}

/* Method save() stores the object through the StorageManager */
void ProcessSampleImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "mesh_", mesh_);
  adv.saveAttribute( "size_", size_ );
  adv.saveAttribute( "dimension_", dimension_ );
  adv.saveAttribute("verticesNumber_", verticesNumber_);
  adv.saveAttribute( "description_", description_ );
  // Flat storage: a total-size attribute followed by the values
  adv.saveAttribute("size", flatData_.getSize());
  AdvocateIterator<Scalar> adv_it(adv);
  const Scalar * pData = flatData_.data();
  const UnsignedInteger totalSize = flatData_.getSize();
  for (UnsignedInteger i = 0; i < totalSize; ++i, ++adv_it)
  {
    *adv_it = pData[i];
  }
}

/* Method load() reloads the object from the StorageManager */
void ProcessSampleImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "mesh_", mesh_);

  if (adv.hasAttribute("data_"))
  {
    // Legacy format: the realizations were stored as a collection of samples
    PersistentCollection<Sample> legacyData;
    adv.loadAttribute( "data_", legacyData );
    size_ = legacyData.getSize();
    dimension_ = (size_ > 0) ? legacyData[0].getDimension() : 0;
    verticesNumber_ = mesh_.getVerticesNumber();
    description_ = (size_ > 0) ? legacyData[0].getDescription() : Description();
    flatData_.resize(size_ * verticesNumber_ * dimension_);
    for (UnsignedInteger k = 0; k < size_; ++k)
      std::copy(legacyData[k].getImplementation()->data_begin(), legacyData[k].getImplementation()->data_end(), flatData_.data() + blockOffset(k));
  }
  else
  {
    adv.loadAttribute( "size_", size_ );
    adv.loadAttribute( "dimension_", dimension_ );
    adv.loadAttribute("verticesNumber_", verticesNumber_);
    adv.loadAttribute( "description_", description_ );
    UnsignedInteger size = 0;
    adv.loadAttribute("size", size);
    flatData_.resize(size);
    AdvocateIterator<Scalar> adv_it(adv);
    Scalar * pData = flatData_.data();
    for (UnsignedInteger i = 0; i < size; ++i, ++adv_it)
    {
      pData[i] = adv_it();
    }
  }
}

/* Comparison function */
Bool ProcessSampleImplementation::operator ==(const ProcessSampleImplementation & other) const
{
  if (this == &other) return true;
  return (mesh_ == other.mesh_) && (dimension_ == other.dimension_) && (size_ == other.size_)
         && std::equal(flatData_.data(), flatData_.data() + flatData_.getSize(), other.flatData_.data());
}

/* In place sum operator between process sample and sample */
ProcessSampleImplementation & ProcessSampleImplementation::operator += (const Sample & translation)
{
  for (UnsignedInteger k = 0; k < getSize(); ++k)
  {
    Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      for (UnsignedInteger d = 0; d < dimension_; ++d)
        block[v * dimension_ + d] += translation(v, d);
  }
  return *this;
}

/* In place difference operator between process sample and sample */
ProcessSampleImplementation & ProcessSampleImplementation::operator -= (const Sample & translation)
{
  for (UnsignedInteger k = 0; k < getSize(); ++k)
  {
    Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      for (UnsignedInteger d = 0; d < dimension_; ++d)
        block[v * dimension_ + d] -= translation(v, d);
  }
  return *this;
}

/* In place sum operator between process sample and point */
ProcessSampleImplementation & ProcessSampleImplementation::operator += (const Point & translation)
{
  for (UnsignedInteger k = 0; k < getSize(); ++k)
  {
    Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      for (UnsignedInteger d = 0; d < dimension_; ++d)
        block[v * dimension_ + d] += translation[d];
  }
  return *this;
}

/* In place difference operator between process sample and point */
ProcessSampleImplementation & ProcessSampleImplementation::operator -= (const Point & translation)
{
  for (UnsignedInteger k = 0; k < getSize(); ++k)
  {
    Scalar * block = flatData_.data() + blockOffset(k);
    for (UnsignedInteger v = 0; v < verticesNumber_; ++v)
      for (UnsignedInteger d = 0; d < dimension_; ++d)
        block[v * dimension_ + d] -= translation[d];
  }
  return *this;
}

/* In place sum operator between process sample and process sample */
ProcessSampleImplementation & ProcessSampleImplementation::operator += (const ProcessSampleImplementation & translation)
{
  for (UnsignedInteger i = 0; i < flatData_.getSize(); ++i) flatData_[i] += translation.flatData_[i];
  return *this;
}

/* In place difference operator between process sample and process sample */
ProcessSampleImplementation & ProcessSampleImplementation::operator -= (const ProcessSampleImplementation & translation)
{
  for (UnsignedInteger i = 0; i < flatData_.getSize(); ++i) flatData_[i] -= translation.flatData_[i];
  return *this;
}

/* Sum operator between process sample and sample */
ProcessSampleImplementation ProcessSampleImplementation::operator + (const Sample & translation) const
{
  ProcessSampleImplementation processSample(*this);
  processSample += translation;
  processSample.setName("");
  return processSample;
}

/* Difference operator between process sample and sample */
ProcessSampleImplementation ProcessSampleImplementation::operator - (const Sample & translation) const
{
  ProcessSampleImplementation processSample(*this);
  processSample -= translation;
  processSample.setName("");
  return processSample;
}

/* Sum operator between process sample and point */
ProcessSampleImplementation ProcessSampleImplementation::operator + (const Point & translation) const
{
  ProcessSampleImplementation processSample(*this);
  processSample += translation;
  processSample.setName("");
  return processSample;
}

/* Difference operator between process sample and point */
ProcessSampleImplementation ProcessSampleImplementation::operator - (const Point & translation) const
{
  ProcessSampleImplementation processSample(*this);
  processSample -= translation;
  processSample.setName("");
  return processSample;
}

/* Sum operator between process sample and process sample */
ProcessSampleImplementation ProcessSampleImplementation::operator + (const ProcessSampleImplementation & translation) const
{
  if (getMesh() != translation.getMesh()) throw InvalidArgumentException(HERE) << "Error: could not sum the two process samples, their meshes are not identical.";
  if (getDimension() != translation.getDimension()) throw InvalidArgumentException(HERE) << "Error: could not sum the two process samples, their dimensions are different.";
  if (getSize() != translation.getSize()) throw InvalidArgumentException(HERE) << "Error: could not sum the two process samples, their sizes are different.";
  ProcessSampleImplementation processSample(*this);
  for (UnsignedInteger i = 0; i < processSample.flatData_.getSize(); ++i) processSample.flatData_[i] += translation.flatData_[i];
  return processSample;
}

/* Difference operator between process sample and process sample */
ProcessSampleImplementation ProcessSampleImplementation::operator - (const ProcessSampleImplementation & translation) const
{
  if (getMesh() != translation.getMesh()) throw InvalidArgumentException(HERE) << "Error: could not sum the two process samples, their meshes are not identical.";
  if (getDimension() != translation.getDimension()) throw InvalidArgumentException(HERE) << "Error: could not sum the two process samples, their dimensions are different.";
  if (getSize() != translation.getSize()) throw InvalidArgumentException(HERE) << "Error: could not sum the two process samples, their sizes are different.";
  ProcessSampleImplementation processSample(*this);
  for (UnsignedInteger i = 0; i < processSample.flatData_.getSize(); ++i) processSample.flatData_[i] -= translation.flatData_[i];
  return processSample;
}

END_NAMESPACE_OPENTURNS
