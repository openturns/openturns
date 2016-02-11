// SWIG file LogUniformFactory.i

%{
#include "openturns/LogUniformFactory.hxx"
%}

%include LogUniformFactory_doc.i

%include openturns/LogUniformFactory.hxx
namespace OT { %extend LogUniformFactory { LogUniformFactory(const LogUniformFactory & other) { return new OT::LogUniformFactory(other); } } }
