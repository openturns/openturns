#!/usr/bin/env python

import argparse
import keyword
import pprint
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


def _attributes(node):
    return {
        attribute.get("name"): attribute.get("value")
        for attribute in node.findall("./attributelist/attribute")
    }


def _python_parameter_name(name):
    if not name or not name.isidentifier() or keyword.iskeyword(name):
        return "_" + (name or "arg")
    return name


def _signature(node):
    parameters = []
    parameter_list = node.find(".//parmlist")
    if parameter_list is None:
        return ()
    for parameter in parameter_list.findall("./parm"):
        parameter_attributes = _attributes(parameter)
        name = parameter_attributes.get("name")
        parameter_type = parameter_attributes.get("type")
        if not name or not parameter_type:
            return None
        parameters.append(
            (
                _python_parameter_name(name),
                "value" in parameter_attributes,
                parameter_type,
            )
        )
    return tuple(parameters)


def _method_signatures(member, toplevel=False):
    member_attributes = _attributes(member)
    candidates = member.findall("./cdecl") if member.tag == "using" else [member]
    signatures = []
    for candidate in candidates:
        attributes = _attributes(candidate)
        if not toplevel and attributes.get("access") != "public":
            continue
        if attributes.get("kind") != "function":
            continue
        if attributes.get("feature_ignore"):
            continue
        signature = _signature(candidate)
        if signature is not None:
            signatures.append(signature)
    return member_attributes.get("sym_name"), signatures


def _read_classes(xml_path):
    classes = {}
    functions = {}
    zero_input_parameters = set()
    class_depth = 0
    typemap_depth = 0
    # Small structural nodes read by their parent AFTER their own end event
    # (e.g. a cdecl reads its attributelist/parmlist children). They must
    # not be cleared early; they are reaped with their parent instead.
    spared_tags = {
        "attributelist",
        "attribute",
        "parmlist",
        "parm",
        "kwargs",
        "typemapitem",
        "baselist",
        "base",
    }
    for event, node in ET.iterparse(xml_path, events=("start", "end")):
        if event == "start":
            if node.tag == "class":
                class_depth += 1
            elif node.tag == "typemap":
                typemap_depth += 1
            continue

        if node.tag == "class":
            attributes = _attributes(node)
            class_name = attributes.get("name")
            if class_name:
                bases = [
                    base.get("name")
                    for base in node.findall("./attributelist/baselist/base")
                ]
                methods = {}
                constructors = []
                for member in node:
                    if member.tag == "constructor":
                        signature = _signature(member)
                        if signature is not None:
                            constructors.append(signature)
                    elif member.tag in {"cdecl", "using"}:
                        name, signatures = _method_signatures(member)
                        if name and signatures:
                            methods.setdefault(name, []).extend(signatures)

                classes[class_name] = {
                    "bases": bases,
                    "constructors": constructors,
                    "methods": methods,
                }
            class_depth -= 1
            node.clear()
        elif node.tag == "typemap":
            attributes = _attributes(node)
            keyword_arguments = node.find("./attributelist/kwargs")
            keyword_attributes = (
                _attributes(keyword_arguments) if keyword_arguments is not None else {}
            )
            if (
                attributes.get("method") == "in"
                and keyword_attributes.get("name") == "numinputs"
                and keyword_attributes.get("value") == "0"
            ):
                for parameter in node.findall(
                    "./typemapitem/attributelist/parmlist/parm"
                ):
                    parameter_attributes = _attributes(parameter)
                    name = parameter_attributes.get("name")
                    parameter_type = parameter_attributes.get("type")
                    if name and parameter_type:
                        zero_input_parameters.add((parameter_type, name))
            node.clear()
            typemap_depth -= 1
        elif class_depth == 0:
            if typemap_depth == 0:
                if node.tag in {"cdecl", "using"}:
                    # Module-level free function (e.g. VisualTest helpers).
                    # The qualified C++ name selects the declaring scope.
                    qualified = _attributes(node).get("name") or ""
                    name, signatures = _method_signatures(node, toplevel=True)
                    if name and signatures:
                        functions.setdefault(name, []).append(
                            (qualified, signatures)
                        )
                if node.tag not in spared_tags:
                    node.clear()
    return classes, functions, zero_input_parameters


def _qualified_base_name(name):
    return name if "::" in name else "OT::" + name


def _short_class_name(qualified_name):
    return qualified_name.split("::")[-1]


_INCLUDE_RE = re.compile(r'^\s*%include\s+"?([^"\s;]+)"?\s*')


def _module_class_names(module_i_path):
    names = []
    for line in Path(module_i_path).read_text(encoding="utf-8").splitlines():
        match = _INCLUDE_RE.match(line)
        if not match:
            continue
        include = match.group(1).strip()
        base = include.split("/")[-1]
        if not base.endswith(".i"):
            continue
        base = base[:-2]
        if base.endswith("_doc"):
            continue
        if base.endswith("Collection") or base.endswith("TemplateDefs"):
            continue
        if not base or not base[0].isupper():
            continue
        if "module" in base:
            continue
        names.append("OT::" + base)
    return names


def _class_signatures(root_name, classes, zero_input_parameters):
    if root_name not in classes:
        return {}
    methods = {"__init__": classes[root_name]["constructors"]}
    visited = set()

    def visit(class_name):
        if class_name in visited:
            return
        visited.add(class_name)
        class_info = classes.get(class_name)
        if class_info is None:
            return
        for name, signatures in class_info["methods"].items():
            if name not in methods:
                methods[name] = list(signatures)
        for base_name in class_info["bases"]:
            visit(_qualified_base_name(base_name))

    visit(root_name)

    result = {}
    for name, signatures in methods.items():
        unique_signatures = _visible_signatures(signatures, zero_input_parameters)
        if unique_signatures:
            result[name] = unique_signatures
    return result


def _visible_signatures(signatures, zero_input_parameters):
    unique_signatures = []
    seen = set()
    for signature in signatures:
        visible_signature = tuple(
            (parameter_name, optional)
            for parameter_name, optional, parameter_type in signature
            if (parameter_type, parameter_name.lstrip("_"))
            not in zero_input_parameters
        )
        if visible_signature not in seen:
            unique_signatures.append(visible_signature)
            seen.add(visible_signature)
    return unique_signatures


def _function_signatures(roots, functions, zero_input_parameters):
    scopes = tuple(root_name + "::" for root_name in roots)
    result = {}
    for name, overloads in functions.items():
        collected = []
        for qualified, signatures in overloads:
            if scopes and not qualified.startswith(scopes):
                continue
            collected.extend(signatures)
        unique_signatures = _visible_signatures(collected, zero_input_parameters)
        if unique_signatures:
            result[name] = unique_signatures
    return result


def _write_metadata(xml_path, output_path, roots=None, legacy=False):
    xml_path = Path(xml_path)
    output_path = Path(output_path)
    if (
        output_path.exists()
        and output_path.stat().st_mtime_ns >= xml_path.stat().st_mtime_ns
        and output_path.stat().st_mtime_ns >= Path(__file__).stat().st_mtime_ns
    ):
        return

    classes, functions, zero_input_parameters = _read_classes(xml_path)
    if legacy or roots is None:
        roots = ["OT::Normal"]
    if legacy:
        signatures = _class_signatures(roots[0], classes, zero_input_parameters)
        if not signatures:
            raise ValueError("SWIG XML has no " + roots[0] + " class")
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            "NORMAL_SIGNATURES = "
            + pprint.pformat(signatures, sort_dicts=True, width=88)
            + "\n",
            encoding="utf-8",
        )
        return
    merged = {}
    for root_name in roots:
        signatures = _class_signatures(root_name, classes, zero_input_parameters)
        if signatures:
            merged[_short_class_name(root_name)] = signatures
    func_signatures = _function_signatures(roots, functions, zero_input_parameters)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(
        "KEYWORD_SIGNATURES = "
        + pprint.pformat(merged, sort_dicts=True, width=88)
        + "\n"
        + "KEYWORD_FUNCTIONS = "
        + pprint.pformat(func_signatures, sort_dicts=True, width=88)
        + "\n",
        encoding="utf-8",
    )


def _parse_args(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("xml")
    parser.add_argument("output")
    parser.add_argument("--module-i", default=None)
    parser.add_argument("--roots", nargs="*", default=None)
    parser.add_argument("--legacy", action="store_true")
    return parser.parse_args(argv)


if __name__ == "__main__":
    _args = _parse_args(sys.argv[1:])
    _roots = list(_args.roots) if _args.roots else None
    if _args.module_i and _roots is None:
        _roots = _module_class_names(_args.module_i)
    _write_metadata(_args.xml, _args.output, roots=_roots, legacy=_args.legacy)
