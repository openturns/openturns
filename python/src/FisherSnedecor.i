// SWIG file FisherSnedecor.i

%{
#include "FisherSnedecor.hxx"
%}

%include FisherSnedecor_doc.i

%include FisherSnedecor.hxx
namespace OT { %extend FisherSnedecor { FisherSnedecor(const FisherSnedecor & other) { return new OT::FisherSnedecor(other); } } }
