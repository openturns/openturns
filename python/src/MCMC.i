// SWIG file MCMC.i

%{
#include "MCMC.hxx"
%}

%include MCMC_doc.i

%include MCMC.hxx
namespace OT { %extend MCMC { MCMC(const MCMC & other) { return new OT::MCMC(other); } } }
