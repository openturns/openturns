// SWIG file

%{
#include "openturns/SubsetSampling.hxx"
%}

%include SubsetSampling_doc.i

%include openturns/SubsetSampling.hxx
namespace OT { %extend SubsetSampling { SubsetSampling(const SubsetSampling & other) { return new OT::SubsetSampling(other); } } }
