// SWIG file DistributionImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalSample & grid ($*ltype temp) %{ temp = OT::NumericalSample(); $1 = &temp; %}
%typemap(argout) OT::NumericalSample & grid %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalSample(*$1), SWIG_TypeQuery("OT::NumericalSample *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalSample OT::DistributionImplementation::computePDF(const OT::NumericalScalar xMin,
                           const OT::NumericalScalar xMax,
                           const OT::UnsignedInteger pointNumber,
                           OT::NumericalSample & grid) const;
OT::NumericalSample OT::DistributionImplementation::computePDF(const OT::NumericalPoint & xMin,
                           const OT::NumericalPoint & xMax,
                           const OT::Indices & pointNumber,
                           OT::NumericalSample & grid) const;

%typemap(in, numinputs=0) OT::NumericalPoint & weights ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalPoint OT::DistributionImplementation::getGaussNodesAndWeights(OT::NumericalPoint & weights) const;

%{
#include "openturns/DistributionImplementation.hxx"
%}

%include DistributionImplementation_doc.i

%ignore OT::DistributionImplementation::pow;
%ignore OT::DistributionImplementation::setWeight;
%ignore OT::DistributionImplementation::getWeight;
%ignore OT::DistributionImplementation::setParallel;
%ignore OT::DistributionImplementation::isParallel;

%include openturns/DistributionImplementation.hxx

namespace OT {

%extend DistributionImplementation {

DistributionImplementation(const DistributionImplementation & other) { return new OT::DistributionImplementation(other); } 

Pointer<DistributionImplementation> __truediv__(const NumericalScalar s) { return (*self) / s; }

Pointer<DistributionImplementation> __truediv__(const DistributionImplementation & d) { return (*self) / d; }

Pointer<DistributionImplementation> __pow__(const NumericalScalar s) { return self->pow(s); }

Pointer<DistributionImplementation> __pow__(const SignedInteger s) { return self->pow(s); }

Pointer<DistributionImplementation> __sub__(const NumericalScalar s)
{
 return *self - s;
}

Pointer<DistributionImplementation> __rsub__(const NumericalScalar s)
{
  return *(*self * (-1.0)) + s;
}

Pointer<DistributionImplementation> __add__(const NumericalScalar s)
{
 return *self + s;
}

Pointer<DistributionImplementation> __radd__(const NumericalScalar s)
{
  return *self + s;
}

Pointer<DistributionImplementation> __mul__(NumericalScalar s)
{
  return (*self) * s;
}

Pointer<DistributionImplementation> __rmul__(NumericalScalar s)
{
  return (*self) * s;
}



} }

%clear OT::NumericalPoint & weights;

