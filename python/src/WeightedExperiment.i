// SWIG file WeightedExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalPoint & weights ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalPoint OT::WeightedExperiment::generateWithWeights(OT::NumericalPoint & weights) const;

%{
#include "openturns/WeightedExperiment.hxx"
%}

%include WeightedExperiment_doc.i

OTTypedInterfaceObjectHelper(WeightedExperiment)

%include openturns/WeightedExperiment.hxx
namespace OT { %extend WeightedExperiment { WeightedExperiment(const WeightedExperiment & other) { return new OT::WeightedExperiment(other); } } }

%clear OT::Sample & weights;
