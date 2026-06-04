// SWIG file MixedHistogramFiniteDiscreteDistribution.i

%{
#include "openturns/MixedHistogramFiniteDiscreteDistribution.hxx"
%}

%include MixedHistogramFiniteDiscreteDistribution_doc.i

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::DistributionImplementation::PointCollection & {
  $1 = (SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) || OT::isAPythonSequenceOf<OT::_PySequence_>($input));
}

%typemap(in) const OT::DistributionImplementation::PointCollection & {
  if (!SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      $1 = OT::buildCollectionFromPySequence< OT::Point >($input);
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Point");
    }
  }
}

%copyctor OT::MixedHistogramFiniteDiscreteDistribution;

%include openturns/MixedHistogramFiniteDiscreteDistribution.hxx

%pythoncode %{
def MixedHistogramUserDefined(*args):
    """Deprecated"""
    openturns.common.Log.Warn('class MixedHistogramUserDefined is deprecated in favor of MixedHistogramFiniteDiscreteDistribution')
    return openturns.dist_bundle2.MixedHistogramFiniteDiscreteDistribution(*args)
%}
