"""
Graph viewer.

Graph or Drawable viewer using matplotlib

Examples
--------
>>> import openturns as ot
>>> from openturns.viewer import View
>>> graph = ot.Normal().drawPDF()
>>> view = View(graph, plot_kw={'color':'blue'})
>>> view.save('graph.png', dpi=100)
>>> view.show()
"""

import openturns as ot
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.colors as cls
import warnings
import io

try:
    from packaging.version import Version
except ImportError:
    from pkg_resources import parse_version as Version

__all__ = ["View", "PlotDesign"]


class RankNormalize(cls.Normalize):
    """
    Color distribution normalization class based on rank and not value.

    This class is used to manage the "rank" norm for Contour drawables
    """

    def _changed(self):
        if self._levels is not None and self.vmin is not None and self.vmax is not None:
            below_threshold = sum(
                1 if level < self.vmin else 0 for level in self._levels
            )
            above_threshold = sum(
                1 if level > self.vmax else 0 for level in self._levels
            )
            active = len(self._levels) - below_threshold - above_threshold
            if active <= 0:
                raise ValueError("No active level; check vmin and vmax")
            self._ranks = np.linspace(
                -below_threshold / active,
                1.0 + above_threshold / active,
                len(self._levels),
            )
        if hasattr(cls.Normalize, "_changed"):
            cls.Normalize._changed(self)

    def __init__(self, levels, vmin=None, vmax=None, clip=False):
        """
        Construct the normalization based on rank

        Parameters
        ----------
        levels : list of floats
            List of level values.

        vmin : float, optional
            Minimum value for color distribution

        vmax : float, optional
            Maximum value for color distribution

        clip : bool, optional
            Indicator for cutting color distribution out of vmin and vmax
            Required by the parent class, but unused.
        """
        super().__init__(vmin, vmax, clip)
        self._levels = None if levels is None else np.array(levels)
        self._ranks = None
        self._changed()

    def __call__(self, value, clip=None):
        if self._levels is None and hasattr(value, "__iter__"):
            self._levels = np.array([level for level in value])
            self._changed()
        if self._levels is not None and self._ranks is None:
            self._ranks = np.linspace(0.0, 1.0, len(self._levels))
        return (
            1.0
            if self._levels is None
            else np.ma.masked_array(np.interp(value, self._levels, self._ranks))
        )


class View:
    """
    Create the figure.

    Parameters
    ----------
    graph : :class:`~openturns.Graph`, :class:`~openturns.Drawable` or :class:`~openturns.GridLayout`
        An object to draw.

    pixelsize : 2-tuple of int
        The requested size in pixels (width, height).

    figure : :class:`matplotlib.figure.Figure`
        The figure to draw on.

    figure_kw : dict, optional
        Passed on to matplotlib.pyplot.figure kwargs

    axes : :class:`matplotlib.axes.Axes`
        The axes to draw on.

    plot_kw : dict, optional
        Used when drawing Curve drawables
        Passed on as matplotlib.axes.Axes.plot kwargs

    axes_kw : dict, optional
        Passed on to matplotlib.figure.Figure.add_subplot kwargs

    bar_kw : dict, optional
        Used when drawing BarPlot drawables
        Passed on to matplotlib.pyplot.bar kwargs

    pie_kw : dict, optional
        Used when drawing Pie drawables
        Passed on to matplotlib.pyplot.pie kwargs

    polygon_kw : dict, optional
        Used when drawing Polygon drawables
        Passed on to matplotlib.patches.Polygon kwargs

    polygoncollection_kw : dict, optional
        Used when drawing PolygonArray drawables
        Passed on to matplotlib.collection.PolygonCollection kwargs

    contour_kw : dict, optional
        Used when drawing Contour drawables
        Passed on to matplotlib.pyplot.contour kwargs

    clabel_kw : dict, optional
        Used when drawing Contour drawables
        Passed on to matplotlib.pyplot.clabel kwargs

    scatter_kw : dict, optional
        Used when drawing Cloud drawables
        Passed on to matplotlib.pyplot.scatter kwargs

    step_kw : dict, optional
        Used when drawing Staircase drawables
        Passed on to matplotlib.pyplot.step kwargs

    text_kw : dict, optional
        Used when drawing Pairs, Text drawables
        Passed on to matplotlib.axes.Axes.text kwargs

    legend_kw : dict, optional
        Passed on to matplotlib.axes.Axes.legend kwargs

    add_legend : bool, optional
        Adds a legend if True. Default is True.

    square_axes : bool, optional
        Forces the axes to share the same scale if True. Default is False.

    Examples
    --------
    >>> import openturns as ot
    >>> from openturns.viewer import View
    >>> graph = ot.Normal().drawPDF()
    >>> view = View(graph, plot_kw={'color':'blue'})
    >>> view.save('graph.png', dpi=100)
    >>> view.show()
    """

    @staticmethod
    def _CheckDict(arg):
        """Check that the argument is a Python dictionary."""
        result = arg
        if arg is None:
            result = {}
        elif not isinstance(arg, dict):
            raise TypeError("Argument is not a dict")
        return result

    @staticmethod
    def _ToUnicode(s):
        """Convert to unicode if necessary."""
        if isinstance(s, bytes):
            s = s.decode("utf8")
        return s

    def __init__(
        self,
        graph,
        pixelsize=None,
        figure=None,
        figure_kw=None,
        axes=[],
        plot_kw=None,
        axes_kw=None,
        bar_kw=None,
        pie_kw=None,
        polygon_kw=None,
        polygoncollection_kw=None,
        contour_kw=None,
        step_kw=None,
        clabel_kw=None,
        scatter_kw=None,
        text_kw=None,
        legend_kw=None,
        add_legend=True,
        square_axes=False,
        **kwargs,
    ):
        if not (
            isinstance(graph, ot.Graph)
            or isinstance(graph, ot.GraphImplementation)
            or isinstance(graph, ot.GridLayout)
        ):
            if isinstance(graph, ot.Drawable) or isinstance(
                graph, ot.DrawableImplementation
            ):
                # convert Drawable => Graph
                drawable = graph
                graph = ot.Graph()
                graph.add(drawable)
            else:
                raise TypeError(
                    "The given object cannot be converted into a Graph nor Drawable."
                )

        # Store matplotlib version
        matplotlib_version = Version(matplotlib.__version__)

        # check that arguments are dictionaries
        figure_kw = self._CheckDict(figure_kw)
        axes_kw = self._CheckDict(axes_kw)
        plot_kw_default = self._CheckDict(plot_kw)
        bar_kw_default = self._CheckDict(bar_kw)
        pie_kw_default = self._CheckDict(pie_kw)
        polygon_kw_default = self._CheckDict(polygon_kw)
        polygoncollection_kw_default = self._CheckDict(polygoncollection_kw)
        contour_kw_default = self._CheckDict(contour_kw)
        step_kw_default = self._CheckDict(step_kw)
        clabel_kw_default = self._CheckDict(clabel_kw)
        scatter_kw_default = self._CheckDict(scatter_kw)
        text_kw_default = self._CheckDict(text_kw)
        legend_kw_default = self._CheckDict(legend_kw)
        legend_handles = []
        legend_labels = []

        # scaled grid graphs
        if (
            isinstance(graph, ot.GridLayout)
            and graph.getNbRows() * graph.getNbColumns() > 1
        ):
            figure_kw.setdefault(
                "figsize", (6.0 * graph.getNbColumns(), 6.0 * graph.getNbRows())
            )

        # set image size in pixels
        if pixelsize is not None:
            if len(pixelsize) != 2:
                raise ValueError("pixelsize must be a 2-tuple.")
            figure_kw.setdefault("dpi", 100)
            dpi = figure_kw["dpi"]
            border = 10  # guess
            width, height = pixelsize
            width -= border
            height -= border
            figure_kw.setdefault("figsize", (width * 1.0 / dpi, height * 1.0 / dpi))

        # set step drawstyle
        step_kw_default.setdefault("where", "post")

        # set figure
        if figure is None:
            if len(axes) == 0:
                self._fig = plt.figure(**figure_kw)
            else:
                self._fig = axes[0].get_figure()
        else:
            self._fig = figure
            if len(axes) == 0:
                axes = self._fig.axes

        if isinstance(graph, ot.GridLayout):
            self._views = [
                [None] * graph.getNbColumns() for _ in range(graph.getNbRows())
            ]
            axes = self._fig.subplots(graph.getNbRows(), graph.getNbColumns())
            # Reshape needed when either the nb of rows or the nb of columns is 1.
            # Does not hurt otherwise.
            self._ax = np.reshape(axes, (graph.getNbRows(), graph.getNbColumns()))
            for i in range(graph.getNbRows()):
                for j in range(graph.getNbColumns()):
                    graphij = graph.getGraph(i, j)
                    if len(graphij.getDrawables()) == 0:
                        self._ax[i, j].axis("off")
                        continue
                    self._ax[i, j].axison = graphij.getAxes()
                    self._ax[i, j].set_title(self._ToUnicode(graphij.getTitle()))
                    # hide frame top/right
                    self._ax[i, j].spines["right"].set_visible(False)
                    self._ax[i, j].spines["top"].set_visible(False)
                    self._views[i][j] = View(
                        graphij,
                        figure=self._fig,
                        axes=[self._ax[i, j]],
                        plot_kw=plot_kw,
                        axes_kw=axes_kw,
                        bar_kw=bar_kw,
                        pie_kw=pie_kw,
                        polygon_kw=polygon_kw,
                        polygoncollection_kw=polygoncollection_kw,
                        contour_kw=contour_kw,
                        step_kw=step_kw,
                        clabel_kw=clabel_kw,
                        scatter_kw=scatter_kw,
                        text_kw=text_kw,
                        legend_kw=legend_kw,
                    )
            self._fig.suptitle(self._ToUnicode(graph.getTitle()))
            return

        drawables = graph.getDrawables()
        n_drawables = len(drawables)
        self._contoursets = []
        if n_drawables == 0:
            warnings.warn("-- Nothing to draw.")
            return

        # set title
        self._fig.suptitle(self._ToUnicode(graph.getTitle()))

        # set axes
        if len(axes) == 0:
            self._ax = [self._fig.add_subplot(111, **axes_kw)]
        else:
            self._ax = axes
        self._views = None

        # activate axes only if wanted
        self._ax[0].axison = graph.getAxes()

        # ticks
        if not graph.getTickLocation() & ot.GraphImplementation.TICKX:
            self._ax[0].set_xticklabels([])
        if not graph.getTickLocation() & ot.GraphImplementation.TICKY:
            self._ax[0].set_yticklabels([])
        if graph.getIntegerXTick():
            self._ax[0].xaxis.set_major_locator(
                matplotlib.ticker.MaxNLocator(integer=True)
            )
        if graph.getIntegerYTick():
            self._ax[0].yaxis.set_major_locator(
                matplotlib.ticker.MaxNLocator(integer=True)
            )

        # set bounding box
        bb = graph.getBoundingBox()
        self._ax[0].set_xlim([bb.getLowerBound()[0], bb.getUpperBound()[0]])
        self._ax[0].set_ylim([bb.getLowerBound()[1], bb.getUpperBound()[1]])

        # set scale
        if (graph.getLogScale() == ot.GraphImplementation.LOGX) or (
            graph.getLogScale() == ot.GraphImplementation.LOGXY
        ):
            self._ax[0].set_xscale("log")
        if (graph.getLogScale() == ot.GraphImplementation.LOGY) or (
            graph.getLogScale() == ot.GraphImplementation.LOGXY
        ):
            self._ax[0].set_yscale("log")

        self._ax[0].grid(graph.getGrid())

        # use scientific notation on non-log axis
        axis_mask = {
            ot.GraphImplementation.NONE: "both",
            ot.GraphImplementation.LOGX: "y",
            ot.GraphImplementation.LOGY: "x",
            ot.GraphImplementation.LOGXY: None,
        }
        axis = axis_mask[graph.getLogScale()]
        if axis is not None:
            try:
                # not all formatters support scientific format, eg FixedFormatter when ticklabels are disabled on subplots
                # FixedFormatter' object has no attribute 'set_scientific'
                self._ax[0].ticklabel_format(axis=axis, style="sci", scilimits=(-3, 5))
            except AttributeError:
                pass

        has_labels = False

        zorder = 2
        for drawable in drawables:
            drawableKind = drawable.getImplementation().getClassName()

            # reset working dictionaries by explicitly creating copies
            plot_kw = dict(plot_kw_default)
            bar_kw = dict(bar_kw_default)
            pie_kw = dict(pie_kw_default)
            polygon_kw = dict(polygon_kw_default)
            polygoncollection_kw = dict(polygoncollection_kw_default)
            contour_kw = dict(contour_kw_default)
            step_kw = dict(step_kw_default)
            clabel_kw = dict(clabel_kw_default)
            scatter_kw = dict(scatter_kw_default)
            text_kw = dict(text_kw_default)

            # use drawable order over default patch/line/text order
            plot_kw["zorder"] = zorder
            bar_kw["zorder"] = zorder
            polygon_kw["zorder"] = zorder
            polygoncollection_kw["zorder"] = zorder
            contour_kw["zorder"] = zorder
            step_kw["zorder"] = zorder
            if matplotlib_version >= Version("3.3"):
                clabel_kw["zorder"] = zorder
            scatter_kw["zorder"] = zorder
            text_kw["zorder"] = zorder
            zorder += 1 / (len(drawables) + 1)

            # set color
            if ("color" not in plot_kw_default) and ("c" not in plot_kw_default):
                plot_kw["color"] = drawable.getColorCode()
            if ("color" not in scatter_kw_default) and ("c" not in scatter_kw_default):
                scatter_kw["color"] = drawable.getColorCode()
            if ("color" not in bar_kw_default) and ("c" not in bar_kw_default):
                bar_kw["color"] = drawable.getColorCode()
            if ("color" not in step_kw_default) and ("c" not in step_kw_default):
                step_kw["color"] = drawable.getColorCode()
            if drawableKind != "Pairs":
                if ("color" not in text_kw_default) and ("c" not in text_kw_default):
                    text_kw["color"] = drawable.getColorCode()

            # set line style
            lineStyleDict = {
                "blank": "",
                "solid": "-",
                "dashed": "--",
                "dotted": ":",
                "dotdash": "-.",
                "longdash": (0, (10, 3)),
                "twodash": (0, (10, 3, 5, 3)),
            }
            if ("linestyle" not in plot_kw_default) and ("ls" not in plot_kw_default):
                try:
                    plot_kw["linestyle"] = lineStyleDict[drawable.getLineStyle()]
                except KeyError:
                    warnings.warn("-- Unknown line style")
            if ("linestyle" not in step_kw_default) and ("ls" not in step_kw_default):
                try:
                    step_kw["linestyle"] = lineStyleDict[drawable.getLineStyle()]
                except KeyError:
                    warnings.warn("-- Unknown line style")

            # set line width
            if ("linewidth" not in plot_kw_default) and ("lw" not in plot_kw_default):
                plot_kw["linewidth"] = drawable.getLineWidth()
            if ("linewidth" not in step_kw_default) and ("lw" not in step_kw_default):
                step_kw["linewidth"] = drawable.getLineWidth()
            if ("linewidth" not in polygon_kw_default) and (
                "lw" not in polygon_kw_default
            ):
                polygon_kw["linewidth"] = drawable.getLineWidth()

            # retrieve data
            data = drawable.getData()
            x = data.getMarginal(0).asPoint()
            if data.getDimension() > 1:
                y = data.getMarginal(1).asPoint()

            # add label, title
            if drawableKind != "Pie":
                self._ax[0].set_xlabel(self._ToUnicode(graph.getXTitle()))
                self._ax[0].set_ylabel(self._ToUnicode(graph.getYTitle()))

                if len(drawable.getLegend()) > 0:
                    label = self._ToUnicode(drawable.getLegend())
                    has_labels = True
                    plot_kw.setdefault("label", label)
                    bar_kw.setdefault("label", label)
                    scatter_kw.setdefault("label", label)
                    step_kw.setdefault("label", label)
                    polygon_kw.setdefault("label", label)
                    polygoncollection_kw.setdefault("label", label)

            if drawableKind == "BarPlot":
                if ("linestyle" not in bar_kw_default) and ("ls" not in bar_kw_default):
                    if drawable.getLineStyle() in lineStyleDict:
                        bar_kw["linestyle"] = lineStyleDict[drawable.getLineStyle()]
                    else:
                        warnings.warn(
                            "-- Unknown line style: " + drawable.getLineStyle()
                        )
                if "linewidth" not in bar_kw_default and "lw" not in bar_kw_default:
                    bar_kw["linewidth"] = drawable.getLineWidth()

                # fillstyle
                if drawable.getFillStyle() == "shaded":
                    bar_kw["fill"] = False
                    bar_kw["hatch"] = "////"
                    bar_kw["edgecolor"] = bar_kw["color"]

                xi = drawable.getOrigin()
                for i in range(x.getSize()):
                    # label only the first bar to avoid getting several items
                    if (i == 1) and ("label" in bar_kw):
                        bar_kw.pop("label")
                    self._ax[0].bar(xi, height=y[i], width=x[i], align="edge", **bar_kw)
                    xi += x[i]

            elif drawableKind == "Cloud":
                # set marker
                marker = drawable.getPointStyle()
                rPointStyleDict = {
                    "square": "s",
                    "circle": "o",
                    "triangleup": "^",
                    "plus": "+",
                    "times": "x",
                    "diamond": "d",
                    "triangledown": "v",
                    "star": "*",
                    "fsquare": "s",
                    "fcircle": "o",
                    "ftriangleup": "^",
                    "fdiamond": "d",
                    "bullet": ".",
                    "dot": ",",
                    "none": "None",
                }
                # we still accept R markers
                if marker in rPointStyleDict:
                    marker = rPointStyleDict[marker]
                scatter_kw.setdefault("marker", marker)

                # https://github.com/matplotlib/matplotlib/issues/11460
                if "marker" in scatter_kw and scatter_kw["marker"] in [","]:
                    scatter_kw["s"] = 1
                    scatter_kw["linewidths"] = 0.0
                lines = self._ax[0].scatter(x, y, **scatter_kw)
                if len(drawable.getLegend()) > 0:
                    legend_handles.append(lines.findobj()[0])
                    legend_labels.append(drawable.getLegend())

            elif drawableKind == "Curve":
                lines = self._ax[0].plot(x, y, **plot_kw)
                if len(drawable.getLegend()) > 0:
                    legend_handles.append(lines[0])
                    legend_labels.append(drawable.getLegend())

            elif drawableKind == "Polygon":
                if ("facecolor" not in polygon_kw_default) and (
                    "fc" not in polygon_kw_default
                ):
                    polygon_kw["facecolor"] = drawable.getColorCode()

                if ("edgecolor" not in polygon_kw_default) and (
                    "ec" not in polygon_kw_default
                ):
                    polygon_kw["edgecolor"] = drawable.ConvertFromName(
                        drawable.getEdgeColor()
                    )
                self._ax[0].add_patch(matplotlib.patches.Polygon(data, **polygon_kw))

            elif drawableKind == "PolygonArray":
                polygonsNumber = drawable.getPalette().getSize()
                verticesNumber = drawable.getData().getSize() // polygonsNumber
                if (
                    "facecolors" not in polygoncollection_kw_default
                    and "fc" not in polygoncollection_kw_default
                ):
                    polygoncollection_kw["facecolors"] = (
                        drawable.getPaletteAsNormalizedRGBA()
                    )

                self._ax[0].add_collection(
                    matplotlib.collections.PolyCollection(
                        np.array(data).reshape((polygonsNumber, verticesNumber, 2)),
                        **polygoncollection_kw,
                    )
                )

            elif drawableKind == "Pie":
                pie_kw.setdefault(
                    "labels", [self._ToUnicode(label) for label in drawable.getLabels()]
                )
                pie_kw.setdefault("colors", drawable.getPalette())
                self._ax[0].set_aspect("equal")
                self._ax[0].pie(x, **pie_kw)

            elif drawableKind == "Contour":
                contour = drawable.getImplementation()
                X, Y = np.meshgrid(drawable.getX(), drawable.getY())
                # X values move with columns
                # Y values move with rows
                # Z shape is therefore (getY().getSize(), getX().getSize())
                Z = np.reshape(
                    drawable.getData(),
                    (drawable.getY().getSize(), drawable.getX().getSize()),
                )
                if len(drawable.getLevels()) > 0:
                    contour_kw.setdefault("levels", drawable.getLevels())
                if (
                    not contour.isFilled()
                    and ("linestyles" not in contour_kw_default)
                    and ("ls" not in contour_kw_default)
                ):
                    # Contours do not accept all styles
                    contourLineStyleDict = {
                        "solid": "-",
                        "dashed": "--",
                        "dotted": ":",
                        "dotdash": "-.",
                        "longdash": "--",
                        "twodash": "--",
                    }
                    try:
                        contour_kw["linestyles"] = contourLineStyleDict[
                            drawable.getLineStyle()
                        ]
                    except KeyError:
                        warnings.warn("-- Unknown line style")
                if (
                    not contour.isFilled()
                    and ("linewidths" not in contour_kw_default)
                    and ("lw" not in contour_kw_default)
                ):
                    contour_kw["linewidths"] = drawable.getLineWidth()
                if (
                    "cmap" not in contour_kw_default
                    and "colors" not in contour_kw_default
                    and contour.getColorMap()
                ):
                    contour_kw["cmap"] = contour.getColorMap()
                if "colors" not in contour_kw_default and "cmap" not in contour_kw:
                    contour_kw["colors"] = [drawable.getColorCode()]
                if "alpha" not in contour_kw_default:
                    contour_kw["alpha"] = contour.getAlpha()
                if "vmin" not in contour_kw_default and contour.isVminUsed():
                    contour_kw["vmin"] = contour.getVmin()
                if "vmax" not in contour_kw_default and contour.isVmaxUsed():
                    contour_kw["vmax"] = contour.getVmax()
                norm = (
                    contour_kw["norm"]
                    if "norm" in contour_kw_default
                    else contour.getColorMapNorm()
                )
                if type(norm) is str and matplotlib_version < Version("3.6.0"):
                    # matplotlib before 3.6 does not support norms as strings
                    try:
                        normDict = {
                            "rank": "rank",
                            "linear": cls.Normalize(),
                            "log": cls.LogNorm(),
                            "symlog": (
                                cls.SymLogNorm(linthresh=0.03)
                                if matplotlib_version < Version("3.2.0")
                                else cls.SymLogNorm(linthresh=0.03, base=10)
                            ),
                        }
                        contour_kw["norm"] = normDict[norm]
                    except KeyError:
                        warnings.warn("-- Unknown norm " + norm)
                else:
                    contour_kw["norm"] = norm
                if contour_kw.get("norm") == "rank":
                    contour_kw["norm"] = RankNormalize(contour_kw.get("levels"))
                if "extend" not in contour_kw_default:
                    contour_kw["extend"] = contour.getExtend()
                if (
                    contour.isFilled()
                    and "hatches" not in contour_kw_default
                    and contour.getHatches()
                ):
                    contour_kw["hatches"] = [hatch for hatch in contour.getHatches()]
                if contour.isFilled():
                    contourset = self._ax[0].contourf(X, Y, Z, **contour_kw)
                else:
                    contourset = self._ax[0].contour(X, Y, Z, **contour_kw)
                self._contoursets.append(contourset)

                # matplotlib may fail with ValueError exception "RGBA sequence should have length 3 or 4'"
                # can either disable labels or color map to work around it
                draw_labels = drawable.getDrawLabels()
                if (
                    draw_labels
                    and len(drawable.getLevels()) == 1
                    and contour_kw.get("cmap", "") != ""
                    and matplotlib_version >= Version("3.6")
                    and matplotlib_version < Version("3.7")
                ):
                    warnings.warn(
                        "openturns.viewer: disabling contour labels to work around matplotlib 3.6.x issue"
                    )
                    draw_labels = False

                if draw_labels and not contour.isFilled():
                    # Matplotlib does not support labels in filled contours well
                    clabel_kw.setdefault("fontsize", 8)
                    # Use labels
                    fmt = {}
                    for lv, s in zip(
                        np.array(drawable.getLevels()), drawable.getLabels()
                    ):
                        fmt[lv] = s
                    if fmt:
                        clabel_kw.setdefault("fmt", fmt)
                    try:
                        plt.clabel(contourset, **clabel_kw)
                    except KeyError:
                        # https://github.com/matplotlib/matplotlib/issues/9742
                        warnings.warn("pyplot.clabel likely failed on boundary level")
                    except UnboundLocalError:
                        # https://github.com/matplotlib/matplotlib/pull/10710
                        warnings.warn("pyplot.clabel likely failed as in #10710")
                artists, _ = contourset.legend_elements()
                if len(drawable.getLegend()) > 0:
                    legend_handles.append(artists[0])
                    legend_labels.append(drawable.getLegend())
                if contour.getColorBarPosition() and len(contour.getLevels()) != 1:
                    colorbar = None
                    if matplotlib_version >= Version("3.7.0"):
                        colorbar = self._fig.colorbar(
                            contourset,
                            location=contour.getColorBarPosition(),
                            format="%.3g",
                        )
                    else:
                        # do not draw colorbar when Z range has too few values
                        # https://github.com/matplotlib/matplotlib/issues/23817
                        nZ = len(set([z[0] for z in drawable.getData()]))
                        if nZ > 2:
                            try:
                                colorbar = self._fig.colorbar(contourset, format="%.3g")
                                if contour.getColorBarPosition() != "right":
                                    warnings.warn(
                                        "-- colorbar location was not used in matplotlib < 3.7.0"
                                    )
                            except ZeroDivisionError:
                                warnings.warn(
                                    "figure.colorbar likely failed on boundary levels"
                                )

                    if colorbar is not None and contour.getLevels():
                        colorbar.formatter.minor_thresholds = (
                            10.0,
                            0.4,
                        )  # Avoids the absence of labels in logarithmic scale

            elif drawableKind == "Staircase":
                lines = self._ax[0].step(x, y, **step_kw)
                if len(drawable.getLegend()) > 0:
                    legend_handles.append(lines[0])
                    legend_labels.append(drawable.getLegend())

            elif drawableKind == "Text":
                # adjust font
                if ("fontsize" not in text_kw_default) and (
                    "size" not in text_kw_default
                ):
                    # R API requiert a character exansion (cex)
                    # considering default scale to be 1
                    # Default fontsize in matplotlib is 10.0
                    # thus we make corresponding the R & matplotlib values
                    text_kw["fontsize"] = drawable.getTextSize() * 10.0
                if "horizontalalignment" in text_kw_default:
                    horizontal_default = text_kw["horizontalalignment"]
                    del text_kw["horizontalalignment"]
                else:
                    horizontal_default = "center"
                if "verticalalignment" in text_kw_default:
                    vertical_default = text_kw["verticalalignment"]
                    del text_kw["verticalalignment"]
                else:
                    vertical_default = "center"
                if "rotation" not in text_kw_default:
                    text_kw["rotation"] = drawable.getImplementation().getRotation()
                if "rotation_mode" not in text_kw_default:
                    text_kw["rotation_mode"] = "anchor"
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
                    if positions[i] == "left":
                        horizontal = "right"
                    elif positions[i] == "right":
                        horizontal = "left"
                    elif positions[i] == "top":
                        vertical = "bottom"
                    elif positions[i] == "bottom":
                        vertical = "top"
                    self._ax[0].text(
                        x[i],
                        y[i],
                        text,
                        horizontalalignment=horizontal,
                        verticalalignment=vertical,
                        **text_kw,
                    )

            else:
                raise ValueError("Drawable type not implemented: " + drawableKind)

        # Add legend
        if add_legend and has_labels and (graph.getLegendPosition() != ""):
            legend_kw = legend_kw_default

            # set legend position
            loc = graph.getLegendPosition()
            rPositionDict = {
                "bottomright": "lower right",
                "bottom": "lower center",
                "bottomleft": "lower left",
                "left": "center left",
                "topleft": "upper left",
                "top": "upper center",
                "topright": "upper right",
                "right": "center right",
            }
            # we still accept old R strings
            if loc in rPositionDict:
                loc = rPositionDict[loc]
            legend_kw.setdefault("loc", loc)

            # set legend bbox
            if len(graph.getLegendCorner()) == 2:
                x0, y0 = graph.getLegendCorner()
                legend_kw.setdefault("bbox_to_anchor", (x0, y0))

            # set a single legend point
            legend_kw.setdefault("numpoints", 1)

            # enable round box by default
            legend_kw.setdefault("fancybox", True)

            # enable shadow by default
            legend_kw.setdefault("shadow", True)

            # by default legend is a bit too large
            legend_kw.setdefault("prop", {"size": 10})

            if len(legend_handles) > 0:
                legend = self._ax[0].legend(legend_handles, legend_labels, **legend_kw)
            else:
                legend = self._ax[0].legend(**legend_kw)

            # legend must be on top of drawables
            legend.set_zorder(zorder + 1)

            # re-adjust bbox if legend was set outside graph
            if "bbox_to_anchor" in legend_kw:
                self._fig.tight_layout()

        # Make squares look like squares
        if square_axes:
            self._ax[0].axis("square")

    def show(self, **kwargs):
        """
        Display the graph.

        See matplotlib.figure.Figure.show

        Examples
        --------
        >>> import openturns as ot
        >>> import openturns.viewer as otv
        >>> n = ot.Normal()
        >>> graph = n.drawPDF()
        >>> view = otv.View(graph)
        >>> view.show()
        """
        self._fig.show(**kwargs)

    @staticmethod
    def ShowAll(**kwargs):
        """
        Display all graphs.

        Examples
        --------
        >>> import openturns as ot
        >>> import openturns.viewer as otv
        >>> n = ot.Normal()
        >>> graph = n.drawPDF()
        >>> view = otv.View(graph)
        >>> u = ot.Uniform()
        >>> graph = u.drawPDF()
        >>> view = otv.View(graph)
        >>> otv.View.ShowAll()
        """
        plt.show(**kwargs)

    def save(self, fname, **kwargs):
        """
        Save the graph as file.

        Parameters
        ----------
        fname : bool, optional
            A string containing a path to a filename from which file format is deduced.

        kwargs : dict
            See matplotlib.figure.Figure.savefig documentation for valid keyword arguments.

        Examples
        --------
        >>> import openturns as ot
        >>> from openturns.viewer import View
        >>> graph = ot.Normal().drawPDF()
        >>> view = View(graph)
        >>> view.save('graph.png', dpi=100)
        """
        self._fig.savefig(fname, **kwargs)

    def getFigure(self):
        """
        Accessor to the underlying figure object.

        See matplotlib.figure.Figure for further information.

        Examples
        --------
        >>> import openturns as ot
        >>> from openturns.viewer import View
        >>> graph = ot.Normal().drawPDF()
        >>> view = View(graph)
        >>> fig = view.getFigure()
        >>> _ = fig.suptitle("The suptitle");
        """
        return self._fig

    def getAxes(self):
        """
        Get the matrix of Axes objects if the graph is a GridLayout, the list of Axes objects otherwise.

        See matplotlib.axes.Axes for further information.

        Examples
        --------
        >>> import openturns as ot
        >>> import openturns.viewer as otv
        >>> n = ot.Normal()
        >>> graph = n.drawPDF()
        >>> view = otv.View(graph)
        >>> axes = view.getAxes()
        >>> _ = axes[0].set_ylim(-0.1, 1.0);
        """
        return self._ax

    def getContourSets(self):
        """
        Get the list of QuadContourSet objects.

        See matplotlib.contour.QuadContourSet for further information.

        Examples
        --------
        >>> import openturns as ot
        >>> import openturns.viewer as otv
        >>> f = ot.SymbolicFunction(['x', 'y'], ['exp(-sin(cos(y)^2*x^2+sin(x)^2*y^2))'])
        >>> view = otv.View(f.draw([0.,0.],[10.,10.],[50]*2))
        >>> contoursets = view.getContourSets()
        >>> colorbar = view.getFigure().colorbar(contoursets[0]);
        """
        return self._contoursets

    def getSubviews(self):
        """
        Get the matrix of View objects if the graph is GridLayout, None otherwise.

        Examples
        --------
        >>> import openturns as ot
        >>> import openturns.viewer as otv
        >>> f = ot.SymbolicFunction(['x', 'y'], ['exp(-sin(cos(y)^2*x^2+sin(x)^2*y^2))'])
        >>> grid = ot.GridLayout(1, 2)
        >>> grid.setGraphCollection(ot.graph._GraphCollection([f.draw(0, 0, [0., 0.], 0., 10., 50), f.draw([0., 0.], [10., 10.], [50]*2)]))
        >>> view = otv.View(grid)
        >>> colorbar = view.getFigure().colorbar(view.getSubviews()[0][1].getContourSets()[0])
        """
        return self._views

    def close(self):
        """
        Close the figure.

        Examples
        --------
        >>> import openturns as ot
        >>> import openturns.viewer as otv
        >>> n = ot.Normal()
        >>> graph = n.drawPDF()
        >>> view = otv.View(graph)
        >>> view.close()
        """
        plt.close(self._fig)


def _ToImageString(graph):
    """
    Convert as image string.

    Parameters
    ----------
    graph : object
        A Graph, Drawable or GridLayout object.

    Returns
    -------
    image_str : str
       An image representation as string

    Examples
    --------
    >>> import openturns as ot
    >>> import openturns.viewer as otv
    >>> n = ot.Normal()
    >>> graph = n.drawPDF()
    >>> imageString = otv._ToImageString(graph)
    """
    # save interactive mode state
    ision = plt.isinteractive()
    plt.ioff()

    view = View(graph)
    output = io.BytesIO()
    fmt = ot.ResourceMap.Get("View-ImageFormat")
    view.save(output, format=fmt, dpi=100)
    view.close()

    # restore interactive mode state
    if ision:
        plt.ion()

    image_bytes = output.getvalue()
    if fmt == "svg":
        image_string = image_bytes.decode("utf-8")
    else:
        # raw bytes
        image_string = image_bytes
    return image_string


def PlotDesign(
    design,
    bounds=None,
    subdivisions=None,
    figure=None,
    axes=[],
    plot_kw=None,
    axes_kw=None,
    text_kw=None,
    enableTicks=True,
):
    """
    Plot a design using a scatter plot.
    If the dimension is equal to 2, then plots the 2D projection.
    If the dimension is greater or equal to 3, then plots all 2D projections.

    In addition, the function plots a grid, i.e. horizontal and vertical lines
    to see the dispersion of the points.
    This allows one to see how the sample fills the space.

    Parameters
    ----------
    design : 2-d sequence of float
        The sample.
    figure : a Matplotlib figure, optional
        If this is not None, then create a new figure.
        Otherwise, use the existing figure.
    axes : a Matplotlib axis, optional
        If empty, then create new axes.
    bounds : :class:`~openturns.Interval`, optional
        Bounds of the plot. By default, compute the bounds from the sample.
    subdivisions : a list of integers, optional
        Number of subdivisions in the each direction.
        By default, set the number of subdivisions in each direction
        as equal to the sample size.
    enableTicks : bool, optional
        A boolean. If True, then the ticks are plotted.

    Returns
    -------
    fig : matplotlib figure
          Figure representing the sample.

    Examples
    --------

    Plot a sample in 2 dimensions.

    >>> import openturns as ot
    >>> from openturns.viewer import PlotDesign
    >>> dim = 2
    >>> X = [ot.Uniform()] * dim
    >>> distribution = ot.JointDistribution(X)
    >>> sampleSize = 10
    >>> sample = distribution.getSample(sampleSize)
    >>> fig = PlotDesign(sample)

    Plot a sample in 5 dimensions.

    >>> import openturns as ot
    >>> from openturns.viewer import PlotDesign
    >>> dim = 5
    >>> size = 10
    >>> distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)]*dim)
    >>> bounds = distribution.getRange()
    >>> lhs = ot.LHSExperiment(distribution, size)
    >>> sample = lhs.generate()
    >>> subdivisions = [size]*dim
    >>> fig = PlotDesign(sample, bounds, subdivisions)
    """
    # check that arguments are dictionaries
    axes_kw = View._CheckDict(axes_kw)
    plot_kw = View._CheckDict(plot_kw)
    text_kw = View._CheckDict(text_kw)

    # retrieve data
    data = ot.Sample(design)
    dim = data.getDimension()
    if dim < 2:
        raise TypeError("Expected designs of dimension >=2")

    # Get the bounds
    if bounds is None:
        lowerBound = data.getMin()
        upperBound = data.getMax()
        bounds = ot.Interval(lowerBound, upperBound)
    if bounds.getDimension() != dim:
        raise ValueError(
            f"Dimension of bounds ({bounds.getDimension}) do not match the dimension of the sample ({dim})"
        )

    # Check the subdivisions
    if subdivisions is None:
        size = data.getSize()
        subdivisions = [size] * dim
    if len(subdivisions) != dim:
        raise ValueError(
            f"Number of subdivisions ({len(subdivisions)}) does not match the dimension of the sample ({dim})"
        )

    # Get description
    labels = data.getDescription()

    # adjust font
    if ("fontsize" not in text_kw) and ("size" not in text_kw):
        text_kw["fontsize"] = max(16 - dim, 4)
    text_kw.setdefault("horizontalalignment", "center")
    text_kw.setdefault("verticalalignment", "center")

    # set marker
    pointStyleDict = {
        "square": "s",
        "circle": "o",
        "triangleup": "2",
        "plus": "+",
        "times": "+",
        "diamond": "+",
        "triangledown": "v",
        "star": "*",
        "fsquare": "s",
        "fcircle": "o",
        "ftriangleup": "2",
        "fdiamond": "D",
        "bullet": ".",
        "dot": ",",
        "none": "None",
    }
    if "marker" not in plot_kw:
        plot_kw["marker"] = pointStyleDict["square"]

    if not enableTicks:
        axes_kw["xticks"] = []
        axes_kw["yticks"] = []

    # set figure
    if figure is None:
        figure = plt.figure()
        axes = figure.axes
    else:
        # Figure exists: get the axes
        axes = figure.axes

    # Special case of dim=2
    if dim == 2:
        Nx = subdivisions[0]
        Ny = subdivisions[1]
        axes = [figure.add_subplot(111, **axes_kw)]
        if not enableTicks:
            axes[0].axison = False

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
            axes[0].plot(xk, yk, "k")
        yk = [y_max, y_max]
        axes[0].plot(xk, yk, "k")
        # Draw vertical lines
        dxdiv = float(Nx)
        yk = [y_min, y_max]
        for k in range(Nx):
            xk = [x_min + k * dx / dxdiv, x_min + k * dx / dxdiv]
            axes[0].plot(xk, yk, "k-")
        xk = [x_max, x_max]
        axes[0].plot(xk, yk, "k-")
        plot_kw["linestyle"] = "None"
        axes[0].plot(x, y, **plot_kw)
        axes[0].set_xlabel(labels[0])
        axes[0].set_ylabel(labels[1])
        axes[0].set_xlim(x_min, x_max)
        axes[0].set_ylim(y_min, y_max)
        return figure

    # General case

    # For the diagonal of a multidimensional plot,
    # disable the ticks, always
    diagonal_axes_kw = axes_kw.copy()  # An independent copy
    diagonal_axes_kw["xticks"] = []
    diagonal_axes_kw["yticks"] = []

    # Graph of type Pairs + horizontal/vertical lines
    # to illustrate the cells
    index_axis = -1
    for i in range(dim):
        Ny = subdivisions[i]
        # y axis
        y = data.getMarginal(i)
        y_min = bounds.getLowerBound()[i]
        y_max = bounds.getUpperBound()[i]
        dy = y_max - y_min
        for j in range(dim):
            Nx = subdivisions[j]
            index_axis += 1

            if i == j:
                axes.append(
                    figure.add_subplot(dim, dim, index_axis + 1, **diagonal_axes_kw)
                )
                text_kw["transform"] = axes[index_axis].transAxes
                axes[index_axis].text(0.5, 0.5, labels[i], **text_kw)
            else:
                axes.append(figure.add_subplot(dim, dim, index_axis + 1, **axes_kw))
                # disable axis : grid, ticks, axis?
                if not enableTicks:
                    axes[index_axis].axison = False

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
                    axes[index_axis].plot(xk, yk, "k-")
                # Draw vertical lines
                dxdiv = float(Nx)
                for k in range(Nx):
                    xk = [x_min + k * dx / dxdiv, x_min + k * dx / dxdiv]
                    yk = [y_min, y_max]
                    axes[index_axis].plot(xk, yk, "k-")
                plot_kw["linestyle"] = "None"
                axes[index_axis].plot(x, y, **plot_kw)
                axes[index_axis].set_xlim(x_min, x_max)
                axes[index_axis].set_ylim(y_min, y_max)

    # Finally get the figure
    return figure
