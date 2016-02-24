// SWIG file RiceFactory.i

%{
#include "openturns/RiceFactory.hxx"
%}

%include RiceFactory_doc.i

%include openturns/RiceFactory.hxx
namespace OT { %extend RiceFactory { RiceFactory(const RiceFactory & other) { return new OT::RiceFactory(other); } } }
