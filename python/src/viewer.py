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
>>> view.show(block=False)
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

        if not isinstance(graph, ot.Graph) and not isinstance(graph, ot.GraphImplementation):
            if not isinstance(graph, ot.Drawable) and not isinstance(graph, ot.DrawableImplementation):
                raise TypeError(
                    '-- The given object cannot be converted into a Graph nor Drawable.')
            else:
                # convert Drawable => Graph
                drawable = graph
                graph = ot.Graph()
                graph.add(drawable)

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
                raise ValueError('-- pixelsize must be a 2-tuple.')
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

        # set title
        axes_kwargs.setdefault('title', self.ToUnicode(graph.getTitle()))

        # set scale
        if (graph.getLogScale() == ot.GraphImplementation.LOGX) or (graph.getLogScale() == ot.GraphImplementation.LOGXY):
            axes_kwargs.setdefault('xscale', 'log')
        if (graph.getLogScale() == ot.GraphImplementation.LOGY) or (graph.getLogScale() == ot.GraphImplementation.LOGXY):
            axes_kwargs.setdefault('yscale', 'log')

        # set bounding box
        axes_kwargs.setdefault(
            'xlim', [graph.getBoundingBox()[0], graph.getBoundingBox()[1]])
        axes_kwargs.setdefault(
            'ylim', [graph.getBoundingBox()[2], graph.getBoundingBox()[3]])

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

        # set axes
        if len(axes) == 0:
            self._ax = [self._fig.add_subplot(111, **axes_kwargs)]
        else:
            self._ax = axes

        has_labels = False
        self._ax[0].grid(b=graph.getGrid())
        for drawable in drawables:
            # reset working dictionaries by excplicitely creating copies
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
            if ('color' not in text_kwargs_default) and ('c' not in text_kwargs_default):
                text_kwargs['color'] = drawable.getColorCode()

            # set marker
            pointStyleDict = {'square': 's', 'circle': 'o', 'triangleup': '2', 'plus': '+', 'times': '+', 'diamond': '+', 'triangledown':
                              'v', 'star': '*', 'fsquare': 's', 'fcircle': 'o', 'ftriangleup': '2', 'fdiamond': 'D', 'bullet': '.', 'dot': ',', 'none': 'None'}
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
            x = data.getMarginal(0)
            if data.getDimension() > 1:
                y = data.getMarginal(1)

            # add label, title
            drawableKind = drawable.getImplementation().getClassName()
            if drawableKind != 'Pie':
                self._ax[0].set_xlabel(self.ToUnicode(graph.getXTitle()))
                self._ax[0].set_ylabel(self.ToUnicode(graph.getYTitle()))

                if (len(drawable.getLegend()) > 0) and ((drawableKind != 'Cloud') or (drawable.getPointStyle() != 'none')):
                    label = self.ToUnicode(drawable.getLegend())
                    has_labels = True
                    plot_kwargs.setdefault('label', label)
                    bar_kwargs.setdefault('label', label)
                    step_kwargs.setdefault('label', label)

            if drawableKind == 'BarPlot':
                # linestyle for bar() is different than the one for plot()
                if 'linestyle' in bar_kwargs_default:
                    bar_kwargs.pop('linestyle')
                if ('linestyle' not in plot_kwargs_default) and ('ls' not in plot_kwargs_default):
                    lineStyleDict = {'solid': 'solid', 'dashed': 'dashed', 'dotted':
                                     'dotted', 'dotdash': 'dashdot', 'longdash': 'dashed', 'twodash': 'dashed'}
                    if drawable.getLineStyle() in lineStyleDict:
                        bar_kwargs['linestyle'] = lineStyleDict[
                            drawable.getLineStyle()]
                    else:
                        warnings.warn(
                            '-- Unknown line style: ' + drawable.getLineStyle())

                xi = drawable.getOrigin()
                for i in range(x.getSize()):
                    # label only the first bar to avoid getting several legend
                    # items
                    if (i == 1) and ('label' in bar_kwargs):
                        bar_kwargs.pop('label')
                    self._ax[0].bar(
                        xi, height=y[i][0], width=x[i][0], **bar_kwargs)
                    xi += x[i][0]

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
                colors = drawable.getPalette()
                colorsRGBA = []
                for i in range(polygonsNumber):
                    hex_code = ot.Drawable.ConvertFromName(colors[i])
                    rgba = ot.Drawable.ConvertToRGBA(hex_code)
                    colorsRGBA.append(
                        (rgba[0] / 255.0, rgba[1] / 255.0, rgba[2] / 255.0, rgba[3] / 255.0))
                if 'facecolors' not in polygoncollection_kwargs_default:
                    polygoncollection_kwargs['facecolors'] = colorsRGBA

                if 'edgecolors' not in polygon_kwargs_default:
                    polygoncollection_kwargs['edgecolors'] = colorsRGBA
                self._ax[0].add_collection(
                    matplotlib.collections.PolyCollection(np.array(data).reshape((polygonsNumber, verticesNumber, 2)), **polygoncollection_kwargs))

            elif drawableKind == 'Pie':
                pie_kwargs.setdefault(
                    'labels', list(map(self.ToUnicode, drawable.getLabels())))
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
                    clabel_kwargs.setdefault('fmt', '%g')
                    plt.clabel(contourset, **clabel_kwargs)
                for i in range(drawable.getLabels().getSize()):
                    contourset.collections[i].set_label(
                        drawable.getLabels()[i])

            elif drawableKind == 'Staircase':
                self._ax[0].step(x, y, **step_kwargs)

            elif drawableKind == 'Pairs':
                # disable axis : grid, ticks, axis
                self._ax[0].axison = False

                if 'title' in axes_kwargs:
                    axes_kwargs.pop('title')
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
                            self._ax[1 + i * dim + j].plot(x, y, **plot_kwargs)
                            self._ax[1 + i * dim + j].set_xlim(
                                x_min - x_margin, x_max + x_margin)
                            self._ax[1 + i * dim + j].set_ylim(
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
                    text_kwargs['fontsize'] = max(16 - dim, 4)
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
                    if len(text) == 0: continue
                    horizontal = horizontal_default
                    vertical = vertical_default
                    # Text.getTextPositions() describes position of text with respect
                    # to given coordinates;  in matplotlib, positional argument controls
                    # which side of the text bounding box is positioned, its meaning is
                    # thus inverted.
                    if positions[i] == 'left':
                        horizontal='right'
                    elif positions[i] == 'right':
                        horizontal='left'
                    elif positions[i] == 'top':
                        vertical='bottom'
                    elif positions[i] == 'bottom':
                        vertical='top'
                    self._ax[0].text(x[i][0], y[i][0], text, horizontalalignment=horizontal, verticalalignment=vertical, **text_kwargs)

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

    def show(self, **kwargs):
        """
        Display the graph on screen.

        Parameters
        ----------
        kwargs:
            block: bool, optional
                If true (default), block until the graph is closed.

            These parameters are passed to matplotlib.pyplot.show()
        """
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


def ToSVGString(graph):
    """
    Convert as SVG file.

    Parameters
    ----------
    graph : object
        A Graph or Drawable object.

    Returns a SVG representation as string
    """
    if sys.version_info[0] >= 3:
        output = io.StringIO()
    else:
        output = io.BytesIO()

    # save interactive mode state
    ision = plt.isinteractive()
    plt.ioff()

    view = View(graph)
    view.save(output, format='svg')
    view.close()

    # restore interactive mode state
    if ision:
        plt.ion()

    return output.getvalue()
