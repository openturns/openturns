// SWIG file SmolyakExperiment.i

%{
#include "openturns/SmolyakExperiment.hxx"
%}

%include SmolyakExperiment_doc.i

%ignore OT::SmolyakExperiment::ComparePointsApproximately;

%copyctor OT::SmolyakExperiment;

%include openturns/SmolyakExperiment.hxx
