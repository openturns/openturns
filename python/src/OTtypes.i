// SWIG file OTtypes.i

%module types

%include std_string.i
%include std_map.i
%include std_complex.i

%include OT.hxx
%include OTtypes.hxx
%include OTdebug.h

%{
#include "OT.hxx"
#include "PythonWrappingFunctions.hxx"
%}

%typemap(in) UnsignedInteger {
  $1 = OT::convert< OT::_PyInt_, OT::UnsignedInteger >($input);
}

%apply UnsignedInteger { OT::Unsigned64BitsInteger };

%typemap(in) NumericalScalar {
  $1 = OT::convert< OT::_PyFloat_, OT::NumericalScalar >($input);
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
