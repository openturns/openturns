// SWIG file Description.i

%{
#include "openturns/Description.hxx"
%}

%include Description_doc.i

%template(_StringCollection) OT::Collection<OT::String>;
%template(_StringPersistentCollection) OT::PersistentCollection<OT::String>;

%typemap(in) const OT::Description & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Description>( $input );
      $1 = &temp;
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a Description");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::Description & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) ||
       OT::isAPythonSequenceOf<OT::_PyString_>($input);
}

%ignore OT::Description::Description(std::initializer_list<String> initList);
%include openturns/Description.hxx

namespace OT {

%extend Description {

Description(const Description & other) { return new OT::Description(other); }

Description(PyObject * pyObj)
{
 OT::Pointer<OT::Collection<OT::String> > p_coll =  OT::buildCollectionFromPySequence<OT::String>( pyObj );
 return new OT::Description( *p_coll );
}

OTCollectionOperatorsHelper(OT::Description, OT::String)

} // Description
} // OT

// needed for compatibility with pandas indexation (see is_list_like internal method)
%pythoncode %{
# Helpers for numpy __array_ufunc__ delegation, shared by Point.i, Sample.i
# and Description.i (same SWIG module, defined once here as Description.i
# is included first).
def _ot_ufunc_convert(value):
    """Convert an OpenTURNS vector object to ndarray for ufunc delegation.

    Uses isinstance so Python subclasses (e.g. class MyPoint(ot.Point) defined
    in __main__) are converted too; a module-name test would miss them and
    recurse into the same __array_ufunc__.
    """
    try:
        import openturns as _ot
        _ot_types = (_ot.Point, _ot.Sample, _ot.Description)
    except Exception:
        return value
    if isinstance(value, _ot_types):
        try:
            import numpy as _np
            return _np.asarray(value)
        except Exception:
            return value
    return value

def _ot_ufunc_out_is_ot(out):
    """Check whether a ufunc out= tuple holds an OpenTURNS object."""
    if out is None:
        return False
    try:
        import openturns as _ot
        _ot_types = (_ot.Point, _ot.Sample, _ot.Description)
    except Exception:
        return False
    for output in out:
        if output is not None and isinstance(output, _ot_types):
            return True
    return False

def _Description___iter__(self):
    for i in range(len(self)):
        yield self[i]
Description.__iter__ = _Description___iter__

# Numpy compatibility: delegate ufuncs to ndarrays (element-wise, copy-based:
# variable-length strings cannot be exposed zero-copy, unlike Point/Sample).
# Note: ot.Description.__eq__ keeps whole-object semantics (returns Bool); for
# element-wise comparison use numpy ufuncs, e.g. np.equal(desc, "a").
def __Description___array_ufunc__(self, ufunc, method, *inputs, **kwargs):
    import numpy as _np
    out = kwargs.get("out", None)
    if _ot_ufunc_out_is_ot(out):
        return NotImplemented
    new_inputs = tuple(_ot_ufunc_convert(x) for x in inputs)
    new_kwargs = dict(kwargs)
    # Only where (besides out, handled above) can carry an array-like
    # operand; converting every keyword would silently alter future kwargs.
    if "where" in new_kwargs:
        new_kwargs["where"] = _ot_ufunc_convert(new_kwargs["where"])
    if ufunc in (_np.equal, _np.not_equal):
        # Numpy < 1.24 has no string loop for equal/not_equal: compare as
        # objects, with identical elementwise semantics
        try:
            return getattr(ufunc, method)(*new_inputs, **new_kwargs)
        except TypeError:
            new_inputs = tuple(_np.asarray(x, dtype=object)
                               if isinstance(x, _np.ndarray) and x.dtype.kind in "US"
                               else x for x in new_inputs)
            return getattr(ufunc, method)(*new_inputs, **new_kwargs)
    return getattr(ufunc, method)(*new_inputs, **new_kwargs)
Description.__array_ufunc__ = __Description___array_ufunc__
Description.__array_priority__ = 10000
%}
