// SWIG file LaguerreFactory.i

%{
#include "LaguerreFactory.hxx"
%}

%include LaguerreFactory_doc.i

%include LaguerreFactory.hxx
namespace OT { %extend LaguerreFactory { LaguerreFactory(const LaguerreFactory & other) { return new OT::LaguerreFactory(other); } } }
