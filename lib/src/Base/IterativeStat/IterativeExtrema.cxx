
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

IterativeExtrema::IterativeExtrema(const UnsignedInteger size)
  : IterativeAlgorithmImplementation()
  , iteration_(0)
  , size_(size)
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

// IterativeAlgorithm * IterativeExtrema::create(const int size)
// {
//   return new IterativeExtrema(size);
// }

/* String converter */
String IterativeExtrema::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeExtrema::GetClassName()
      << " iteration=" << iteration_
      << " size=" << size_
      << " min=" << minData_.__repr__()
      << " max=" << maxData_.__repr__();
  return oss;
}

String IterativeExtrema::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    oss << separator << "(min = " << minData_[i] << ", max = " << maxData_[i] << ")";
    separator = ", ";
  }
  return oss;
}

// // Sample & IterativeExtrema::operator[] (const UnsignedInteger index)
// // {
// //   if (index >= data_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
// //   return data_[index];
// // }
// //
// const Sample & IterativeExtrema::operator[] (const UnsignedInteger index) const
// {
//   if (index >= data_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
//   return data_[index];
// }

UnsignedInteger IterativeExtrema::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeExtrema::getSize() const
{
  return size_;
}

Point IterativeExtrema::getMin() const
{
  return minData_;
}

Point IterativeExtrema::getMax() const
{
  return maxData_;
}

void IterativeExtrema::increment(const Scalar newData)
{
  iteration_ += 1;
  if (iteration_ > 1)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      if (newData > maxData_[i])
      {
        maxData_[i] = newData;
      }
      if (newData < minData_[i])
      {
        minData_[i] = newData;
      }
    }
  }
  else
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      maxData_[i] = newData;
      minData_[i] = newData;
    }
  }
}

void IterativeExtrema::increment(const Point & newData)
{
  if (newData.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the size of the iterative extrema.";
  iteration_ += 1;
  if (iteration_ > 1)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
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
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      maxData_[i] = newData[i];
      minData_[i] = newData[i];
    }
  }
}

void IterativeExtrema::increment(const Sample & newData)
{
  if (newData.getDimension() != size_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the size of the extrema.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];
    iteration_ += 1;
    if (iteration_ > 1)
    {
      for (UnsignedInteger i = 0; i < size_; ++i)
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
      for (UnsignedInteger i = 0; i < size_; ++i)
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
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "minData_", minData_);
  adv.saveAttribute( "maxData_", maxData_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeExtrema::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
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
