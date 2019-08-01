
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

IterativeVariance::IterativeVariance(const UnsignedInteger size)
  : IterativeAlgorithmImplementation()
  , iteration_(0)
  , size_(size)
  , meanData_(size_, 0.0)
  , varData_(size_, 0.0)
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
      << " size=" << size_
      << " values=" << varData_.__repr__()
      << " mean values=" << meanData_.__repr__();
  return oss;
}

String IterativeVariance::__str__(const String & offset) const
{
  return varData_.__str__(offset);
}

// // Sample & IterativeVariance::operator[] (const UnsignedInteger index)
// // {
// //   if (index >= varData_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << varData_.getSize() - 1;
// //   return varData_[index];
// // }
// // 
// const Sample & IterativeVariance::operator[] (const UnsignedInteger index) const
// {
//   if (index >= varData_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << varData_.getSize() - 1;
//   return varData_[index];
// }

UnsignedInteger IterativeVariance::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeVariance::getSize() const
{
  return varData_.getSize();
}
  
Point IterativeVariance::getVariance() const
{ 
  return varData_;
}
  
Point IterativeVariance::getMean() const
{ 
  return meanData_;
}

void IterativeVariance::increment(const Scalar newData)
{  
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = meanData_[i];
    meanData_[i] = temp + (newData - temp) / (iteration_ + 1);
    if (iteration_ > 0)
    {
      varData_[i] = (varData_[i] * (iteration_ - 1) + (newData - temp) * (newData - meanData_[i])) / iteration_;
    }
  }
  iteration_ += 1;
}

void IterativeVariance::increment(const Point & newData)
{
  if (newData.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the size of the iterative variance.";

  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = meanData_[i];
    meanData_[i] = temp + (newData[i] - temp) / (iteration_ + 1);
    if (iteration_ > 0)
    {
      varData_[i] = (varData_[i] * (iteration_ - 1) + (newData[i] - temp) * (newData[i] - meanData_[i])) / iteration_;
    }
  }
  iteration_ += 1;
}

void IterativeVariance::increment(const Sample & newData)
{
  if (newData.getDimension() != size_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the size of the iterative variance.";
  
  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point rawData = newData[j];
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      Scalar temp = meanData_[i];
      meanData_[i] = temp + (rawData[i] - temp) / (iteration_ + 1);
      if (iteration_ > 0)
      {
        varData_[i] = (varData_[i] * (iteration_ - 1) + (rawData[i] - temp) * (rawData[i] - meanData_[i])) / iteration_;
      }
    }
    iteration_ += 1;
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeVariance::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "meanData_", meanData_);
  adv.saveAttribute( "varData_", varData_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeVariance::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "iteration_", iteration_);
  adv.loadAttribute( "meanData_", meanData_);
  adv.loadAttribute( "varData_", varData_);
}

void IterativeVariance::finalize()
{
  // Nothing to do
};

END_NAMESPACE_OPENTURNS
