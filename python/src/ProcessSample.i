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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Collection<OT::Sample> *"), SWIG_POINTER_NO_NULL))) {
      OT::Collection<OT::Sample> * p_it = reinterpret_cast< OT::Collection<OT::Sample> * >(ptr);
      return *p_it;
    }
    else {
      OT::Pointer<OT::Collection<OT::Sample> > p_coll = OT::buildCollectionFromPySequence< OT::Sample >(pyObj);
      return *p_coll;
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
    return SWIG_IsOK(SWIG_ConvertPtr(pyObj, NULL, SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_NO_NULL))
             || OT::isAPythonBufferOf<OT::Scalar, 2>(pyObj) || OT::isAPythonSequenceOf<OT::_PySequence_>(pyObj);
  }
}
%}

%include ProcessSample_doc.i

OTTypedInterfaceObjectHelper(ProcessSample)

%copyctor OT::ProcessSample;
%include openturns/ProcessSample.hxx

namespace OT {
%extend ProcessSample {

Sample __getitem__(SignedInteger index) const
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

PyObject * __getitem__(PyObject * args) const
{
  if (PySlice_Check(args))
  {
    Py_ssize_t start = 0;
    Py_ssize_t stop = 0;
    Py_ssize_t step = 0;

    if (PySlice_Unpack(args, &start, &stop, &step) < 0)
      throw OT::InvalidArgumentException(HERE) << "ProcessSample.__getitem__: PySlice_Unpack failed";
    Py_ssize_t size = PySlice_AdjustIndices(self->getSize(), &start, &stop, step);

    OT::ProcessSample result(self->getMesh(), size, self->getDimension());
    for (Py_ssize_t i = 0; i < size; ++ i)
      result.getImplementation()->operator[](i) = self->operator[](start + i * step);
    return SWIG_NewPointerObj(new OT::ProcessSample(result), SWIG_TypeQuery("OT::ProcessSample *"), SWIG_POINTER_OWN);
  }
  else if (PySequence_Check(args))
  {
    OT::ScopedPyObjectPointer seq(PySequence_Fast(args, ""));
    const Py_ssize_t size = OT::Sequence_Fast_GET_SIZE(seq.get());
    OT::ProcessSample result(self->getMesh(), size, self->getDimension());
    for (Py_ssize_t i = 0; i < size; ++ i)
    {
      PyObject * elt = OT::Sequence_Fast_GET_ITEM(seq.get(), i);
      long index = 0;
      if (OT::isAPython<OT::_NumPyInt_>(elt))
        index = OT::convert< OT::_NumPyInt_, OT::SignedInteger>(elt);
      else
        throw OT::InvalidArgumentException(HERE) << "Indexing list expects int type";
      OT::Sequence_Fast_DECREF_ITEM(elt);
      if (index < 0)
        index += self->getSize();
      if (index < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
      result.getImplementation()->operator[](i) = self->operator[](index);
    }
    return SWIG_NewPointerObj(new OT::ProcessSample(result), SWIG_TypeQuery("OT::ProcessSample *"), SWIG_POINTER_OWN);
  }
  else if (PyObject_HasAttrString(args, "__int__"))
  {
    OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(args, const_cast<char *>("__int__"), const_cast<char *>("()")));
    if (intValue.isNull())
      OT::handleException();
    long index = PyLong_AsLong(intValue.get());
    if (index < 0)
      index += self->getSize();
    if (index < 0)
      throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
    OT::Sample result(self->operator[](index));
    return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN);
  }
  throw OT::InvalidArgumentException(HERE) << "ProcessSample.__getitem__ expects int, slice or sequence arguments";
  return NULL;
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
  if (values.getSize() != self->getMesh().getVerticesNumber())
    throw OT::InvalidArgumentException(HERE) << "got value of size " << values.getSize() << " expected " << self->getMesh().getVerticesNumber();
  if (values.getDimension() != self->getDimension())
    throw OT::InvalidArgumentException(HERE) << "got value of dimension " << values.getDimension() << " expected " << self->getDimension();
  self->operator[](index) = values;
}

void __setitem__(PyObject * args, PyObject * valObj)
{
  if (PySlice_Check(args))
  {
    Py_ssize_t start = 0;
    Py_ssize_t stop = 0;
    Py_ssize_t step = 0;

    if (PySlice_Unpack(args, &start, &stop, &step) < 0)
      throw OT::InvalidArgumentException(HERE) << "ProcessSample.__setitem__: PySlice_Unpack failed";
    Py_ssize_t size = PySlice_AdjustIndices(self->getSize(), &start, &stop, step);

    OT::ProcessSample *val = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIG_TypeQuery("OT::ProcessSample *"), SWIG_POINTER_NO_NULL))) {
      throw OT::InvalidArgumentException(HERE) << "ProcessSample.__setitem__ expects a ProcessSample value for slice assignment";
    }
    for (Py_ssize_t i = 0; i < size; ++ i)
      self->operator[](start + i * step) = val->operator[](i);
  }
  else if (PySequence_Check(args))
  {
    OT::ScopedPyObjectPointer seq(PySequence_Fast(args, ""));
    const Py_ssize_t size = OT::Sequence_Fast_GET_SIZE(seq.get());

    OT::ProcessSample *val = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIG_TypeQuery("OT::ProcessSample *"), SWIG_POINTER_NO_NULL))) {
      throw OT::InvalidArgumentException(HERE) << "ProcessSample.__setitem__ expects a ProcessSample value for sequence assignment";
    }
    for (Py_ssize_t i = 0; i < size; ++ i)
    {
      PyObject * elt = OT::Sequence_Fast_GET_ITEM(seq.get(), i);
      long index = 0;
      if (OT::isAPython<OT::_NumPyInt_>(elt))
        index = OT::convert< OT::_NumPyInt_, OT::SignedInteger>(elt);
      else
        throw OT::InvalidArgumentException(HERE) << "Indexing list expects int type";
      OT::Sequence_Fast_DECREF_ITEM(elt);
      if (index < 0)
        index += self->getSize();
      if (index < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
      self->operator[](index) = val->operator[](i);
    }
  }
  else if (PyObject_HasAttrString(args, "__int__"))
  {
    OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(args, const_cast<char *>("__int__"), const_cast<char *>("()")));
    if (intValue.isNull())
      OT::handleException();
    long index = PyLong_AsLong(intValue.get());
    if (index < 0)
      index += self->getSize();
    if (index < 0)
      throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
    OT::Sample temp;
    OT::Sample *val = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_NO_NULL))) {
      temp = OT::convert<OT::_PySequence_, OT::Sample>(valObj);
      val = &temp;
    }
    if (val->getSize() != self->getMesh().getVerticesNumber())
      throw OT::InvalidArgumentException(HERE) << "got value of size " << val->getSize() << " expected " << self->getMesh().getVerticesNumber();
    if (val->getDimension() != self->getDimension())
      throw OT::InvalidArgumentException(HERE) << "got value of dimension " << val->getDimension() << " expected " << self->getDimension();
    self->operator[](index) = *val;
  }
  else
    throw OT::InvalidArgumentException(HERE) << "ProcessSample.__setitem__ expects int, slice or sequence arguments";
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

} // ProcessSample

} // OT
