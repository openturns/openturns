// SWIG file CorrectedLeaveOneOut.i

%{
#include "CorrectedLeaveOneOut.hxx"
%}

%include CorrectedLeaveOneOut_doc.i

%include CorrectedLeaveOneOut.hxx
namespace OT { %extend CorrectedLeaveOneOut { CorrectedLeaveOneOut(const CorrectedLeaveOneOut & other) { return new OT::CorrectedLeaveOneOut(other); } } }
