// SWIG file UserDefinedMetropolisHastings.i

%{
#include "openturns/UserDefinedMetropolisHastings.hxx"
%}

%include UserDefinedMetropolisHastings_doc.i

%include openturns/UserDefinedMetropolisHastings.hxx
namespace OT { %extend UserDefinedMetropolisHastings { UserDefinedMetropolisHastings(const UserDefinedMetropolisHastings & other) { return new OT::UserDefinedMetropolisHastings(other); } } }

