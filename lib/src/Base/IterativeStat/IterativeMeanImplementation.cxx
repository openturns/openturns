
#include "openturns/IterativeMean.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeMeanImplementation)

static const Factory<IterativeMeanImplementation> Factory_IterativeMeanImplementation;


IterativeMeanImplementation::IterativeMeanImplementation(const UnsignedInteger size)
  : IterativeAlgorithm()
  , iteration_(0)
  , size_(size)
  , data_(size_, 0.0)
{
  // Nothing to do
}

// IterativeAlgorithm * IterativeMeanImplementation::create(const int size)
// {
//   return new IterativeMeanImplementation(size);
// }

/* Virtual constructor */
IterativeMeanImplementation * IterativeMeanImplementation::clone() const
{
  return new IterativeMeanImplementation(*this);
}

/* String converter */
String IterativeMeanImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeMeanImplementation::GetClassName()
      << " iteration=" << iteration_
      << " size=" << size_
      << " values=" << data_.__repr__();
  return oss;
}

String IterativeMeanImplementation::__str__(const String & offset) const
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

UnsignedInteger IterativeMeanImplementation::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeMeanImplementation::getSize() const
{
  return data_.getSize();
}
  
Point IterativeMeanImplementation::getValues() const
{ 
  return data_;
}

void IterativeMeanImplementation::increment(const Scalar newData)
{
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = data_[i];
    data_[i] = temp + (newData - temp)/iteration_;
  }
}

// void IterativeMeanImplementation::increment(PersistentCollection<Scalar> & newData)
// {
//   iteration_ += 1;
//   for (UnsignedInteger i = 0; i < size_; ++i)
//   {
//     Scalar temp = data_[i];
//     data_[i] = temp + (newData[i] - temp)/iteration_;
//   }
// }

void IterativeMeanImplementation::increment(const Point & newData)
{
  if (newData.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the size of the iterative mean.";
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar temp = data_[i];
    data_[i] = temp + (newData[i] - temp)/iteration_;
  }
}

void IterativeMeanImplementation::increment(const Sample & newData)
{
  if (newData.getDimension() != size_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the size of the iterative mean.";
  
  for (UnsignedInteger i = 0; i < newData.getSize(); ++i)
  {
    Point rawData = newData[i];
    iteration_ += 1;
    for (UnsignedInteger j = 0; j < size_; ++j)
    {
      Scalar temp = data_[i];
      data_[j] = temp + (rawData[j] - temp)/iteration_;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeMeanImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "data_", data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeMeanImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "iteration_", iteration_);
  adv.loadAttribute( "data_", data_);
}

void IterativeMeanImplementation::finalize()
{};

// AlgoRegister IterativeMean::reg("IterativeMean", &IterativeMean::create);

END_NAMESPACE_OPENTURNS
