// SWIG file ImportanceSamplingExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::ImportanceSamplingExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/ImportanceSamplingExperiment.hxx"
%}

%include ImportanceSamplingExperiment_doc.i

%include openturns/ImportanceSamplingExperiment.hxx
namespace OT { %extend ImportanceSamplingExperiment { ImportanceSamplingExperiment(const ImportanceSamplingExperiment & other) { return new OT::ImportanceSamplingExperiment(other); } } }

%clear OT::Point & weightsOut;
