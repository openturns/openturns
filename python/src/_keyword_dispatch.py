import functools as _functools
import importlib as _importlib


def _match_prefix(signature_specs, args, kwargs):
    # Match (args, kwargs) against overloads and return the positional
    # argument tuple for the native call, or None when no overload accepts
    # them. Only a leading run of explicitly provided parameters can be
    # forwarded: C++ default values are unknown to Python (the sidecar only
    # records whether a default exists), so a skipped parameter can never
    # be filled. Gaps, duplicates and unknown names reject the overload.
    for signature_spec in signature_specs:
        names = [parameter_name for parameter_name, _ in signature_spec]
        if len(args) > len(names):
            continue
        if any(name not in names for name in kwargs):
            continue
        if any(name in names[:len(args)] for name in kwargs):
            continue
        values = list(args)
        position = len(args)
        for parameter_name, _ in signature_spec[position:]:
            if parameter_name in kwargs:
                values.append(kwargs[parameter_name])
                position += 1
            else:
                break
        if position - len(args) != len(kwargs):
            continue
        if any(not optional for _, optional in signature_spec[position:]):
            continue
        return tuple(values)
    return None


def wrap_method(class_name, method_name, native_method, signature_specs, skip_first=True):
    @_functools.wraps(native_method)
    def wrapped(*args, **kwargs):
        if not kwargs:
            return native_method(*args)
        leading = args[:1] if skip_first else ()
        trailing = args[1:] if skip_first else args
        values = _match_prefix(signature_specs, trailing, kwargs)
        if values is None:
            if class_name:
                raise TypeError(
                    "No matching " + class_name + " signature for " + method_name
                )
            raise TypeError("No matching signature for " + method_name)
        return native_method(*leading, *values)

    wrapped._ot_kw_wrapped = True
    return wrapped


def _descriptor_kind(cls, method_name):
    for klass in getattr(cls, "__mro__", ()):
        raw = klass.__dict__.get(method_name)
        if raw is None:
            continue
        if isinstance(raw, staticmethod):
            return "static"
        if isinstance(raw, classmethod):
            return "class"
        return None
    return None


def apply_keyword_signatures(namespace, signatures_by_class):
    for class_name, methods in signatures_by_class.items():
        cls = namespace.get(class_name)
        if cls is None:
            continue
        for method_name, signature_specs in methods.items():
            try:
                native_method = getattr(cls, method_name, None)
            except Exception:
                continue
            if not callable(native_method):
                continue
            if getattr(native_method, "_ot_kw_wrapped", False):
                continue
            try:
                kind = _descriptor_kind(cls, method_name)
                target = native_method
                if kind == "class":
                    target = getattr(native_method, "__func__", native_method)
                wrapped_method = wrap_method(
                    class_name,
                    method_name,
                    target,
                    signature_specs,
                    skip_first=(kind != "static"),
                )
                if kind == "static":
                    wrapped_method = staticmethod(wrapped_method)
                elif kind == "class":
                    wrapped_method = classmethod(wrapped_method)
                setattr(cls, method_name, wrapped_method)
            except Exception:
                continue


def apply_keyword_functions(namespace, signatures_by_function):
    for function_name, signature_specs in signatures_by_function.items():
        try:
            native_function = namespace.get(function_name)
        except Exception:
            continue
        if not callable(native_function):
            continue
        if isinstance(native_function, type):
            continue
        if getattr(native_function, "_ot_kw_wrapped", False):
            continue
        try:
            namespace[function_name] = wrap_method(
                "", function_name, native_function, signature_specs,
                skip_first=False,
            )
        except Exception:
            continue


def apply_module_keywords(module_name, namespace):
    try:
        sidecar = _importlib.import_module(
            "openturns._" + module_name.split(".")[-1] + "_keyword_signatures"
        )
    except ImportError:
        return
    apply_keyword_signatures(namespace, getattr(sidecar, "KEYWORD_SIGNATURES", {}))
    apply_keyword_functions(namespace, getattr(sidecar, "KEYWORD_FUNCTIONS", {}))
