// SWIG file ConstantBasisFactory.i

%{
#include "ConstantBasisFactory.hxx"
%}

%include ConstantBasisFactory_doc.i 

%include ConstantBasisFactory.hxx
namespace OT { %extend ConstantBasisFactory { ConstantBasisFactory(const ConstantBasisFactory & other) { return new OT::ConstantBasisFactory(other); } } }
