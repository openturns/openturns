// SWIG file MeixnerDistribution.i

%{
#include "openturns/MeixnerDistribution.hxx"
%}

%include MeixnerDistribution_doc.i

%include openturns/MeixnerDistribution.hxx
namespace OT { %extend MeixnerDistribution { MeixnerDistribution(const MeixnerDistribution & other) { return new OT::MeixnerDistribution(other); } } }
