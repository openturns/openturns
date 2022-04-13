//                                               -*- C++ -*-
/**
 *  @brief ProcessSampleImplementation Class
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/Drawable.hxx"
#include "openturns/NonStationaryCovarianceModelFactory.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<ProcessSampleImplementation>)

TEMPLATE_CLASSNAMEINIT(PersistentCollection<Field>)

static const Factory<PersistentCollection<Field> > Factory_PersistentCollection_Field;

CLASSNAMEINIT(ProcessSampleImplementation)

static const Factory<ProcessSampleImplementation> Factory_ProcessSampleImplementation;

ProcessSampleImplementation::ProcessSampleImplementation()
  : PersistentObject()
  , mesh_()
  , data_(0)
{
  // Nothing to do
}

/* Default constructor */
ProcessSampleImplementation::ProcessSampleImplementation(const UnsignedInteger size,
    const Field & field)
  : PersistentObject()
  , mesh_(field.getMesh())
  , data_(size, field.getValues())
{
  // Nothing to do
}

ProcessSampleImplementation::ProcessSampleImplementation(const Mesh & mesh,
    const UnsignedInteger size,
    const UnsignedInteger dimension)
  : PersistentObject()
  , mesh_(mesh)
  , data_(size, Sample(mesh.getVerticesNumber(), dimension))
{
  // Nothing to do
}

ProcessSampleImplementation::ProcessSampleImplementation(const Mesh & mesh,
    const SampleCollection & collection)
  : PersistentObject()
  , mesh_(mesh)
  , data_(collection)
{
  // Nothing to do
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
      << " values=" << data_;
  return oss;
}

String ProcessSampleImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "[";
  String separator("");
  for (UnsignedInteger i = 0; i < data_.getSize(); ++i, separator = Os::GetEndOfLine()) oss << separator << offset << "field " << i << ":" << Os::GetEndOfLine() << getField(i).__str__(offset);
  oss << "]";
  return oss;
}

void ProcessSampleImplementation::erase(const UnsignedInteger first, const UnsignedInteger last)
{
  data_.erase(data_.begin() + first, data_.begin() + last);
}

void ProcessSampleImplementation::clear()
{
  data_.clear();
}

/* Partial copy constructor */
void ProcessSampleImplementation::add(const Field & field)
{
  if (getSize() == 0)
  {
    data_.add(field.getValues());
    mesh_ = field.getMesh();
  }
  else if ((data_[0].getDimension() == field.getOutputDimension()) && (mesh_ == field.getMesh())) data_.add(field.getValues());
  else throw InvalidArgumentException(HERE) << "Error: could not add the field. Either its dimenson or its mesh are incompatible.";
}

void ProcessSampleImplementation::add(const Sample & values)
{
  if (values.getSize() != mesh_.getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: could not add the values. Their size=" << values.getSize() << " does not match the number of vertices=" << mesh_.getVerticesNumber() << " of the mesh.";
  if (!(getSize() == 0 || data_[0].getDimension() == values.getDimension())) throw InvalidArgumentException(HERE) << "Error: could not add the values. Their dimension=" << values.getDimension() << " does not match the process sample dimension=" << data_[0].getDimension();
  data_.add(values);
}


/* Operators accessors */
Field ProcessSampleImplementation::getField(const UnsignedInteger index) const
{
  if (!(index < data_.getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
  return Field(mesh_, data_[index]);
}

void ProcessSampleImplementation::setField(const Field & field,
    const UnsignedInteger index)
{
  if (!(index < data_.getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
  if (field.getOutputDimension() != data_[0].getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a field of dimension=" << data_[0].getDimension() << ", got a field of dimension=" << field.getOutputDimension();
  data_[index] = field.getValues();
}

Sample & ProcessSampleImplementation::operator[] (const UnsignedInteger index)
{
  if (!(index < data_.getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
  return data_[index];
}

const Sample & ProcessSampleImplementation::operator[] (const UnsignedInteger index) const
{
  if (!(index < data_.getSize())) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
  return data_[index];
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
  if (data_.getSize() == 0) return 0;
  return data_[0].getDimension();
}

/* Dimension accessors */
UnsignedInteger ProcessSampleImplementation::getSize() const
{
  return data_.getSize();
}

/* Mean accessor */
Field ProcessSampleImplementation::computeMean() const
{
  const UnsignedInteger size = getSize();
  if (!(size > 0)) throw InternalException(HERE) << "Error: cannot compute the mean of an empty sample.";
  if (size == 1) return Field(mesh_, data_[0]);
  Sample meanValues(data_[0]);
  for (UnsignedInteger i = 1; i < size; ++i) meanValues += data_[i];
  meanValues *= Point(getDimension(), 1.0 / size);
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
 * Gives the centered moment of order k of the sample (by component)
 */
Field ProcessSampleImplementation::computeCenteredMoment(const UnsignedInteger k) const
{
  const UnsignedInteger verticesNumber = mesh_.getVerticesNumber();
  Sample values(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    values[i] = getSampleAtVertex(i).computeCenteredMoment(k);
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
  const UnsignedInteger size = getSize();
  const UnsignedInteger dimension = getDimension();
  Sample result(size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = data_[i].computeMean();
  return result;
}

/* Standard deviation accessor */
Field ProcessSampleImplementation::computeStandardDeviation() const
{
  const UnsignedInteger size = getSize();
  if (size == 0) return Field();
  if (size == 1) return Field(mesh_, getDimension());
  Sample meanValues(computeMean().getValues());
  const UnsignedInteger verticesNumber = getMesh().getVerticesNumber();
  const UnsignedInteger dimension = getDimension();
  Sample stdValues(verticesNumber, getDimension());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Sample slack(data_[i] - meanValues);
    for (UnsignedInteger j = 0; j < verticesNumber; ++ j)
      for (UnsignedInteger k = 0; k < dimension; ++ k)
        slack(j, k) = slack(j, k) * slack(j, k);
    stdValues += slack;
  }
  stdValues *= Point(getDimension(), 1.0 / (size - 1.0));
  for (UnsignedInteger j = 0; j < verticesNumber; ++ j)
    for (UnsignedInteger k = 0; k < dimension; ++ k)
      stdValues(j, k) = std::sqrt(stdValues(j, k));
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
  if (size == 1) return Field(mesh_, data_[0]);
  const UnsignedInteger dimension = data_[0].getDimension();
  const UnsignedInteger length = data_[0].getSize();
  const UnsignedInteger sampleSize = dimension * length;

  // Store and transpose values in a contiguous buffer
  Point contiguous(size * sampleSize);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const SampleImplementation::data_const_iterator data_begin = data_[k].getImplementation()->data_begin();
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

  const UnsignedInteger dimension = data_[0].getDimension();
  const UnsignedInteger length = data_[0].getSize();
  const UnsignedInteger sampleSize = dimension * length;

  // Store and transpose values in a contiguous buffer
  Point contiguous(size * sampleSize);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const SampleImplementation::data_const_iterator data_begin = data_[k].getImplementation()->data_begin();
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
  const UnsignedInteger size = getSize();
  const UnsignedInteger dimension = getDimension();
  Sample result(size, dimension);
  const UnsignedInteger offsetBegin = index * dimension;
  const UnsignedInteger offsetEnd = offsetBegin + dimension;
  UnsignedInteger offsetResult = 0;
  for (UnsignedInteger i = 0; i < size; ++i, offsetResult += dimension)
    std::copy(data_[i].getImplementation()->data_begin() + offsetBegin, data_[i].getImplementation()->data_begin() + offsetEnd, result.getImplementation()->data_begin() + offsetResult);
  return result;
}

/* Get the i-th marginal process sample */
ProcessSampleImplementation ProcessSampleImplementation::getMarginal(const UnsignedInteger index) const
{
  const UnsignedInteger size = data_.getSize();
  ProcessSampleImplementation result(mesh_, size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = data_[i].getMarginal(index);
  return result;
}

/* Get the marginal field corresponding to indices dimensions */
ProcessSampleImplementation ProcessSampleImplementation::getMarginal(const Indices & indices) const
{
  const UnsignedInteger size = data_.getSize();
  ProcessSampleImplementation result(mesh_, size, indices.getSize());
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = data_[i].getMarginal(indices);
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
  Graph graph(title, "Time", "Values", true, "topright");
  const UnsignedInteger size = data_.getSize();
  const Description colors(Drawable::BuildDefaultPalette(size));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Drawable drawable(Field(mesh_, data_[i]).drawMarginal(index, interpolate).getDrawable(0));
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
  adv.saveAttribute( "data_", data_ );
}

/* Method load() reloads the object from the StorageManager */
void ProcessSampleImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "mesh_", mesh_);
  adv.loadAttribute( "data_", data_ );
}


ProcessSampleImplementation & ProcessSampleImplementation::operator += (const Sample & translation)
{
  const UnsignedInteger size = getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
    data_[i] += translation;
  return *this;
}

ProcessSampleImplementation & ProcessSampleImplementation::operator -= (const Sample & translation)
{
  const UnsignedInteger size = getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
    data_[i] -= translation;
  return *this;
}

END_NAMESPACE_OPENTURNS
