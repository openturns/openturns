// SWIG file Point.i

%ignore OT::Point::at; // Use __getitem__ instead
%ignore OT::Point::getCollection;

%{
#include "openturns/Point.hxx"
%}

%include Point_doc.i

%template(ScalarCollection)           OT::Collection<OT::Scalar>;
%template(ScalarPersistentCollection) OT::PersistentCollection<OT::Scalar>;

%typemap(in) const ScalarCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::Scalar> >( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Scalar");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const ScalarCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) ||
       OT::isAPythonBufferOf<OT::Scalar, 1>($input) || OT::isAPythonSequenceOf<OT::_PyFloat_>( $input );
}

%template(PointCollection)            OT::Collection<OT::Point>;
%template(PointPersistentCollection)  OT::PersistentCollection<OT::Point>;


#define OT_TYPECHECK_NUMERICALPOINT 4

%typemap(in) const Point & ($1_basetype temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0)))
  {
    //Nothing to do for NP
  }
  else if (OT::isAPythonBufferOf<OT::Scalar, 1>($input) || OT::isAPythonSequenceOf<OT::_PyFloat_>( $input ))
  {
    temp = OT::convert<OT::_PySequence_,OT::Point>( $input );
    $1 = &temp;
  }
  else
  {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a Point");
  }
}

%typemap(typecheck,precedence=OT_TYPECHECK_NUMERICALPOINT) const Point & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) ||
       OT::isAPythonBufferOf<OT::Scalar, 1>($input) || OT::isAPythonSequenceOf<OT::_PyFloat_>( $input );
}

%apply const Point & { const OT::Point & };

%include openturns/Point.hxx
%copyctor Point;

namespace OT {

%extend Point {

Point(PyObject * pyObj)
{
  return new OT::Point(OT::convert<OT::_PySequence_,OT::Point>(pyObj));
}

OTCollectionOperatorsHelper(OT::Point, OT::Scalar)

Point __radd__(const Scalar s)
{
  // we allow 0+Point for the sum operator, but disable it for any other value
  if (s != 0.0)
    throw OT::NotYetImplementedException(HERE) << "Unsupported float+Point operator";
  return *self;
}

Point operator +(const Point & other)
{
 return *self + other;
}

Point __sub__(const Point & other)
{
 return *self - other;
}

Point __mul__(Scalar s)
{
 return (*self) * s;
}

Point __rmul__(Scalar s)
{
 return s * (*self);
}

Point __div__(Scalar s)
{
 return (*self) / s;
}

Point __truediv__(Scalar s) { return (*self) / s; }

Point __iadd__(const Point & other)
{
 *self += other;
 return *self;
}

Point __isub__(const Point & other)
{
 *self -= other;
 return *self;
}

} // %extend

} // OT

