// SWIG file CharlierFactory.i

%{
#include "openturns/CharlierFactory.hxx"
%}

%include CharlierFactory_doc.i

%include openturns/CharlierFactory.hxx
namespace OT { %extend CharlierFactory { CharlierFactory(const CharlierFactory & other) { return new OT::CharlierFactory(other); } } }
