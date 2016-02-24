// SWIG file FORMResult.i

%{
#include "openturns/FORMResult.hxx"
%}

%include FORMResult_doc.i

%include openturns/FORMResult.hxx
namespace OT{ %extend FORMResult { FORMResult(const FORMResult & other) { return new OT::FORMResult(other); } } }
