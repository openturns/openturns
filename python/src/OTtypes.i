// SWIG file OTtypes.i

%include std_string.i
%include std_map.i
%include std_complex.i

%include openturns/OT.hxx
%include openturns/OTtypes.hxx
%include openturns/OTdebug.h

%{
#include "openturns/OT.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

// for SWIG 4.3 compat
inline PyObject* OT_AppendOutput(PyObject* result, PyObject* obj)
{
#if SWIG_VERSION >= 0x040300
  return SWIG_Python_AppendOutput(result, obj, 1);
#else
  return SWIG_Python_AppendOutput(result, obj);
#endif
}
%}

%typemap(in) UnsignedInteger {
  $1 = OT::convert< OT::_PyInt_, OT::UnsignedInteger >($input);
}

%apply UnsignedInteger { OT::Unsigned64BitsInteger };

%typemap(in) Scalar {
  $1 = OT::convert< OT::_PyFloat_, OT::Scalar >($input);
}

%typemap(in) Complex {
  $1 = OT::convert< OT::_PyComplex_, OT::Complex >($input);
}

%typemap(out) Complex {
  $result = OT::convert< OT::Complex, OT::_PyComplex_ >($1);
}



%ignore *::clone() const;
%ignore *::load(Advocate & adv);
%ignore *::save(Advocate & adv) const;
%ignore *::save(StorageManager & mgr, const String & label, const Bool fromStudy) const;
%ignore *::save(StorageManager & mgr, const String & label) const;
%ignore *::save(StorageManager & mgr, const Bool fromStudy) const;
%ignore *::save(StorageManager & mgr) const;

%define OTAutodoc(PythonClass)
%feature("autodoc", "Proxy of C++ OT::" #PythonClass ".") OT::PythonClass;
%enddef

