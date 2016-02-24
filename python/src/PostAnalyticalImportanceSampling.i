// SWIG file PostAnalyticalImportanceSampling.i

%{
#include "openturns/PostAnalyticalImportanceSampling.hxx"
%}

%include PostAnalyticalImportanceSampling_doc.i

%include openturns/PostAnalyticalImportanceSampling.hxx
namespace OT{ %extend PostAnalyticalImportanceSampling { PostAnalyticalImportanceSampling(const PostAnalyticalImportanceSampling & other) { return new OT::PostAnalyticalImportanceSampling(other); } } }
