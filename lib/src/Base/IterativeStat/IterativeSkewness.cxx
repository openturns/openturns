

#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeSkewness.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeSkewness)

static const Factory<IterativeSkewness> Factory_IterativeSkewness;

IterativeSkewness::IterativeSkewness(const UnsignedInteger dimension)
  : IterativeAlgorithmImplementation(dimension)
  , mean1Data_(dimension_, 0.0)
  , mean2Data_(dimension_, 0.0)
  , mean3Data_(dimension_, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeSkewness * IterativeSkewness::clone() const
{
  return new IterativeSkewness(*this);
}

/* String converter */
String IterativeSkewness::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeSkewness::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " mean1 values=" << mean1Data_.__repr__()
      << " mean2 values=" << mean2Data_.__repr__()
      << " mean3 values=" << mean3Data_.__repr__();
  return oss;
}

String IterativeSkewness::__str__(const String & offset) const
{
  return getSkewness().__str__(offset);
}

Point IterativeSkewness::getSkewness() const
{
  Point skewnessData(dimension_, 0.0);

  if (iteration_ > 0)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      skewnessData[i] = (mean3Data_[i] - 3 * mean1Data_[i] * mean2Data_[i] + 2 * pow(mean1Data_[i], 3)) / pow(mean2Data_[i] - pow(mean1Data_[i], 2), 1.5);
    }
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      skewnessData[i] = 0;
    }
  }

  return skewnessData;
}

Point IterativeSkewness::getVariance() const
{
  Point varData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    varData[i] = (mean2Data_[i] - pow(mean1Data_[i], 2))*iteration_/(iteration_-1);
  }

  return varData;
}

Point IterativeSkewness::getCoeficientOfVariation() const
{
  Point coeficientOfVariationData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    coeficientOfVariationData[i] = pow(mean2Data_[i] - pow(mean1Data_[i], 2), 0.5) / mean1Data_[i];
  }

  return coeficientOfVariationData;
}

Point IterativeSkewness::getStandardDeviation() const
{
  Point standardDevData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    standardDevData[i] = pow(mean2Data_[i] - pow(mean1Data_[i], 2), 0.5);
  }

  return standardDevData;
}

Point IterativeSkewness::getMean() const
{
  return mean1Data_;
}

void IterativeSkewness::increment(const Point & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative skewness.";

  iteration_ += 1;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar temp = mean1Data_[i];
    mean1Data_[i] = temp + (newData[i] - temp)/iteration_;
  }

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar temp = mean2Data_[i];
    mean2Data_[i] = temp + (pow(newData[i], 2) - temp)/iteration_;
  }

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar temp = mean3Data_[i];
    mean3Data_[i] = temp + (pow(newData[i], 3) - temp)/iteration_;
  }
}

void IterativeSkewness::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the iterative skewness.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];

    iteration_ += 1;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      Scalar temp = mean1Data_[i];
      mean1Data_[i] = temp + (tempData[i] - temp)/iteration_;
    }

    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      Scalar temp = mean2Data_[i];
      mean2Data_[i] = temp + (pow(tempData[i], 2) - temp)/iteration_;
    }

    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      Scalar temp = mean3Data_[i];
      mean3Data_[i] = temp + (pow(tempData[i], 3) - temp)/iteration_;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeSkewness::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "mean1Data_", mean1Data_);
  adv.saveAttribute( "mean2Data_", mean2Data_);
  adv.saveAttribute( "mean3Data_", mean3Data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeSkewness::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  adv.loadAttribute( "mean1Data_", mean1Data_);
  adv.loadAttribute( "mean2Data_", mean2Data_);
  adv.loadAttribute( "mean3Data_", mean3Data_);
}

void IterativeSkewness::finalize()
{
  // Nothing to do
};

END_NAMESPACE_OPENTURNS
