// SWIG file FisherSnedecorFactory.i

%{
#include "openturns/FisherSnedecorFactory.hxx"
%}

%include FisherSnedecorFactory_doc.i

%include openturns/FisherSnedecorFactory.hxx
namespace OT { %extend FisherSnedecorFactory { FisherSnedecorFactory(const FisherSnedecorFactory & other) { return new OT::FisherSnedecorFactory(other); } } }
