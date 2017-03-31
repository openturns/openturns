// SWIG file SparseMethod.i

%{
#include "openturns/SparseMethod.hxx"
%}

%include SparseMethod_doc.i

%include openturns/SparseMethod.hxx
namespace OT { %extend SparseMethod { SparseMethod(const SparseMethod & other) { return new OT::SparseMethod(other); } } }
