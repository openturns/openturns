// SWIG file TaylorExpansionMoments.i

%{
#include "openturns/TaylorExpansionMoments.hxx"
%}

%include TaylorExpansionMoments_doc.i

%include openturns/TaylorExpansionMoments.hxx
namespace OT{ %extend TaylorExpansionMoments { TaylorExpansionMoments(const TaylorExpansionMoments & other) { return new OT::TaylorExpansionMoments(other); } } }

