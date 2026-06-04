from sphinx_gallery.scrapers import matplotlib_scraper
import matplotlib as mpl

mpl.rcParams["svg.hashsalt"] = "nacl"


def matplotlib_svg_scraper(*args, **kwargs):
    return matplotlib_scraper(*args, format="svg", **kwargs)
