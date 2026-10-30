#!/usr/bin/env python3
"""Strip nondeterministic sphinx imgmath output for reproducible doc builds.

Workaround sphinx imgmath parallel race, see
https://github.com/sphinx-doc/sphinx/pull/13457

Parallel workers append the depth comment concurrently, so embedded svg
images randomly end up with a duplicated or missing
<!-- DEPTH=n --> comment, and the matching
style="vertical-align: ..." attribute is randomly present or not.
Remove depth comments (and normalize trailing newline) and drop the
vertical-align style so two builds produce identical html.
Takes ~20s on the full html tree.

Parameters
----------
html_dir : str, optional
    Html tree to fix, defaults to ~/.local/share/doc/openturns/html.
"""
import base64
import re
import sys
from pathlib import Path


def fix_file(path):
    text = path.read_text(encoding="utf-8")
    changed = False

    def fix_src(m):
        nonlocal changed
        raw = base64.b64decode(m.group(1))
        fixed = depth_re.sub(b"\n", raw).rstrip(b"\n") + b"\n"
        if fixed != raw:
            changed = True
            return "data:image/svg+xml;base64," + base64.b64encode(fixed).decode()
        return m.group(0)

    text = src_re.sub(fix_src, text)
    stripped, n = style_re.subn("", text)
    if n:
        changed = True
        text = stripped
    if changed:
        path.write_text(text, encoding="utf-8")
    return changed


if __name__ == "__main__":
    html_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path.home() / ".local/share/doc/openturns/html"
    depth_re = re.compile(rb"\n?<!-- DEPTH=-?\d+ -->\n?")
    src_re = re.compile(r"data:image/svg\+xml;base64,([A-Za-z0-9+/=]+)")
    style_re = re.compile(r' style="vertical-align: -?\d+px"')
    for html_file in html_dir.rglob("*.html"):
        fix_file(html_file)
