// SWIG file ARMAFactoryImplementation.i

%{
#include "ARMAFactoryImplementation.hxx"
%}

%include ARMAFactoryImplementation_doc.i

%include ARMAFactoryImplementation.hxx
namespace OT { %extend ARMAFactoryImplementation { ARMAFactoryImplementation(const ARMAFactoryImplementation & other) { return new OT::ARMAFactoryImplementation(other); } } }
