// SWIG file KrigingAlgorithm.i

%{
#include "KrigingAlgorithm.hxx"
%}

%include KrigingAlgorithm_doc.i

%include KrigingAlgorithm.hxx

namespace OT{ %extend KrigingAlgorithm { KrigingAlgorithm(const KrigingAlgorithm & other) { return new OT::KrigingAlgorithm(other); } } }
