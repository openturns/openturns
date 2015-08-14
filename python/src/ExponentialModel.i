// SWIG file ExponentialModel.i

%{
#include "ExponentialModel.hxx"
%}

%include ExponentialModel_doc.i

%include ExponentialModel.hxx

namespace OT { %extend ExponentialModel { ExponentialModel(const ExponentialModel & other) { return new OT::ExponentialModel(other); } } }
