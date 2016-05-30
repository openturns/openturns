// SWIG file SoizeGhanemFactory.i

%{
#include "openturns/SoizeGhanemFactory.hxx"
%}

%include SoizeGhanemFactory_doc.i

%include openturns/SoizeGhanemFactory.hxx
namespace OT { %extend SoizeGhanemFactory { SoizeGhanemFactory(const SoizeGhanemFactory & other) { return new OT::SoizeGhanemFactory(other); } } }
