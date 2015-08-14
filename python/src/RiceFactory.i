// SWIG file RiceFactory.i

%{
#include "RiceFactory.hxx"
%}

%include RiceFactory_doc.i

%include RiceFactory.hxx
namespace OT { %extend RiceFactory { RiceFactory(const RiceFactory & other) { return new OT::RiceFactory(other); } } }
