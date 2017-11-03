// SWIG file DistributionImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & gridOut ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & gridOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::Point & marginalProbOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & marginalProbOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::Point & thresholdOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & thresholdOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::Scalar & marginalProbOut ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & marginalProbOut %{ $result = SWIG_Python_AppendOutput($result, PyFloat_FromDouble(*$1)); %}

%typemap(in, numinputs=0) OT::Scalar & thresholdOut ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & thresholdOut %{ $result = SWIG_Python_AppendOutput($result, PyFloat_FromDouble(*$1)); %}


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

#if SWIG_VERSION < 0x030011
Pointer<DistributionImplementation> __truediv__(const Scalar s) { return (*self) / s; }

Pointer<DistributionImplementation> __truediv__(const DistributionImplementation & d) { return (*self) / d; }
#endif

Pointer<DistributionImplementation> __rtruediv__(const Scalar s) { return s / (*self); }

Pointer<DistributionImplementation> __rdiv__(const Scalar s) { return s / (*self); }

Pointer<DistributionImplementation> __pow__(const Scalar s) { return self->pow(s); }

Pointer<DistributionImplementation> __pow__(const SignedInteger s) { return self->pow(s); }

Pointer<DistributionImplementation> __sub__(const Scalar s)
{
 return *self - s;
}

Pointer<DistributionImplementation> __rsub__(const Scalar s)
{
  return *(*self * (-1.0)) + s;
}

Pointer<DistributionImplementation> __add__(const Scalar s)
{
 return *self + s;
}

Pointer<DistributionImplementation> __radd__(const Scalar s)
{
  return *self + s;
}

Pointer<DistributionImplementation> __mul__(Scalar s)
{
  return (*self) * s;
}

Pointer<DistributionImplementation> __rmul__(Scalar s)
{
  return (*self) * s;
}



} }
