// SWIG file GaussianProcessRegression.i

%{
#include "openturns/GaussianProcessRegression.hxx"
%}

%include GaussianProcessRegression_doc.i

%include openturns/GaussianProcessRegression.hxx

namespace OT{ %extend GaussianProcessRegression { GaussianProcessRegression(const GaussianProcessRegression & other) { return new OT::GaussianProcessRegression(other); } } }
