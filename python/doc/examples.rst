.. _examples:

========
Examples
========

This is the OpenTURNS examples gallery.

.. _examples_simple_example:

A simple example
----------------

.. plot::
    :format: doctest
    :include-source: True

    Let's do some imports:

    >>> import openturns as ot
    >>> import numpy as np
    >>> import pylab as pl

    Let's use `openturns`:

    >>> X = ot.Normal()
    >>> sample = X.getSample(1000)
    >>> x = np.linspace(X.computeQuantile(1e-6)[0],
    ...                 X.computeQuantile(1. - 1e-6)[0],
    ...                 100)
    >>> fx = X.computePDF(x[:, np.newaxis])

    Let's plot:

    >>> pl.hist(np.ravel(sample), normed=True, alpha=.5, ec='g', fc='g') # doctest: +SKIP
    >>> pl.plot(x, fx, 'r', lw=1.5) # doctest: +SKIP
    >>> pl.xlabel('$x$') # doctest: +SKIP
    >>> pl.ylabel('$f_X$') # doctest: +SKIP
    >>> pl.grid() # doctest: +SKIP
    >>> pl.show() # doctest: +SKIP

    Hell it rocks!
