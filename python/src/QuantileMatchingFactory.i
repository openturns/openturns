// SWIG file QuantileMatchingFactory.i

%{
#include "openturns/QuantileMatchingFactory.hxx"
%}

%include QuantileMatchingFactory_doc.i

%ignore OT::QuantileMatchingFactory::setParallel;
%ignore OT::QuantileMatchingFactory::buildParameter;

%include openturns/QuantileMatchingFactory.hxx
namespace OT { %extend QuantileMatchingFactory { QuantileMatchingFactory(const QuantileMatchingFactory & other) { return new OT::QuantileMatchingFactory(other); } } }
