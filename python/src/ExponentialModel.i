// SWIG file ExponentialModel.i

%{
#include "openturns/ExponentialModel.hxx"
%}

%include ExponentialModel_doc.i

%include openturns/ExponentialModel.hxx

namespace OT { %extend ExponentialModel { ExponentialModel(const ExponentialModel & other) { return new OT::ExponentialModel(other); } } }
