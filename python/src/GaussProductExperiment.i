// SWIG file GaussProductExperiment.i

%{
#include "openturns/GaussProductExperiment.hxx"
%}

%include GaussProductExperiment_doc.i

%include openturns/GaussProductExperiment.hxx
namespace OT { %extend GaussProductExperiment { GaussProductExperiment(const GaussProductExperiment & other) { return new OT::GaussProductExperiment(other); } } }
