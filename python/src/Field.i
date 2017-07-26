// SWIG file Field.i

%{
#include "openturns/Field.hxx"
%}

%include Field_doc.i

OTTypedInterfaceObjectHelper(Field)

%include openturns/Field.hxx

namespace OT {
%extend Field {

Field(const Field & other)
{
  return new OT::Field(other);
}

Point __getitem__ (SignedInteger index) const
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  return self->at(index);
}

void __setitem__ (SignedInteger index,
                  const Point & val)
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  self->at(index) = val;
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

} // Field

} // OT

