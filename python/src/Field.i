// SWIG file Field.i

%{
#include "openturns/Field.hxx"
%}

%include Field_doc.i

OTTypedInterfaceObjectHelper(Field)

%include openturns/Field.hxx


%pythoncode %{
def Field__repr_html_(self):
    """Get HTML representation."""
    data = self.getMesh().getVertices()
    data.stack(self.getValues())
    return data._repr_html_()
Field._repr_html_ = Field__repr_html_
%}

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
  // CopyOnWrite only if index is ok
  self->copyOnWrite();
  self->at(index) = val;
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

PyObject * __getitem__(PyObject * args) const
{
  SWIG_PYTHON_THREAD_BEGIN_BLOCK;
  if (!PyTuple_Check(args))
  {
    if (PyObject_HasAttrString(args, "__int__"))
    {
      // case 0.3: [numpy.int64] => Point
      OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(args, const_cast<char *>("__int__"), const_cast<char *>("()")));
      if (intValue.isNull())
        OT::handleException();
      long index = PyInt_AsLong(intValue.get());
      if (index < 0)
        index += self->getSize();
      if (index < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
      OT::Point result(self->at(index));
      return SWIG_NewPointerObj(new OT::Point(result), SWIGTYPE_p_OT__Point, SWIG_POINTER_OWN);
    }
  }

  OT::UnsignedInteger arg2;
  OT::UnsignedInteger arg3;
  unsigned long val2;
  int ecode2 = 0;
  unsigned long val3;
  int ecode3 = 0;
  PyObject * obj1 = 0;
  PyObject * obj2 = 0;

  if (!PyArg_ParseTuple(args,(char *)"OO:Field___getitem__",&obj1,&obj2)) SWIG_fail;

  if (OT::isAPython< OT::_PyInt_ >(obj1))
  {
    ecode2 = SWIG_AsVal_unsigned_SS_long(obj1, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Field___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    arg2 = static_cast< OT::UnsignedInteger >(val2);

    if (OT::isAPython< OT::_PyInt_ >(obj2))
    {
      ecode3 = SWIG_AsVal_unsigned_SS_long(obj2, &val3);
      if (!SWIG_IsOK(ecode3)) {
        SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "Field___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
      }
      arg3 = static_cast< OT::UnsignedInteger >(val3);
      return OT::convert< OT::Scalar, OT::_PyFloat_>(self->at(arg2, arg3));
    }
  }
  else
    SWIG_exception(SWIG_TypeError, "Field.__getitem__ expects int, slice or sequence arguments");
fail:
  return NULL;
}


void __setitem__(PyObject * args, PyObject * valObj)
{
  SWIG_PYTHON_THREAD_BEGIN_BLOCK;
  if (!PyTuple_Check(args))
  {
    if (PyObject_HasAttrString(args, "__int__"))
    {
      // case 0.3: [numpy.int64] => Point
      OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(args, const_cast<char *>("__int__"), const_cast<char *>("()")));
      if (intValue.isNull())
        OT::handleException();
      long index = PyInt_AsLong(intValue.get());
      if (index < 0)
        index += self->getSize();
      if (index < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "].";
      self->at(index) = OT::convert<OT::_PySequence_, OT::Point>(valObj);
      return;
    }
  }

  OT::UnsignedInteger arg2;
  OT::UnsignedInteger arg3;
  unsigned long val2;
  int ecode2 = 0;
  unsigned long val3;
  int ecode3 = 0;
  PyObject * obj1 = 0;
  PyObject * obj2 = 0;

  if (!PyArg_ParseTuple(args,(char *)"OO:Field___setitem__",&obj1,&obj2)) SWIG_fail;

  ecode2 = SWIG_AsVal_unsigned_SS_long(obj1, &val2);
  if (!SWIG_IsOK(ecode2)) {
    SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Field___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
  }
  arg2 = static_cast< OT::UnsignedInteger >(val2);
  ecode3 = SWIG_AsVal_unsigned_SS_long(obj2, &val3);
  if (!SWIG_IsOK(ecode3)) {
    SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "Field___setitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
  }
  arg3 = static_cast< OT::UnsignedInteger >(val3);
  self->at(arg2, arg3) = OT::checkAndConvert<OT::_PyFloat_, OT::Scalar>(valObj);
fail:
  return;
}

} // Field

} // OT

