// SWIG file SkellamFactory.i

%{
#include "openturns/SkellamFactory.hxx"
%}

%include SkellamFactory_doc.i

%include openturns/SkellamFactory.hxx
namespace OT { %extend SkellamFactory { SkellamFactory(const SkellamFactory & other) { return new OT::SkellamFactory(other); } } }
