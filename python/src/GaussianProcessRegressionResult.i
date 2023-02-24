// SWIG file GaussianProcessRegressionResult.i

%{
#include "openturns/GaussianProcessRegressionResult.hxx"
%}

%include GaussianProcessRegressionResult_doc.i

%include openturns/GaussianProcessRegressionResult.hxx

namespace OT{ %extend GaussianProcessRegressionResult { GaussianProcessRegressionResult(const GaussianProcessRegressionResult & other) { return new OT::GaussianProcessRegressionResult(other); } } }
