// SWIG file KarhunenLoeveAlgorithm.i

%{
#include "openturns/KarhunenLoeveAlgorithm.hxx"
%}

%include KarhunenLoeveAlgorithm_doc.i

OTTypedInterfaceObjectHelper(KarhunenLoeveAlgorithm)

%copyctor OT::KarhunenLoeveAlgorithm;

%include openturns/KarhunenLoeveAlgorithm.hxx
