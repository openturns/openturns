// SWIG file ImportanceSamplingExperiment.i

%{
#include "openturns/ImportanceSamplingExperiment.hxx"
%}

%include ImportanceSamplingExperiment_doc.i

%include openturns/ImportanceSamplingExperiment.hxx
namespace OT { %extend ImportanceSamplingExperiment { ImportanceSamplingExperiment(const ImportanceSamplingExperiment & other) { return new OT::ImportanceSamplingExperiment(other); } } }
