// SWIG file MemoizeEvaluation.i

%{
#include "openturns/MemoizeEvaluation.hxx"
%}

%include MemoizeEvaluation_doc.i

%include openturns/MemoizeEvaluation.hxx
namespace OT{ %extend MemoizeEvaluation { MemoizeEvaluation(const MemoizeEvaluation & other) { return new OT::MemoizeEvaluation(other); } } }
