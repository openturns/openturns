// SWIG file RayleighFactory.i

%{
#include "openturns/RayleighFactory.hxx"
%}

%include RayleighFactory_doc.i

%include openturns/RayleighFactory.hxx
namespace OT { %extend RayleighFactory { RayleighFactory(const RayleighFactory & other) { return new OT::RayleighFactory(other); } } }
