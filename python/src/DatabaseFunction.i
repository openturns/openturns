// SWIG file DatabaseFunction.i

%{
#include "openturns/DatabaseFunction.hxx"
%}

%include DatabaseFunction_doc.i

%include openturns/DatabaseFunction.hxx
namespace OT { %extend DatabaseFunction { DatabaseFunction(const DatabaseFunction & other) { return new OT::DatabaseFunction(other); } } }
