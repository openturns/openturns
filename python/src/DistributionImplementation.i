// SWIG file DistributionImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Sample & grid ($*ltype temp) %{ temp = OT::Sample(); $1 = &temp; %}
%typemap(argout) OT::Sample & grid %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Sample(*$1), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::DistributionImplementation::computePDF(const OT::Scalar xMin,
                           const OT::Scalar xMax,
                           const OT::UnsignedInteger pointNumber,
                           OT::Sample & grid) const;
OT::Sample OT::DistributionImplementation::computePDF(const OT::Point & xMin,
                           const OT::Point & xMax,
                           const OT::Indices & pointNumber,
                           OT::Sample & grid) const;

%typemap(in, numinputs=0) OT::Point & weights ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Point OT::DistributionImplementation::getGaussNodesAndWeights(OT::Point & weights) const;

%typemap(in, numinputs=0) OT::Point & marginalProb ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & marginalProb %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Interval OT::DistributionImplementation::computeMinimumVolumeInterval(const OT::Scalar prob, OT::Point & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeBilateralConfidenceInterval(const OT::Scalar prob, OT::Point & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeUnilateralConfidenceInterval(const OT::Scalar prob, const OT::Bool tail, OT::Point & marginalProb) const;

%typemap(in, numinputs=0) OT::Point & threshold ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & threshold %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::LevelSet OT::DistributionImplementation::computeMinimumVolumeLevelSet(const OT::Scalar prob, OT::Point & threshold) const;


%typemap(in, numinputs=0) OT::Scalar & marginalProb ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & marginalProb %{ $result = SWIG_Python_AppendOutput($result, PyFloat_FromDouble(*$1)); %}
OT::Interval OT::DistributionImplementation::computeMinimumVolumeIntervalWithMarginalProbability(const OT::Scalar prob, OT::Scalar & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeBilateralConfidenceIntervalWithMarginalProbability(const OT::Scalar prob, OT::Scalar & marginalProb) const;
OT::Interval OT::DistributionImplementation::computeUnilateralConfidenceIntervalWithMarginalProbability(const OT::Scalar prob, const OT::Bool tail, OT::Scalar & marginalProb) const;
%typemap(in, numinputs=0) OT::Scalar & threshold ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & threshold %{ $result = SWIG_Python_AppendOutput($result, PyFloat_FromDouble(*$1)); %}
OT::LevelSet OT::DistributionImplementation::computeMinimumVolumeLevelSetWithThreshold(const OT::Scalar prob, OT::Scalar & threshold) const;


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

%clear OT::Point & weights;

