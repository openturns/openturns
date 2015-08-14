// SWIG file MeixnerDistributionFactory.i

%{
#include "MeixnerDistributionFactory.hxx"
%}

%include MeixnerDistributionFactory_doc.i

%include MeixnerDistributionFactory.hxx
namespace OT { %extend MeixnerDistributionFactory { MeixnerDistributionFactory(const MeixnerDistributionFactory & other) { return new OT::MeixnerDistributionFactory(other); } } }
