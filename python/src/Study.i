// SWIG file Study.i

%{
#include "openturns/Study.hxx"
%}

%ignore OT::Study::operator=(const Study &);

%include Study_doc.i

%copyctor OT::Study;

%include openturns/Study.hxx
