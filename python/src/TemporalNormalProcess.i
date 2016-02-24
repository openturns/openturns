// SWIG file TemporalNormalProcess.i

%{
#include "openturns/TemporalNormalProcess.hxx"
%}

%include TemporalNormalProcess_doc.i

%include openturns/TemporalNormalProcess.hxx
namespace OT { %extend TemporalNormalProcess { TemporalNormalProcess(const TemporalNormalProcess & other) { return new OT::TemporalNormalProcess(other); } } }
