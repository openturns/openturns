// SWIG file BasisFactory.i

%{
#include "BasisFactory.hxx"
%}

%include BasisFactory_doc.i

%include BasisFactory.hxx
namespace OT { %extend BasisFactory { BasisFactory(const BasisFactory & other) { return new OT::BasisFactory(other); } } }
