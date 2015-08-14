// SWIG file ExponentialFactory.i

%{
#include "ExponentialFactory.hxx"
%}

%include ExponentialFactory_doc.i

%include ExponentialFactory.hxx
namespace OT { %extend ExponentialFactory { ExponentialFactory(const ExponentialFactory & other) { return new OT::ExponentialFactory(other); } } }
