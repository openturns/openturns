// SWIG file LinearBasisFactory.i

%{
#include "openturns/LinearBasisFactory.hxx"
%}

%include LinearBasisFactory_doc.i

%include openturns/LinearBasisFactory.hxx
namespace OT { %extend LinearBasisFactory { LinearBasisFactory(const LinearBasisFactory & other) { return new OT::LinearBasisFactory(other); } } }
