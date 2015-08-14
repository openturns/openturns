// SWIG file PostAnalyticalImportanceSampling.i

%{
#include "PostAnalyticalImportanceSampling.hxx"
%}

%include PostAnalyticalImportanceSampling.hxx
namespace OT{ %extend PostAnalyticalImportanceSampling { PostAnalyticalImportanceSampling(const PostAnalyticalImportanceSampling & other) { return new OT::PostAnalyticalImportanceSampling(other); } } }
