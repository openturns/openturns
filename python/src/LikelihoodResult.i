// SWIG file LikelihoodResult.i

%{
#include "openturns/LikelihoodResult.hxx"
%}

%include LikelihoodResult_doc.i

%include openturns/LikelihoodResult.hxx
namespace OT { %extend LikelihoodResult { LikelihoodResult(const LikelihoodResult & other) { return new OT::LikelihoodResult(other); } } }
