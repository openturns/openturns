// SWIG file BootstrapExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::BootstrapExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/BootstrapExperiment.hxx"
%}

%include BootstrapExperiment_doc.i

%include openturns/BootstrapExperiment.hxx
namespace OT { %extend BootstrapExperiment { BootstrapExperiment(const BootstrapExperiment & other) { return new OT::BootstrapExperiment(other); } } }

%clear OT::Point & weightsOut;
