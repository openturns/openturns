


#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeKurtosis.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeKurtosis)

static const Factory<IterativeKurtosis> Factory_IterativeKurtosis;

IterativeKurtosis::IterativeKurtosis(const UnsignedInteger dimension)
  : IterativeAlgorithmImplementation()
  , iteration_(0)
  , dimension_(dimension)
  , mean1Data_(dimension_, 0.0)
  , mean2Data_(dimension_, 0.0)
  , mean3Data_(dimension_, 0.0)
  , mean4Data_(dimension_, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeKurtosis * IterativeKurtosis::clone() const
{
  return new IterativeKurtosis(*this);
}

/* String converter */
String IterativeKurtosis::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeKurtosis::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " mean1 values=" << mean1Data_.__repr__()
      << " mean2 values=" << mean2Data_.__repr__()
      << " mean3 values=" << mean3Data_.__repr__()
      << " mean4 values=" << mean4Data_.__repr__();
  return oss;
}

String IterativeKurtosis::__str__(const String & offset) const
{
  return getSkewness().__str__(offset);
}

// // Sample & IterativeKurtosis::operator[] (const UnsignedInteger index)
// // {
// //   if (index >= varData_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << varData_.getSize() - 1;
// //   return varData_[index];
// // }
// //
// const Sample & IterativeKurtosis::operator[] (const UnsignedInteger index) const
// {
//   if (index >= varData_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << varData_.getSize() - 1;
//   return varData_[index];
// }

UnsignedInteger IterativeKurtosis::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeKurtosis::getSize() const
{
  return dimension_;
}

Point IterativeKurtosis::getKurtosis() const
{
  PersistentCollection<Scalar> kurtosisData(dimension_, 0.0);

  if (iteration_ > 0)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      kurtosisData[i] = mean4Data_[i] - 4 * mean1Data_[i] * mean3Data_[i] + 6 * pow(mean1Data_[i], 2) * mean2Data_[i] - 3 * pow(mean1Data_[i], 4) / (mean3Data_[i] - 3 * mean1Data_[i] * mean2Data_[i] + 2 * pow(mean1Data_[i], 3));
    }
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      kurtosisData[i] = 0;
    }
  }

  return kurtosisData;
}

Point IterativeKurtosis::getSkewness() const
{
  PersistentCollection<Scalar> skewnessData(dimension_, 0.0);

  if (iteration_ > 0)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      skewnessData[i] = mean3Data_[i] - 3 * mean1Data_[i] * mean2Data_[i] + 2 * pow(mean1Data_[i], 3) / pow(mean2Data_[i] - pow(mean1Data_[i], 2), 1.5);
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

Point IterativeKurtosis::getVariance() const
{
  PersistentCollection<Scalar> varData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    varData[i] = mean2Data_[i] - pow(mean1Data_[i], 2);
  }

  return varData;
}

Point IterativeKurtosis::getCoeficientOfVariation() const
{
  PersistentCollection<Scalar> coeficientOfVariationData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    coeficientOfVariationData[i] = pow(mean2Data_[i] - pow(mean1Data_[i], 2), 0.5) / mean1Data_[i];
  }

  return coeficientOfVariationData;
}

Point IterativeKurtosis::getStandardDeviation() const
{
  PersistentCollection<Scalar> standardDevData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    standardDevData[i] = pow(mean2Data_[i] - pow(mean1Data_[i], 2), 0.5);
  }

  return standardDevData;
}

Point IterativeKurtosis::getMean() const
{
  return mean1Data_;
}

void IterativeKurtosis::increment(const Point & newData)
{
  if (newData.getSize() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative kurtosis.";

  iteration_ += 1;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar temp = mean1Data_[i];
    mean1Data_[i] = temp + (pow(newData[i], 1) - temp)/iteration_;
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

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar temp = mean4Data_[i];
    mean4Data_[i] = temp + (pow(newData[i], 4) - temp)/iteration_;
  }
}

void IterativeKurtosis::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the iterative kurtosis.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];

    iteration_ += 1;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      Scalar temp = mean1Data_[i];
      mean1Data_[i] = temp + (pow(tempData[i], 1) - temp)/iteration_;
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

    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      Scalar temp = mean4Data_[i];
      mean4Data_[i] = temp + (pow(tempData[i], 4) - temp)/iteration_;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeKurtosis::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "dimension_", dimension_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "mean1Data_", mean1Data_);
  adv.saveAttribute( "mean2Data_", mean2Data_);
  adv.saveAttribute( "mean3Data_", mean3Data_);
  adv.saveAttribute( "mean4Data_", mean4Data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeKurtosis::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "dimension_", dimension_);
  adv.loadAttribute( "iteration_", iteration_);
  adv.loadAttribute( "mean1Data_", mean1Data_);
  adv.loadAttribute( "mean2Data_", mean2Data_);
  adv.loadAttribute( "mean3Data_", mean3Data_);
  adv.loadAttribute( "mean4Data_", mean4Data_);
}

void IterativeKurtosis::finalize()
{
  // Nothing to do
};

END_NAMESPACE_OPENTURNS
