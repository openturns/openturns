// SWIG file LowDiscrepancyExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::LowDiscrepancyExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/LowDiscrepancyExperiment.hxx"
%}

%include LowDiscrepancyExperiment_doc.i

%include openturns/LowDiscrepancyExperiment.hxx
namespace OT { %extend LowDiscrepancyExperiment { LowDiscrepancyExperiment(const LowDiscrepancyExperiment & other) { return new OT::LowDiscrepancyExperiment(other); } } }

%clear OT::Point & weightsOut;
