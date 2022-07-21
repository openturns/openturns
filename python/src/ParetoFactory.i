// SWIG file ParetoFactory.i

%{
#include "openturns/ParetoFactory.hxx"
%}

%include ParetoFactory_doc.i

%include openturns/ParetoFactory.hxx
namespace OT { %extend ParetoFactory { ParetoFactory(const ParetoFactory & other) { return new OT::ParetoFactory(other); } } }

