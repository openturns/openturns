// SWIG file SmoothedUniform.i

%{
#include "openturns/SmoothedUniform.hxx"
%}

%include SmoothedUniform_doc.i

%include openturns/SmoothedUniform.hxx
namespace OT { %extend SmoothedUniform { SmoothedUniform(const SmoothedUniform & other) { return new OT::SmoothedUniform(other); } } }
