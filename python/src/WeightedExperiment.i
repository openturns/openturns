// SWIG file WeightedExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weights ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Point OT::WeightedExperiment::generateWithWeights(OT::Point & weights) const;

%{
#include "openturns/WeightedExperiment.hxx"
%}

%include WeightedExperiment_doc.i

OTTypedInterfaceObjectHelper(WeightedExperiment)

%include openturns/WeightedExperiment.hxx
namespace OT { %extend WeightedExperiment { WeightedExperiment(const WeightedExperiment & other) { return new OT::WeightedExperiment(other); } } }

%clear OT::Sample & weights;
