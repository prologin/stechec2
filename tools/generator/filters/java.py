# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>
# Copyright (c) 2020 Sacha Delanoue
# Copyright (c) 2020 Antoine Pietri

from functools import partial

from . import register_filter
from .common import camel_case, generic_args, generic_comment
from .common import generic_prototype, get_array_inner, is_array
from .cxx import cxx_type


@register_filter
def java_type(value: str) -> str:
    """Get the type name in the Java language"""
    base_types = {
        'void': 'void',
        'int': 'int',
        'double': 'double',
        'bool': 'boolean',
        'string': 'String',
    }
    if value in base_types:
        return base_types[value]
    if is_array(value):
        return "{}[]".format(java_type(get_array_inner(value)))
    return camel_case(value)


@register_filter
def java_args(value, type_mapper=lambda x: x) -> str:
    return generic_args(value, type_mapper=java_type)


java_prototype = register_filter(
    partial(generic_prototype, type_mapper=java_type, arg_mapper=java_args),
    name='java_prototype',
)


@register_filter
def java_comment(value: str, doc: bool = False, indent: int = 0) -> str:
    if not doc:
        return generic_comment(value, "// ", indent)
    return "/**\n" + indent * " " + generic_comment(
        value, " * ", indent) + "\n" + indent * " " + " */"


def _jtype(typename: str) -> str:
    """Return the type to be used as for java in java_to_cxx and cxx_to_java"""
    generics = {
        'bool': 'jboolean',
        'double': 'jdouble',
        'int': 'jint',
        'string': 'jstring',
    }
    if typename in generics.keys():
        return generics[typename]
    return camel_case(typename)


@register_filter
def java_to_cxx(value: str) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        return 'java_to_cxx_array<{}, {}>'.format(
            _jtype(base_type),
            cxx_type(base_type)
        )
    elif value in ('int', 'double', 'bool', 'string'):
        return 'java_to_cxx<{}, {}>'.format(_jtype(value), cxx_type(value))
    else:
        return 'java_to_cxx<jobject, {}>'.format(cxx_type(value))


@register_filter
def cxx_to_java(value: str) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        return 'cxx_to_java_array<{}, {}>'.format(
            cxx_type(base_type),
            _jtype(base_type)
        )
    elif value in ('int', 'double', 'bool', 'string'):
        return 'cxx_to_java<{}, {}>'.format(cxx_type(value), _jtype(value))
    else:
        return 'cxx_to_java<{}, jobject>'.format(cxx_type(value))


@register_filter
def jni_type(value: str) -> str:
    """Return the real JNI type"""
    if value in ('boolean', 'int', 'double', 'string'):
        return 'j' + value
    elif value == 'bool':
        return "jboolean"
    elif is_array(value):
        return "jarray"
    elif value == "void" or not value:
        return "void"
    return "jobject"


@register_filter
def jni_type_signature(value: str) -> str:
    """Get Java VM's representation of type signature for a type"""
    if is_array(value):
        return "[" + jni_type_signature(get_array_inner(value))
    dic = {
        'bool': 'Z',
        'double': 'D',
        'int': 'I',
        'void': 'V',
        'string': 'Ljava/lang/String;',
    }
    if value in dic.keys():
        return dic[value]
    return 'L' + camel_case(value) + ';'


@register_filter
def jni_method_signature(func) -> str:
    """Get Java VM's representation of type signature for a method"""
    return "({}){}".format(
        "".join(
            jni_type_signature(arg_type)
            for _, arg_type, _ in func['fct_arg']),
        jni_type_signature(func['fct_ret_type']))


@register_filter
def java_type_method_call(value: str) -> str:
    """return type used in the Call<TYPE>Method functions (and alike)"""
    if value in ('int', 'double'):
        return value.capitalize()
    elif value == 'bool':
        return "Boolean"
    elif value == 'void' or not value:
        return "Void"
    return "Object"
