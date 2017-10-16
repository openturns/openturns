// SWIG file FrechetFactory.i

%{
#include "openturns/FrechetFactory.hxx"
%}

%include FrechetFactory_doc.i

%include openturns/FrechetFactory.hxx
namespace OT { %extend FrechetFactory { FrechetFactory(const FrechetFactory & other) { return new OT::FrechetFactory(other); } } }
