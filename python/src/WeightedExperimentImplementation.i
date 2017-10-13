// SWIG file WeightedExperimentImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::WeightedExperimentImplementation::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/WeightedExperimentImplementation.hxx"
%}

%include WeightedExperimentImplementation_doc.i

%include openturns/WeightedExperimentImplementation.hxx
namespace OT { %extend WeightedExperimentImplementation { WeightedExperimentImplementation(const WeightedExperimentImplementation & other) { return new OT::WeightedExperimentImplementation(other); } } }

%clear OT::Point & weightsOut;
