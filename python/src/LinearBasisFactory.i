// SWIG file LinearBasisFactory.i

%{
#include "LinearBasisFactory.hxx"
%}

%include LinearBasisFactory_doc.i

%include LinearBasisFactory.hxx
namespace OT { %extend LinearBasisFactory { LinearBasisFactory(const LinearBasisFactory & other) { return new OT::LinearBasisFactory(other); } } }
