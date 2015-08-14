// SWIG file ChiFactory.i

%{
#include "ChiFactory.hxx"
%}

%include ChiFactory_doc.i

%include ChiFactory.hxx
namespace OT { %extend ChiFactory { ChiFactory(const ChiFactory & other) { return new OT::ChiFactory(other); } } }
