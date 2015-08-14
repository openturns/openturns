// SWIG file FisherSnedecorFactory.i

%{
#include "FisherSnedecorFactory.hxx"
%}

%include FisherSnedecorFactory_doc.i

%include FisherSnedecorFactory.hxx
namespace OT { %extend FisherSnedecorFactory { FisherSnedecorFactory(const FisherSnedecorFactory & other) { return new OT::FisherSnedecorFactory(other); } } }
