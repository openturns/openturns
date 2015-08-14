// SWIG file HermiteFactory.i

%{
#include "HermiteFactory.hxx"
%}

%include HermiteFactory_doc.i

%include HermiteFactory.hxx
namespace OT { %extend HermiteFactory { HermiteFactory(const HermiteFactory & other) { return new OT::HermiteFactory(other); } } }
