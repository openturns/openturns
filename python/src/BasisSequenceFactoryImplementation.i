// SWIG file BasisSequenceFactoryImplementation.i

%{
#include "openturns/BasisSequenceFactoryImplementation.hxx"
%}

%include BasisSequenceFactoryImplementation_doc.i

%ignore OT::BasisSequenceFactoryImplementation::getSelectionHistory;

%copyctor OT::BasisSequenceFactoryImplementation;

%include openturns/BasisSequenceFactoryImplementation.hxx
