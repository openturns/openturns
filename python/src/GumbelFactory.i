// SWIG file GumbelFactory.i

%{
#include "GumbelFactory.hxx"
%}

%include GumbelFactory_doc.i

%include GumbelFactory.hxx
namespace OT { %extend GumbelFactory { GumbelFactory(const GumbelFactory & other) { return new OT::GumbelFactory(other); } } }
