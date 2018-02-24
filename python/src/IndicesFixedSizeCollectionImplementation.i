// SWIG file IndicesFixedSizeCollectionImplementation.i

%{
#include "openturns/IndicesFixedSizeCollectionImplementation.hxx"
%}

%include openturns/IndicesFixedSizeCollectionImplementation.hxx
namespace OT{ %extend IndicesFixedSizeCollectionImplementation { IndicesFixedSizeCollectionImplementation(const IndicesFixedSizeCollectionImplementation & other) { return new OT::IndicesFixedSizeCollectionImplementation(other); } } }
