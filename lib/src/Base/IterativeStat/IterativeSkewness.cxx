 

#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeSkewness.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeSkewness)

static const Factory<IterativeSkewness> Factory_IterativeSkewness;

IterativeSkewness::IterativeSkewness(const UnsignedInteger size)
  : IterativeAlgorithmImplementation()
  , iteration_(0)
  , size_(size)
  , mean1Data_(size_, 0.0)
  , mean2Data_(size_, 0.0)
  , mean3Data_(size_, 0.0)
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
      << " size=" << size_
      << " mean1 values=" << mean1Data_.__repr__()
      << " mean2 values=" << mean2Data_.__repr__()
      << " mean3 values=" << mean3Data_.__repr__();
  return oss;
}

String IterativeSkewness::__str__(const String & offset) const
{
  return getSkewness().__str__(offset);
}

// // Sample & IterativeSkewness::operator[] (const UnsignedInteger index)
// // {
// //   if (index >= varData_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << varData_.getSize() - 1;
// //   return varData_[index];
// // }
// // 
// const Sample & IterativeSkewness::operator[] (const UnsignedInteger index) const
// {
//   if (index >= varData_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << varData_.getSize() - 1;
//   return varData_[index];
// }

UnsignedInteger IterativeSkewness::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeSkewness::getSize() const
{
  return size_;
}
  
Point IterativeSkewness::getSkewness() const
{ 
  PersistentCollection<Scalar> skewnessData(size_, 0.0);
  
  if (iteration_ > 0)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      skewnessData[i] = mean3Data_[i] - 3 * mean1Data_[i] * mean2Data_[i] + 2 * pow(mean1Data_[i], 3) / pow(mean2Data_[i] - pow(mean1Data_[i], 2), 1.5);
    }
  }
  else
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      skewnessData[i] = 0;
    }
  }
  
  return skewnessData;
}
  
Point IterativeSkewness::getVariance() const
{ 
  PersistentCollection<Scalar> varData(size_, 0.0);
  
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    varData[i] = mean2Data_[i] - pow(mean1Data_[i], 2);
  }
  
  return varData;
}
  
Point IterativeSkewness::getMean() const
{   
  return mean1Data_;
}

void IterativeSkewness::increment(const Scalar newData)
{
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = mean1Data_[i];
    mean1Data_[i] = temp + (pow(newData, 1) - temp)/iteration_;
  }
  
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = mean2Data_[i];
    mean2Data_[i] = temp + (pow(newData, 2) - temp)/iteration_;
  }
  
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = mean3Data_[i];
    mean3Data_[i] = temp + (pow(newData, 3) - temp)/iteration_;
  }
}

void IterativeSkewness::increment(const Point & newData)
{
  if (newData.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the size of the iterative skewness.";
  
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = mean1Data_[i];
    mean1Data_[i] = temp + (pow(newData[i], 1) - temp)/iteration_;
  }
  
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = mean2Data_[i];
    mean2Data_[i] = temp + (pow(newData[i], 2) - temp)/iteration_;
  }
  
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = mean3Data_[i];
    mean3Data_[i] = temp + (pow(newData[i], 3) - temp)/iteration_;
  }
}

void IterativeSkewness::increment(const Sample & newData)
{
  if (newData.getDimension() != size_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the size of the iterative skewness.";
  
  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point newDataJ = newData[j];
    
    iteration_ += 1;
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      Scalar temp = mean1Data_[i];
      mean1Data_[i] = temp + (pow(newDataJ[i], 1) - temp)/iteration_;
    }
    
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      Scalar temp = mean2Data_[i];
      mean2Data_[i] = temp + (pow(newDataJ[i], 2) - temp)/iteration_;
    }
    
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      Scalar temp = mean3Data_[i];
      mean3Data_[i] = temp + (pow(newDataJ[i], 3) - temp)/iteration_;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeSkewness::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "mean1Data_", mean1Data_);
  adv.saveAttribute( "mean2Data_", mean2Data_);
  adv.saveAttribute( "mean3Data_", mean3Data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeSkewness::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "iteration_", iteration_);
  adv.loadAttribute( "mean1Data_", mean1Data_);
  adv.loadAttribute( "mean2Data_", mean2Data_);
  adv.loadAttribute( "mean3Data_", mean3Data_);
}

void IterativeSkewness::finalize()
{
  // Nothing to do
};

END_NAMESPACE_OPENTURNS
