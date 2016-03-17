// SWIG file

%{
#include "openturns/SubsetSamplingResult.hxx"
%}

%include SubsetSamplingResult_doc.i

%include openturns/SubsetSamplingResult.hxx
namespace OT { %extend SubsetSamplingResult { SubsetSamplingResult(const SubsetSamplingResult & other) { return new OT::SubsetSamplingResult(other); } } }
