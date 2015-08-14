// SWIG file DiracFactory.i

%{
#include "DiracFactory.hxx"
%}

%include DiracFactory_doc.i

%include DiracFactory.hxx
namespace OT { %extend DiracFactory { DiracFactory(const DiracFactory & other) { return new OT::DiracFactory(other); } } }
