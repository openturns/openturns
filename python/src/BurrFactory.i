// SWIG file BurrFactory.i

%{
#include "BurrFactory.hxx"
%}

%include BurrFactory_doc.i

%include BurrFactory.hxx
namespace OT { %extend BurrFactory { BurrFactory(const BurrFactory & other) { return new OT::BurrFactory(other); } } }
