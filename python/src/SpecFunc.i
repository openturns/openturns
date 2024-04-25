// SWIG file SpecFunc.i

%{
#include "openturns/SpecFunc.hxx"
%}

%include SpecFunc_doc.i

%ignore OT::SpecFunc::BinomialCoefficient; // math.comb

%include openturns/SpecFunc.hxx
