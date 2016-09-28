// SWIG file NLopt.i

%{
#include "openturns/NLopt.hxx"
%}

%include NLopt_doc.i

%include openturns/NLopt.hxx

namespace OT { %extend NLopt { NLopt(const NLopt & other) { return new OT::NLopt(other); } } }

namespace OT { %extend SLSQP { SLSQP(const SLSQP & other) { return new OT::SLSQP(other); } } }
namespace OT { %extend LBFGS { LBFGS(const LBFGS & other) { return new OT::LBFGS(other); } } }
namespace OT { %extend NelderMead { NelderMead(const NelderMead & other) { return new OT::NelderMead(other); } } }
