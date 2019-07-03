
#include "openturns/IterativeMean.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeMean)

IterativeMean::IterativeMean(const UnsignedInteger size)
  :  TypedInterfaceObject<IterativeMeanImplementation>(new IterativeMeanImplementation(size))
{
  // Nothing to do
}

/* Parameters constructor */
IterativeMean::IterativeMean(const IterativeMeanImplementation & implementation)
  : TypedInterfaceObject<IterativeMeanImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
IterativeMean::IterativeMean(const Pointer<IterativeMeanImplementation> & p_implementation)
  : TypedInterfaceObject<IterativeMeanImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
IterativeMean::IterativeMean(IterativeMeanImplementation * p_implementation)
  : TypedInterfaceObject<IterativeMeanImplementation>(p_implementation)
{
  // Nothing to do
}


// IterativeAlgorithm * IterativeMean::create(const int size)
// {
//   return new IterativeMean(size);
// }

/* String converter */
String IterativeMean::__repr__() const
{
  return getImplementation()->__repr__();
}

String IterativeMean::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
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
  return getImplementation()->getIteration();
}

UnsignedInteger IterativeMean::getSize() const
{
  return getImplementation()->getSize();
}
  
Point IterativeMean::getValues() const
{ 
  return getImplementation()->getValues();
}

void IterativeMean::increment(const Scalar newData)
{
  getImplementation()->increment(newData);
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
  getImplementation()->increment(newData);
}

void IterativeMean::increment(const Sample & newData)
{
  getImplementation()->increment(newData);
}

void IterativeMean::finalize()
{
  getImplementation()->finalize();
};

// AlgoRegister IterativeMean::reg("IterativeMean", &IterativeMean::create);

END_NAMESPACE_OPENTURNS
