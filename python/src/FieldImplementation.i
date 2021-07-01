// SWIG file FieldImplementation.i

%{
#include "openturns/FieldImplementation.hxx"
%}

%include FieldImplementation_doc.i

#define OT_TYPECHECK_FIELDIMPLEMENTATION 3

%typemap(typecheck,precedence=OT_TYPECHECK_FIELDIMPLEMENTATION) const FieldImplementation & {
  $1  = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL));
  // || OT::isAPythonSequenceOf<OT::_PySequence_>( $input );
}

%apply const FieldImplementation & { const OT::FieldImplementation & };

%include openturns/FieldImplementation.hxx


namespace OT{
%extend FieldImplementation {

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

Scalar __getitem__(PyObject * args) const {

          OT::UnsignedInteger arg2 ;
          OT::UnsignedInteger arg3 ;
          unsigned long val2 ;
          int ecode2 = 0 ;
          unsigned long val3 ;
          int ecode3 = 0 ;
          PyObject * obj1 = 0 ;
          PyObject * obj2 = 0 ;

          if (!PyArg_ParseTuple(args,(char *)"OO:FieldImplementation___getitem__",&obj1,&obj2)) SWIG_fail;

          ecode2 = SWIG_AsVal_unsigned_SS_long(obj1, &val2);
          if (!SWIG_IsOK(ecode2)) {
            SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "FieldImplementation___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
          }
          arg2 = static_cast< OT::UnsignedInteger >(val2);


          ecode3 = SWIG_AsVal_unsigned_SS_long(obj2, &val3);
          if (!SWIG_IsOK(ecode3)) {
            SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "FieldImplementation___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
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

          if (!PyArg_ParseTuple(args,(char *)"OO:FieldImplementation___setitem__",&obj1,&obj2)) SWIG_fail;

          ecode2 = SWIG_AsVal_unsigned_SS_long(obj1, &val2);
          if (!SWIG_IsOK(ecode2)) {
            SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "FieldImplementation___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
          }
          arg2 = static_cast< OT::UnsignedInteger >(val2);
          ecode3 = SWIG_AsVal_unsigned_SS_long(obj2, &val3);
          if (!SWIG_IsOK(ecode3)) {
            SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "FieldImplementation___setitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
          }
          arg3 = static_cast< OT::UnsignedInteger >(val3);
          (*self).at(arg2, arg3) = val;
fail:
          return;
}

FieldImplementation(const FieldImplementation & other)
{
  return new OT::FieldImplementation(other);
}


} // %extend
} // OT
