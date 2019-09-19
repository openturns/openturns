
#include "openturns/IterativeExtrema.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeExtrema.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeExtrema)

static const Factory<IterativeExtrema> Factory_IterativeExtrema;

IterativeExtrema::IterativeExtrema(const UnsignedInteger dimension)
  : IterativeAlgorithmImplementation()
  , iteration_(0)
  , dimension_(dimension)
  , minData_(0)
  , maxData_(0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeExtrema * IterativeExtrema::clone() const
{
  return new IterativeExtrema(*this);
}

// IterativeAlgorithm * IterativeExtrema::create(const int dimension)
// {
//   return new IterativeExtrema(dimension);
// }

/* String converter */
String IterativeExtrema::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeExtrema::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " min=" << minData_.__repr__()
      << " max=" << maxData_.__repr__();
  return oss;
}

String IterativeExtrema::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    oss << separator << "(min = " << minData_[i] << ", max = " << maxData_[i] << ")";
    separator = ", ";
  }
  return oss;
}

UnsignedInteger IterativeExtrema::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeExtrema::getSize() const
{
  return dimension_;
}

Point IterativeExtrema::getMin() const
{
  return minData_;
}

Point IterativeExtrema::getMax() const
{
  return maxData_;
}

void IterativeExtrema::increment(const Point & newData)
{
  if (newData.getSize() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative extrema.";
  iteration_ += 1;
  if (iteration_ > 1)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      if (newData[i] > maxData_[i])
      {
        maxData_[i] = newData[i];
      }
      if (newData[i] < minData_[i])
      {
        minData_[i] = newData[i];
      }
    }
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      maxData_[i] = newData[i];
      minData_[i] = newData[i];
    }
  }
}

void IterativeExtrema::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the extrema.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];
    iteration_ += 1;
    if (iteration_ > 1)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        if (tempData[i] > maxData_[i])
        {
          maxData_[i] = tempData[i];
        }
        if (tempData[i] < minData_[i])
        {
          minData_[i] = tempData[i];
        }
      }
    }
    else
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        maxData_[i] = tempData[i];
        minData_[i] = tempData[i];
      }
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeExtrema::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "dimension_", dimension_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "minData_", minData_);
  adv.saveAttribute( "maxData_", maxData_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeExtrema::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "dimension_", dimension_);
  adv.loadAttribute( "iteration_", iteration_);
  adv.loadAttribute( "minData_", minData_);
  adv.loadAttribute( "maxData_", maxData_);
}

void IterativeExtrema::finalize()
{
  // Nothing to do
};

// AlgoRegister IterativeExtrema::reg("IterativeExtrema", &IterativeExtrema::create);

END_NAMESPACE_OPENTURNS
