%feature("docstring") OT::GaussKronrodRule
R"RAW(Gauss-Kronrod rule used in the integration algorithm.

Parameters
----------
myGaussKronrodPair : int
    It encodes the selected rule. 

    Available rules: 

    - GaussKronrodRule.G1K3, 
    - GaussKronrodRule.G3K7, 
    - GaussKronrodRule.G7K15, 
    - GaussKronrodRule.G11K23,
    - GaussKronrodRule.G15K31, 
    - GaussKronrodRule.G25K51. 

Notes
-----
We consider a function :math:`f: \Rset \mapsto \Rset^p`. The Gauss-Kronrod rule
:math:`G_mK_{2m+1}` with  :math:`m=2n+1` enables to build two approximations of the definite integral :math:`\int_{-1}^1 f(t)\di{t}` respectively defined by:

.. math::

    \int_{-1}^1 f(t)\di{t} \simeq  \omega_0f(0) + \sum_{k=1}^n \omega_k (f(\xi_k)+f(-\xi_k))

and:

.. math::

  \int_{-1}^1 f(t)\di{t}\simeq  \alpha_0f(0) + \sum_{k=1}^{m} \alpha_k (f(\zeta_k)+f(-\zeta_k))

We have :math:`\xi_k>0`,  :math:`\zeta_k>0`, :math:`\zeta_{2j}=\xi_j`, :math:`\omega_k>0` and :math:`\alpha_k>0`.

The rule :math:`G_mK_{2m+1}` combines a :math:`m`-point Gauss rule and a :math:`(2m+1)`-point Kronrod rule (re-using the :math:`m` nodes of the Gauss method). The nodes are defined on :math:`[-1, 1]` and always contain the node 0 as :math:`m`  is odd. 

Examples
--------
Create an Gauss-Kronrod rule:

>>> import openturns as ot
>>> myRule = ot.GaussKronrodRule(ot.GaussKronrodRule.G15K31)
)RAW"


// ---------------------------------------------------------------------
%feature("docstring") OT::GaussKronrodRule::getOrder
"Accessor to :math:`m` parameter.

Returns
-------
m : int
    The number of points used for the Gauss approximation."

// ---------------------------------------------------------------------
%feature("docstring") OT::GaussKronrodRule::getPair
"Accessor to pair definig the rule.

Returns
-------
gkPair : int
    Id of the Gauss-Kronrod rule."

// ---------------------------------------------------------------------
%feature("docstring") OT::GaussKronrodRule::getOtherGaussWeights
R"RAW(Accessor to the weights used in the Gauss approximation.

Returns
-------
otherGaussWeights : :class:`~openturns.Point`
    The  weights :math:`(\omega_k)_{1 \leq k \leq n}`)RAW"

// ---------------------------------------------------------------------
%feature("docstring") OT::GaussKronrodRule::getOtherKronrodWeights
R"RAW(Accessor to the weights used in the Gauss-Kronrod approximation.

Returns
-------
otherKronrodWeights : :class:`~openturns.Point`
    The  weights :math:`(\alpha_k)_{1 \leq k \leq m}`.
)RAW"
// ---------------------------------------------------------------------
%feature("docstring") OT::GaussKronrodRule::getOtherKronrodNodes
R"RAW(Accessor to the positive nodes used in the Gauss-Kronrod approximation.

Returns
-------
otherKronrodNodes : :class:`~openturns.Point`
    The positive nodes :math:`(\zeta_k)_{1 \leq k \leq m}`.
    
    It contains the positive Gauss nodes as we have :math:`\zeta_{2j}=\xi_j`.
)RAW"
// ---------------------------------------------------------------------
%feature("docstring") OT::GaussKronrodRule::getZeroGaussWeight
R"RAW(Accessor to the first Gauss weight.

Returns
-------
zeroKronrodWeight : float
    The  first weight :math:`\omega_0`.
)RAW"
// ---------------------------------------------------------------------
%feature("docstring") OT::GaussKronrodRule::getZeroKronrodWeight
R"RAW(Accessor to the first Kronrod weight.

Returns
-------
zeroKronrodWeight : float
    The  first weight :math:`\alpha_0`.
)RAW"
