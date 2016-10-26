// SWIG file KarhunenLoeveAlgorithmImplementation.i

%{
#include "openturns/KarhunenLoeveAlgorithmImplementation.hxx"
%}

%include KarhunenLoeveAlgorithmImplementation_doc.i

%include openturns/KarhunenLoeveAlgorithmImplementation.hxx
namespace OT { %extend KarhunenLoeveAlgorithmImplementation { KarhunenLoeveAlgorithmImplementation(const KarhunenLoeveAlgorithmImplementation & other) { return new OT::KarhunenLoeveAlgorithmImplementation(other); } } }
