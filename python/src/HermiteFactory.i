// SWIG file HermiteFactory.i

%{
#include "openturns/HermiteFactory.hxx"
%}

%include HermiteFactory_doc.i

%include openturns/HermiteFactory.hxx
namespace OT { %extend HermiteFactory { HermiteFactory(const HermiteFactory & other) { return new OT::HermiteFactory(other); } } }
