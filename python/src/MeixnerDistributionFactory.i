// SWIG file MeixnerDistributionFactory.i

%{
#include "openturns/MeixnerDistributionFactory.hxx"
%}

%include MeixnerDistributionFactory_doc.i

%include openturns/MeixnerDistributionFactory.hxx
namespace OT { %extend MeixnerDistributionFactory { MeixnerDistributionFactory(const MeixnerDistributionFactory & other) { return new OT::MeixnerDistributionFactory(other); } } }
