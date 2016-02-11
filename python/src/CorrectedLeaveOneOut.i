// SWIG file CorrectedLeaveOneOut.i

%{
#include "openturns/CorrectedLeaveOneOut.hxx"
%}

%include CorrectedLeaveOneOut_doc.i

%include openturns/CorrectedLeaveOneOut.hxx
namespace OT { %extend CorrectedLeaveOneOut { CorrectedLeaveOneOut(const CorrectedLeaveOneOut & other) { return new OT::CorrectedLeaveOneOut(other); } } }
