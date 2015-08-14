// SWIG file FORMResult.i

%{
#include "FORMResult.hxx"
%}

%include FORMResult_doc.i

%include FORMResult.hxx
namespace OT{ %extend FORMResult { FORMResult(const FORMResult & other) { return new OT::FORMResult(other); } } }
