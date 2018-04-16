// SWIG file FractionalBrownianMotionModel.i

%{
#include "openturns/FractionalBrownianMotionModel.hxx"
%}

%include FractionalBrownianMotionModel_doc.i

%include openturns/FractionalBrownianMotionModel.hxx
namespace OT{ %extend FractionalBrownianMotionModel { FractionalBrownianMotionModel(const FractionalBrownianMotionModel & other) { return new OT::FractionalBrownianMotionModel(other); } } }
