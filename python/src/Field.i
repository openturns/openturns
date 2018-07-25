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

Scalar __getitem__(PyObject * args) const {

          OT::UnsignedInteger arg2 ;
          OT::UnsignedInteger arg3 ;
          unsigned long val2 ;
          int ecode2 = 0 ;
          unsigned long val3 ;
          int ecode3 = 0 ;
          PyObject * obj1 = 0 ;
          PyObject * obj2 = 0 ;

          if (!PyArg_ParseTuple(args,(char *)"OO:Field___getitem__",&obj1,&obj2)) SWIG_fail;

          ecode2 = SWIG_AsVal_unsigned_SS_long(obj1, &val2);
          if (!SWIG_IsOK(ecode2)) {
            SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Field___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
          }
          arg2 = static_cast< OT::UnsignedInteger >(val2);


          ecode3 = SWIG_AsVal_unsigned_SS_long(obj2, &val3);
          if (!SWIG_IsOK(ecode3)) {
            SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "Field___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
          }
          arg3 = static_cast< OT::UnsignedInteger >(val3);

          return (*self).at(arg2, arg3);
fail:
          return 0.;
}



void __setitem__(PyObject * args, Scalar val) {

          OT::UnsignedInteger arg2 ;
          OT::UnsignedInteger arg3 ;
          unsigned long val2 ;
          int ecode2 = 0 ;
          unsigned long val3 ;
          int ecode3 = 0 ;
          PyObject * obj1 = 0 ;
          PyObject * obj2 = 0 ;

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
          (*self).at(arg2, arg3) = val;
fail:
          return;
}

} // Field

} // OT

