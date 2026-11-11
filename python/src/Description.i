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
    def _cvt(a):
        mod = getattr(type(a), "__module__", "") or ""
        if mod.startswith("openturns"):
            try:
                return _np.asarray(a)
            except Exception:
                return a
        return a
    out = kwargs.get("out", None)
    if out is not None:
        for o in out:
            if o is not None:
                mod = getattr(type(o), "__module__", "") or ""
                if mod.startswith("openturns"):
                    return NotImplemented
    new_inputs = tuple(_cvt(x) for x in inputs)
    new_kwargs = {k: _cvt(v) for k, v in kwargs.items()}
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
