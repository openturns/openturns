// SWIG file BetaFactory.i

%{
#include "openturns/BetaFactory.hxx"
%}

%include BetaFactory_doc.i

%include openturns/BetaFactory.hxx
namespace OT { %extend BetaFactory { BetaFactory(const BetaFactory & other) { return new OT::BetaFactory(other); } } }
