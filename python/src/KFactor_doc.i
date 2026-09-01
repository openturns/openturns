%define OT_KFactor_doc
R"RAW(Base class.

Notes
-----
This class uses the following entries of :class:`~openturns.ResourceMap`:

- *KFactor-Precision* (`Scalar`, default: `1e-08`): precision of the zerofinding algorithm used
  to compute the tolerance factor,
- *KFactor-DefaultIntegrationNodesNumber* (`UnsignedInteger`, default: `256`): number of nodes of the
  Gauss-Kronrod integration rule used to compute the tolerance factor,
- *KFactor-MaximumIteration* (`UnsignedInteger`, default: `32`): maximum number of iterations of the
  zerofinding algorithm.
)RAW"
%enddef
