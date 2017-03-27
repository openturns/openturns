// SWIG file KarhunenLoeveSVDAlgorithm.i

%{
#include "openturns/KarhunenLoeveSVDAlgorithm.hxx"
%}

%include KarhunenLoeveSVDAlgorithm_doc.i

%include openturns/KarhunenLoeveSVDAlgorithm.hxx
namespace OT { %extend KarhunenLoeveSVDAlgorithm { KarhunenLoeveSVDAlgorithm(const KarhunenLoeveSVDAlgorithm & other) { return new OT::KarhunenLoeveSVDAlgorithm(other); } } }
