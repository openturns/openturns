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

// ptr conversion argument to handle None correctly in swig 4.x, this just allows to pass compilation with swig 3.x
#if SWIGVERSION < 0x040000
#define SWIG_POINTER_NO_NULL 0x0
#endif
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
%ignore *::save(StorageManager & mgr, const String & label, bool fromStudy) const;
%ignore *::save(StorageManager & mgr, const String & label) const;
%ignore *::save(StorageManager & mgr, bool fromStudy) const;
%ignore *::save(StorageManager & mgr) const;

%define OTAutodoc(PythonClass)
%feature("autodoc", "Proxy of C++ OT::" #PythonClass ".") OT::PythonClass;
%enddef

