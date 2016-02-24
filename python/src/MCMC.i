// SWIG file MCMC.i

%{
#include "openturns/MCMC.hxx"
%}

%include MCMC_doc.i

%include openturns/MCMC.hxx
namespace OT { %extend MCMC { MCMC(const MCMC & other) { return new OT::MCMC(other); } } }
