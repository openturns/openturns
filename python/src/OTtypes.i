// SWIG file OTtypes.i

%module types

%include std_string.i
%include std_map.i
%include std_complex.i

%include openturns/OT.hxx
%include openturns/OTtypes.hxx
%include openturns/OTdebug.h

%{
#include "openturns/OT.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
%}

%typemap(in) UnsignedInteger {
  $1 = OT::convert< OT::_PyInt_, OT::UnsignedInteger >($input);
}

%apply UnsignedInteger { OT::Unsigned64BitsInteger };

%typemap(in) Scalar {
  $1 = OT::convert< OT::_PyFloat_, OT::Scalar >($input);
}

%typemap(in) NumericalComplex {
  $1 = OT::convert< OT::_PyComplex_, OT::NumericalComplex >($input);
}

%typemap(out) NumericalComplex {
  $result = OT::convert< OT::NumericalComplex, OT::_PyComplex_ >($1);
}



%ignore *::clone() const;
%ignore *::load(Advocate & adv);
%ignore *::save(Advocate & adv) const;
%ignore *::save(StorageManager & mgr, const String & label, bool fromStudy) const;
%ignore *::save(StorageManager & mgr, const String & label) const;
%ignore *::save(StorageManager & mgr, bool fromStudy) const;
%ignore *::save(StorageManager & mgr) const;

%define OTAutodoc(PythonClass)
%feature("autodoc", "Proxy of C++ OT::" #PythonClass) OT::PythonClass;
%enddef
