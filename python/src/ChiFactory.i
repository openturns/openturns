// SWIG file ChiFactory.i

%{
#include "openturns/ChiFactory.hxx"
%}

%include ChiFactory_doc.i

%include openturns/ChiFactory.hxx
namespace OT { %extend ChiFactory { ChiFactory(const ChiFactory & other) { return new OT::ChiFactory(other); } } }
