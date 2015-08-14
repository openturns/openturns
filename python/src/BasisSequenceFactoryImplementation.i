// SWIG file BasisSequenceFactoryImplementation.i

%{
#include "BasisSequenceFactoryImplementation.hxx"
%}

%include BasisSequenceFactoryImplementation_doc.i

%include BasisSequenceFactoryImplementation.hxx
namespace OT { %extend BasisSequenceFactoryImplementation { BasisSequenceFactoryImplementation(const BasisSequenceFactoryImplementation & other) { return new OT::BasisSequenceFactoryImplementation(other); } } }
