.. _sphere_sampling:

Sphere sampling method
----------------------

Within the context of the First and Second Order of the Reliability
Method, the Strong Maximum Test helps to
check whether the design point computed is:

-  the true design point, which means a global maximum point,

-  a strong design point, which means that there is no other local
   maximum verifying the event and associated to a value near the global
   maximum.

The Strong Maximum Test samples a sphere in the standard space.
the gaussian random sampling technique used is described hereafter.

#. sampling of points in :math:`\Rset^N` according to a radial
   distribution: we generate :math:`N` independent standard normal
   samples,

#. projection of the points onto :math:`\cS^{*}` : we map the points
   different from the origin using the transformation
   :math:`M\longmapsto m` such as
   :math:`\displaystyle\mathbf{Om}=R\frac{\mathbf{OM}}{\|\mathbf{OM}\|}`
   where :math:`R` is the radius of the sphere of interest. This
   transformation does not depend on the angular coordinates. Thus, the
   generated points follow a uniform distribution on :math:`\cS^{*}`.

.. plot::

    from mpl_toolkits.mplot3d import Axes3D
    import matplotlib.pyplot as plt
    import openturns as ot

    x = ot.Normal(3).getSample(1000)
    for i in range(len(x)):
        x[i] /= x[i].norm()
    xs, ys, zs = map(lambda j: x.getMarginal(j).asPoint(), range(x.getDimension()))

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(xs, ys, zs, marker='.')


.. topic:: API:

    - See :class:`~openturns.StrongMaximumTest`
    - See :class:`~openturns.FORM`


.. topic:: References:

    - Luban, Marshall, Staunton, 1988, "An efficient method for generating a uniform distribution of points within a hypersphere," Computer in Physics, 2(6), 55.

