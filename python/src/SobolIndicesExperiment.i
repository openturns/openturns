// SWIG file SobolIndicesExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::SobolIndicesExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/SobolIndicesExperiment.hxx"
%}

%include SobolIndicesExperiment_doc.i

%include openturns/SobolIndicesExperiment.hxx
namespace OT { %extend SobolIndicesExperiment { SobolIndicesExperiment(const SobolIndicesExperiment & other) { return new OT::SobolIndicesExperiment(other); } } }

%clear OT::Point & weightsOut;
