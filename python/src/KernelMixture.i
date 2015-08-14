// SWIG file KernelMixture.i

%{
#include "KernelMixture.hxx"
%}

%include KernelMixture_doc.i

%include KernelMixture.hxx
namespace OT { %extend KernelMixture { KernelMixture(const KernelMixture & other) { return new OT::KernelMixture(other); } } }
namespace OT { %extend KernelMixture { KernelMixture(const OT::DistributionImplementation & kernel, const OT::NumericalPoint & bandwidth, const OT::NumericalSample & sample) { return new OT::KernelMixture(OT::Distribution(kernel), bandwidth, sample); } } }
