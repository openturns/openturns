// SWIG file KarhunenLoeveResult.i

%{
#include "openturns/KarhunenLoeveResult.hxx"
%}

%include KarhunenLoeveResult_doc.i

OTTypedInterfaceObjectHelper(KarhunenLoeveResult)

%copyctor OT::KarhunenLoeveResult;

%include openturns/KarhunenLoeveResult.hxx
