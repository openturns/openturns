// SWIG file PostAnalyticalControlledImportanceSampling.i

%{
#include "PostAnalyticalControlledImportanceSampling.hxx"
%}

%include PostAnalyticalControlledImportanceSampling.hxx
namespace OT{ %extend PostAnalyticalControlledImportanceSampling { PostAnalyticalControlledImportanceSampling(const PostAnalyticalControlledImportanceSampling & other) { return new OT::PostAnalyticalControlledImportanceSampling(other); } } }
