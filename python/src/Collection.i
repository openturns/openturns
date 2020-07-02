// SWIG file Collection.i

%{
#include "openturns/Collection.hxx"
%}

%include Collection_doc.i

%rename(__contains__) OT::Collection::contains;
%rename(__baseaddress__) OT::Collection::data;
%rename(__elementsize__) OT::Collection::elementSize;
%ignore OT::Collection::Collection(std::initializer_list<T> initList);

%include openturns/Collection.hxx
%copyctor Collection;

namespace OT {

%extend Collection {

Collection(PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence< T >(pyObj);
}

template <class T> Collection(const Collection<T> & other)
{
  return new OT::Collection<T>(other);
}


%define OT_COLLECTION_GETITEM(collectionType, elementType)
PyObject * __getitem__(PyObject * arg) const
{
  if (OT::isAPython< OT::_PyInt_ >(arg))
  {
    long val2 = 0;
    int ecode2 = 0;
    ecode2 = SWIG_AsVal_long(arg, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" #collectionType "___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    if (val2 < 0) {
      val2 += self->getSize();
    }
    OT::UnsignedInteger arg2 = static_cast< OT::UnsignedInteger >(val2);
    return OT::convert< elementType, OT::traitsPythonType<elementType>::Type>(self->at(arg2));
  }
  else if (PySlice_Check(arg))
  {
    Py_ssize_t start = 0;
    Py_ssize_t stop = 0;
    Py_ssize_t step = 0;
    Py_ssize_t size = 0;
    if (PySlice_GetIndicesEx(OT::SliceCast(arg), self->getSize(), &start, &stop, &step, &size ) < 0)
      throw OT::InternalException(HERE) << "PySlice_GetIndicesEx failed";
    collectionType result(size);
    if ((step == 1) && (size > 0))
      std::copy(&self->operator[](start), &self->operator[](start + size - 1) + 1, &result[0]);
    else
      for (Py_ssize_t i = 0; i < size; ++ i)
        result.at(i) = self->at(start + i * step);
    return SWIG_NewPointerObj((new collectionType(static_cast< const collectionType& >(result))), SWIG_TypeQuery(#collectionType " *"), SWIG_POINTER_OWN |  0);
  }
  else if (PySequence_Check(arg))
  {
    OT::ScopedPyObjectPointer newPyObj(PySequence_Fast(arg, ""));
    const Py_ssize_t size = PySequence_Fast_GET_SIZE(newPyObj.get());
    collectionType result(size);
    for (Py_ssize_t i = 0; i < size; ++ i)
    {
      PyObject * elt = PySequence_Fast_GET_ITEM(newPyObj.get(), i);
      if (PyInt_Check(elt))
      {
        long index = PyInt_AsLong(elt);
        if (index < 0) {
          index += self->getSize();
        }
        if (index < 0) {
          throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
        }
        result[i] = self->at(index);
      }
      else
        SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
    }
    return SWIG_NewPointerObj((new collectionType(static_cast< const collectionType& >(result))), SWIG_TypeQuery(#collectionType " *"), SWIG_POINTER_OWN |  0);
  }
  else
    SWIG_exception(SWIG_TypeError, "Collection.__getitem__ expects int, slice or sequence argument");
fail:
  return NULL;
}
%enddef


%define OT_COLLECTION_SETITEM(collectionType, elementType)
PyObject * __setitem__(PyObject * arg, PyObject * valObj)
{
  if (OT::isAPython< OT::_PyInt_ >(arg))
  {
    long val2 = 0;
    int ecode2 = 0;
    ecode2 = SWIG_AsVal_long(arg, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" #collectionType "___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    if (val2 < 0) {
      val2 += self->getSize();
    }
    OT::UnsignedInteger arg2 = static_cast< OT::UnsignedInteger >(val2);
    elementType val = OT::convert< OT::traitsPythonType<elementType>::Type, elementType >(valObj);
    self->at(arg2) = val;
  }
  else if (PySlice_Check(arg))
  {
    Py_ssize_t start = 0;
    Py_ssize_t stop = 0;
    Py_ssize_t step = 0;
    Py_ssize_t size = 0;
    PySlice_GetIndicesEx(OT::SliceCast(arg), self->getSize(), &start, &stop, &step, &size);
    collectionType temp2;
    collectionType *val2 = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery(#collectionType " *"), 0))) {
      temp2 = OT::convert< OT::_PySequence_, collectionType >(valObj);
      val2 = &temp2;
    }
    assert(val2);
    if ((step == 1) && (size > 0))
      std::copy(&temp2[0], &temp2[size - 1] + 1, &self->operator[](start));
    else
      for (Py_ssize_t i = 0; i < size; ++ i)
        self->at(start + i*step) = temp2.at(i);
  }
  else if (PySequence_Check(arg))
  {
    OT::ScopedPyObjectPointer newPyObj(PySequence_Fast(arg, ""));
    const Py_ssize_t size = PySequence_Fast_GET_SIZE(newPyObj.get());
    OT::Indices indices(size);
    for (Py_ssize_t i = 0; i < size; ++ i)
    {
      PyObject * elt = PySequence_Fast_GET_ITEM(newPyObj.get(), i);
      if (PyInt_Check(elt))
      {
        long index = PyInt_AsLong(elt);
        if (index < 0) {
          index += self->getSize();
        }
        if (index < 0) {
          throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
        }
        indices[i] = index;
      }
      else
        SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
    }
    collectionType temp2;
    collectionType *val2 = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery(#collectionType " *"), 0))) {
      temp2 = OT::convert< OT::_PySequence_, collectionType >(valObj);
      val2 = &temp2;
    }
    assert(val2);
    for (Py_ssize_t i = 0; i < size; ++ i)
      self->at(indices[i]) = temp2.at(i);
  }
  else
    SWIG_exception(SWIG_TypeError, "Collection.__setitem__ expects int, slice or sequence argument");
  return SWIG_Py_Void();
fail:
  return NULL;
}
%enddef

%define OTCollectionOperatorsHelper(collectionType, elementType)
OT_COLLECTION_SETITEM(collectionType, elementType)
OT_COLLECTION_GETITEM(collectionType, elementType)
Bool __eq__(const collectionType & other) { return (*self) == other; }
Bool __ne__(const collectionType & other) { return (*self) != other; }
%enddef

} // %extend

}
