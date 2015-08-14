// SWIG file LogUniform.i

%{
#include "LogUniform.hxx"
%}

%include LogUniform_doc.i

%include LogUniform.hxx
namespace OT { %extend LogUniform { LogUniform(const LogUniform & other) { return new OT::LogUniform(other); } } }
