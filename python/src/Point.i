// SWIG file Point.i

%ignore OT::Point::at; // Use __getitem__ instead
%ignore OT::Point::getCollection;

%{
#include "openturns/Point.hxx"
%}

%include Point_doc.i

%template(ScalarCollection)           OT::Collection<OT::Scalar>;
%template(_ScalarPersistentCollection) OT::PersistentCollection<OT::Scalar>;

%typemap(in) const ScalarCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::Scalar> >( $input );
      $1 = &temp;
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Scalar");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const ScalarCollection & {
  $1 = (SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) ||
       OT::isAPythonBufferOf<OT::Scalar, 1>($input) || OT::isAPythonSequenceOf<OT::_PyFloat_>($input));
}

%template(PointCollection)            OT::Collection<OT::Point>;
%template(_PointPersistentCollection)  OT::PersistentCollection<OT::Point>;


#define OT_TYPECHECK_NUMERICALPOINT 4

%typemap(in) const Point & ($1_basetype temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL)))
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
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) ||
    OT::isAPythonBufferOf<OT::Scalar, 1>($input) || OT::isAPythonSequenceOf<OT::_PyFloat_>($input);
}

%apply const Point & { const OT::Point & };

%ignore OT::Point::Point(std::initializer_list<Scalar> initList);

%include openturns/Point.hxx

namespace OT {

%extend Point {

Point(const Point & other)
{
  return new OT::Point(other);
}

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

Point __neg__()
{
 return -1.0 * (*self);
}

} // %extend

} // OT

// needed for compatibility with pandas indexation (see is_list_like internal method)
%pythoncode %{
def _Point___iter__(self):
    for i in range(self.getSize()):
        yield self[i]
Point.__iter__ = _Point___iter__


def _Point___add__(self, other):
    # The SWIG wrapper swallows the C++ dimension check into a generic
    # "unsupported operand" TypeError, and CPython drops any TypeError raised
    # by __add__ before trying __radd__ (hence the generic message). Raise a
    # ValueError on dimension mismatch, as NumPy does for shape mismatches, so
    # the informative message is preserved, see issue #2432.
    if isinstance(other, Point) and other.getDimension() != self.getDimension():
        raise ValueError(
            "Points of different dimensions cannot be added "
            "(LHS dimension = %d; RHS dimension = %d)"
            % (self.getDimension(), other.getDimension())
        )
    return _typ.Point___add__(self, other)


def _Point___sub__(self, other):
    # same rationale as _Point___add__
    if isinstance(other, Point) and other.getDimension() != self.getDimension():
        raise ValueError(
            "Points of different dimensions cannot be subtracted "
            "(LHS dimension = %d; RHS dimension = %d)"
            % (self.getDimension(), other.getDimension())
        )
    return _typ.Point___sub__(self, other)


Point.__add__ = _Point___add__
Point.__sub__ = _Point___sub__
%}
