// SWIG file NearestPointProblem.i

%{
#include "openturns/NearestPointProblem.hxx"
%}

%include NearestPointProblem_doc.i

%include openturns/NearestPointProblem.hxx
namespace OT{ %extend NearestPointProblem { NearestPointProblem(const NearestPointProblem & other) { return new OT::NearestPointProblem(other); } } }
