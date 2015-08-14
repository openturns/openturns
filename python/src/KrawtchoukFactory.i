// SWIG file KrawtchoukFactory.i

%{
#include "KrawtchoukFactory.hxx"
%}

%include KrawtchoukFactory_doc.i

%include KrawtchoukFactory.hxx
namespace OT { %extend KrawtchoukFactory { KrawtchoukFactory(const KrawtchoukFactory & other) { return new OT::KrawtchoukFactory(other); } } }
