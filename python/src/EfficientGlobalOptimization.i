// SWIG file EfficientGlobalOptimization.i

%{
#include "openturns/EfficientGlobalOptimization.hxx"
%}

%include EfficientGlobalOptimization_doc.i

%include openturns/EfficientGlobalOptimization.hxx
namespace OT{ %extend EfficientGlobalOptimization { EfficientGlobalOptimization(const EfficientGlobalOptimization & other) { return new OT::EfficientGlobalOptimization(other); } } }

