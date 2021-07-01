// SWIG file

%{
#include "openturns/LeaveOneOutSplitter.hxx"
%}

%include LeaveOneOutSplitter_doc.i

%include openturns/LeaveOneOutSplitter.hxx
namespace OT { %extend LeaveOneOutSplitter { LeaveOneOutSplitter(const LeaveOneOutSplitter & other) { return new OT::LeaveOneOutSplitter(other); } } }
