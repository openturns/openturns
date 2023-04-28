// SWIG file IndicesCollection.i

%{
#include "openturns/IndicesCollection.hxx"
%}

%include IndicesCollection_doc.i

OTTypedInterfaceObjectHelper(IndicesCollection)

%typemap(in) const IndicesCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_, OT::IndicesCollection>($input);
      $1 = &temp;
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to an IndicesCollection");
    }
  }
}

%typemap(typecheck,precedence=5) const IndicesCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) ||
       OT::isAPythonBufferOf<OT::UnsignedInteger, 2>($input) || OT::isAPythonSequenceOf<OT::_PySequence_>($input);
}

%apply const IndicesCollection & { const OT::IndicesCollection & };

%include openturns/IndicesCollection.hxx

namespace OT {
%extend IndicesCollection {

Indices __getitem__(SignedInteger index) const {
  OT::UnsignedInteger size = self->getSize();
  if (size == 0) throw OT::OutOfBoundException(HERE) << "collection is empty.";
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0 || index >= static_cast<OT::SignedInteger>(size)) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  return OT::Indices(self->cbegin_at(index), self->cend_at(index));
}

void __setitem__ (SignedInteger index,
                  const Indices & val) {
  OT::UnsignedInteger size = self->getSize();
  if (size == 0) throw OT::OutOfBoundException(HERE) << "collection is empty.";
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0 || index >= static_cast<OT::SignedInteger>(size)) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  OT::UnsignedInteger thisSize = self->cend_at(index) - self->cbegin_at(index);
  if (val.getSize() != thisSize)
    throw OT::InvalidArgumentException(HERE) << "element at position " << index << " is of size " << thisSize << " whereas value size is " << val.getSize();
  std::copy(val.begin(), val.end(), self->begin_at(index));
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

IndicesCollection(const IndicesCollection & other)
{
  return new OT::IndicesCollection(other);
}

IndicesCollection(PyObject * pyObj)
{
  return new OT::IndicesCollection( OT::convert< OT::_PySequence_, OT::IndicesCollection >(pyObj) );
}

}
}
