// SWIG file SkellamFactory.i

%{
#include "SkellamFactory.hxx"
%}

%include SkellamFactory_doc.i

%include SkellamFactory.hxx
namespace OT { %extend SkellamFactory { SkellamFactory(const SkellamFactory & other) { return new OT::SkellamFactory(other); } } }
