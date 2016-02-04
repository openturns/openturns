// SWIG file NLopt.i

%{
#include "openturns/NLopt.hxx"
%}

%include NLopt_doc.i

%include openturns/NLopt.hxx

namespace OT { %extend NLopt { NLopt(const NLopt & other) { return new OT::NLopt(other); } } }

namespace OT { %extend SLSQP { SLSQP(const SLSQP & other) { return new OT::SLSQP(other); } } }
namespace OT { %extend LBFGS { LBFGS(const LBFGS & other) { return new OT::LBFGS(other); } } }
namespace OT { %extend MMA { MMA(const MMA & other) { return new OT::MMA(other); } } }
namespace OT { %extend CCSAQ { CCSAQ(const CCSAQ & other) { return new OT::CCSAQ(other); } } }
namespace OT { %extend COBYLANLOPT { COBYLANLOPT(const COBYLANLOPT & other) { return new OT::COBYLANLOPT(other); } } }
namespace OT { %extend BOBYQA { BOBYQA(const BOBYQA & other) { return new OT::BOBYQA(other); } } }
namespace OT { %extend NelderMead { NelderMead(const NelderMead & other) { return new OT::NelderMead(other); } } }
