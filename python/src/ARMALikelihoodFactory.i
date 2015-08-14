// SWIG file ARMALikelihoodFactory.i

%{
#include "ARMALikelihoodFactory.hxx"
%}

%include ARMALikelihoodFactory_doc.i

%include ARMALikelihoodFactory.hxx

namespace OT { %extend ARMALikelihoodFactory { ARMALikelihoodFactory(const ARMALikelihoodFactory & other) { return new OT::ARMALikelihoodFactory(other); } } }
