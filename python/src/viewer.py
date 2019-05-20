"""
Graph viewer.

Graph or Drawable viewer using matplotlib

Examples
--------
>>> import openturns as ot
>>> from openturns.viewer import View
>>> graph = ot.Normal().drawPDF()
>>> view = View(graph, plot_kwargs={'color':'blue'})
>>> view.save('curve.png', dpi=100)
>>> view.show()
"""
import openturns as ot
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from distutils.version import LooseVersion
import os
import re
import warnings
import io
import sys

__all__ = ["View", "PlotDesign"]


class View(object):

    """
    Create the figure.

    Parameters
    ----------
    graph : :class:`~openturns.Graph, :class:`~openturns.Drawable`
        A Graph or Drawable object.

    pixelsize : 2-tuple of int
        The requested size in pixels (width, height).

    figure : :class:`matplotlib.figure.Figure`
        The figure to draw on.

    figure_kwargs : dict, optional
        Passed on to matplotlib.pyplot.figure kwargs

    axes : :class:`matplotlib.axes.Axes`
        The axes to draw on.

    plot_kwargs : dict, optional
        Used when drawing Cloud, Curve drawables
        Passed on as matplotlib.axes.Axes.plot kwargs

    axes_kwargs : dict, optional
        Passed on to matplotlib.figure.Figure.add_subplot kwargs

    bar_kwargs : dict, optional
        Used when drawing BarPlot drawables
        Passed on to matplotlib.pyplot.bar kwargs

    pie_kwargs : dict, optional
        Used when drawing Pie drawables
        Passed on to matplotlib.pyplot.pie kwargs

    polygon_kwargs : dict, optional
        Used when drawing Polygon drawables
        Passed on to matplotlib.patches.Polygon kwargs

    polygoncollection_kwargs : dict, optional
        Used when drawing PolygonArray drawables
        Passed on to matplotlib.collection.PolygonCollection kwargs

    contour_kwargs : dict, optional
        Used when drawing Contour drawables
        Passed on to matplotlib.pyplot.contour kwargs

    clabel_kwargs : dict, optional
        Used when drawing Contour drawables
        Passed on to matplotlib.pyplot.clabel kwargs

    step_kwargs : dict, optional
        Used when drawing Staircase drawables
        Passed on to matplotlib.pyplot.step kwargs

    text_kwargs : dict, optional
        Used when drawing Pairs, Text drawables
        Passed on to matplotlib.axes.Axes.text kwargs

    legend_kwargs : dict, optional
        Passed on to matplotlib.axes.Axes.legend kwargs

    add_legend : bool, optional
        Adds a legend if True. Default is True.
    square_axes : bool, optional
        Forces the axes to share the same scale if True. Default is False.
    """

    @staticmethod
    def CheckDict(arg):
        """Check that the argument is a python dictionary."""
        result = arg
        if arg is None:
            result = dict()
        elif not isinstance(arg, dict):
            raise TypeError('Argument is not a dict')
        return result

    @staticmethod
    def ToUnicode(s):
        """Convert to unicode if necessary."""
        if isinstance(s, bytes):
            s = s.decode('utf8')
        return s

    def __init__(self,
                 graph,
                 pixelsize=None,
                 figure=None,
                 figure_kwargs=None,
                 axes=[],
                 plot_kwargs=None,
                 axes_kwargs=None,
                 bar_kwargs=None,
                 pie_kwargs=None,
                 polygon_kwargs=None,
                 polygoncollection_kwargs=None,
                 contour_kwargs=None,
                 step_kwargs=None,
                 clabel_kwargs=None,
                 text_kwargs=None,
                 legend_kwargs=None,
                 add_legend=True,
                 square_axes=False,
                 **kwargs):

        # prevent Qt from stopping the interpreter, see matplotlib PR #1905
        if LooseVersion(matplotlib.__version__) < LooseVersion('1.3'):
            # check for DISPLAY env variable on X11 build of Qt
            if plt.get_backend().startswith('Qt4'):
                from matplotlib.backends.qt4_compat import QtGui
                if hasattr(QtGui, 'QX11Info'):
                    display = os.environ.get('DISPLAY')
                    if display is None or not re.search(':\d', display):
                        raise RuntimeError('Invalid DISPLAY variable')

        if not (isinstance(graph, ot.Graph) or isinstance(graph, ot.GraphImplementation)):
            if isinstance(graph, ot.Drawable) or isinstance(graph, ot.DrawableImplementation):
                # convert Drawable => Graph
                drawable = graph
                graph = ot.Graph()
                graph.add(drawable)
            else:
                raise TypeError(
                    'The given object cannot be converted into a Graph nor Drawable.')

        drawables = graph.getDrawables()
        n_drawables = len(drawables)
        if n_drawables == 0:
            warnings.warn('-- Nothing to draw.')
            return

        # check that arguments are dictionnaries
        figure_kwargs = self.CheckDict(figure_kwargs)
        axes_kwargs = self.CheckDict(axes_kwargs)
        plot_kwargs_default = self.CheckDict(plot_kwargs)
        bar_kwargs_default = self.CheckDict(bar_kwargs)
        pie_kwargs_default = self.CheckDict(pie_kwargs)
        polygon_kwargs_default = self.CheckDict(polygon_kwargs)
        polygoncollection_kwargs_default = self.CheckDict(
            polygoncollection_kwargs)
        contour_kwargs_default = self.CheckDict(contour_kwargs)
        step_kwargs_default = self.CheckDict(step_kwargs)
        clabel_kwargs_default = self.CheckDict(clabel_kwargs)
        text_kwargs_default = self.CheckDict(text_kwargs)
        legend_kwargs = self.CheckDict(legend_kwargs)

        # set image size in pixels
        if pixelsize is not None:
            if len(pixelsize) != 2:
                raise ValueError('pixelsize must be a 2-tuple.')
            figure_kwargs.setdefault('dpi', 100)
            dpi = figure_kwargs['dpi']
            border = 10  # guess
            width, height = pixelsize
            width -= border
            height -= border
            figure_kwargs.setdefault(
                'figsize', (width * 1.0 / dpi, height * 1.0 / dpi))

        # set step drawstyle
        step_kwargs_default.setdefault('where', 'post')

        # set figure
        if figure is None:
            if len(axes) == 0:
                self._fig = plt.figure(**figure_kwargs)
            else:
                self._fig = axes[0].get_figure()
        else:
            self._fig = figure
            if len(axes) == 0:
                axes = self._fig.axes

        # set title
        self._fig.suptitle(self.ToUnicode(graph.getTitle()))

        # set axes
        if len(axes) == 0:
            self._ax = [self._fig.add_subplot(111, **axes_kwargs)]
        else:
            self._ax = axes

        # activate axes only if wanted
        self._ax[0].axison = graph.getAxes()

        # set bounding box
        bb = graph.getBoundingBox()
        self._ax[0].set_xlim([bb.getLowerBound()[0], bb.getUpperBound()[0]])
        self._ax[0].set_ylim([bb.getLowerBound()[1], bb.getUpperBound()[1]])

        # set scale
        if (graph.getLogScale() == ot.GraphImplementation.LOGX) or (graph.getLogScale() == ot.GraphImplementation.LOGXY):
            self._ax[0].set_xscale('log')
        if (graph.getLogScale() == ot.GraphImplementation.LOGY) or (graph.getLogScale() == ot.GraphImplementation.LOGXY):
            self._ax[0].set_yscale('log')

        self._ax[0].grid(b=graph.getGrid())

        # use scientific notation on non-log axis
        axis_mask = {ot.GraphImplementation.NONE: 'both',
                     ot.GraphImplementation.LOGX: 'y',
                     ot.GraphImplementation.LOGY: 'x',
                     ot.GraphImplementation.LOGXY: None}
        axis = axis_mask[graph.getLogScale()]
        if axis is not None:
            self._ax[0].ticklabel_format(
                axis=axis, style='sci', scilimits=(-3, 5))

        has_labels = False

        for drawable in drawables:
            drawableKind = drawable.getImplementation().getClassName()

            # reset working dictionaries by explicitely creating copies
            plot_kwargs = dict(plot_kwargs_default)
            bar_kwargs = dict(bar_kwargs_default)
            pie_kwargs = dict(pie_kwargs_default)
            polygon_kwargs = dict(polygon_kwargs_default)
            polygoncollection_kwargs = dict(polygoncollection_kwargs_default)
            contour_kwargs = dict(contour_kwargs_default)
            step_kwargs = dict(step_kwargs_default)
            clabel_kwargs = dict(clabel_kwargs_default)
            text_kwargs = dict(text_kwargs_default)

            # set color
            if ('color' not in plot_kwargs_default) and ('c' not in plot_kwargs_default):
                plot_kwargs['color'] = drawable.getColorCode()
            if ('color' not in bar_kwargs_default) and ('c' not in bar_kwargs_default):
                bar_kwargs['color'] = drawable.getColorCode()
            if ('color' not in step_kwargs_default) and ('c' not in step_kwargs_default):
                step_kwargs['color'] = drawable.getColorCode()
            if drawableKind != 'Pairs':
                if ('color' not in text_kwargs_default) and ('c' not in text_kwargs_default):
                    text_kwargs['color'] = drawable.getColorCode()

            # set marker
            pointStyleDict = {'square': 's', 'circle': 'o', 'triangleup': '^', 'plus': '+', 'times': 'x', 'diamond': 'd', 'triangledown':
                              'v', 'star': '*', 'fsquare': 's', 'fcircle': 'o', 'ftriangleup': '^', 'fdiamond': 'd', 'bullet': '.', 'dot': ',', 'none': 'None'}
            if 'marker' not in plot_kwargs_default:
                try:
                    plot_kwargs['marker'] = pointStyleDict[
                        drawable.getPointStyle()]
                except:
                    warnings.warn(
                        '-- Unknown marker: ' + drawable.getPointStyle())

            # set line style
            lineStyleDict = {'solid': '-', 'dashed': '--', 'dotted':
                             ':', 'dotdash': '-.', 'longdash': '--', 'twodash': '--'}
            if ('linestyle' not in plot_kwargs_default) and ('ls' not in plot_kwargs_default):
                try:
                    plot_kwargs['linestyle'] = lineStyleDict[
                        drawable.getLineStyle()]
                except:
                    warnings.warn('-- Unknown line style')
            if ('linestyle' not in step_kwargs_default) and ('ls' not in step_kwargs_default):
                try:
                    step_kwargs['linestyle'] = lineStyleDict[
                        drawable.getLineStyle()]
                except:
                    warnings.warn('-- Unknown line style')

            # set line width
            if ('linewidth' not in plot_kwargs_default) and ('lw' not in plot_kwargs_default):
                plot_kwargs['linewidth'] = drawable.getLineWidth()
            if ('linewidth' not in step_kwargs_default) and ('lw' not in step_kwargs_default):
                step_kwargs['linewidth'] = drawable.getLineWidth()

            # retrieve data
            data = drawable.getData()
            x = data.getMarginal(0).asPoint()
            if data.getDimension() > 1:
                y = data.getMarginal(1).asPoint()

            # add label, title
            if drawableKind != 'Pie':
                self._ax[0].set_xlabel(self.ToUnicode(graph.getXTitle()))
                self._ax[0].set_ylabel(self.ToUnicode(graph.getYTitle()))

                if (len(drawable.getLegend()) > 0) and ((drawableKind != 'Cloud') or (drawable.getPointStyle() != 'none')):
                    label = self.ToUnicode(drawable.getLegend())
                    has_labels = True
                    plot_kwargs.setdefault('label', label)
                    bar_kwargs.setdefault('label', label)
                    step_kwargs.setdefault('label', label)
                    polygon_kwargs.setdefault('label', label)
                    polygoncollection_kwargs.setdefault('label', label)

            if drawableKind == 'BarPlot':
                # linestyle for bar() is different than the one for plot()
                if 'linestyle' in bar_kwargs_default:
                    bar_kwargs.pop('linestyle')
                if ('linestyle' not in plot_kwargs_default) and ('ls' not in plot_kwargs_default):
                    lineStyleDict = {'solid': '-', 'dashed': '--', 'dotted': ':',
                                     'dotdash': '-.', 'longdash': '--', 'twodash': '--'}
                    if drawable.getLineStyle() in lineStyleDict:
                        bar_kwargs['linestyle'] = lineStyleDict[
                            drawable.getLineStyle()]
                    else:
                        warnings.warn(
                            '-- Unknown line style: ' + drawable.getLineStyle())

                # fillstyle
                if drawable.getFillStyle() == 'shaded':
                    bar_kwargs['fill'] = False
                    bar_kwargs['hatch'] = '////'
                    bar_kwargs['edgecolor'] = bar_kwargs['color']

                xi = drawable.getOrigin()
                for i in range(x.getSize()):
                    # label only the first bar to avoid getting several items
                    if (i == 1) and ('label' in bar_kwargs):
                        bar_kwargs.pop('label')
                    self._ax[0].bar(
                        xi, height=y[i], width=x[i], align='edge', **bar_kwargs)
                    xi += x[i]

            elif drawableKind == 'Cloud':
                plot_kwargs['linestyle'] = 'None'
                self._ax[0].plot(x, y, **plot_kwargs)

            elif drawableKind == 'Curve':
                self._ax[0].plot(x, y, **plot_kwargs)

            elif drawableKind == 'Polygon':

                if ('facecolor' not in polygon_kwargs_default) and ('fc' not in polygon_kwargs_default):
                    polygon_kwargs['facecolor'] = drawable.getColorCode()

                if ('edgecolor' not in polygon_kwargs_default) and ('ec' not in polygon_kwargs_default):
                    polygon_kwargs['edgecolor'] = drawable.ConvertFromName(
                        drawable.getEdgeColor())
                self._ax[0].add_patch(
                    matplotlib.patches.Polygon(data, **polygon_kwargs))

            elif drawableKind == 'PolygonArray':

                polygonsNumber = drawable.getPalette().getSize()
                verticesNumber = drawable.getData().getSize() // polygonsNumber
                colorsRGBA = drawable.getPaletteAsNormalizedRGBA()
                if 'facecolors' not in polygoncollection_kwargs_default:
                    polygoncollection_kwargs['facecolors'] = colorsRGBA

                if 'edgecolors' not in polygoncollection_kwargs_default:
                    polygoncollection_kwargs['edgecolors'] = colorsRGBA
                self._ax[0].add_collection(
                    matplotlib.collections.PolyCollection(np.array(data).reshape((polygonsNumber, verticesNumber, 2)), **polygoncollection_kwargs))

            elif drawableKind == 'Pie':
                pie_kwargs.setdefault(
                    'labels', [self.ToUnicode(label) for label in drawable.getLabels()])
                pie_kwargs.setdefault('colors', drawable.getPalette())
                self._ax[0].set_aspect('equal')
                self._ax[0].pie(x, **pie_kwargs)

            elif drawableKind == 'Contour':
                X, Y = np.meshgrid(drawable.getX(), drawable.getY())
                Z = np.reshape(drawable.getData(), (
                    drawable.getX().getSize(), drawable.getY().getSize()))
                contour_kwargs.setdefault('levels', drawable.getLevels())
                if ('linestyles' not in contour_kwargs_default) and ('ls' not in contour_kwargs_default):
                    try:
                        contour_kwargs['linestyles'] = lineStyleDict[
                            drawable.getLineStyle()]
                    except:
                        warnings.warn('-- Unknown line style')
                if 'colors' not in contour_kwargs_default:
                    contour_kwargs['colors'] = [drawable.getColorCode()]
                contourset = self._ax[0].contour(X, Y, Z, **contour_kwargs)
                if drawable.getDrawLabels():
                    clabel_kwargs.setdefault('fontsize', 8)
                    # Use labels
                    fmt = {}
                    for l, s in zip(np.array(drawable.getLevels()), drawable.getLabels()):
                        fmt[l] = s
                    clabel_kwargs.setdefault('fmt', fmt)
                    try:
                        plt.clabel(contourset, **clabel_kwargs)
                    except KeyError:
                        # https://github.com/matplotlib/matplotlib/issues/9742
                        warnings.warn(
                            'pyplot.clabel likely failed on boundary level')
                    except UnboundLocalError:
                        # https://github.com/matplotlib/matplotlib/pull/10710
                        warnings.warn(
                            'pyplot.clabel likely failed as in #10710')
                for i in range(len(contourset.levels)):
                    contourset.collections[i].set_label(
                        '_nolegend_' if i > 0 else drawable.getLegend())

            elif drawableKind == 'Staircase':
                self._ax[0].step(x, y, **step_kwargs)

            elif drawableKind == 'Pairs':
                # disable axis : grid, ticks, axis
                self._ax[0].axison = False

                axes_kwargs['xticks'] = []
                axes_kwargs['yticks'] = []

                dim = drawable.getData().getDimension()
                labels = drawable.getLabels()

                # adjust font
                if ('fontsize' not in text_kwargs_default) and ('size' not in text_kwargs_default):
                    text_kwargs['fontsize'] = max(16 - dim, 4)
                text_kwargs.setdefault('horizontalalignment', 'center')
                text_kwargs.setdefault('verticalalignment', 'center')
                for i in range(dim):
                    for j in range(dim):
                        if len(self._ax) <= dim * dim:
                            self._ax.append(self._fig.add_subplot(
                                dim, dim, 1 + i * dim + j, **axes_kwargs))
                        if i != j:
                            x = drawable.getData().getMarginal(i)
                            y = drawable.getData().getMarginal(j)
                            x_min = x.getMin()[0]
                            x_max = x.getMax()[0]
                            x_margin = 0.1 * (x_max - x_min)
                            y_min = y.getMin()[0]
                            y_max = y.getMax()[0]
                            y_margin = 0.1 * (y_max - y_min)
                            plot_kwargs['linestyle'] = 'None'
                            self._ax[1 + i * dim + j].plot(y, x, **plot_kwargs)
                            self._ax[1 + i * dim + j].set_ylim(
                                x_min - x_margin, x_max + x_margin)
                            self._ax[1 + i * dim + j].set_xlim(
                                y_min - y_margin, y_max + y_margin)
                        else:
                            text_kwargs['transform'] = self._ax[
                                1 + i * dim + j].transAxes
                            self._ax[1 + i * dim + j].text(
                                0.5, 0.5, labels[i], **text_kwargs)

            elif drawableKind == 'Text':
                dim = drawable.getData().getDimension()

                # adjust font
                if ('fontsize' not in text_kwargs_default) and ('size' not in text_kwargs_default):
                    # R API requiert a character exansion (cex)
                    # considering default scale to be 1
                    # Default fontsize in matplotlib is 10.0
                    # thus we make corresponding the R & matplotlib values
                    text_kwargs['fontsize'] = drawable.getTextSize() * 10.0
                if 'horizontalalignment' in text_kwargs_default:
                    horizontal_default = text_kwargs['horizontalalignment']
                    del text_kwargs['horizontalalignment']
                else:
                    horizontal_default = 'center'
                if 'verticalalignment' in text_kwargs_default:
                    vertical_default = text_kwargs['verticalalignment']
                    del text_kwargs['verticalalignment']
                else:
                    vertical_default = 'center'
                positions = drawable.getTextPositions()

                for i, text in enumerate(drawable.getTextAnnotations()):
                    if len(text) == 0:
                        continue
                    horizontal = horizontal_default
                    vertical = vertical_default
                    # Text.getTextPositions() describes position of text with respect
                    # to given coordinates;  in matplotlib, positional argument controls
                    # which side of the text bounding box is positioned, its meaning is
                    # thus inverted.
                    if positions[i] == 'left':
                        horizontal = 'right'
                    elif positions[i] == 'right':
                        horizontal = 'left'
                    elif positions[i] == 'top':
                        vertical = 'bottom'
                    elif positions[i] == 'bottom':
                        vertical = 'top'
                    self._ax[0].text(
                        x[i], y[i], text, horizontalalignment=horizontal, verticalalignment=vertical, **text_kwargs)

            else:
                raise ValueError(
                    'Drawable type not implemented: ' + drawableKind)

        # Add legend
        if add_legend and has_labels and (graph.getLegendPosition() != ''):
            # set legend position
            if 'loc' not in legend_kwargs:
                try:
                    legendPositionDict = {'bottomright': 'lower right',
                                          'bottom': 'lower center',
                                          'bottomleft': 'lower left',
                                          'left': 'center left',
                                          'topleft': 'upper left',
                                          'topright': 'upper right',
                                          'right': 'center right',
                                          'center': 'center'}
                    legend_kwargs['loc'] = legendPositionDict[
                        graph.getLegendPosition()]
                except:
                    warnings.warn(
                        '-- Unknown legend position: ' + graph.getLegendPosition())

            # set a single legend point
            legend_kwargs.setdefault('numpoints', 1)

            # enable round box by default
            legend_kwargs.setdefault('fancybox', True)

            # enable shadow by default
            legend_kwargs.setdefault('shadow', True)

            # by default legend is a bit too large
            legend_kwargs.setdefault('prop', {'size': 10})

            self._ax[0].legend(**legend_kwargs)
        # Make squares look like squares
        if square_axes:
            try:
                self._ax[0].axis('square')
            except ValueError:
                warnings.warn('axis square keyword not supported')
                plt.gca().set_aspect('equal', adjustable='box')

    def show(self, **kwargs):
        """
        Display the graph.

        See http://matplotlib.org/api/figure_api.html#matplotlib.figure.Figure.show
        """
        if hasattr(self._fig, 'show'):
            self._fig.show(**kwargs)
        else:
            # mpl < 1.3.1, see https://github.com/ipython/ipython/pull/1615
            plt.show(**kwargs)

    @staticmethod
    def ShowAll(**kwargs):
        """Display all graphs."""
        plt.show(**kwargs)

    def save(self, fname, **kwargs):
        """
        Save the graph as file.

        Parameters
        ----------
        fname: bool, optional
            A string containing a path to a filename from which file format is deduced.

        kwargs:
            Refer to matplotlib.figure.Figure.savefig documentation for valid keyword arguments.
        """
        self._fig.savefig(fname, **kwargs)

    def getFigure(self):
        """
        Accessor to the underlying figure object.

        Refer to matplotlib.figure.Figure for further information.
        """
        return self._fig

    def getAxes(self):
        """
        Get the list of Axes objects.

        Refer to matplotlib.axes.Axes for further information.
        """
        return self._ax

    def close(self):
        """Close the figure."""
        plt.close(self._fig)


def ToImageString(graph):
    """
    Convert as image string.

    Parameters
    ----------
    graph : object
        A Graph or Drawable object.

    Returns
    -------
    image_str : str
       An image representation as string
    """
    # save interactive mode state
    ision = plt.isinteractive()
    plt.ioff()

    view = View(graph)
    output = io.BytesIO()
    fmt = ot.ResourceMap.Get('View-ImageFormat')
    view.save(output, format=fmt, dpi=100)
    view.close()

    # restore interactive mode state
    if ision:
        plt.ion()

    image_bytes = output.getvalue()
    if fmt == 'svg':
        image_string = image_bytes.decode('utf-8')
    else:
        # raw bytes
        image_string = image_bytes
    return image_string


def PlotDesign(design, bounds, Nx, Ny, figure=None, axes=[], plot_kwargs={}, axes_kwargs={}, text_kwargs={}):
    """
    Plot a design using a scatter plot approach (plots 2D marginals).

    In addition, the function plots a grid, i.e. horizontal and vertical lines
    to distinguish LHS character

    Parameters
    ----------
    design : 2-d sequence of float
        Design
    bounds: :class:`openturns.Interval`
        Bounds of the underlying distribution
    nxdiv : int
        Number of subdivisions in the X axis
    nydiv : int
        Number of subdivisions in the Y axis

    Returns
    -------
    fig : matplotlib figure
          Figure representing the LHS

    Examples
    --------
    >>> import openturns as ot
    >>> from openturns.viewer import PlotDesign
    >>> # Bounds are [0,1]^5
    >>> # Size of sample
    >>> size = 10
    >>> # Factory: lhs generates (here centered)
    >>> distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)]*5)
    >>> bounds = distribution.getRange()
    >>> lhs = ot.LHSExperiment(distribution, size)
    >>> lhs.setRandomShift(False) # centered
    >>> lhs.setAlwaysShuffle(True) # randomized
    >>> # Generate a design
    >>> design = lhs.generate()
    >>> # Plot the design
    >>> fig = PlotDesign(design, bounds, 10, 10)
    """
    # check that arguments are dictionnaries
    assert(isinstance(axes_kwargs, dict))
    assert(isinstance(plot_kwargs, dict))
    assert(isinstance(text_kwargs, dict))

    # retrieve data
    data = ot.Sample(design)
    dim = data.getDimension()
    if dim < 2:
        raise TypeError('Expected designs of dimension >=2')
    labels = data.getDescription()

    # set bounding box
    # set figure
    if figure is None:
        figure = plt.figure()
    else:
        if len(axes) == 0:
            axes = figure.axes

    # set axes
    if len(axes) == 0:
        axes = [figure.add_subplot(111, **axes_kwargs)]

    # disable axis : grid, ticks, axis?
    axes[0].axison = False

    axes_kwargs['xticks'] = []
    axes_kwargs['yticks'] = []

    # adjust font
    if (not 'fontsize' in text_kwargs) and (not 'size' in text_kwargs):
        text_kwargs['fontsize'] = max(16 - dim, 4)
    text_kwargs.setdefault('horizontalalignment', 'center')
    text_kwargs.setdefault('verticalalignment', 'center')

    # set marker
    pointStyleDict = {'square': 's', 'circle': 'o', 'triangleup': '2', 'plus': '+', 'times': '+', 'diamond': '+', 'triangledown':
                      'v', 'star': '*', 'fsquare': 's', 'fcircle': 'o', 'ftriangleup': '2', 'fdiamond': 'D', 'bullet': '.', 'dot': ',', 'none': 'None'}
    if not 'marker' in plot_kwargs:
        plot_kwargs['marker'] = pointStyleDict["square"]

    # Particular case of dim=2
    if dim == 2:
        x = data.getMarginal(0)
        y = data.getMarginal(1)
        # x axis
        x_min = bounds.getLowerBound()[0]
        x_max = bounds.getUpperBound()[0]
        dx = x_max - x_min
        # y axis
        y_min = bounds.getLowerBound()[1]
        y_max = bounds.getUpperBound()[1]
        dy = y_max - y_min
        # Draw horizontal lines
        dydiv = float(Ny)
        xk = [x_min, x_max]
        for k in range(Ny):
            yk = [y_min + k * dy / dydiv, y_min + k * dy / dydiv]
            axes[0].plot(xk, yk, 'k')
        yk = [y_max, y_max]
        axes[0].plot(xk, yk, 'k')
        # Draw vertical lines
        dxdiv = float(Nx)
        yk = [y_min, y_max]
        for k in range(Nx):
            xk = [x_min + k * dx / dxdiv, x_min + k * dx / dxdiv]
            axes[0].plot(xk, yk, 'k-')
        xk = [x_max, x_max]
        axes[0].plot(xk, yk, 'k-')
        plot_kwargs['linestyle'] = 'None'
        axes[0].plot(x, y, **plot_kwargs)
        axes[0].set_xlabel(labels[0])
        axes[0].set_ylabel(labels[1])
        axes[0].set_xlim(x_min, x_max)
        axes[0].set_ylim(y_min, y_max)
        return figure

    # General case
    # Graph of type Pairs + horizontal/vertical lines
    # to illustrate the cells
    for i in range(dim):
        # y axis
        y = data.getMarginal(i)
        y_min = bounds.getLowerBound()[i]
        y_max = bounds.getUpperBound()[i]
        dy = y_max - y_min
        for j in range(dim):
            if len(axes) <= dim * dim:
                axes.append(
                    figure.add_subplot(dim, dim, 1 + i * dim + j, **axes_kwargs))
            if i != j:
                # x axis
                x = data.getMarginal(j)
                x_min = bounds.getLowerBound()[j]
                x_max = bounds.getUpperBound()[j]
                dx = x_max - x_min
                # Draw horizontal lines
                dydiv = float(Ny)
                for k in range(Ny):
                    xk = [x_min, x_max]
                    yk = [y_min + k * dy / dydiv, y_min + k * dy / dydiv]
                    axes[1 + i * dim + j].plot(xk, yk, 'k-')
                # Draw vertical lines
                dxdiv = float(Nx)
                for k in range(Nx):
                    xk = [x_min + k * dx / dxdiv, x_min + k * dx / dxdiv]
                    yk = [y_min, y_max]
                    axes[1 + i * dim + j].plot(xk, yk, 'k-')
                plot_kwargs['linestyle'] = 'None'
                axes[1 + i * dim + j].plot(x, y, **plot_kwargs)
                axes[1 + i * dim + j].set_xlim(x_min, x_max)
                axes[1 + i * dim + j].set_ylim(y_min, y_max)
            else:
                text_kwargs['transform'] = axes[1 + i * dim + j].transAxes
                axes[1 + i * dim + j].text(0.5, 0.5, labels[i], **text_kwargs)

    # Finally get the figure
    return figure
