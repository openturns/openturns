Estimating a quantile by Wilks' method
--------------------------------------

Let us denote
:math:`\underline{Y} = h\left( \vect{X},\vect{d} \right) = \left( Y^1,\ldots,Y^{n_Y} \right)`,
where :math:`\vect{X}= \left( X^1,\ldots,X^{n_X} \right)` is a random
vector, and :math:`\vect{d}` a deterministic vector. We seek here to
evaluate, using the probability distribution of the random vector
:math:`\vect{X}`, the :math:`\alpha`-quantile :math:`q_{Y^i}(\alpha)` of
:math:`Y^i`, where :math:`\alpha \in (0, 1)`:

.. math::

   \begin{aligned}
       \Prob{ Y^i \leq q_{Y^i}(\alpha)} = \alpha
     \end{aligned}

If we have a sample
:math:`\left\{ \vect{x}_1,\ldots,\vect{x}_N \right\}` of :math:`N`
independent samples of the random vector :math:`\vect{X}`,
:math:`q_{Y^i}(\alpha)` can be estimated as follows:

-  the sample :math:`\left\{ \vect{x}_1,\ldots,\vect{x}_N \right\}` of
   vector :math:`\vect{X}` is first transformed to a sample
   :math:`\left\{ y^i_1,\ldots,y^i_N \right\}` of the variable
   :math:`Y^i`, using :math:`\underline{y} = h(\vect{x}_i,\vect{d})`,

-  the sample :math:`\left\{ y^i_1,\ldots,y^i_N \right\}` is then placed
   in ascending order, which gives the sample
   :math:`\left\{ y^{(1)},\ldots,y^{(N)} \right\}`,

-  this empirical estimation of the quantile is then calculated by the
   formula:

   .. math::

      \begin{aligned}
            \widehat{q}_{y^i}(\alpha) = y^{([N\alpha]+1)}
          \end{aligned}

where :math:`[N\alpha]` denotes the integral part of
:math:`N\alpha`.

For example, if :math:`N=100` and :math:`\alpha = 0.95`,
:math:`\widehat{q}_Z(0.95)` is equal to :math:`y^{(96)}`, which is the
:math:`5^\textrm{th}` largest value of the sample
:math:`\left\{ y^i_1,\ldots,y^i_N \right\}`. We note that this
estimation has no meaning unless :math:`1/N \leq \alpha \leq 1-1/N`. For
example, if :math:`N=100`, one can only consider values of a to be
between 1% and 99%.

It is also possible to calculate an upper limit for the quantile with a
confidence level :math:`\beta` chosen by the user; one can then be sure
with a :math:`\beta` level of confidence that the real value of
:math:`q_{Y^i}(\alpha))` is less than or equal to
:math:`\widehat{q}_{Y^i}(\alpha)_{\sup}`:

.. math::

   \begin{aligned}
       \Prob{q_{Y^i}(\alpha) \leq \widehat{q}_{Y^i}(\alpha)_{\sup}} = \beta
     \end{aligned}

The most robust method for calculating this upper limit consists of
taking
:math:`\widehat{q}_{Y^i}(\alpha)_{\sup} = y^{(j(\alpha,\beta,N))}` where
:math:`j(\alpha,\beta,N)` is an integer between 2 and :math:`N` found by
solving the equation:

.. math::

   \begin{aligned}
       \sum_{k=1}^{j(\alpha,\beta,N) - 1} C^k_N \alpha^k \left( 1-\alpha \right)^{N-k} = \beta
     \end{aligned}

A solution to this does not necessarily exist, i.e. there may be no
integer value for :math:`j(\alpha,\beta,N)` satisfying this equality;
one can in this case choose the smallest integer :math:`j` such that:

.. math::

   \begin{aligned}
       \sum_{k=1}^{j(\alpha,\beta,N) - 1} C^k_N \alpha^k \left( 1-\alpha \right)^{N-k} > \beta
     \end{aligned}

which ensures that
:math:`\Prob{q_{Y^i}(\alpha) \leq \widehat{q}_{Y^i}(\alpha)_{\sup}} > \beta`;
in other words, the level of confidence of the quantile estimation is
greater than that initially required.

This formula of the confidence interval can be used in two ways:

-  either directly to determine :math:`j(\alpha,\beta,N)` for the values
   :math:`\alpha,\beta,N` chosen by the user,

-  or in reverse to determine the number :math:`N` of simulations to be
   carried out for the values :math:`\alpha,\beta` and
   :math:`j(\alpha,\beta,N)` chosen by the user; this is known as Wilks’
   formula.

For example for :math:`\alpha = \beta = 95\%`, we take :math:`j=59` with
:math:`N = 59` simulations (that is the maximum value out of 59 samples)
or else :math:`j = 92` with :math:`N = 93` simulations (that is the
second largest result out of the 93 selections). For values of :math:`N`
between :math:`59` and :math:`92`, the upper limit is the maximum value
of the sample. The following tabular presents the whole results for
:math:`N \leq 1000`, still for :math:`\alpha = \beta = 95\%`.

+-------------+------------------------------------------+--------------------------------------+
| :math:`N`   | Rank of the upper bound of the quantile  | Rank of the empirical quantile       |
+=============+==========================================+======================================+
| 59          | 59                                       | 57                                   |
+-------------+------------------------------------------+--------------------------------------+
| 93          | 92                                       | 89                                   |
+-------------+------------------------------------------+--------------------------------------+
| 124         | 122                                      | 118                                  |
+-------------+------------------------------------------+--------------------------------------+
| 153         | 150                                      | 146                                  |
+-------------+------------------------------------------+--------------------------------------+
| 181         | 177                                      | 172                                  |
+-------------+------------------------------------------+--------------------------------------+
| 208         | 203                                      | 198                                  |
+-------------+------------------------------------------+--------------------------------------+
| 234         | 228                                      | 223                                  |
+-------------+------------------------------------------+--------------------------------------+
| 260         | 253                                      | 248                                  |
+-------------+------------------------------------------+--------------------------------------+
| 286         | 278                                      | 272                                  |
+-------------+------------------------------------------+--------------------------------------+
| 311         | 302                                      | 296                                  |
+-------------+------------------------------------------+--------------------------------------+
| 336         | 326                                      | 320                                  |
+-------------+------------------------------------------+--------------------------------------+
| 361         | 350                                      | 343                                  |
+-------------+------------------------------------------+--------------------------------------+
| 386         | 374                                      | 367                                  |
+-------------+------------------------------------------+--------------------------------------+
| 410         | 397                                      | 390                                  |
+-------------+------------------------------------------+--------------------------------------+
| 434         | 420                                      | 413                                  |
+-------------+------------------------------------------+--------------------------------------+
| 458         | 443                                      | 436                                  |
+-------------+------------------------------------------+--------------------------------------+
| 482         | 466                                      | 458                                  |
+-------------+------------------------------------------+--------------------------------------+
| 506         | 489                                      | 481                                  |
+-------------+------------------------------------------+--------------------------------------+
| 530         | 512                                      | 504                                  |
+-------------+------------------------------------------+--------------------------------------+
| 554         | 535                                      | 527                                  |
+-------------+------------------------------------------+--------------------------------------+
| 577         | 557                                      | 549                                  |
+-------------+------------------------------------------+--------------------------------------+
| 601         | 580                                      | 571                                  |
+-------------+------------------------------------------+--------------------------------------+
| 624         | 602                                      | 593                                  |
+-------------+------------------------------------------+--------------------------------------+
| 647         | 624                                      | 615                                  |
+-------------+------------------------------------------+--------------------------------------+
| 671         | 647                                      | 638                                  |
+-------------+------------------------------------------+--------------------------------------+
| 694         | 669                                      | 660                                  |
+-------------+------------------------------------------+--------------------------------------+
| 717         | 691                                      | 682                                  |
+-------------+------------------------------------------+--------------------------------------+
| 740         | 713                                      | 704                                  |
+-------------+------------------------------------------+--------------------------------------+
| 763         | 735                                      | 725                                  |
+-------------+------------------------------------------+--------------------------------------+
| 786         | 757                                      | 747                                  |
+-------------+------------------------------------------+--------------------------------------+
| 809         | 779                                      | 769                                  |
+-------------+------------------------------------------+--------------------------------------+
| 832         | 801                                      | 791                                  |
+-------------+------------------------------------------+--------------------------------------+
| 855         | 823                                      | 813                                  |
+-------------+------------------------------------------+--------------------------------------+
| 877         | 844                                      | 834                                  |
+-------------+------------------------------------------+--------------------------------------+
| 900         | 866                                      | 856                                  |
+-------------+------------------------------------------+--------------------------------------+
| 923         | 888                                      | 877                                  |
+-------------+------------------------------------------+--------------------------------------+
| 945         | 909                                      | 898                                  |
+-------------+------------------------------------------+--------------------------------------+
| 968         | 931                                      | 920                                  |
+-------------+------------------------------------------+--------------------------------------+
| 991         | 953                                      | 942                                  |
+-------------+------------------------------------------+--------------------------------------+

:math:`\widehat{q}_{Y^i}(\alpha)` is often called the “empirical
:math:`\alpha`-quantile” for the variable :math:`{Y^i}`.

.. topic:: API:

    - See :class:`~openturns.Wilks`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/quantile_estimation_wilks`

.. topic:: References:

    - Wilks, S.S. (1962). "Mathematical Statistics", New York-London
    - Robert C.P., Casella G. (2004). Monte-Carlo Statistical Methods, Springer, ISBN 0-387-21239-6, 2nd ed.
    - Rubinstein R.Y. (1981). Simulation and The Monte-Carlo methods, John Wiley & Sons
