// SWIG file LHSExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::LHSExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/LHSExperiment.hxx"
%}

%include LHSExperiment_doc.i

%include openturns/LHSExperiment.hxx
namespace OT { %extend LHSExperiment { LHSExperiment(const LHSExperiment & other) { return new OT::LHSExperiment(other); } } }

%clear OT::Point & weightsOut;
