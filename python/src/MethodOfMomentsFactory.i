// SWIG file MethodOfMomentsFactory.i

%{
#include "openturns/MethodOfMomentsFactory.hxx"
%}

%include MethodOfMomentsFactory_doc.i

%ignore OT::MethodOfMomentsFactory::setParallel;
%ignore OT::MethodOfMomentsFactory::buildParameter;

%copyctor OT::MethodOfMomentsFactory;

%include openturns/MethodOfMomentsFactory.hxx
