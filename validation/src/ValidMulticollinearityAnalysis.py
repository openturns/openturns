import openturns as ot
import openturns.viewer as otv
import time


def compute_measure(X, Y, type):
    """
    Compute a measure for the given dataset

    Parameters
    - X: the input sample
    - Y: the output sample
    - type: the measure type ("LMG", "PMVD", "Johnson" or "VIF")
    """
    analysis = ot.MulticollinearityAnalysis(X, Y)
    if type == "LMG":
        lmg, _ = analysis.computeLmgPmvd()
        return lmg
    elif type == "PMVD":
        _, pmvd = analysis.computeLmgPmvd()
        return pmvd
    elif type == "Johnson":
        return analysis.computeJohnson()
    elif type == "VIF":
        return analysis.computeVif()
    else:
        raise Exception(f"Invalid type: {type}")


def create_graph(title, names, mean, interval):
    dimension = mean.getDimension()
    palette = ot.Drawable.BuildDefaultPalette(2)
    graph = ot.Graph(title, "", "")

    # Confidence bounds
    lb = interval.getLowerBound()
    ub = interval.getUpperBound()
    for i in range(dimension):
        curve = ot.Curve([1 + i, 1 + i], [lb[i], ub[i]])
        curve.setLineWidth(2.0)
        curve.setColor(palette[1])
        graph.add(curve)

    # Means
    data = ot.Sample(0, 2)
    for i in range(dimension):
        data.add([i + 1, mean[i]])
    cloud = ot.Cloud(data, palette[0], "circle")
    graph.add(cloud)

    box = graph.getBoundingBox()
    box.setLowerBound([0.9, 0.0])
    graph.setBoundingBox(box)
    axes_kw = {"xticks": range(1, dimension + 1), "xticklabels": names}
    otv.View(graph, axes_kw=axes_kw)


def compute(case, type, bootstrap_size=100):
    """
    Compute a measure for the given test case, display the value and the calculation time;
    then perform a bootstrap and show a graph with the results

    Parameters
    - case: the case number (between 1 and 8)
    - type: the measure type ("LMG", "PMVD", "Johnson" or "VIF")
    - bootstrap_size: the number of iterations used by the bootstrap
    """

    # Load data from CSV file
    sample = ot.Sample.ImportFromCSVFile(f"ValidMulticollinearityAnalysis-case{case}.csv", ";")
    Y = sample.getMarginal(0)
    X = sample.getMarginal(range(1, sample.getDimension()))
    dimension = X.getDimension()
    names = X.getDescription()
    title = f"Case #{case} - {type}"

    # Compute the requested measure
    print("-" * 18)
    print(title)
    print("-" * 18)
    print()
    rep = 20
    t1 = time.time()
    for i in range(rep):
        measure = compute_measure(X, Y, type)
    t2 = time.time()
    for i in range(dimension):
        print(f"{names[i]} = {measure[i]:.8f}")
    duration = (t2 - t1) / rep
    print()
    print(f"Duration: {1000 * duration:.1f} ms\n")

    # Perform a bootstrap
    sample_size = sample.getSize()
    boot = ot.Sample(bootstrap_size, dimension)
    for i in range(bootstrap_size):
        selection = ot.BootstrapExperiment.GenerateSelection(sample_size, sample_size)
        X_boot = X[selection]
        Y_boot = Y[selection]
        boot[i, :] = compute_measure(X_boot, Y_boot, type)

    # Show a graph with mean values and confidence intervals
    mean = boot.computeMean()
    alpha = 0.05
    lb = boot.computeQuantilePerComponent(alpha / 2.0)
    ub = boot.computeQuantilePerComponent(1.0 - alpha / 2.0)
    interval = ot.Interval(lb, ub)
    create_graph(title, names, mean, interval)


case = 1
for type in ["LMG", "PMVD", "Johnson", "VIF"]:
    compute(case, type)

otv.View.ShowAll()
