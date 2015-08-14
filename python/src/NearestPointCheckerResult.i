// SWIG file NearestPointCheckerResult.i

%{
#include "NearestPointCheckerResult.hxx"
%}

%include NearestPointCheckerResult.hxx
namespace OT{ %extend NearestPointCheckerResult { NearestPointCheckerResult(const NearestPointCheckerResult & other) { return new OT::NearestPointCheckerResult(other); } } }
