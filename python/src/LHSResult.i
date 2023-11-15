// SWIG file

%{
#include "openturns/LHSResult.hxx"
%}

%include LHSResult_doc.i

%ignore OT::LHSResult::add;

%copyctor OT::LHSResult;

%include openturns/LHSResult.hxx
