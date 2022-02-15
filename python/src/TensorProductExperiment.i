// SWIG file TensorProductExperiment.i

%{
#include "openturns/TensorProductExperiment.hxx"
%}

%include TensorProductExperiment_doc.i

%include openturns/TensorProductExperiment.hxx
namespace OT { %extend TensorProductExperiment { TensorProductExperiment(const TensorProductExperiment & other) { return new OT::TensorProductExperiment(other); } } }
