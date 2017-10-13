// SWIG file FixedExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::FixedExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/FixedExperiment.hxx"
%}

%include FixedExperiment_doc.i

%include openturns/FixedExperiment.hxx
namespace OT { %extend FixedExperiment { FixedExperiment(const FixedExperiment & other) { return new OT::FixedExperiment(other); } } }

%clear OT::Point & weightsOut;
