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
  if (index < 0) {
    index += self->getSize();
  }
  return self->at(index);
}

void __setitem__ (SignedInteger index,
                  const Point & val)
{
  if (index < 0) {
    index += self->getSize();
  }
  self->at(index) = val;
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

} // Field

} // OT

