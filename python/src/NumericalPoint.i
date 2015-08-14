// SWIG file NumericalPoint.i

%ignore OT::NumericalPoint::at; // Use __getitem__ instead
%ignore OT::NumericalPoint::getCollection;

%{
#include "NumericalPoint.hxx"
%}

%include NumericalPoint_doc.i

%template(NumericalScalarCollection)           OT::Collection<OT::NumericalScalar>;
%template(NumericalScalarPersistentCollection) OT::PersistentCollection<OT::NumericalScalar>;

%typemap(in) const NumericalScalarCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    temp = OT::convert<OT::_PySequence_,OT::Collection<OT::NumericalScalar> >( $input );
    $1 = &temp;
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const NumericalScalarCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) ||
       OT::isAPythonSequenceOf<OT::_PyFloat_>( $input );
}

%template(NumericalPointCollection)            OT::Collection<OT::NumericalPoint>;
%template(NumericalPointPersistentCollection)  OT::PersistentCollection<OT::NumericalPoint>;


#define OT_TYPECHECK_NUMERICALPOINT 5

%typemap(in) const NumericalPoint & ($1_basetype temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0)))
  {
    //Nothing to do for NP
  }
  else if (OT::isAPythonSequenceOf<OT::_PyFloat_>( $input ))
  {
    temp = OT::convert<OT::_PySequence_,OT::NumericalPoint>( $input );
    $1 = &temp;
  }
  else
  {
    SWIG_exception(SWIG_TypeError, "InvalidArgumentException : Object passed as argument is not a sequence of float");
  }
}

%typemap(typecheck,precedence=OT_TYPECHECK_NUMERICALPOINT) const NumericalPoint & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonSequenceOf<OT::_PyFloat_>( $input );
}

%apply const NumericalPoint & { const OT::NumericalPoint & };

%include NumericalPoint.hxx
%copyctor NumericalPoint;

namespace OT {

%extend NumericalPoint {

NumericalPoint(PyObject * pyObj)
{
  return new OT::NumericalPoint(OT::convert<OT::_PySequence_,OT::NumericalPoint>(pyObj));
}

OT_COLLECTION_GETITEM(OT::NumericalPoint, OT::NumericalScalar)
OT_COLLECTION_SETITEM(OT::NumericalPoint, OT::NumericalScalar)

Bool __eq__(const NumericalPoint & other)
{
 return operator == (*self, other);
}

/* NumericalPoint __add__(const NumericalPoint & other) */
/* { */
/*  return *self + other; */
/* } */

NumericalPoint operator +(const NumericalPoint & other)
{
 return *self + other;
}

NumericalPoint __sub__(const NumericalPoint & other)
{
 return *self - other;
}

NumericalPoint __mul__(NumericalScalar s)
{
 return (*self) * s;
}

NumericalPoint __rmul__(NumericalScalar s)
{
 return s * (*self);
}

NumericalPoint __div__(NumericalScalar s)
{
 return (*self) / s;
}

NumericalPoint __truediv__(NumericalScalar s) { return (*self) / s; }

NumericalPoint __iadd__(const NumericalPoint & other)
{
 *self += other;
 return *self;
}

NumericalPoint __isub__(const NumericalPoint & other)
{
 *self -= other;
 return *self;
}

} // %extend

} // OT
