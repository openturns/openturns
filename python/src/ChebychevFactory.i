// SWIG file ChebychevFactory.i

%{
#include "openturns/ChebychevFactory.hxx"
%}

%include ChebychevFactory_doc.i

%include openturns/ChebychevFactory.hxx
namespace OT { %extend ChebychevFactory { ChebychevFactory(const ChebychevFactory & other) { return new OT::ChebychevFactory(other); } } }
