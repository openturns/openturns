// SWIG file GaussProductExperiment.i

%{
#include "GaussProductExperiment.hxx"
%}

%include GaussProductExperiment_doc.i

%include GaussProductExperiment.hxx
namespace OT { %extend GaussProductExperiment { GaussProductExperiment(const GaussProductExperiment & other) { return new OT::GaussProductExperiment(other); } } }
