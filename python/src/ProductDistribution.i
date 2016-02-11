// SWIG file ProductDistribution.i

%{
#include "openturns/ProductDistribution.hxx"
%}

%include ProductDistribution_doc.i

%include openturns/ProductDistribution.hxx
namespace OT { %extend ProductDistribution { ProductDistribution(const ProductDistribution & other) { return new OT::ProductDistribution(other); } } }
