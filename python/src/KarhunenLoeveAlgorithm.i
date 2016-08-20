// SWIG file KarhunenLoeveAlgorithm.i

%{
#include "openturns/KarhunenLoeveAlgorithm.hxx"
%}

%include KarhunenLoeveAlgorithm_doc.i

OTTypedInterfaceObjectHelper(KarhunenLoeveAlgorithm)

%include openturns/KarhunenLoeveAlgorithm.hxx
namespace OT { %extend KarhunenLoeveAlgorithm { KarhunenLoeveAlgorithm(const KarhunenLoeveAlgorithm & other) { return new OT::KarhunenLoeveAlgorithm(other); } } }
