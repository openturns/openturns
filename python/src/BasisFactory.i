// SWIG file BasisFactory.i

%{
#include "openturns/BasisFactory.hxx"
%}

%include BasisFactory_doc.i

%include openturns/BasisFactory.hxx
namespace OT { %extend BasisFactory { BasisFactory(const BasisFactory & other) { return new OT::BasisFactory(other); } } }
