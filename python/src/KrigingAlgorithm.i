// SWIG file KrigingAlgorithm.i

%{
#include "openturns/KrigingAlgorithm.hxx"
%}

%include KrigingAlgorithm_doc.i

%include openturns/KrigingAlgorithm.hxx

namespace OT{ %extend KrigingAlgorithm { KrigingAlgorithm(const KrigingAlgorithm & other) { return new OT::KrigingAlgorithm(other); } } }
