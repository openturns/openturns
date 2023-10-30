// SWIG file LHSExperiment.i

%{
#include "openturns/LHSExperiment.hxx"
%}

%include LHSExperiment_doc.i

%ignore OT::LHSExperiment::generateStandard;

%include openturns/LHSExperiment.hxx
namespace OT { %extend LHSExperiment { LHSExperiment(const LHSExperiment & other) { return new OT::LHSExperiment(other); } } }
