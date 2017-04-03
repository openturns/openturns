#! /usr/bin/env python

import os
import traceback

try:

    # use non-interactive backend
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    from openturns.viewer import View
    import openturns as ot

    # Curve
    graph = ot.Normal().drawCDF()
    # graph.draw('curve1.png')
    view = View(graph, pixelsize=(800, 600), plot_kwargs={'color': 'blue'})
    # view.save('curve1.png')
    view.show()

    # Contour
    graph = ot.Normal([1, 2], [3, 5], ot.CorrelationMatrix(2)).drawPDF()
    # graph.draw('curve2.png')
    view = View(graph)
    # view.save('curve2.png')
    view.show()

    # Histogram tests
    normal = ot.Normal(1)
    size = 100
    sample = normal.getSample(size)
    graph = ot.VisualTest.DrawHistogram(sample, 10)
    # graph.draw('curve3.png')
    view = View(graph)
    # view.save('curve3.png')
    view.show()

    # QQPlot tests
    size = 100
    normal = ot.Normal(1)
    sample = normal.getSample(size)
    sample2 = ot.Gamma(3.0, 4.0, 0.0).getSample(size)
    graph = ot.VisualTest.DrawQQplot(sample, sample2, 100)
    # graph.draw('curve4.png')
    view = View(graph)
    # view.save('curve4.png')
    plt.show(block=True)

    # Clouds tests
    dimension = (2)
    R = ot.CorrelationMatrix(dimension)
    R[0, 1] = 0.8
    distribution = ot.Normal(
        ot.Point(dimension, 3.0), ot.Point(dimension, 2.0), R)
    size = 100
    sample2D = distribution.getSample(size)
    firstSample = ot.Sample(size, 1)
    secondSample = ot.Sample(size, 1)
    for i in range(size):
        firstSample[i] = ot.Point(1, sample2D[i, 0])
        secondSample[i] = ot.Point(1, sample2D[i, 1])
    graph = ot.VisualTest.DrawClouds(
        sample2D, ot.Normal(ot.Point(dimension, 2.0), ot.Point(dimension, 3.0), R).getSample(size // 2))
    # graph.draw('curve5.png')
    view = View(graph)
    # view.save('curve5.png')
    View.ShowAll()

    # Text
    graph = ot.Graph('Annotated cloud', 'x', 'y', True, '')
    distribution = ot.Normal(2)
    size = 30
    sample2D = distribution.getSample(size)
    cloud = ot.Cloud(sample2D, "red", "fsquare", "Sample2D Cloud");
    graph.add(cloud)

    # Display extrema indices
    x1 = [ x[0] for x in sample2D[:,0]]
    x2 = [ x[0] for x in sample2D[:,1]]
    idx = [0] * 4
    idx[0] = x1.index(min(x1))
    idx[1] = x1.index(max(x1))
    idx[2] = x2.index(min(x2))
    idx[3] = x2.index(max(x2))

    labels = ot.Description(sample2D.getSize())
    for i in range(4):
      labels[idx[i]] = str(idx[i])

    position = ot.Description(sample2D.getSize(), "top")
    position[idx[0]] = "right"
    position[idx[1]] = "left"
    position[idx[2]] = "top"
    position[idx[3]] = "bottom"

    text = ot.Text(sample2D, labels)
    text.setColor("red")
    text.setTextPositions(position)

    graph.add(text)
    view = View(graph)
    view.show()

    # CobWeb tests
    size = 100
    dim = 6
    inputSample = ot.Normal(dim).getSample(size)
    inputVar = list(['X' + str(i) for i in range(dim)])
    expression = ''
    for i in range(dim):
        if i > 0:
            expression += '+'
        expression += 'cos(' + str(i + 1) + '*' + inputVar[i] + ')'
    model = ot.Function(inputVar, ['y'], [expression])
    outputSample = model(inputSample)
    graph = ot.VisualTest.DrawCobWeb(
        inputSample, outputSample, 2.5, 3.0, 'red', False)
    # graph.draw('curve6.png')
    view = View(graph, legend_kwargs={'loc': 'lower center'})
    # view.save('curve6.png')
    view.show()

    # Staircase
    distribution = ot.Poisson(10.0)
    graph = distribution.drawCDF()
    # graph.draw('curve7.png')
    view = View(graph)
    # view.save('curve7.png')
    view.show()

    # Pie
    graph = ot.SobolIndicesAlgorithmImplementation.DrawImportanceFactors(
        [.4, .3, .2, .1], ['a0', 'a1', 'a2', 'a3'], 'Zou')
    # graph.draw('curve8.png')
    view = View(graph)
    # view.save('curve8.png')
    plt.show(block=True)

    # Pairs
    dim = 5
    meanPoint = ot.Point(dim, 0.0)
    sigma = ot.Point(dim, 1.0)
    R = ot.CorrelationMatrix(dim)
    for i in range(dim):
        meanPoint[i] = (i + 1) * dim
    distribution = ot.Normal(meanPoint, sigma, R)
    size = 1000
    sample = distribution.getSample(size)
    graph = ot.Graph('Pairs', ' ', ' ', True, 'topright')
    labels = list(['x' + str(i) for i in range(dim)])
    myPairs = ot.Pairs(sample, 'Pairs example',
                       labels, 'green', 'bullet')
    graph.add(myPairs)
    # graph.draw('curve9.png')
    view = View(graph)
    # view.save('curve9.png')
    View.ShowAll()

    # Convergence graph curve
    aCollection = []
    aCollection.append(ot.LogNormalFactory().build(ot.LogNormalMuSigma()([300.0, 30.0, 0.0])))
    aCollection.append(ot.Normal(75e3, 5e3))
    myDistribution = ot.ComposedDistribution(aCollection)
    vect = ot.RandomVector(myDistribution)
    LimitState = ot.SymbolicFunction(
        ('R', 'F'), ('R-F/(_pi*100.0)',))
    G = ot.RandomVector(LimitState, vect)
    myEvent = ot.Event(G, ot.Less(), 0.0)
    myAlgo = ot.MonteCarlo(myEvent)
    myAlgo.setMaximumCoefficientOfVariation(0.05)
    myAlgo.setMaximumOuterSampling(int(1e5))
    myAlgo.run()
    graph = myAlgo.drawProbabilityConvergence()
    # graph.draw('curve10.png')
    view = View(graph)
    # view.save('curve10.png')
    view.show()

    # Polygon
    size = 50
    cursor = [0.] * 2
    data1 = ot.Sample(size, 2)  # polygon y = 2x for x in [-25]
    data2 = ot.Sample(size, 2)  # polygon y = x*x for x in [-11]
    for i in range(size):
        tmp = 7. * i / size + 2
        cursor[0] = tmp
        cursor[1] = 2 * tmp
        data1[i] = cursor
        tmp = 9. * i / size + 1
        cursor[0] = tmp
        cursor[1] = tmp * tmp
        data2[i] = cursor
    graph = ot.Graph('Some polygons', 'x1', 'x2', True, 'topright', 1.0)
    myPolygon1 = ot.Polygon(data1)
    myPolygon1.setColor('blue')
    graph.add(myPolygon1)
    myPolygon2 = ot.Polygon(data2)
    myPolygon2.setColor('red')
    graph.add(myPolygon2)
    # graph.draw('curve11.png')
    view = View(graph)
    # view.save('curve11.png')
    view.show()

    # PolygonArray
    generator = ot.Normal(2)
    size = 5
    array = ot.PolygonCollection(size)
    palette = ot.Drawable.BuildDefaultPalette(size)
    palette[0] = 'blue'
    for i in range(size):
        vertices = generator.getSample(3)
        array[i] = ot.Polygon(vertices, palette[i], palette[size - i - 1])
    graph = ot.Graph('An array of polygons', 'x', 'y', True, '')
    graph.add(ot.PolygonArray(array))
    # graph.draw('curve12.png')
    view = View(graph)
    # view.save('curve12.png')
    View.ShowAll()

except:
    traceback.print_exc()
    os._exit(1)
