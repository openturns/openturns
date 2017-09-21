// SWIG file SobolIndicesExperiment.i

%{
#include "openturns/SobolIndicesExperiment.hxx"
%}

%include SobolIndicesExperiment_doc.i

%include openturns/SobolIndicesExperiment.hxx
namespace OT { %extend SobolIndicesExperiment { SobolIndicesExperiment(const SobolIndicesExperiment & other) { return new OT::SobolIndicesExperiment(other); } } }
