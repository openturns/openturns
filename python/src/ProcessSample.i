// SWIG file ProcessSamples.i

%{
#include "openturns/ProcessSample.hxx"
%}

%include ProcessSample_doc.i

OTTypedInterfaceObjectHelper(ProcessSample)

%include openturns/ProcessSample.hxx


namespace OT {
%extend ProcessSample {

ProcessSample(const ProcessSample & other)
{
  return new OT::ProcessSample(other);
}

Sample __getitem__ (SignedInteger index) const
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  return self->operator[](index);
}

void __setitem__(SignedInteger index,
                 const Sample & values)
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  self->operator[](index) = values;
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

} // ProcessSample

} // OT

