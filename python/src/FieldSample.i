// SWIG file ProcessSamples.i

%{
#include "openturns/FieldSample.hxx"
%}

%include FieldSample_doc.i

%include openturns/FieldSample.hxx


namespace OT {
%extend FieldSample {

FieldSample(const FieldSample & other)
{
  return new OT::FieldSample(other);
}

Field __getitem__ (SignedInteger index) const
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  return self->getField(index);
}

void __setitem__(SignedInteger index,
                 const Field & field)
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  self->setField(field, index);
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

} // FieldSample

} // OT

