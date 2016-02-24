// SWIG file ExponentialCauchy.i

%{
#include "openturns/ExponentialCauchy.hxx"
%}

%include ExponentialCauchy_doc.i

%include openturns/ExponentialCauchy.hxx
namespace OT { %extend ExponentialCauchy { ExponentialCauchy(const ExponentialCauchy & other) { return new OT::ExponentialCauchy(other); } } }
