
import gdb
import re
import sys
if sys.version > '3':
    long = int

objfile = gdb.current_objfile() or gdb.objfiles()[0]
int_ptr  = gdb.lookup_type('int').pointer()

ot_pretty_printers = []

def AddToPrettyPrinter(rx):
    def class_wrapper(cls):
        ot_pretty_printers.append( (re.compile(rx), cls) )
        return cls
    return class_wrapper

@AddToPrettyPrinter(r'^OT::PersistentObject$')
class PersistentObjectPrinter(object):
    def __init__(self, val):
        self.val = val

    def to_string(self):
        name = self.val['p_name_']['ptr_']['ptr_']
        if name == gdb.Value(0):
            return "[unnamed]"
        else:
            return "[name=%s]" % (str(name.dereference()))

@AddToPrettyPrinter(r'^OT::Pointer<.*>$')
class PointerPrinter(object):
    def __init__(self, val):
        self.val = val

    def to_string(self):
        global int_ptr

        pointee = self.val['ptr_']['ptr_']
        if long(pointee) == 0L:
            return "NULL"
        else:
            atomicVal = self.val['ptr_']['count_']['p_impl_']['use_count_']['val_']
            # This atomicVal points either to an int or a tbb::atomic<int>
            # The implementation of tbb::atomic depends on tbb's version, thus it is easier
            # to dereference it.
            atomicVal = atomicVal.address.cast(int_ptr).dereference()
            return "([counter=%s] %s) 0x%x = %s" % (str(atomicVal), str(pointee.type), long(pointee), str(pointee.dereference()))

@AddToPrettyPrinter(r'^OT::TypedInterfaceObject<.*>$')
class TypedInterfaceObjectPrinter(object):
    def __init__(self, val):
        self.val = val
        self.template_name = val.type.template_argument(0)

    def to_string(self):
        return "{ p_implementation_ -> [ %s ] }" % (str(self.val['p_implementation_']))

@AddToPrettyPrinter(r'^OT::(PersistentCollection<.*>|.*PersistentCollection)$')
class PersistentCollectionPrinter(object):
    def __init__(self, val):
        self.val = val

    def display_hint(self):
        return 'array'

    def to_string(self):
        # Dereference PersistentObject
        name = self.val['p_name_']['ptr_']['ptr_']
        if long(name) == 0L:
            decoratedName = "[unnamed]"
        else:
            decoratedName = "[name=%s]" % (str(name.dereference()))
        # Dereference Collection
        return "%s %s" % (decoratedName, self.val['coll__'])

@AddToPrettyPrinter(r'^OT::(Collection<.*>|.*Collection)$')
class CollectionPrinter(object):
    def __init__(self, val):
        self.val = val

    def display_hint(self):
        return 'array'

    def to_string(self):
        return str(self.val['coll__'])


def lookup_function(val):
    type = val.type
    if type.code == gdb.TYPE_CODE_REF:
        type = type.target()
    type = type.unqualified().strip_typedefs()
    typename = type.tag
    if not typename or not typename.startswith('OT::'):
        return None
    for regex, pretty_printer in ot_pretty_printers:
        if regex.search(typename):
            return pretty_printer(val)
    return None

objfile.pretty_printers.append(lookup_function)
