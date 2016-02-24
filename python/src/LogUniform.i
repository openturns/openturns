// SWIG file LogUniform.i

%{
#include "openturns/LogUniform.hxx"
%}

%include LogUniform_doc.i

%include openturns/LogUniform.hxx
namespace OT { %extend LogUniform { LogUniform(const LogUniform & other) { return new OT::LogUniform(other); } } }
