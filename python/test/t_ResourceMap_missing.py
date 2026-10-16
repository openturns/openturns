#!/usr/bin/env python

import openturns as ot
from pathlib import Path
import re

ot.TESTPREAMBLE()

# keys not registered in the ResourceMap but used by the docstrings
skipped_keys = ["SymbolicParser-Backend"]

# find all keys
resourcemap_file = (
    Path(__file__)
    .parents[2]
    .joinpath("lib")
    .joinpath("src")
    .joinpath("Base")
    .joinpath("Common")
    .joinpath("ResourceMap.cxx")
)
resourcemap_lines = []
with open(resourcemap_file) as f:
    resourcemap_lines += f.read().splitlines()
resourcemap_content = {}
resourcemap_types = {}
for line in resourcemap_lines:
    match = re.search(r'addAs(\w+)\("([\w\-]+)",[ ]*([\w\d\.\-\"]+)[\),]', line)
    if match is not None:
        key, vtype, value = match.group(2), match.group(1), match.group(3)
        print(key, vtype, value)
        if key in skipped_keys:
            continue
        if vtype == "Scalar":
            resourcemap_content[key] = float(value)
        elif vtype == "UnsignedInteger":
            resourcemap_content[key] = int(value)
        elif vtype == "Bool":
            resourcemap_content[key] = {"true": True, "false": False}[value]
        elif vtype.startswith("String"):
            resourcemap_content[key] = value.strip('"')
        else:
            raise ValueError(f"got {key}")
        resourcemap_types[key] = vtype
print(f"resourcemap: {len(resourcemap_content)} keys")

conf_file = (
    Path(__file__)
    .parents[2]
    .joinpath("lib")
    .joinpath("etc")
    .joinpath("openturns.conf.in")
)
conf_lines = []
with open(conf_file) as f:
    conf_lines += f.read().splitlines()
conf_content = {}
for line in conf_lines:
    match = re.search(r'^[ ]+<([\w\-]+)[ ]+value_(\w+)="([\w\d\.\-]+)"', line)
    if match is not None:
        key, vtype, value = match.group(1), match.group(2), match.group(3)
        if vtype == "float":
            conf_content[key] = float(value)
        elif vtype == "int":
            conf_content[key] = int(value)
        elif vtype == "bool":
            assert value in ["true", "false"], f"value should be true|false got {value}"
            conf_content[key] = value == "true"
        elif vtype == "str":
            conf_content[key] = value
        else:
            raise ValueError(f"got {vtype}")
print(f"config: {len(conf_content)} keys")

# check missing keys in config wrt resourcemap
for key in resourcemap_content:
    if key in conf_content:
        if resourcemap_content[key] != conf_content[key]:
            raise ValueError(
                f"key={key} RM={resourcemap_content[key]} CONF={conf_content[key]}"
            )
    else:
        raise ValueError(f"missing {key} in rm")

# check extra keys in config wrt resourcemap
for key in conf_content:
    if key not in resourcemap_content:
        raise ValueError(f"extra {key} in config")

# find all docstring files
docstring_files = sorted(Path(__file__).parents[1].joinpath("src").glob("*_doc.i"))
docstring_lines = []
for docstring_file in docstring_files:
    with open(docstring_file, errors="replace") as f:
        docstring_lines += f.read().splitlines()

# count undocumented keys
# Search in a single blob rather than line by line: keys contain no
# newline, so a key matches the blob iff it matches one of the lines
docstring_blob = "\n".join(docstring_lines)
undoc_keys = dict(resourcemap_content)
for key in resourcemap_content:
    if key in docstring_blob:
        del undoc_keys[key]

for key in undoc_keys:
    print(key)
n_undoc_keys = len(undoc_keys)
n_keys = len(resourcemap_content)
print(f"found {n_undoc_keys} undocumented keys out of {n_keys}")
assert not undoc_keys, f"undocumented resourcemap keys: {sorted(undoc_keys)}"


# check the documented type and default value of the keys documented as
# - ``Key`` (``Type``, default: ``value``): description
# the pattern spans several lines to deal with wrapped entries
doc_pattern = re.compile(
    r"-\s+``([\w\-]+)``\s+\(`+(\w+)`+,\s*default:\s*`+([^`]+)`+"
)


def cast_docstring_value(key, vtype, value):
    """Convert a value given in a docstring into a python object."""
    if vtype == "Scalar":
        return float(value)
    if vtype == "UnsignedInteger":
        return int(value)
    if vtype == "Bool":
        if value.lower() not in ("true", "false"):
            raise ValueError(f"key={key} got bool value={value}")
        return value.lower() == "true"
    if vtype == "String":
        return value
    raise ValueError(f"key={key} got type={vtype}")


n_typed_keys = 0
for docstring_file in docstring_files:
    with open(docstring_file, errors="replace") as f:
        for key, vtype, value in doc_pattern.findall(f.read()):
            if key in skipped_keys:
                continue
            if key not in resourcemap_content:
                raise ValueError(f"{docstring_file.name}: key={key} is not a ResourceMap key")
            if resourcemap_types[key] != vtype:
                raise ValueError(
                    f"{docstring_file.name}: key={key} DOC={vtype} RM={resourcemap_types[key]}"
                )
            docstring_value = cast_docstring_value(key, vtype, value)
            if docstring_value != resourcemap_content[key]:
                raise ValueError(
                    f"{docstring_file.name}: key={key} DOC={docstring_value} RM={resourcemap_content[key]}"
                )
            n_typed_keys += 1

print(f"found {n_typed_keys} documented keys with type and default value")
assert n_typed_keys > 0, "no documented key with type and default value found"
