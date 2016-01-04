// SWIG file NearestPointChecker.i

OTAutodoc(NearestPointChecker)

%{
#include "NearestPointChecker.hxx"
%}

%include NearestPointChecker.hxx
namespace OT{ %extend NearestPointChecker { NearestPointChecker(const NearestPointChecker & other) { return new OT::NearestPointChecker(other); } } }
