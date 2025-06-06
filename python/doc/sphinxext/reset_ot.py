# this resets OT between each example

def reset_ot(gallery_conf, fname):
    import openturns as ot
    ot.Log.Show(ot.Log.NONE)
    ot.RandomGenerator.SetSeed(0)
    ot.ResourceMap.Reload()

    # explicitely set agg backend
    import matplotlib
    matplotlib_backend = matplotlib.get_backend().lower()
    if matplotlib_backend != "agg":
        matplotlib.use("agg")
