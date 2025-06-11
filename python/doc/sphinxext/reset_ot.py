# this resets OT between each example


def reset_ot(gallery_conf, fname):
    import openturns as ot

    ot.Log.Show(ot.Log.NONE)
    ot.ResourceMap.Reload()
    seed = ot.ResourceMap.GetAsUnsignedInteger("RandomGenerator-InitialSeed")
    ot.RandomGenerator.SetSeed(seed)

    # explicitly set agg backend
    import matplotlib

    matplotlib_backend = matplotlib.get_backend().lower()
    if matplotlib_backend != "agg":
        matplotlib.use("agg")
