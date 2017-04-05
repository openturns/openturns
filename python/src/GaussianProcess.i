// SWIG file GaussianProcess.i

%{
#include "openturns/GaussianProcess.hxx"
%}

%include GaussianProcess_doc.i

%include openturns/GaussianProcess.hxx
namespace OT { %extend GaussianProcess { GaussianProcess(const GaussianProcess & other) { return new OT::GaussianProcess(other); } } }


