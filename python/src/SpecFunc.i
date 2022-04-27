// SWIG file SpecFunc.i

%{
#include "openturns/SpecFunc.hxx"
%}

%include SpecFunc_doc.i

// available in math module
%ignore OT::SpecFunc::IsInf;
%ignore OT::SpecFunc::IsNaN;

// available in cmath module
%ignore OT::SpecFunc::Acosh;
%ignore OT::SpecFunc::Asinh;
%ignore OT::SpecFunc::Atanh;

%include openturns/SpecFunc.hxx
