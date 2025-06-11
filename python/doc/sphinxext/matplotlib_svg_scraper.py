from sphinx_gallery.scrapers import matplotlib_scraper


def matplotlib_svg_scraper(*args, **kwargs):
    return matplotlib_scraper(*args, format="svg", **kwargs)
