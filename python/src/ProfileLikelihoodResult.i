// SWIG file ProfileLikelihoodResult.i

%{
#include "openturns/ProfileLikelihoodResult.hxx"
%}

%include ProfileLikelihoodResult_doc.i

%include openturns/ProfileLikelihoodResult.hxx
namespace OT { %extend ProfileLikelihoodResult { ProfileLikelihoodResult(const ProfileLikelihoodResult & other) { return new OT::ProfileLikelihoodResult(other); } } }
