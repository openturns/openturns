// SWIG file NearestPointChecker.i

OTAutodoc(NearestPointChecker)

%{
#include "openturns/NearestPointChecker.hxx"
%}

%include openturns/NearestPointChecker.hxx
namespace OT{ %extend NearestPointChecker { NearestPointChecker(const NearestPointChecker & other) { return new OT::NearestPointChecker(other); } } }
