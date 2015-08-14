// SWIG file LogUniformFactory.i

%{
#include "LogUniformFactory.hxx"
%}

%include LogUniformFactory_doc.i

%include LogUniformFactory.hxx
namespace OT { %extend LogUniformFactory { LogUniformFactory(const LogUniformFactory & other) { return new OT::LogUniformFactory(other); } } }
