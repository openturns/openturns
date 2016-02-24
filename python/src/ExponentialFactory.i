// SWIG file ExponentialFactory.i

%{
#include "openturns/ExponentialFactory.hxx"
%}

%include ExponentialFactory_doc.i

%include openturns/ExponentialFactory.hxx
namespace OT { %extend ExponentialFactory { ExponentialFactory(const ExponentialFactory & other) { return new OT::ExponentialFactory(other); } } }
