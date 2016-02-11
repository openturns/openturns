// SWIG file ConstantBasisFactory.i

%{
#include "openturns/ConstantBasisFactory.hxx"
%}

%include ConstantBasisFactory_doc.i 

%include openturns/ConstantBasisFactory.hxx
namespace OT { %extend ConstantBasisFactory { ConstantBasisFactory(const ConstantBasisFactory & other) { return new OT::ConstantBasisFactory(other); } } }
