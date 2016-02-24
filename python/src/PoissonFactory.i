// SWIG file PoissonFactory.i

%{
#include "openturns/PoissonFactory.hxx"
%}

%include PoissonFactory_doc.i

%include openturns/PoissonFactory.hxx
namespace OT { %extend PoissonFactory { PoissonFactory(const PoissonFactory & other) { return new OT::PoissonFactory(other); } } }
