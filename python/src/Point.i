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

#if SWIG_VERSION >= 0x040200
Point operator +(const Point & other)
{
 return *self + other;
}

Point __sub__(const Point & other)
{
 return *self - other;
}
#else
// Fallback for SWIG<4.2: slot wrappers (__add__, ...) turn any exception
// into NotImplemented, losing the dimension message. Expose
// the operations under non-slot names (normal wrappers propagate) and
// alias them to the dunders in %pythoncode below. The slot then calls the
// builtin directly, so there is no per-call Python overhead.
Point _add(const Point & other)
{
 return *self + other;
}

Point _sub(const Point & other)
{
 return *self - other;
}
#endif

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

#if SWIG_VERSION >= 0x040200
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
#else
Point _iadd(const Point & other)
{
 *self += other;
 return *self;
}

Point _isub(const Point & other)
{
 *self -= other;
 return *self;
}
#endif

Point __neg__()
{
 return -1.0 * (*self);
}

const OT::Scalar * __baseaddress__() const
{
  return self->data();
}

UnsignedInteger __elementsize__() const
{
  (void) self;
  return sizeof(OT::Scalar);
}

} // %extend

} // OT

// needed for compatibility with pandas indexation (see is_list_like internal method)
%pythoncode %{
# Helpers _ot_ufunc_convert/_ot_ufunc_out_is_ot for numpy __array_ufunc__
# delegation are defined once in Description.i (same SWIG module).
def _Point___iter__(self):
    for i in range(self.getSize()):
        yield self[i]
Point.__iter__ = _Point___iter__

# Numpy compatibility: expose zero-copy array interface and ufunc delegation.
# Note: ot.Point.__eq__ keeps whole-object semantics (returns Bool); for element-wise
# comparison use numpy ufuncs, e.g. np.equal(pt, 1) or np.asarray(pt) == 1,
# then np.where(...) works on the resulting boolean ndarray.
# Lifetime contract of the zero-copy view: np.asarray(pt) borrows the
# internal buffer (ndarray.base keeps the Point proxy alive, but not the
# buffer itself). Any size-changing mutation (add, resize, clear, erase)
# may reallocate the buffer and invalidates outstanding views; reading such
# a view afterwards is a use-after-free. Call .copy() on the array if it
# must outlive mutations of the Point.
def __Point_getattribute(self, name):
    if name == "__array_interface__":
        # Keep a nonzero address even when empty: a NULL data pointer
        # makes some numpy versions skip the interface and fall back to
        # sequence conversion. With shape (0,) it is never dereferenced.
        self.__dict__["__array_interface__"] = {"shape": (self.getSize(),),
                                                "typestr": f"|f{self.__elementsize__()}",
                                                "data": (int(self.__baseaddress__() or 1), True),
                                                "version": 3,
                                                }
    return super(Point, self).__getattribute__(name)
Point.__getattribute__ = __Point_getattribute

def __Point___array_ufunc__(self, ufunc, method, *inputs, **kwargs):
    out = kwargs.get("out", None)
    if _ot_ufunc_out_is_ot(out):
        return NotImplemented
    new_inputs = tuple(_ot_ufunc_convert(x) for x in inputs)
    new_kwargs = dict(kwargs)
    # Only where (besides out, handled above) can carry an array-like
    # operand; converting every keyword would silently alter future kwargs.
    if "where" in new_kwargs:
        new_kwargs["where"] = _ot_ufunc_convert(new_kwargs["where"])
    return getattr(ufunc, method)(*new_inputs, **new_kwargs)
Point.__array_ufunc__ = __Point___array_ufunc__
Point.__array_priority__ = 10000
%}

#if SWIG_VERSION < 0x040200
%pythoncode %{
# Fallback for SWIG<4.2, see dimension note above
Point.__add__ = Point._add
Point.__sub__ = Point._sub
Point.__iadd__ = Point._iadd
Point.__isub__ = Point._isub
%}
#endif
