// SWIG file LinearEnumerateFunction.i

%{
#include "openturns/LinearEnumerateFunction.hxx"
%}

%include LinearEnumerateFunction_doc.i

%include openturns/LinearEnumerateFunction.hxx
namespace OT { %extend LinearEnumerateFunction { LinearEnumerateFunction(const LinearEnumerateFunction & other) { return new OT::LinearEnumerateFunction(other); } } }
