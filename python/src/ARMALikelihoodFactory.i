// SWIG file ARMALikelihoodFactory.i

%{
#include "openturns/ARMALikelihoodFactory.hxx"
%}

%include ARMALikelihoodFactory_doc.i

%include openturns/ARMALikelihoodFactory.hxx

namespace OT { %extend ARMALikelihoodFactory { ARMALikelihoodFactory(const ARMALikelihoodFactory & other) { return new OT::ARMALikelihoodFactory(other); } } }
