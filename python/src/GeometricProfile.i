// SWIG file

%{
#include "openturns/GeometricProfile.hxx"
%}

%include GeometricProfile_doc.i

%include openturns/GeometricProfile.hxx
namespace OT { %extend GeometricProfile { GeometricProfile(const GeometricProfile & other) { return new OT::GeometricProfile(other); } } }
