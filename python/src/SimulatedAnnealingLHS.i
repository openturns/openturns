// SWIG file

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::SimulatedAnnealingLHS::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/SimulatedAnnealingLHS.hxx"
%}

%include SimulatedAnnealingLHS_doc.i

%include openturns/SimulatedAnnealingLHS.hxx
namespace OT { %extend SimulatedAnnealingLHS { SimulatedAnnealingLHS(const SimulatedAnnealingLHS & other) { return new OT::SimulatedAnnealingLHS(other); } } }

%clear OT::Point & weightsOut;
