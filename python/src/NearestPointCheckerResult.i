// SWIG file NearestPointCheckerResult.i

OTAutodoc(NearestPointCheckerResult)

%{
#include "NearestPointCheckerResult.hxx"
%}

%include NearestPointCheckerResult.hxx
namespace OT{ %extend NearestPointCheckerResult { NearestPointCheckerResult(const NearestPointCheckerResult & other) { return new OT::NearestPointCheckerResult(other); } } }
