// SWIG file ExpertMixture.i

%{
#include "ExpertMixture.hxx"
%}

%include ExpertMixture_doc.i

%include ExpertMixture.hxx
namespace OT { %extend ExpertMixture { ExpertMixture(const ExpertMixture & other) { return new OT::ExpertMixture(other); } } }
