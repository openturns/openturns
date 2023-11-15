// SWIG file LHSExperiment.i

%{
#include "openturns/LHSExperiment.hxx"
%}

%include LHSExperiment_doc.i

%ignore OT::LHSExperiment::generateStandard;

%copyctor OT::LHSExperiment;

%include openturns/LHSExperiment.hxx
