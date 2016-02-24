// SWIG file SORMResult.i

%{
#include "openturns/SORMResult.hxx"
%}

%include SORMResult_doc.i

%include openturns/SORMResult.hxx
namespace OT{ %extend SORMResult { SORMResult(const SORMResult & other) { return new OT::SORMResult(other); } } }
