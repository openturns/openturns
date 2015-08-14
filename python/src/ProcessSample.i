// SWIG file ProcessSamples.i

%{
#include "ProcessSample.hxx"
%}

%include ProcessSample.hxx


namespace OT {
%extend ProcessSample {

ProcessSample(const ProcessSample & other)
{
  return new OT::ProcessSample(other);
}

Field __getitem__ (SignedInteger index) const
{
  if (index < 0) {
    index += self->getSize();
  }
  return self->getField(index);
}

void __setitem__(SignedInteger index,
                 const Field & field)
{
  if (index < 0) {
    index += self->getSize();
  }
  self->setField(field, index);
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

} // ProcessSample

} // OT

