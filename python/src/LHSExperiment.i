// SWIG file LHSExperiment.i

%{
#include "LHSExperiment.hxx"
%}

%include LHSExperiment_doc.i

%include LHSExperiment.hxx
namespace OT { %extend LHSExperiment { LHSExperiment(const LHSExperiment & other) { return new OT::LHSExperiment(other); } } }
