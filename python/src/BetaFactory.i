// SWIG file BetaFactory.i

%{
#include "BetaFactory.hxx"
%}

%include BetaFactory_doc.i

%include BetaFactory.hxx
namespace OT { %extend BetaFactory { BetaFactory(const BetaFactory & other) { return new OT::BetaFactory(other); } } }
