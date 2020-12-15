// SWIG file ProcessSamples.i

%{
#include "openturns/ProcessSample.hxx"



namespace OT {
  template <>
  inline
  OT::Collection<OT::Sample>
  convert< OT::_PySequence_, OT::Collection<OT::Sample> >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Collection<OT::Sample> *"), 0))) {
      OT::Collection<OT::Sample> * p_it = reinterpret_cast< OT::Collection<OT::Sample> * >( ptr );
      return *p_it;
    }
    else {
      return *OT::buildCollectionFromPySequence< OT::Sample >(pyObj);
    }
    return OT::Collection<OT::Sample>();
  }

  template <>
  inline
  bool canConvert<OT::_PySequence_, OT::Collection<OT::Sample> >(_object * pyObj)
  {
    return OT::canConvertCollectionObjectFromPySequence< OT::Sample >(pyObj);
  }

  template <>
  inline
  bool canConvert<OT::_PySequence_, OT::Sample >(_object * pyObj)
  {
   return SWIG_IsOK(SWIG_ConvertPtr(pyObj, NULL, SWIG_TypeQuery("OT::Sample *"), 0)) || OT::isAPythonBufferOf<OT::Scalar, 2>(pyObj) || OT::isAPythonSequenceOf<OT::_PySequence_>(pyObj);
  }
}
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

