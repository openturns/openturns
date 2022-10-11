import sys
from os.path import basename
import openturns as ot


from io import StringIO
from docutils.parsers.rst import Directive
from docutils import nodes, statemachine


class execforresourcemap_directive(Directive):

    """Execute the specified python code and insert the output into the document"""

    has_content = True

    def run(self):
        oldStdout, sys.stdout = sys.stdout, StringIO()
        source = self.state_machine.input_lines.source(
            self.lineno - self.state_machine.input_offset - 1
        )
        try:

            table = nodes.table()

            tgroup = nodes.tgroup(cols=3)
            table += tgroup
            tgroup += nodes.colspec(colwidth=25, classes=["key"])
            tgroup += nodes.colspec(colwidth=8, classes=["value"])
            tgroup += nodes.colspec(colwidth=8, classes=["type"])
            thead = nodes.thead()
            tgroup += thead

            # Add headers
            row = nodes.row()
            thead += row

            entry = nodes.entry()
            row += entry
            node = nodes.paragraph(text="Key")
            entry += node

            entry = nodes.entry()
            row += entry
            node = nodes.paragraph(text="Value")
            entry += node

            entry = nodes.entry()
            row += entry
            node = nodes.paragraph(text="Type")
            entry += node

            # Add body
            tbody = nodes.tbody()
            tgroup += tbody
            row = nodes.row()
            tbody += row

            # reload in case altered by examples
            ot.ResourceMap.Reload()

            for key in ot.ResourceMap.GetKeys():
                row = nodes.row()
                tbody += row

                entry = nodes.entry()
                row += entry

                node = nodes.paragraph(text=key)
                entry += node

                entry = nodes.entry()
                row += entry

                value = ot.ResourceMap.Get(key)
                if not len(value):
                    value = " ".__repr__()
                if "\t" in value:
                    value = value.replace("\t", "\\t")
                node = nodes.paragraph(text=value)
                entry += node

                entry = nodes.entry()
                row += entry

                keyType = ot.ResourceMap.GetType(key)
                node = nodes.paragraph(text=keyType)
                entry += node

            return [table]
        except Exception:
            return [
                nodes.error(
                    None,
                    nodes.paragraph(
                        text="Unable to execute python code at %s:%d:"
                        % (basename(source), self.lineno)
                    ),
                    nodes.paragraph(text=str(sys.exc_info()[1])),
                )
            ]
        finally:
            sys.stdout = oldStdout


def setup(app):
    setup.app = app
    setup.config = app.config
    setup.confdir = app.confdir
    app.add_directive("execforresourcemap", execforresourcemap_directive)

    metadata = {"version": "0", "parallel_read_safe": True}
    return metadata
