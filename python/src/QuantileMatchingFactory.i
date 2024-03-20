// SWIG file QuantileMatchingFactory.i

%{
#include "openturns/QuantileMatchingFactory.hxx"
%}

%include QuantileMatchingFactory_doc.i

%ignore OT::QuantileMatchingFactory::setParallel;
%ignore OT::QuantileMatchingFactory::buildParameter;

%copyctor OT::QuantileMatchingFactory;

%include openturns/QuantileMatchingFactory.hxx
