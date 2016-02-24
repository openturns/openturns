// SWIG file BurrFactory.i

%{
#include "openturns/BurrFactory.hxx"
%}

%include BurrFactory_doc.i

%include openturns/BurrFactory.hxx
namespace OT { %extend BurrFactory { BurrFactory(const BurrFactory & other) { return new OT::BurrFactory(other); } } }
