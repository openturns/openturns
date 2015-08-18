// SWIG file PostAnalyticalControlledImportanceSampling.i

%{
#include "PostAnalyticalControlledImportanceSampling.hxx"
%}

%include PostAnalyticalControlledImportanceSampling_doc.i

%include PostAnalyticalControlledImportanceSampling.hxx
namespace OT{ %extend PostAnalyticalControlledImportanceSampling { PostAnalyticalControlledImportanceSampling(const PostAnalyticalControlledImportanceSampling & other) { return new OT::PostAnalyticalControlledImportanceSampling(other); } } }
