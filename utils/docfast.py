#!/usr/bin/env python3
"""
Create a sphinx build environment for the OpenTURNS documentation
with only some of the source files in order to accelerate Sphinx builds.
Sphinx is then called to build the HTML doc using all available cores.
HTML files are written to a subfolder of the build folder called `install`.

Parameters
----------
build_name : str
    Name of the build folder

source_files : sequence of str
    Names of all source example Python scripts and RST source files to be included.

Notes
-----

Sphinx reads the docstrings of the OpenTURNS methods to generate the API doc.
This means that changing the source _doc.i.in files will have no effect,
since those files are not read by Sphinx.

The script attempts to address this problem by relying on the git diff command
to find out which parts of the source _doc.i.in files were changed and patch
the docstrings in the Python files of the OpenTURNS install folder accordingly.
The patch is reversed at the end of the script to restore the OpenTURNS install
folder to its original state.

By default, the script uses the command `git diff python/src/*_doc.i.in`
(which takes into account unstaged changes only),
but an additional argument xxx can be passed using the --diff option:

$ utils/docfast.py --diff xxx ...

will result in `git diff xxx python.src/*_doc.i.in` being used instead.

Be aware that the patch can fail in various ways. That is why the --nodiff
option is available to suppress the attempt to guess changes to the _doc.i.in
source files. That way, the OpenTURNS install folder is left untouched:

$ utils/docfast.py --nodiff ...

Examples
--------

Create a build directory `build-sphinxonly` that will contain only source files
required by Sphinx.

$ utils/docfast.py build-sphinxonly

Create a build directory `build-sphinxonly` that will contain only the mandatory
source files plus the `plot_smoothing_mixture.py` and `base_objects.rst` files.

$ utils/docfast.py build-sphinxonly base_objects.rst plot_smoothing_mixture.py

Do the same, but apply the changes made to the source _doc.i.in files with
respect to the master branch.

$ utils/docfast.py build-sphinxonly base_objects.rst plot_smoothing_mixture.py --diff master

"""
import os
import sys
import shutil
import glob

# from sphinx.cmd import build
import subprocess
import openturns as ot


def create_patch(gitdiff_arg):
    """
    Attempt to translate differences in _doc.i.in files into differences in the actual docstrings.

    Parameters
    ----------
    gitdiff_arg : str
        Additional argument to be passed to git diff
    """
    script_folder = os.path.dirname(os.path.abspath(__file__))  # openturns/utils
    sources_folder = os.path.dirname(script_folder)  # root folder openturns
    src_folder = os.path.join(sources_folder, "python", "src")  # openturns/python/src
    docstring_files = os.path.join(src_folder, "*_doc.i.in")
    gitdiff_filename = os.path.join(
        script_folder, "changes.patch"
    )  # openturns/utils/changes.patch

    # Call git diff
    if gitdiff_arg is None:
        gitdiff_listargs = ["git", "diff", docstring_files]
    else:
        gitdiff_listargs = ["git", "diff", gitdiff_arg, docstring_files]
    gitdiff = subprocess.check_output(gitdiff_listargs).decode()

    # Divide the output of git diff into chunks
    # Each chunks contains changes to one _doc.i.in file
    chunkstart = "diff --git a/"
    chunks = [chunkstart + chunk for chunk in gitdiff.split(chunkstart)[1:]]

    # Parse the chunks and try to recreate the actual docstring context
    for chunk_number, chunk in enumerate(chunks):
        lines = chunk.split("\n")
        for line_number, line in enumerate(lines[5:]):
            if len(line) == 0:
                lines[5 + line_number] = line
            elif line[0] == "@":
                lines[5 + line_number] = line
            elif line[0] == "+":
                if len(line) == 1:
                    lines[5 + line_number] = line
                elif line[1] == '"':
                    lines[5 + line_number] = "+" + "    " + line[2:]
                else:
                    lines[5 + line_number] = "+" + "    " + line[1:]
            elif line[0] == "-":
                if len(line) == 1:
                    lines[5 + line_number] = line
                elif line[1] == '"':
                    lines[5 + line_number] = "    " + '"""\n' + "-" + "    " + line[2:]
                else:
                    lines[5 + line_number] = "-" + "    " + line[1:]
            elif line[0] == '"':
                lines[5 + line_number] = "    " + '"""\n' + line
            else:
                lines[5 + line_number] = "    " + line
            if len(line) > 0:
                if lines[5 + line_number][-1] == '"':
                    if line[0] == "-":
                        lines[5 + line_number] = lines[5 + line_number][0:-1]
                    else:
                        lines[5 + line_number] = (
                            lines[5 + line_number][0:-1] + "\n" + "    " + '"""'
                        )
        lines = [line + "\n" for line in lines]
        chunks[chunk_number] = "".join(lines)

    # Join the chunks into one single string containing all diffs
    gitdiff = "".join(chunks)

    # Write the doctored string to changes.patch
    gitdiff_file = open(gitdiff_filename, "w")
    gitdiff_file.write(gitdiff)
    gitdiff_file.close()


def apply_patch():
    """
    Apply the patch to the Python files of the install folder.
    Create backup files to be used at the end of the script in order
    to rollback the patch.
    """
    script_folder = os.path.dirname(os.path.abspath(__file__))  # openturns/utils
    ot_folder = os.path.dirname(os.path.abspath(ot.__file__))  # install folder
    gitdiff_filename = os.path.join(
        script_folder, "changes.patch"
    )  # openturns/utils/changes.patch

    # Create backup Python scripts, apply the patch, remove byproducts
    for pypath in glob.iglob(os.path.join(ot_folder, "**", "*.py"), recursive=True):
        shutil.copyfile(pypath, pypath[0:-3] + ".BACKUP")
        subprocess.run(["patch", "-u", "-l", pypath, "-i", gitdiff_filename])
        try:
            os.remove(pypath + ".rej")
        except FileNotFoundError:
            pass
        try:
            os.remove(pypath + ".orig")
        except FileNotFoundError:
            pass

    # Remove patch file
    os.remove(gitdiff_filename)


# Get the arguments
arguments = sys.argv[1:]

if "--diff" in arguments and "--nodiff" in arguments:
    raise ValueError("--diff and --nodiff are incompatible")

# Options --diff and --nodiff determine gitdiff_flag and gitdiff_arg
gitdiff_flag = True
gitdiff_arg = None

# Non-optional arguments determine build_folder_name and list_preserved_files
build_folder_name = None
list_preserved_files = []

# Parse arguments
index = 0
while index < len(arguments):
    if arguments[index] == "--diff":
        if index + 1 == len(arguments):
            raise ValueError("No argument given after --diff")
        else:
            gitdiff_arg = arguments[index + 1]
            index += 1
    elif arguments[index] == "--nodiff":
        gitdiff_flag = False
    elif arguments[index][0] == "-":
        raise ValueError("Unrecognized option {}".format(arguments[index]))
    else:
        if build_folder_name is None:
            build_folder_name = arguments[index]
        else:
            list_preserved_files.append(arguments[index])
    index += 1

# Check arguments
if build_folder_name is None:
    raise ValueError("Build folder name should be passed as argument.")

# Create and apply patch to OT install Python files if required
if gitdiff_flag:
    create_patch(gitdiff_arg)
    apply_patch()

script_folder = os.path.dirname(os.path.abspath(__file__))  # openturns/utils
sources_folder = os.path.dirname(script_folder)  # root folder openturns
doc_folder = os.path.join(sources_folder, "python", "doc")  # openturns/python/doc
build_folder = os.path.join(sources_folder, build_folder_name)

# Copy everything from python/doc into the build folder
shutil.copytree(doc_folder, build_folder, dirs_exist_ok=True)  # Python 3.8+

# Remove all Python scripts in the examples folder...
for pypath in glob.iglob(
    os.path.join(build_folder, "examples", "**", "*.py"), recursive=True
):
    # ...unless their name was passed as argument.
    if os.path.basename(pypath) not in list_preserved_files:
        os.remove(pypath)

list_rstsections = ["developer_guide", "theory", "usecases", "user_manual"]

# For every doc section listed...
for section in list_rstsections:
    # ...remove all RST files...
    for rstpath in glob.iglob(
        os.path.join(build_folder, section, "**", "*.rst"), recursive=True
    ):
        # ...that do not have the same name as their directory...
        if (
            os.path.basename(rstpath)
            != os.path.basename(os.path.dirname(rstpath)) + ".rst"
        ):
            # ...unless their name was passed as argument.
            if os.path.basename(rstpath) not in list_preserved_files:
                os.remove(rstpath)

# Rename conf.py.in -> conf.py
os.rename(
    os.path.join(build_folder, "conf.py.in"), os.path.join(build_folder, "conf.py")
)

# build.build_main(['-j', 'auto', build_folder, os.path.join(build_folder, 'install')]) # does not work when git diff is used (because it shares a Python session?)
subprocess.run(
    ["sphinx-build", "-j", "auto", build_folder, os.path.join(build_folder, "install")]
)

# Restore all python files in openturns install directory
ot_folder = os.path.dirname(os.path.abspath(ot.__file__))  # install folder
for pypath in glob.iglob(os.path.join(ot_folder, "**", "*.BACKUP"), recursive=True):
    os.remove(pypath[0:-7] + ".py")
    os.rename(pypath, pypath[0:-7] + ".py")
