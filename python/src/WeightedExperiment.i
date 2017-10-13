// SWIG file WeightedExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::WeightedExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/WeightedExperiment.hxx"
%}

%include WeightedExperiment_doc.i

OTTypedInterfaceObjectHelper(WeightedExperiment)

%include openturns/WeightedExperiment.hxx
namespace OT { %extend WeightedExperiment { WeightedExperiment(const WeightedExperiment & other) { return new OT::WeightedExperiment(other); } } }

%clear OT::Point & weightsOut;
