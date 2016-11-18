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

%typemap(in, numinputs=0) OT::NumericalPoint & marginalProb ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & marginalProb %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::Interval OT::DistributionImplementation::computeMinimumVolumeInterval(const OT::NumericalScalar prob, OT::NumericalPoint & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeBilateralConfidenceInterval(const OT::NumericalScalar prob, OT::NumericalPoint & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeUnilateralConfidenceInterval(const OT::NumericalScalar prob, const OT::Bool tail, OT::NumericalPoint & marginalProb) const;

%typemap(in, numinputs=0) OT::NumericalPoint & threshold ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & threshold %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::LevelSet OT::DistributionImplementation::computeMinimumVolumeLevelSet(const OT::NumericalScalar prob, OT::NumericalPoint & threshold) const;


%typemap(in, numinputs=0) OT::NumericalScalar & marginalProb ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::NumericalScalar & marginalProb %{ $result = SWIG_Python_AppendOutput($result, PyFloat_FromDouble(*$1)); %}
OT::Interval OT::DistributionImplementation::computeMinimumVolumeIntervalWithMarginalProbability(const OT::NumericalScalar prob, OT::NumericalScalar & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeBilateralConfidenceIntervalWithMarginalProbability(const OT::NumericalScalar prob, OT::NumericalScalar & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeUnilateralConfidenceIntervalWithMarginalProbability(const OT::NumericalScalar prob, const OT::Bool tail, OT::NumericalScalar & marginalProb) const;
%typemap(in, numinputs=0) OT::NumericalScalar & threshold ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::NumericalScalar & threshold %{ $result = SWIG_Python_AppendOutput($result, PyFloat_FromDouble(*$1)); %}
OT::LevelSet OT::DistributionImplementation::computeMinimumVolumeLevelSetWithThreshold(const OT::NumericalScalar prob, OT::NumericalScalar & threshold) const;


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

