"""Batch LaTeX compilation for :mod:`sphinx.ext.imgmath`.

Interim stand-in for https://github.com/sphinx-doc/sphinx/pull/14535
(``ext/imgmath: batch LaTeX compilation via pre-rendering``).

When ``imgmath_batch_size > 1``, all math expressions are collected on
``env-updated`` and compiled into multi-page DVI documents before the
write phase, reducing *N* LaTeX processes to *ceil(N / batch_size)*.
Pages are split via ``dvipng -pp`` / ``dvisvgm --page``.

Filenames, embed mode, depth handling, and error reporting match
single-equation mode. Batching is disabled when a custom
``template.tex``/``preview.tex`` template is detected.
"""

from __future__ import annotations

__all__ = (
    'pre_render_batch',
    'setup',
)

import contextlib
import os
import re
import shutil
import subprocess
import tempfile
from concurrent.futures import ThreadPoolExecutor, as_completed
from hashlib import sha1
from pathlib import Path
from subprocess import CalledProcessError
from typing import TYPE_CHECKING, Any

from docutils import nodes

import sphinx
from sphinx.errors import SphinxError
from sphinx.ext.imgmath import (
    InvokeError,
    MathExtError,
    convert_dvi_to_image,
    generate_latex_macro,
)
from sphinx.locale import __
from sphinx.util import logging
from sphinx.util.display import status_iterator
from sphinx.util.math import wrap_displaymath
from sphinx.util.png import write_png_depth

if TYPE_CHECKING:
    from sphinx.application import Sphinx
    from sphinx.builders import Builder
    from sphinx.config import Config
    from sphinx.environment import BuildEnvironment
    from sphinx.util.typing import ExtensionMetadata

logger = logging.getLogger(__name__)

__version__ = '0.1.0'

SUPPORT_FORMAT = ('png', 'svg')

depth_re = re.compile(r'\[\d+[^\]]*?depth=(-?\d+)\]')
depthsvg_re = re.compile(r'depth=(.*)pt')
depthsvgcomment_re = re.compile(r'<!-- DEPTH=(-?\d+) -->')


def write_svg_depth(filename: Path, depth: int) -> None:
    """Write the depth to SVG file as a comment at end of file."""
    with open(filename, 'a', encoding='utf-8') as f:
        f.write('\n<!-- DEPTH=%s -->' % depth)


def _latex_variables(image_format: str, math: str, config: Config) -> dict[str, Any]:
    """Shared variables for single and batch LaTeX documents."""
    return {
        'fontsize': config.imgmath_font_size,
        'baselineskip': round(config.imgmath_font_size * 1.2),
        'preamble': config.imgmath_latex_preamble,
        # the dvips option is important when imgmath_latex in {"xelatex", "tectonic"},
        # it has no impact when imgmath_latex="latex"
        'tightpage': '' if image_format == 'png' else ',dvips,tightpage',
        'math': math,
    }


def _cleanup_latex_tempdir(dvipath: Path) -> None:
    """Remove the temp dir created for *dvipath* (best effort)."""
    with contextlib.suppress(Exception):
        parent = dvipath.parent
        if 'sphinx-imgmath' in parent.name:
            shutil.rmtree(parent, ignore_errors=True)


def _compile_latex_document(latex: str, *, config: Config, suffix: str) -> Path:
    """Write *latex* to a temp dir and compile it to DVI/XDV."""
    tempdir = Path(tempfile.mkdtemp(suffix=suffix))
    filename = tempdir / 'math.tex'
    with open(filename, 'w', encoding='utf-8') as f:
        f.write(latex)

    imgmath_latex_name = os.path.basename(config.imgmath_latex)

    command = [config.imgmath_latex]
    if imgmath_latex_name != 'tectonic':
        command.append('--interaction=nonstopmode')
    command.extend(config.imgmath_latex_args)
    command.append('math.tex')

    try:
        subprocess.run(
            command, capture_output=True, cwd=tempdir, check=True, encoding='ascii'
        )
        if imgmath_latex_name in {'xelatex', 'tectonic'}:
            return tempdir / 'math.xdv'
        else:
            return tempdir / 'math.dvi'
    except OSError as exc:
        logger.warning(
            __(
                'LaTeX command %r cannot be run (needed for math '
                'display), check the imgmath_latex setting'
            ),
            config.imgmath_latex,
        )
        shutil.rmtree(tempdir, ignore_errors=True)
        raise InvokeError from exc
    except CalledProcessError as exc:
        msg = 'latex exited with error'
        shutil.rmtree(tempdir, ignore_errors=True)
        raise MathExtError(msg, exc.stderr, exc.stdout) from exc


def compile_math(latex: str, *, config: Config) -> Path:
    """Compile LaTeX macros for math to DVI (with tempdir cleanup by caller)."""
    return _compile_latex_document(latex, config=config, suffix='-sphinx-imgmath')


def convert_dvi_to_png_batch(
    dvipath: Path,
    out_path: Path,
    *,
    config: Config,
    page: int | None = None,
) -> int | None:
    """Convert DVI file to PNG image, optionally selecting *page*."""
    name = 'dvipng'
    command: list[str] = [
        config.imgmath_dvipng,
        '-o',
        os.fspath(out_path),
        '-T',
        'tight',
        '-z9',
    ]
    command.extend(config.imgmath_dvipng_args)
    if page is not None:
        command.extend(['-pp', str(page)])
    if config.imgmath_use_preview:
        command.append('--depth')
    command.append(os.fspath(dvipath))

    stdout, _stderr = convert_dvi_to_image(command, name)

    depth = None
    if config.imgmath_use_preview:
        for line in stdout.splitlines():
            matched = depth_re.search(line)
            if matched:
                depth = int(matched.group(1))
                write_png_depth(out_path, depth)
                break
    return depth


_glyph_id_re = re.compile(r'g\d+-\d+')
_path_tag_re = re.compile(r'<path\b[^>]*>', re.IGNORECASE)
_path_id_re = re.compile(r"""\bid\s*=\s*['"](g\d+-\d+)['"]""")
_path_d_re = re.compile(r"""\bd\s*=\s*['"]([^'"]*)['"]""")


def _normalize_svg_glyph_ids(svg_text: str) -> str:
    """Remap dvisvgm ``gF-C`` glyph IDs to canonical sequential numbers.

    ``dvisvgm --no-fonts`` derives the ``F`` (font index) part from the
    DVI's global font table, so the same equation gets different ``gF-C``
    prefixes depending on which other equations share the batch DVI
    (page order, chunk composition, cache state).  This remaps IDs based
    solely on the glyph outline data in *this* file, making the output
    invariant under batch composition and therefore deterministic
    across parallel/incremental builds.
    """
    defs: dict[str, str] = {}
    for tag in _path_tag_re.findall(svg_text):
        id_match = _path_id_re.search(tag)
        d_match = _path_d_re.search(tag)
        if id_match and d_match and id_match.group(1) not in defs:
            defs[id_match.group(1)] = d_match.group(1)
    if not defs and 'g' not in svg_text:
        return svg_text

    def _suffix(glyph_id: str) -> str:
        return glyph_id.split('-', 1)[1]

    # Deterministic order: sort glyph definitions by outline data.
    ordered_defs = sorted(defs, key=lambda gid: (defs[gid], _suffix(gid), gid))
    mapping: dict[str, str] = {}
    for new_index, old_id in enumerate(ordered_defs):
        mapping[old_id] = f'g{new_index}-{_suffix(old_id)}'

    # ``<use id='g..'>`` anchors (e.g. scaled glyphs) have no outline;
    # give them deterministic numbers after the definitions.
    remaining = sorted(set(_glyph_id_re.findall(svg_text)) - mapping.keys())
    for extra_index, old_id in enumerate(remaining, start=len(mapping)):
        mapping[old_id] = f'g{extra_index}-{_suffix(old_id)}'

    if not mapping:
        return svg_text

    def _replace(match: re.Match[str]) -> str:
        return mapping.get(match.group(0), match.group(0))

    return _glyph_id_re.sub(_replace, svg_text)


def _canonicalize_svg_file(out_path: Path) -> None:
    """Rewrite *out_path* with canonical glyph IDs (best effort)."""
    try:
        text = out_path.read_text(encoding='utf-8')
    except OSError:
        return
    normalized = _normalize_svg_glyph_ids(text)
    if normalized != text:
        out_path.write_text(normalized, encoding='utf-8')


def convert_dvi_to_svg_batch(
    dvipath: Path,
    out_path: Path,
    *,
    config: Config,
    page: int | None = None,
) -> int | None:
    """Convert DVI file to SVG image, optionally selecting *page*."""
    name = 'dvisvgm'
    command: list[str] = [config.imgmath_dvisvgm, '-o', os.fspath(out_path)]
    command.extend(config.imgmath_dvisvgm_args)
    if page is not None:
        command.append(f'--page={page}')
    command.append(os.fspath(dvipath))

    _stdout, stderr = convert_dvi_to_image(command, name)
    _canonicalize_svg_file(out_path)

    depth = None
    if config.imgmath_use_preview:
        for line in stderr.splitlines():  # not stdout !
            matched = depthsvg_re.search(line)
            if matched:
                depth = round(float(matched.group(1)) * 100 / 72.27)  # assume 100ppi
                write_svg_depth(out_path, depth)
                break
    return depth


def _has_custom_template(config: Config, confdir: str | os.PathLike[str]) -> bool:
    """Return True if the user overrides the default imgmath LaTeX template."""
    template_names = (
        'template.tex.jinja',
        'template.tex_t',
        'preview.tex.jinja',
        'preview.tex_t',
    )
    for template_dir in config.templates_path:
        base = Path(template_dir)
        if not base.is_absolute():
            base = Path(confdir) / base
        for template_name in template_names:
            if (base / template_name).exists():
                return True
    return False


def _collect_math_expressions(env: BuildEnvironment) -> list[str]:
    """Collect unique math expressions from all doctrees in document order."""
    seen: set[str] = set()
    ordered: list[str] = []

    def _add(math: str) -> None:
        if math not in seen:
            seen.add(math)
            ordered.append(math)

    # Sort docnames: ``all_docs`` insertion order varies with parallel
    # reading (set-based discovery + completion order), which would
    # otherwise leak into batch DVI page order and dvisvgm font indices.
    for docname in sorted(env.all_docs):
        try:
            doctree = env.get_doctree(docname)
        except Exception as exc:
            logger.debug(
                __('Failed to load doctree %r for imgmath batch: %s'), docname, exc
            )
            continue
        for math_node in doctree.findall(nodes.math):
            _add('$' + math_node.astext() + '$')
        for block_node in doctree.findall(nodes.math_block):
            if block_node.get('no-wrap', block_node.get('nowrap', False)):
                latex = block_node.astext()
            else:
                latex = wrap_displaymath(block_node.astext(), None, False)
            _add(latex)
    return ordered


def _build_batch_document(
    maths: list[str], *, config: Config, image_format: str
) -> str:
    r"""Build a multi-page LaTeX document matching the default templates.

    The preamble, packages, and ``\fontsize`` body must stay in sync with
    ``sphinx/templates/imgmath/template.tex.jinja`` and
    ``preview.tex.jinja`` (batching is disabled for custom templates).
    """
    variables = _latex_variables(image_format, '', config)
    fontsize = variables['fontsize']
    baselineskip = variables['baselineskip']
    preamble = variables['preamble']
    tightpage = variables['tightpage']
    use_preview = config.imgmath_use_preview

    lines = [
        r'\documentclass[12pt]{article}',
        r'\usepackage[utf8]{inputenc}',
        r'\usepackage{amsmath}',
        r'\usepackage{amsthm}',
        r'\usepackage{amssymb}',
        r'\usepackage{amsfonts}',
        r'\usepackage{anyfontsize}',
        r'\usepackage{bm}',
        r'\pagestyle{empty}',
        preamble,
        '',
    ]
    if use_preview:
        lines.extend([rf'\usepackage[active{tightpage}]{{preview}}', ''])
    lines.append(r'\begin{document}')
    for i, math in enumerate(maths):
        body = rf'\fontsize{{{fontsize}}}{{{baselineskip}}}\selectfont {math}'
        if use_preview:
            lines.extend([r'\begin{preview}', body, r'\end{preview}'])
        else:
            lines.append(body)
        if i < len(maths) - 1:
            lines.append(r'\clearpage')
    lines.append(r'\end{document}')
    return '\n'.join(lines) + '\n'


def _compile_batch_document(combined: str, *, config: Config) -> Path:
    """Compile a combined LaTeX document to DVI/XDV."""
    return _compile_latex_document(
        combined, config=config, suffix='-sphinx-imgmath-batch'
    )


def _compile_single_math(
    math: str,
    image_format: str,
    out_path: Path,
    *,
    config: Config,
    confdir: str | os.PathLike[str],
) -> None:
    latex = generate_latex_macro(image_format, math, config, confdir)  # type: ignore[arg-type]
    dvipath = compile_math(latex, config=config)
    try:
        if image_format == 'png':
            convert_dvi_to_png_batch(dvipath, out_path, config=config)
        else:
            convert_dvi_to_svg_batch(dvipath, out_path, config=config)
    finally:
        _cleanup_latex_tempdir(dvipath)


def _render_batch_chunk(
    maths: list[str],
    out_paths: list[Path],
    *,
    config: Config,
    confdir: str | os.PathLike[str],
    builder: Builder,
    image_format: str,
    max_workers: int = 1,
) -> None:
    uncached = [
        (m, p) for m, p in zip(maths, out_paths, strict=True) if not p.is_file()
    ]
    if not uncached:
        return
    uncached_maths = [m for m, _p in uncached]

    try:
        combined = _build_batch_document(
            uncached_maths, config=config, image_format=image_format
        )
        dvipath = _compile_batch_document(combined, config=config)
    except InvokeError:
        builder._imgmath_warned_latex = True  # type: ignore[attr-defined]
        return
    except MathExtError as err:
        logger.warning(
            __(
                'Batch LaTeX compilation failed, falling back to single compilation: %s'
            ),
            err,
        )
        for math, path in uncached:
            try:
                _compile_single_math(
                    math, image_format, path, config=config, confdir=confdir
                )
            except (InvokeError, MathExtError) as err_single:
                if isinstance(err_single, InvokeError):
                    builder._imgmath_warned_latex = True  # type: ignore[attr-defined]
                    return
                logger.warning(
                    __('Failed to render math %r in single-compilation fallback: %s'),
                    math,
                    err_single,
                )
        return

    def _convert_page(
        idx: int, math: str, path: Path
    ) -> tuple[int, str, Path, Exception | None]:
        page = idx + 1
        try:
            if image_format == 'png':
                convert_dvi_to_png_batch(dvipath, path, config=config, page=page)
            else:
                convert_dvi_to_svg_batch(dvipath, path, config=config, page=page)
        except (InvokeError, MathExtError) as err_convert:
            return page, math, path, err_convert
        return page, math, path, None

    try:
        workers = max(1, min(len(uncached), max_workers))
        with ThreadPoolExecutor(max_workers=workers) as executor:
            results = list(
                executor.map(
                    _convert_page,
                    range(len(uncached)),
                    [math for math, _path in uncached],
                    [path for _math, path in uncached],
                )
            )

        for page, math, path, error in results:
            if error is None:
                continue
            if isinstance(error, InvokeError):
                builder._imgmath_warned_image_translator = True  # type: ignore[attr-defined]
                return
            logger.warning(
                __('Failed to convert batch page %d, retrying singly: %s'),
                page,
                error,
            )
            try:
                _compile_single_math(
                    math, image_format, path, config=config, confdir=confdir
                )
            except (InvokeError, MathExtError) as err_retry:
                if isinstance(err_retry, InvokeError):
                    builder._imgmath_warned_image_translator = True  # type: ignore[attr-defined]
                    return
                logger.warning(
                    __('Failed to render math %r after batch page failure: %s'),
                    math,
                    err_retry,
                )
    finally:
        _cleanup_latex_tempdir(dvipath)


def pre_render_batch(app: Sphinx, env: BuildEnvironment) -> None:
    """Pre-render all math expressions in batches before the write phase.

    Runs on ``env-updated`` so that by the time HTML translation calls
    :func:`sphinx.ext.imgmath.render_math`, images already exist on disk.
    This keeps ``imgmath_embed``, depth handling, and error reporting
    identical to single-equation mode, and is safe for parallel writes.
    """
    config = app.config
    batch_size: int = getattr(config, 'imgmath_batch_size', 1)
    if not isinstance(batch_size, int) or batch_size <= 1:
        return
    builder = app.builder
    if builder.format != 'html':
        return

    image_format = str(config.imgmath_image_format).lower()
    if image_format not in SUPPORT_FORMAT:
        return

    confdir = builder.confdir
    if _has_custom_template(config, confdir):
        logger.info(__('imgmath batch disabled: custom LaTeX template detected'))
        return

    maths = _collect_math_expressions(env)
    if not maths:
        return

    out_paths: list[Path] = []
    for math in maths:
        latex = generate_latex_macro(image_format, math, config, confdir)  # type: ignore[arg-type]
        filename = (
            f'{sha1(latex.encode(), usedforsecurity=False).hexdigest()}.{image_format}'
        )
        path = Path(builder.outdir) / builder.imagedir / 'math' / filename
        path.parent.mkdir(parents=True, exist_ok=True)
        out_paths.append(path)

    parallel = getattr(app, 'parallel', 0)
    max_workers = parallel if parallel and parallel > 1 else 1

    chunks = [
        (maths[i : i + batch_size], out_paths[i : i + batch_size])
        for i in range(0, len(maths), batch_size)
    ]
    if max_workers > 1 and len(chunks) > 1:
        # Compile LaTeX batches in parallel (each chunk is one LaTeX
        # invocation).  Inner page conversions run sequentially to keep
        # total concurrent processes bounded by max_workers.
        outer_workers = min(max_workers, len(chunks))
        with ThreadPoolExecutor(max_workers=outer_workers) as executor:
            futures = [
                executor.submit(
                    _render_batch_chunk,
                    chunk_maths,
                    chunk_paths,
                    config=config,
                    confdir=confdir,
                    builder=builder,
                    image_format=image_format,
                    max_workers=1,
                )
                for chunk_maths, chunk_paths in chunks
            ]
            for future in as_completed(futures):
                future.result()
        return
    for idx in status_iterator(
        range(len(chunks)),
        __('pre-rendering math... '),
        length=len(chunks),
        stringify_func=lambda i: f'batch {i + 1}/{len(chunks)}',
    ):
        if hasattr(builder, '_imgmath_warned_latex') or hasattr(
            builder, '_imgmath_warned_image_translator'
        ):
            break
        chunk_maths, chunk_paths = chunks[idx]
        _render_batch_chunk(
            chunk_maths,
            chunk_paths,
            config=config,
            confdir=confdir,
            builder=builder,
            image_format=image_format,
            max_workers=max_workers,
        )


def setup(app: Sphinx) -> ExtensionMetadata:
    # Ensure base imgmath config values exist even if the user enables only
    # this extension.
    try:
        app.setup_extension('sphinx.ext.imgmath')
    except SphinxError:
        pass

    app.add_config_value(
        'imgmath_batch_size', 1, 'html', types=frozenset({int})
    )
    app.connect('env-updated', pre_render_batch)
    return {
        'version': sphinx.__display_version__,
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
