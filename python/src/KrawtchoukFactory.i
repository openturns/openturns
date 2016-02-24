// SWIG file KrawtchoukFactory.i

%{
#include "openturns/KrawtchoukFactory.hxx"
%}

%include KrawtchoukFactory_doc.i

%include openturns/KrawtchoukFactory.hxx
namespace OT { %extend KrawtchoukFactory { KrawtchoukFactory(const KrawtchoukFactory & other) { return new OT::KrawtchoukFactory(other); } } }
