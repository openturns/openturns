// SWIG file Collection.i

%{
#include "openturns/Collection.hxx"
%}

%include Collection_doc.i

%rename(__contains__) OT::Collection::contains;
%rename(__baseaddress__) OT::Collection::data;
%rename(__elementsize__) OT::Collection::elementSize;

%include openturns/Collection.hxx
%copyctor Collection;

namespace OT {

%extend Collection {

Collection(PyObject * pyObj)
{
 return OT::buildCollectionFromPySequence< T >( pyObj );
}

 
template <class T> Collection(const Collection<T> & other)
{
 return new OT::Collection<T>(other);
}


%define OT_COLLECTION_GETITEM(collectionType, elementType)
PyObject * __getitem__(PyObject * arg) const
{
  if ( PySlice_Check(arg) )
  {
    Py_ssize_t start = 0;
    Py_ssize_t stop = 0;
    Py_ssize_t step = 0;
    Py_ssize_t slicelength = 0;
    if( PySlice_GetIndicesEx( OT::SliceCast( arg ), self->getSize(), &start, &stop, &step, &slicelength ) < 0 )
      throw OT::InternalException(HERE);
    collectionType result( slicelength );
    for ( Py_ssize_t i = 0; i < slicelength; ++ i )
    {
      result.at(i) = self->at( start + i*step );
    }
    return SWIG_NewPointerObj((new collectionType(static_cast< const collectionType& >(result))), SWIG_TypeQuery( #collectionType " *"), SWIG_POINTER_OWN |  0 );
  }
  else
  {
    long val2 ;
    int ecode2 = 0 ;
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
fail:
  return NULL;
}
%enddef


%define OT_COLLECTION_SETITEM(collectionType, elementType)
PyObject * __setitem__(PyObject * arg, PyObject * valObj)
{
  if (PySlice_Check(arg))
  {
    Py_ssize_t start1 = 0;
    Py_ssize_t stop1 = 0;
    Py_ssize_t step1 = 0;
    Py_ssize_t slicelength1 = 0;
    PySlice_GetIndicesEx( OT::SliceCast( arg ), self->getSize(), &start1, &stop1, &step1, &slicelength1 );
    collectionType temp2 ;
    collectionType *val2 = 0 ;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery( #collectionType " *"), 0))) {
      temp2 = OT::convert< OT::_PySequence_, collectionType >( valObj );
      val2 = &temp2;
    }
    assert(val2);
    for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
    {
      self->at( start1 + i*step1 ) = val2->at(i);
    }
  } 
  else
  {
    long val2 ;
    int ecode2 = 0 ;
    ecode2 = SWIG_AsVal_long(arg, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" #collectionType "___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    if (val2 < 0) {
      val2 += self->getSize();
    }
    OT::UnsignedInteger arg2 = static_cast< OT::UnsignedInteger >(val2);
    elementType val = OT::convert< OT::traitsPythonType<elementType>::Type, elementType >( valObj );
    self->at(arg2) = val;
  }

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
