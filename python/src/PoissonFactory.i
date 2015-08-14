// SWIG file PoissonFactory.i

%{
#include "PoissonFactory.hxx"
%}

%include PoissonFactory_doc.i

%include PoissonFactory.hxx
namespace OT { %extend PoissonFactory { PoissonFactory(const PoissonFactory & other) { return new OT::PoissonFactory(other); } } }
