// SWIG file LinearModelAnalysis.i

%{
#include "openturns/LinearModelAnalysis.hxx"
%}

%include LinearModelAnalysis_doc.i

%include openturns/LinearModelAnalysis.hxx
namespace OT { %extend LinearModelAnalysis { LinearModelAnalysis(const LinearModelAnalysis & other) { return new OT::LinearModelAnalysis(other); } } }
