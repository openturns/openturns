// SWIG file FisherSnedecor.i

%{
#include "openturns/FisherSnedecor.hxx"
%}

%include FisherSnedecor_doc.i

%include openturns/FisherSnedecor.hxx
namespace OT { %extend FisherSnedecor { FisherSnedecor(const FisherSnedecor & other) { return new OT::FisherSnedecor(other); } } }
