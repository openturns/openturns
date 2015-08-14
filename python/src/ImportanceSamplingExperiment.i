// SWIG file ImportanceSamplingExperiment.i

%{
#include "ImportanceSamplingExperiment.hxx"
%}

%include ImportanceSamplingExperiment_doc.i

%include ImportanceSamplingExperiment.hxx
namespace OT { %extend ImportanceSamplingExperiment { ImportanceSamplingExperiment(const ImportanceSamplingExperiment & other) { return new OT::ImportanceSamplingExperiment(other); } } }
