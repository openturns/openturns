// SWIG file ProductDistribution.i

%{
#include "ProductDistribution.hxx"
%}

%include ProductDistribution_doc.i

%include ProductDistribution.hxx
namespace OT { %extend ProductDistribution { ProductDistribution(const ProductDistribution & other) { return new OT::ProductDistribution(other); } } }
