// SWIG file ARMAFactoryImplementation.i

%{
#include "openturns/ARMAFactoryImplementation.hxx"
%}

%include ARMAFactoryImplementation_doc.i

%include openturns/ARMAFactoryImplementation.hxx
namespace OT { %extend ARMAFactoryImplementation { ARMAFactoryImplementation(const ARMAFactoryImplementation & other) { return new OT::ARMAFactoryImplementation(other); } } }
