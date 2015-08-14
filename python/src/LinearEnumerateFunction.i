// SWIG file LinearEnumerateFunction.i

%{
#include "LinearEnumerateFunction.hxx"
%}

%include LinearEnumerateFunction_doc.i

%include LinearEnumerateFunction.hxx
namespace OT { %extend LinearEnumerateFunction { LinearEnumerateFunction(const LinearEnumerateFunction & other) { return new OT::LinearEnumerateFunction(other); } } }
