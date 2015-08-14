// SWIG file TemporalNormalProcess.i

%{
#include "TemporalNormalProcess.hxx"
%}

%include TemporalNormalProcess_doc.i

%include TemporalNormalProcess.hxx
namespace OT { %extend TemporalNormalProcess { TemporalNormalProcess(const TemporalNormalProcess & other) { return new OT::TemporalNormalProcess(other); } } }
