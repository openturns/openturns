// SWIG file MeixnerDistribution.i

%{
#include "MeixnerDistribution.hxx"
%}

%include MeixnerDistribution_doc.i

%include MeixnerDistribution.hxx
namespace OT { %extend MeixnerDistribution { MeixnerDistribution(const MeixnerDistribution & other) { return new OT::MeixnerDistribution(other); } } }
