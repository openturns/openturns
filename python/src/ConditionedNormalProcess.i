// SWIG file ConditionedNormalProcess.i

%{
#include "ConditionedNormalProcess.hxx"
%}

%include ConditionedNormalProcess_doc.i

%include ConditionedNormalProcess.hxx
namespace OT { %extend ConditionedNormalProcess { ConditionedNormalProcess(const ConditionedNormalProcess & other) { return new OT::ConditionedNormalProcess(other); } } }
