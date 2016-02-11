// SWIG file ExpertMixture.i

%{
#include "openturns/ExpertMixture.hxx"
%}

%include ExpertMixture_doc.i

%include openturns/ExpertMixture.hxx
namespace OT { %extend ExpertMixture { ExpertMixture(const ExpertMixture & other) { return new OT::ExpertMixture(other); } } }
