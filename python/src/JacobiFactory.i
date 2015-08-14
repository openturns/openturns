// SWIG file JacobiFactory.i

%{
#include "JacobiFactory.hxx"
%}

%include JacobiFactory_doc.i

%include JacobiFactory.hxx
namespace OT { %extend JacobiFactory { JacobiFactory(const JacobiFactory & other) { return new OT::JacobiFactory(other); } } }
