// SWIG file SORMResult.i

%{
#include "SORMResult.hxx"
%}

%include SORMResult_doc.i

%include SORMResult.hxx
namespace OT{ %extend SORMResult { SORMResult(const SORMResult & other) { return new OT::SORMResult(other); } } }
