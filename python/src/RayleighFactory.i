// SWIG file RayleighFactory.i

%{
#include "RayleighFactory.hxx"
%}

%include RayleighFactory_doc.i

%include RayleighFactory.hxx
namespace OT { %extend RayleighFactory { RayleighFactory(const RayleighFactory & other) { return new OT::RayleighFactory(other); } } }
