// SWIG file TBB.i

%{
#include "openturns/TBB.hxx"
%}

%include TBB_doc.i

%ignore OT::TBBContext;

%ignore OT::TBB::Split;
%ignore OT::TBB_init;

%nodefaultctor TBB;

%include openturns/TBB.hxx
