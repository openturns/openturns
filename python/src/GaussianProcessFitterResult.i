// SWIG file GaussianProcessFitterResult.i

%{
#include "openturns/GaussianProcessFitterResult.hxx"
%}

%include GaussianProcessFitterResult_doc.i

%include openturns/GaussianProcessFitterResult.hxx

namespace OT{ %extend GaussianProcessFitterResult { GaussianProcessFitterResult(const GaussianProcessFitterResult & other) { return new OT::GaussianProcessFitterResult(other); } } }


