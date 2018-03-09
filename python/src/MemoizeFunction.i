// SWIG file MemoizeFunction.i

%{
#include "openturns/MemoizeFunction.hxx"
%}

%include MemoizeFunction_doc.i

%include openturns/MemoizeFunction.hxx
namespace OT{ %extend MemoizeFunction { MemoizeFunction(const MemoizeFunction & other) { return new OT::MemoizeFunction(other); } } }
