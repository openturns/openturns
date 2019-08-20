
#include "openturns/IterativeMean.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeMean.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeMean)

static const Factory<IterativeMean> Factory_IterativeMean;

IterativeMean::IterativeMean(const UnsignedInteger size)
  : IterativeAlgorithmImplementation()
  , iteration_(0)
  , size_(size)
  , data_(size_, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeMean * IterativeMean::clone() const
{
  return new IterativeMean(*this);
}

// IterativeAlgorithm * IterativeMean::create(const int size)
// {
//   return new IterativeMean(size);
// }

/* String converter */
String IterativeMean::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeMean::GetClassName()
      << " iteration=" << iteration_
      << " size=" << size_
      << " values=" << data_.__repr__();
  return oss;
}

String IterativeMean::__str__(const String & offset) const
{
  return data_.__str__(offset);
}

// // Sample & IterativeMean::operator[] (const UnsignedInteger index)
// // {
// //   if (index >= data_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
// //   return data_[index];
// // }
// // 
// const Sample & IterativeMean::operator[] (const UnsignedInteger index) const
// {
//   if (index >= data_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
//   return data_[index];
// }

UnsignedInteger IterativeMean::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeMean::getSize() const
{
  return data_.getSize();
}
  
Point IterativeMean::getMean() const
{ 
  return data_;
}

void IterativeMean::increment(const Scalar newData)
{
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = data_[i];
    data_[i] = temp + (newData - temp)/iteration_;
  }
}

// void IterativeMean::increment(PersistentCollection<Scalar> & newData)
// {
//   iteration_ += 1;
//   for (UnsignedInteger i = 0; i < size_; ++i)
//   {
//     Scalar temp = data_[i];
//     data_[i] = temp + (newData[i] - temp)/iteration_;
//   }
// }

void IterativeMean::increment(const Point & newData)
{
  if (newData.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the size of the iterative mean.";
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = data_[i];
    data_[i] = temp + (newData[i] - temp)/iteration_;
  }
}

void IterativeMean::increment(const Sample & newData)
{
  if (newData.getDimension() != size_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the size of the iterative mean.";
  
  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point rawData = newData[j];
    iteration_ += 1;
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      Scalar temp = data_[i];
      data_[i] = temp + (rawData[i] - temp)/iteration_;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeMean::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "data_", data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeMean::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "iteration_", iteration_);
  adv.loadAttribute( "data_", data_);
}

void IterativeMean::finalize()
{
  // Nothing to do
};

// AlgoRegister IterativeMean::reg("IterativeMean", &IterativeMean::create);

END_NAMESPACE_OPENTURNS
