// SWIG file IndicesFixedSizeCollection.i

%{
#include "openturns/IndicesFixedSizeCollection.hxx"
%}

OTTypedInterfaceObjectHelper(IndicesFixedSizeCollection)

%typemap(in) const IndicesFixedSizeCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_, OT::IndicesFixedSizeCollection>( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to an IndicesFixedSizeCollection");
    }
  }
}

%typemap(typecheck,precedence=5) const IndicesFixedSizeCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) ||
       OT::isAPythonBufferOf<OT::UnsignedInteger, 2>($input) || OT::isAPythonSequenceOf<OT::_PySequence_>( $input );
}

%apply const IndicesFixedSizeCollection & { const OT::IndicesFixedSizeCollection & };

%include openturns/IndicesFixedSizeCollection.hxx

namespace OT {
%extend IndicesFixedSizeCollection {

Indices __getitem__(SignedInteger index) const {
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  return OT::Indices(self->cbegin_at(index), self->cend_at(index));
}

void __setitem__ (SignedInteger index,
                  const Indices & val) {
  OT::UnsignedInteger size = self->getSize();
  OT::UnsignedInteger thisSize = self->cend_at(index) - self->cbegin_at(index);
  if (val.getSize() != thisSize)
    throw OT::InvalidArgumentException(HERE) << "element at position " << index << " is of size " << thisSize << " whereas value size is " << val.getSize();
  std::copy(val.begin(), val.end(), self->begin_at(index));
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

IndicesFixedSizeCollection(const IndicesFixedSizeCollection & other)
{
  return new OT::IndicesFixedSizeCollection(other);
}

IndicesFixedSizeCollection(PyObject * pyObj)
{
  return new OT::IndicesFixedSizeCollection( OT::convert< OT::_PySequence_, OT::IndicesFixedSizeCollection >(pyObj) );
}

}
}
