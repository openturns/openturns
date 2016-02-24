// SWIG file JacobiFactory.i

%{
#include "openturns/JacobiFactory.hxx"
%}

%include JacobiFactory_doc.i

%include openturns/JacobiFactory.hxx
namespace OT { %extend JacobiFactory { JacobiFactory(const JacobiFactory & other) { return new OT::JacobiFactory(other); } } }
