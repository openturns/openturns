// SWIG file WeightedExperimentImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalPoint & weights ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalPoint OT::WeightedExperimentImplementation::generateWithWeights(OT::NumericalPoint & weights) const;

%{
#include "openturns/WeightedExperimentImplementation.hxx"
%}

%include WeightedExperimentImplementation_doc.i

%include openturns/WeightedExperimentImplementation.hxx
namespace OT { %extend WeightedExperimentImplementation { WeightedExperimentImplementation(const WeightedExperimentImplementation & other) { return new OT::WeightedExperimentImplementation(other); } } }

%clear OT::Sample & weights;
