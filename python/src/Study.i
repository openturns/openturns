// SWIG file Study.i

%{
#include "openturns/Study.hxx"
%}

%ignore OT::Study::operator=(const Study &);

%include Study_doc.i

%copyctor OT::Study;

// for getLabels
%include "std_vector.i"
%template(_StringVector) std::vector<OT::String>;

%include openturns/Study.hxx
