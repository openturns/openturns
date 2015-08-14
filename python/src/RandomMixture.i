// SWIG file RandomMixture.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalPoint & kolmogorovNorm ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & kolmogorovNorm %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::DistributionCollection OT::RandomMixture::project(const DistributionFactoryCollection & factoryCollection, OT::NumericalPoint & kolmogorovNorm, const OT::UnsignedInteger size) const;

%{
#include "RandomMixture.hxx"
%}
%include RandomMixture_doc.i

%include RandomMixture.hxx
namespace OT { %extend RandomMixture { RandomMixture(const RandomMixture & other) { return new OT::RandomMixture(other); } } }
