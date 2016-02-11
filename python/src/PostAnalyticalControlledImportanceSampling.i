// SWIG file PostAnalyticalControlledImportanceSampling.i

%{
#include "openturns/PostAnalyticalControlledImportanceSampling.hxx"
%}

%include PostAnalyticalControlledImportanceSampling_doc.i

%include openturns/PostAnalyticalControlledImportanceSampling.hxx
namespace OT{ %extend PostAnalyticalControlledImportanceSampling { PostAnalyticalControlledImportanceSampling(const PostAnalyticalControlledImportanceSampling & other) { return new OT::PostAnalyticalControlledImportanceSampling(other); } } }
