// SWIG file BoundingVolumeHierarchy.i

%{
#include "openturns/BoundingVolumeHierarchy.hxx"
%}

%include BoundingVolumeHierarchy_doc.i

%include openturns/BoundingVolumeHierarchy.hxx
namespace OT { %extend BoundingVolumeHierarchy { BoundingVolumeHierarchy(const BoundingVolumeHierarchy & other) { return new OT::BoundingVolumeHierarchy(other); } } }
