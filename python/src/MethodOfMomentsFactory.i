// SWIG file MethodOfMomentsFactory.i

%{
#include "openturns/MethodOfMomentsFactory.hxx"
%}

%include MethodOfMomentsFactory_doc.i

%ignore OT::MethodOfMomentsFactory::setParallel;
%ignore OT::MethodOfMomentsFactory::buildParameter;

%include openturns/MethodOfMomentsFactory.hxx
namespace OT { %extend MethodOfMomentsFactory { MethodOfMomentsFactory(const MethodOfMomentsFactory & other) { return new OT::MethodOfMomentsFactory(other); } } }
