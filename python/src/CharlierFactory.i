// SWIG file CharlierFactory.i

%{
#include "CharlierFactory.hxx"
%}

%include CharlierFactory_doc.i

%include CharlierFactory.hxx
namespace OT { %extend CharlierFactory { CharlierFactory(const CharlierFactory & other) { return new OT::CharlierFactory(other); } } }
