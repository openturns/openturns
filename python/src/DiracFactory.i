// SWIG file DiracFactory.i

%{
#include "openturns/DiracFactory.hxx"
%}

%include DiracFactory_doc.i

%include openturns/DiracFactory.hxx
namespace OT { %extend DiracFactory { DiracFactory(const DiracFactory & other) { return new OT::DiracFactory(other); } } }
