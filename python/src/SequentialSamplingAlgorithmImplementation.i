// SWIG file SequentialSamplingAlgorithmImplementation.i

%{
#include "openturns/SequentialSamplingAlgorithmImplementation.hxx"
%}

%include SequentialSamplingAlgorithmImplementation_doc.i

%ignore OT::SequentialSamplingAlgorithmImplementation::getSelectionHistory;
%ignore OT::SequentialSamplingAlgorithmImplementation::getErrorHistory;

%copyctor OT::SequentialSamplingAlgorithmImplementation;

%include openturns/SequentialSamplingAlgorithmImplementation.hxx
