// SWIG file EfficientGlobalOptimizationCriterion.i

%{
#include "openturns/EfficientGlobalOptimizationCriterion.hxx"
#include "openturns/PythonEfficientGlobalOptimizationCriterion.hxx"
%}

%include EfficientGlobalOptimizationCriterion_doc.i

OTTypedInterfaceObjectHelper(EfficientGlobalOptimizationCriterion)

%copyctor OT::EfficientGlobalOptimizationCriterion;

%include openturns/EfficientGlobalOptimizationCriterion.hxx

namespace OT {

%extend EfficientGlobalOptimizationCriterion {

EfficientGlobalOptimizationCriterion(const EfficientGlobalOptimizationCriterion & other)
{
  return new OT::EfficientGlobalOptimizationCriterion(other);
}

EfficientGlobalOptimizationCriterion(PyObject * pyObj)
{
  return new OT::EfficientGlobalOptimizationCriterion(new OT::PythonEfficientGlobalOptimizationCriterion(pyObj));
}

} // class EfficientGlobalOptimizationCriterion
} // namespace OT

%pythoncode %{
class PythonEfficientGlobalOptimizationCriterion:
    """
    Allow one to override EfficientGlobalOptimizationCriterion from Python.

    Notes
    -----
    You must overload `buildObjective`, which returns the scalar
    :class:`~openturns.Function` that the algorithm maximizes, and you may
    overload `checkConvergence`, which receives the criterion value reached at
    the new point and returns whether the enrichment is over. The default
    `checkConvergence` never stops the algorithm.

    The current state is pushed into the criterion before each call to
    `buildObjective`, and is available through
    `getGaussianProcessRegression`, `getInputSample`, `getOptimalValue`,
    `getMinimization`, `getNoiseFunction` and `hasNoiseFunction`. The Gaussian
    process is the one of the current iteration, so the usual route is
    :class:`~openturns.GaussianProcessConditionalCovariance`.

    The returned objective is always maximized, whatever the direction of the
    problem: return the opposite of your quantity of interest for a
    minimization.

    Examples
    --------
    >>> import openturns as ot
    >>> import openturns.experimental as otexp
    >>> class MyCriterion(otexp.PythonEfficientGlobalOptimizationCriterion):
    ...     def buildObjective(self):
    ...         covariance = ot.GaussianProcessConditionalCovariance(
    ...             self.getGaussianProcessRegression())
    ...         best = self.getOptimalValue()
    ...         sign = 1.0 if self.getMinimization() else -1.0
    ...         def objective(x):
    ...             mean = covariance.getConditionalMean(x)[0]
    ...             deviation = covariance.getConditionalMarginalVariance(x) ** 0.5
    ...             return [max(0.0, sign * (best - mean)) + 0.01 * deviation]
    ...         return ot.PythonFunction(1, 1, objective)
    >>> criterion = otexp.EfficientGlobalOptimizationCriterion(MyCriterion())
    """
    def __init__(self):
        self._gprResult = None
        self._inputSample = None
        self._optimalValue = 0.0
        self._minimization = True
        self._noiseFunction = None

    def __str__(self):
        return 'PythonEfficientGlobalOptimizationCriterion()'

    def __repr__(self):
        return self.__str__()

    def setGaussianProcessRegression(self, gprResult):
        """Gaussian process surrogate accessor, set by the algorithm at each iteration."""
        self._gprResult = gprResult

    def getGaussianProcessRegression(self):
        """Gaussian process surrogate accessor."""
        return self._gprResult

    def setInputSample(self, inputSample):
        """Current design of experiments accessor."""
        self._inputSample = inputSample

    def getInputSample(self):
        """Current design of experiments accessor."""
        return self._inputSample

    def setOptimalValue(self, optimalValue):
        """Best objective value observed so far accessor."""
        self._optimalValue = float(optimalValue)

    def getOptimalValue(self):
        """Best objective value observed so far accessor."""
        return self._optimalValue

    def setMinimization(self, minimization):
        """Direction of the problem accessor."""
        self._minimization = bool(minimization)

    def getMinimization(self):
        """Direction of the problem accessor."""
        return self._minimization

    def setNoiseFunction(self, noiseFunction):
        """Noise variance function accessor, set by the algorithm."""
        self._noiseFunction = noiseFunction

    def getNoiseFunction(self):
        """Noise variance function accessor."""
        return self._noiseFunction

    def hasNoiseFunction(self):
        """Whether the algorithm provided a noise variance function.

        Returns
        -------
        hasNoise : bool
            Whether the objective is noisy. The algorithm pushes an empty
            function (output dimension 0) when the objective is noiseless, so
            the dimension is the test that works whatever the implementation
            of the function, including a function defined in Python.
        """
        return (
            self._noiseFunction is not None
            and self._noiseFunction.getOutputDimension() == 1
        )

    def buildObjective(self):
        """Build the scalar function the algorithm maximizes."""
        raise RuntimeError('You must define a method buildObjective() -> Function, returning the 1-d function maximized by the algorithm')

    def checkConvergence(self, criterionValue):
        """Convergence test on the criterion value at the new point, never stops by default."""
        return False

    def __getstate__(self):
        # The Gaussian process and noise function are SWIG objects restored from
        # C++ on load/clone/copy: drop them from the pickle.
        state = self.__dict__.copy()
        state['_gprResult'] = None
        state['_noiseFunction'] = None
        # A subclass may declare __slots__: its values are not stored in
        # __dict__ and would be lost on persistence or C++ cloning.
        for cls in type(self).__mro__:
            slots = cls.__dict__.get('__slots__', ())
            if isinstance(slots, str):
                slots = (slots,)
            for name in slots:
                if name in ('__dict__', '__weakref__', '_gprResult', '_noiseFunction') or name in state:
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
