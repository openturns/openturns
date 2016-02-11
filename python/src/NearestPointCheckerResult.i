// SWIG file NearestPointCheckerResult.i

OTAutodoc(NearestPointCheckerResult)

%{
#include "openturns/NearestPointCheckerResult.hxx"
%}

%include openturns/NearestPointCheckerResult.hxx
namespace OT{ %extend NearestPointCheckerResult { NearestPointCheckerResult(const NearestPointCheckerResult & other) { return new OT::NearestPointCheckerResult(other); } } }
