
#include "openturns/IterativeMean.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeVariance.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeVariance)

static const Factory<IterativeVariance> Factory_IterativeVariance;

IterativeVariance::IterativeVariance(const UnsignedInteger dimension)
  : IterativeAlgorithmImplementation(dimension)
  , meanData_(dimension, 0.0)
  , varData_(dimension, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeVariance * IterativeVariance::clone() const
{
  return new IterativeVariance(*this);
}

/* String converter */
String IterativeVariance::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeVariance::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " values=" << varData_.__repr__()
      << " mean values=" << meanData_.__repr__();
  return oss;
}

String IterativeVariance::__str__(const String & offset) const
{
  return varData_.__str__(offset);
}

Point IterativeVariance::getVariance() const
{
  return varData_;
}

Point IterativeVariance::getCoeficientOfVariation() const
{
  Point coeficientOfVariationData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    coeficientOfVariationData[i] = pow(varData_[i], 0.5) / meanData_[i];
  }

  return coeficientOfVariationData;
}

Point IterativeVariance::getStandardErrorOfTheMean() const
{
  Point standardErrorOfTheMean(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    standardErrorOfTheMean[i] = pow(varData_[i], 0.5) / pow(iteration_, 0.5);
  }

  return standardErrorOfTheMean;
}

Point IterativeVariance::getStandardDeviation() const
{
  Point standardDeviationData(dimension_, 0.0);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    standardDeviationData[i] = pow(varData_[i], 0.5);
  }

  return standardDeviationData;
}

Point IterativeVariance::getMean() const
{
  return meanData_;
}

void IterativeVariance::increment(const Point & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative variance.";

  // update mean
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    meanData_[i] = meanData_[i] + (newData[i] - meanData_[i]) / iteration_;
  }
  if (iteration_ > 1)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
//     Scalar temp = meanData_[i];
//       varData_[i] = (varData_[i] * (iteration_ - 1) + (newData[i] - temp) * (newData[i] - meanData_[i])) / iteration_;
      varData_[i] = varData_[i] * (iteration_ - 2);
      varData_[i] = varData_[i] + iteration_ * pow((newData[i] - meanData_[i]), 2) / (iteration_ - 1);
      varData_[i] = varData_[i] / (iteration_ - 1);
    }
  }
}

void IterativeVariance::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the iterative variance.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];
    iteration_ += 1;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      meanData_[i] = meanData_[i] + (tempData[i] - meanData_[i]) / iteration_;
    }
    if (iteration_ > 1)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        varData_[i] = varData_[i] * (iteration_ - 2);
        varData_[i] = varData_[i] + iteration_ * pow((tempData[i] - meanData_[i]), 2) / (iteration_ - 1);
        varData_[i] = varData_[i] / (iteration_ - 1);
      }
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeVariance::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "meanData_", meanData_);
  adv.saveAttribute( "varData_", varData_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeVariance::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  adv.loadAttribute( "meanData_", meanData_);
  adv.loadAttribute( "varData_", varData_);
}

void IterativeVariance::finalize()
{
  // Nothing to do
};

END_NAMESPACE_OPENTURNS
