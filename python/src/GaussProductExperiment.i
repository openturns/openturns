// SWIG file GaussProductExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::GaussProductExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/GaussProductExperiment.hxx"
%}

%include GaussProductExperiment_doc.i

%include openturns/GaussProductExperiment.hxx
namespace OT { %extend GaussProductExperiment { GaussProductExperiment(const GaussProductExperiment & other) { return new OT::GaussProductExperiment(other); } } }

%clear OT::Point & weightsOut;
