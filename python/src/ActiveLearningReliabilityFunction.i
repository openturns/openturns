// SWIG file ActiveLearningReliabilityFunction.i

%{
#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/PythonActiveLearningReliabilityFunction.hxx"
%}

%include ActiveLearningReliabilityFunction_doc.i

OTTypedInterfaceObjectHelper(ActiveLearningReliabilityFunction)

%copyctor OT::ActiveLearningReliabilityFunction;

%include openturns/ActiveLearningReliabilityFunction.hxx

namespace OT {

%extend ActiveLearningReliabilityFunction {

ActiveLearningReliabilityFunction(const ActiveLearningReliabilityFunction & other)
{
  return new OT::ActiveLearningReliabilityFunction(other);
}

ActiveLearningReliabilityFunction(PyObject * pyObj)
{
  return new OT::ActiveLearningReliabilityFunction(new OT::PythonActiveLearningReliabilityFunction(pyObj));
}

} // class ActiveLearningReliabilityFunction
} // namespace OT

%pythoncode %{
class PythonActiveLearningReliabilityFunction:
    """
    Allow one to override ActiveLearningReliabilityFunction from Python.

    Parameters
    ----------
    reliabilityThreshold : float
        Threshold of the reliability event.
    learningThreshold : float
        Threshold used to check the active learning convergence.

    Notes
    -----
    You must at least overload the methods `computeAsScalar` or `__call__`,
    `checkConvergenceLearning` and `isMaximization`.
    The default `__call__` loops over `computeAsScalar` and ignores the
    design sample, as the U and EFF criteria do. Override `__call__` for
    a criterion that needs the whole pool and the design, as the GMM
    criterion does. The default `getInfillSample` selects the minimizer
    or maximizer of the criterion values according to `isMaximization`;
    override it only to select a different infill sample, e.g. to add
    several points at once.
    The Gaussian process result set by the algorithm through
    `setGaussianProcessRegression` is available to the scoring code via
    `getGaussianProcessRegression`, e.g. through
    :class:`~openturns.GaussianProcessConditionalCovariance`.

    Examples
    --------
    >>> import openturns as ot
    >>> import openturns.experimental as otexp
    >>> class MyCriterion(otexp.PythonActiveLearningReliabilityFunction):
    ...     def __init__(self):
    ...         super().__init__(0.0, 2.0)
    ...     def computeAsScalar(self, x):
    ...         cov = ot.GaussianProcessConditionalCovariance(self.getGaussianProcessRegression())
    ...         m = cov.getConditionalMean(x)[0]
    ...         s2 = cov.getConditionalMarginalVariance(x)
    ...         return abs(m - self.getReliabilityThreshold()) / max(s2 ** 0.5, 1e-300)
    ...     def checkConvergenceLearning(self, values):
    ...         return values.getMin()[0] >= self.getLearningThreshold()
    ...     def isMaximization(self):
    ...         return False
    >>> criterion = otexp.ActiveLearningReliabilityFunction(MyCriterion())
    """
    def __init__(self, reliabilityThreshold, learningThreshold):
        self._reliabilityThreshold = float(reliabilityThreshold)
        self._learningThreshold = float(learningThreshold)
        self._gprResult = None

    def __str__(self):
        return 'PythonActiveLearningReliabilityFunction(%s, %s)' % (self._reliabilityThreshold, self._learningThreshold)

    def __repr__(self):
        return self.__str__()

    def getReliabilityThreshold(self):
        """Reliability threshold accessor."""
        return self._reliabilityThreshold

    def setReliabilityThreshold(self, reliabilityThreshold):
        """Reliability threshold accessor."""
        self._reliabilityThreshold = float(reliabilityThreshold)

    def getLearningThreshold(self):
        """Learning threshold accessor."""
        return self._learningThreshold

    def setLearningThreshold(self, learningThreshold):
        """Learning threshold accessor."""
        self._learningThreshold = float(learningThreshold)

    def setGaussianProcessRegression(self, gprResult):
        """Gaussian process result accessor, set by the algorithm at each iteration."""
        self._gprResult = gprResult

    def getGaussianProcessRegression(self):
        """Gaussian process result accessor."""
        return self._gprResult

    def computeAsScalar(self, x):
        """Pointwise score, looped over by the default __call__."""
        raise RuntimeError('You must define a method computeAsScalar(x) -> score, where x is a 1-d sequence of float and score a float')

    def __call__(self, candidateSample, doeSample):
        """Score the candidate pool, defaults to a loop over computeAsScalar."""
        return [[self.computeAsScalar(point)] for point in candidateSample]

    def getInfillSample(self, candidateSample, criterionValues):
        """Select the infill sample, defaults to the optimizer of the criterion values."""
        ascending = not self.isMaximization()
        return [candidateSample[criterionValues.argsort(ascending)[0]]]

    def checkConvergenceLearning(self, criterionValues):
        """Test convergence of active learning."""
        raise RuntimeError('You must define a method checkConvergenceLearning(values) -> bool, where values is a Sample')

    def isMaximization(self):
        """Selection direction: True if the infill point maximizes the criterion."""
        raise RuntimeError('You must define a method isMaximization() -> bool')

    def __getstate__(self):
        # The GPR result is a SWIG object restored from C++ on load/clone/copy: drop it from the pickle
        state = self.__dict__.copy()
        state['_gprResult'] = None
        # A subclass may declare __slots__: its values are not stored in
        # __dict__ and would be lost on persistence or C++ cloning
        for cls in type(self).__mro__:
            slots = cls.__dict__.get('__slots__', ())
            if isinstance(slots, str):
                slots = (slots,)
            for name in slots:
                if name in ('__dict__', '__weakref__', '_gprResult') or name in state:
                    continue
                if name.startswith('__') and not name.endswith('__'):
                    name = '_%s%s' % (cls.__name__.lstrip('_'), name)
                try:
                    state[name] = getattr(self, name)
                except AttributeError:
                    pass
        return state

    def __setstate__(self, state):
        for name, value in state.items():
            setattr(self, name, value)
%}
