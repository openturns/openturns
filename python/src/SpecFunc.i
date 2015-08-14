// SWIG file SpecFunc.i

%{
#include "SpecFunc.hxx"
%}

%include SpecFunc_doc.i

%nodefaultctor SpecFunc;

// available in math module
%ignore OT::SpecFunc::IsInf;
%ignore OT::SpecFunc::IsNan;

// available in cmath module
%ignore OT::SpecFunc::Acosh;
%ignore OT::SpecFunc::Asinh;
%ignore OT::SpecFunc::Atanh;

%include SpecFunc.hxx
