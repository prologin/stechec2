# -*- ruby -*-
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2011 Prologin

class JavaCxxFileGenerator < CxxProto

  def conv_java_type(typename)
    typename.split("_").map { |s| s.capitalize }.join
  end

  # returns the C++ type for the provided type
  def get_cxx_type(type)
    if type.is_array?
      type.type.name
    elsif type.name == "string"
      "std::string"
    elsif type.name == "bool"
      "boolean"
    else
      type.name
    end
  end

  # return the type to be used as Lang in lang2cxx and cxx2lang
  def get_lang_type(typename)
    if ['boolean', 'byte', 'char', 'short', 'int', 'long', 'float', 'double'].include? typename
      'j' + typename
    elsif typename == "std::string"
      "jstring"
    else
      conv_java_type(typename)
    end
  end

  # return the real JNI type
  def get_jni_type(type)
    if ['boolean', 'byte', 'char', 'short', 'int', 'long', 'float', 'double', 'string'].include? type.name
      'j' + type.name
    elsif type.name == 'bool'
      "jboolean"
    elsif type.is_array?
      "jarray"
    elsif type.is_nil?
      "void"
    else
      "jobject"
    end
  end

  # return the type to be used in the Call<TYPE>Method functions (and alike)
  def get_java_type(type)
    if ['byte', 'char', 'short', 'int', 'long', 'float', 'double'].include? type.name
      type.name.capitalize()
    elsif type.name == 'bool'
      "Boolean"
    elsif type.is_nil?
      "Void"
    else
      "Object"
    end
  end

  def get_signature(type)
    if ['bool', 'byte', 'char', 'short', 'int', 'long', 'float', 'double', 'void'].include? type.name
      {'bool' => 'Z', 'byte' => 'B', 'char' => 'C', 'short' => 'S', 'int' => 'I', 'long' => 'J', 'float' => 'F', 'double' => 'D', 'void' => 'V'}[type.name]
    elsif type.is_nil?
      'void'
    elsif type.is_array?
      '[' + get_signature(type.type)
    elsif type.name == "string"
      'Ljava/lang/String;'
    else
      'L' + conv_java_type(type.name) + ';'
    end
  end

  def get_fn_signature(fn)
  "(" + fn.args.map { |arg| get_signature(arg.type) }.join() + ")" + get_signature(fn.ret)
  end

  def get_lang2cxx(type)
    typename = get_cxx_type(type)
    jtypename = get_lang_type(typename)
    if type.is_array?
      "lang2cxx_array<#{jtypename}, #{typename}>"
    elsif type.is_simple?
      "lang2cxx<#{jtypename}, #{typename}>"
    else
      "lang2cxx<jobject, #{typename}>"
    end
  end

  def get_cxx2lang(type)
    typename = get_cxx_type(type)
    jtypename = get_lang_type(typename)
    if type.is_array?
      "cxx2lang_array<#{typename}, #{jtypename}>"
    elsif type.is_simple?
      "cxx2lang<#{typename}, #{jtypename}>"
    else
      "cxx2lang<#{typename}, jobject>"
    end
  end

  def initialize
    super
    @lang = "C++ (for Java interface)"
  end

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner 'generator_java.rb'

    @f.puts <<-EOF
#ifndef INTERFACE_HH_
# define INTERFACE_HH_

# include <cstdlib>
# include <jni.h>
# include <vector>
# include <string>

typedef unsigned char boolean;
typedef signed char byte;

EOF

    build_interface_class
    build_enums
    build_structs
    build_vm_init_interface

    @f.puts "", 'extern "C" {', ""
    for_each_fun do |fn|
      @f.print cxx_proto(fn, "api_")
      @f.puts ";"
    end
    @f.puts "}", "", "#endif // !INTERFACE_HH_"
    @f.close
  end

  def build_interface_class
    @f.puts "typedef struct Prologin {", "  static jclass Class();", "} Prologin;", ""
    for_each_enum do |e|
      n = conv_java_type(e['enum_name'])
      @f.puts "typedef struct #{n} {", "  static jclass Class();", "} #{n};", ""
    end
    for_each_struct do |s|
      n = conv_java_type(s['str_name'])
      @f.puts "typedef struct #{n} {", "  static jclass Class();", "} #{n};", ""
    end
  end

  def build_vm_init_interface
    @f.puts <<-EOF
struct ProloginJavaRunTime {
  ProloginJavaRunTime();
  ~ProloginJavaRunTime();
  JNIEnv* env();

  JavaVM* jvm;
  JNIEnv* env_;
  jobject prologin;
};

extern struct ProloginJavaRunTime jrt;
EOF
  end

  def build_common_wrappers

    # In case of errors...
    @f.puts '// In case of errors...'
    @f.puts <<-EOF
template<typename Lang, typename Cxx>
Cxx lang2cxx(Lang in)
{
  return in.error_should_not_happens;
}

template<typename Cxx, typename Lang>
Lang cxx2lang(Cxx in)
{
  return in.error_should_not_happens;
}

template<typename Lang, typename Cxx>
std::vector<Cxx> lang2cxx_array(jobject in);

template<typename Cxx, typename Lang>
jarray cxx2lang_array(std::vector<Cxx> in);

EOF

    # Basic type wrappers
    @f.puts '// Basic type wrappers'
    ['boolean', 'byte', 'char', 'short', 'int', 'long', 'float', 'double'].each do |type|
      jnitype = 'j' + type
      arraytype = jnitype + 'Array'
      @f.puts <<-EOF
template <>
#{jnitype} cxx2lang<#{type}, #{jnitype}>(#{type} in)
{
  return (#{jnitype})in;
}

template <>
#{type} lang2cxx<#{jnitype}, #{type}>(#{jnitype} in)
{
  return (#{type})in;
}

template <>
jarray cxx2lang_array<#{type}, #{jnitype}>(std::vector<#{type}> in)
{
  #{arraytype} out = jrt.env()->New#{type.capitalize}Array((jsize)in.size());
  jrt.env()->Set#{type.capitalize}ArrayRegion(out, (jsize)0, (jsize)in.size(), (const #{jnitype}*)in.data());
  return (jarray)out;
}

template <>
std::vector<#{type}> lang2cxx_array<#{jnitype}, #{type}>(jobject in)
{
  #{arraytype} array = (#{arraytype})in;
  jsize size = jrt.env()->GetArrayLength(array);
  #{jnitype}* datas = jrt.env()->Get#{type.capitalize}ArrayElements(array, NULL);
  std::vector<#{type}> out(datas, datas + size);
  jrt.env()->Release#{type.capitalize}ArrayElements(array, datas, JNI_ABORT);
  return out;
}

EOF
    end

    # Strings wrappers
    @f.puts '// String wrappers'
    @f.puts <<-EOF
template <>
jstring cxx2lang<std::string, jstring>(std::string in)
{
  return jrt.env()->NewStringUTF(in.data());
}

template <>
std::string lang2cxx<jstring, std::string>(jstring in)
{
  jboolean is_copy;
  const jchar* datas = jrt.env()->GetStringChars(in, &is_copy);
  jsize size = jrt.env()->GetStringLength(in);
  std::string out((const char*)datas, (size_t)size);
  if (is_copy)
    jrt.env()->ReleaseStringChars(in, datas);
  return out;
}

EOF

    # Object array wrappers (assume Lang::class exists)
    @f.puts '// Object array wrappers (assume Lang::class exists)'
    @f.puts <<-EOF
template <typename Cxx, typename Lang>
jarray cxx2lang_array(std::vector<Cxx> in)
{
  jobjectArray out = jrt.env()->NewObjectArray((jsize)in.size(), Lang::Class(), NULL);
  for (size_t i = 0; i < in.size(); i++)
    jrt.env()->SetObjectArrayElement(out, (jsize)i, cxx2lang<Cxx, jobject>(in[i]));
  return out;
}

template <typename Lang, typename Cxx>
std::vector<Cxx> lang2cxx_array(jobject in)
{
  jobjectArray array = (jobjectArray)in;
  size_t size = (size_t)jrt.env()->GetArrayLength(array);
  std::vector<Cxx> out;
  for (size_t i = 0; i < size; i++)
    out.push_back(lang2cxx<jobject, Cxx>(jrt.env()->GetObjectArrayElement(array, (jsize)i)));
  return out;
}

EOF
  end

  # Assume Lang::class exists
  def build_enum_wrappers(enum)
    name = enum['enum_name']
    jname = conv_java_type(name)
    @f.puts <<-EOF
template<>
#{name} lang2cxx<jobject, #{name}>(jobject in)
{
  jmethodID ordinal = jrt.env()->GetMethodID(#{jname}::Class(), "ordinal", "()I");
  return #{name}(lang2cxx<jint, int>(jrt.env()->CallIntMethod(in, ordinal)));
}

template<>
jobject cxx2lang<#{name}, jobject>(#{name} in)
{
  jmethodID method = jrt.env()->GetStaticMethodID(#{jname}::Class(), "values", "()[L#{jname};");
  jobjectArray values = (jobjectArray)jrt.env()->CallStaticObjectMethod(#{jname}::Class(), method);
  return jrt.env()->GetObjectArrayElement(values, (jsize)in);
}

EOF
  end

  def build_struct_wrappers(str)
    name = str['str_name']
    jname = conv_java_type(name)
    fields = str['str_field']
    @f.puts "template <>", "#{name} lang2cxx<jobject, #{name}>(jobject in)", "{", "  #{name} out;"
    fields.each do |f|
      field = f[0]
      type = @types[f[1]]
      javatype = get_java_type(type)
      signature = get_signature(type)
      @f.puts "  out.#{field} = #{get_lang2cxx(type)}(jrt.env()->Get#{javatype}Field(in, jrt.env()->GetFieldID(#{jname}::Class(), \"#{field}\", \"#{signature}\")));"
    end
    @f.puts '  return out;', '}', ''
    @f.puts "template <>", "jobject cxx2lang<#{name}, jobject>(#{name} in)", "{"
    @f.puts "  jobject out = jrt.env()->NewObject(#{jname}::Class(), jrt.env()->GetMethodID(#{jname}::Class(), \"<init>\", \"()V\"));"
    fields.each do |f|
      field = f[0]
      type = @types[f[1]]
      javatype = get_java_type(type)
      signature = get_signature(type)
      @f.puts "  jrt.env()->Set#{javatype}Field(out, jrt.env()->GetFieldID(#{jname}::Class(), \"#{field}\", \"#{signature}\"), #{get_cxx2lang(type)}(in.#{field}));"
    end
    @f.puts '  return out;', '}', ''
  end

  def build_function(fn)
    @f.print "#{get_jni_type(fn.ret)} #{fn.name}(JNIEnv* _env, jobject _obj"
    args = fn.args.map { |arg| "#{get_jni_type(arg.type)} #{arg.name}" }
    @f.print ", " if not fn.args.empty?
    @f.puts args.join(", ") + ")", "{"
    call = "api_" + fn.name + "(" + fn.args.map { |arg| get_lang2cxx(arg.type) + "(#{arg.name})"}.join(", ") + ")"
    call = "return #{get_cxx2lang(fn.ret)}(" + call + ")" if not fn.ret.is_nil?
    @f.puts "  #{call};", "}", ""
  end

  # Assume jrt.prologin AND Prologin::class
  def build_user_function(fn)
    @f.puts cxx_proto(fn, '', 'extern "C"'), "{"
    @f.puts "  jmethodID method = jrt.env()->GetMethodID(Prologin::Class(), \"#{fn.name}\", \"#{get_fn_signature(fn)}\");"
    args = fn.args.map { |arg| get_lang2cxx(arg.type) + "(#{arg.name})"}.join(", ")
    call = "jrt.env()->Call#{get_java_type(fn.ret)}Method(jrt.prologin, method" + args + ")"
    call = "#{get_cxx_type(fn.ret)} out = #{get_lang2cxx(fn.ret)}(" + call + ")" if not fn.ret.is_nil?
    @f.puts <<-EOF
  #{call};
  if (jrt.env()->ExceptionOccurred())
  {
    jrt.env()->ExceptionDescribe();
    exit(1);
  }
EOF
  @f.puts "  return out;" if not fn.ret.is_nil?
  @f.puts "}", ""
  end

  def build_vm_init
    # Initialize the interface class (used to translate to Java type)
    @f.puts "jclass Prologin::Class()", "{", "  return jrt.env()->FindClass(\"Prologin\");", "}", ""
    for_each_enum(false) do |e|
      name = conv_java_type(e['enum_name'])
      @f.puts "jclass #{name}::Class()", "{", "  return jrt.env()->FindClass(\"#{name}\");", "}", ""
    end
    for_each_struct(false) do |s|
      name = conv_java_type(s['str_name'])
      @f.puts "jclass #{name}::Class()", "{", "  return jrt.env()->FindClass(\"#{name}\");", "}", ""
    end

    # Bound native methods to the Java interface
    methods = []
    for_each_fun(false) do |fn|
      methods.push("    {(char*)\"#{fn.name}\", (char*)\"#{get_fn_signature(fn)}\", (void*)&#{fn.name}}")
    end
    @f.puts "static void _register_native_methods(JNIEnv* env)", "{"
    @f.puts "  JNINativeMethod methods[] = {", methods.join(",\n"), "  };"
    @f.puts "  env->RegisterNatives(Prologin::Class(), methods, sizeof(methods)/sizeof(methods[0]));", "}", ""

    # Initialize the runtime
    @f.puts <<-EOF
ProloginJavaRunTime jrt;

ProloginJavaRunTime::ProloginJavaRunTime()
{
 std::string classpath = "-Djava.class.path=";
  char* champion_path = getenv("CHAMPION_PATH");
  if (champion_path == NULL)
    champion_path = (char*)"./";
  classpath.append(champion_path);

  JavaVMInitArgs vm_args; /* JDK/JRE 6 VM initialization arguments */
  JavaVMOption options[2];
  options[0].optionString = (char*)classpath.c_str();
  options[1].optionString = (char*)"-ea";
  vm_args.version = JNI_VERSION_1_6;
  vm_args.nOptions = 2;
  vm_args.options = options;
  vm_args.ignoreUnrecognized = false;
  JNI_CreateJavaVM(&jvm, (void**)&env_, &vm_args);
  prologin = env_->NewObject(Prologin::Class(), env_->GetMethodID(Prologin::Class(), "<init>", "()V"));
  _register_native_methods(env_);
}

ProloginJavaRunTime::~ProloginJavaRunTime()
{
  jvm->DestroyJavaVM();
}

JNIEnv* ProloginJavaRunTime::env()
{
  if (jvm->GetEnv((void**)&env_, JNI_VERSION_1_6) != JNI_OK)
   jvm->AttachCurrentThread((void**)&env_, NULL);
  return env_;
}

EOF
  end

  def generate_source
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_java.rb"

    @f.puts '#include "interface.hh"', ''

    build_common_wrappers
    for_each_enum { |e| build_enum_wrappers e }
    for_each_struct { |s| build_struct_wrappers s }
    for_each_fun { |fn| build_function fn }
    for_each_user_fun { |fn| build_user_function fn }
    build_vm_init

    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "java"
    @header_file = "interface.hh"
    @source_file = "interface.cc"

    generate_header
    generate_source
  end
end

class JavaFileGenerator < JavaProto
  def initialize
    super
    @lang = "java"
    @java_types = {
      'void' => 'void',
      'int' => 'int',
      'bool' => 'boolean'
    }
    @java_obj_types = {
      'int' => 'Integer',
      'bool' => 'Boolean'
    }
  end

  def name_to_type(str)
    $conf['enum'].each do |x|
      if str == x['enum_name']
        return x
      end
    end
    return nil #error
  end

  def build
    @path = Pathname.new($install_path) + "java"
    @java_interface = 'Interface'
    @java_file = $conf['conf']['player_filename'].capitalize

    JavaCxxFileGenerator.new.build

    ######################################
    ##  Interface.java file generating  ##
    ######################################
    @f = File.new(@path + (@java_interface + '.java'), 'w')
    print_banner "generator_java.rb"

    for_each_enum do |x|
      name = conv_java_type(x['enum_name'])
      @f.puts "enum #{name} {"
      x['enum_field'].each do |f|
        @f.puts "  #{f[0].upcase}, // <- #{f[1]}"
      end
      @f.puts "}"
    end

    for_each_struct do |x|
      name = conv_java_type(x['str_name'])
      @f.puts "class #{name} {"
      x["str_field"].each do |f|
        @f.print "  public #{conv_java_type(f[1])} #{f[0]}"
        if @types[f[1]].is_struct? then
          # Initialize structures so that the candidate can use the object
          # more easily
          @f.print " = new #{conv_java_type(f[1])}()";
        end
        @f.puts "; // #{f[2]}"
      end
      @f.puts "}"
    end

    @f.puts "public class #{@java_interface}", "{"
    build_constants '  '
    for_each_fun(true, 'function', '  ') do |f|
      print_proto("  public static native", f)
      @f.puts ";"
    end
    @f.puts "}"
    @f.close

    #####################################
    ##  Prologin.java file generating  ##
    #####################################
    @f = File.new(@path + (@java_file + '.java'), 'w')
    print_banner "generator_java.rb"
    # generate functions bodies
    @f.puts "public class #{@java_file} extends #{@java_interface}", "{"
    for_each_user_fun(true, '  ') do |f|
      @f.print "  "
      print_proto("public", f)
      @f.puts "",
        "  {",
        "    // Place ton code ici",
        "  }",
        ""
    end
    @f.puts "}"
    @f.close

    #
    # Makefile generation
    #
    target = $conf['conf']['player_lib']
    @f = File.new(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

#{target}-srcs = Interface.java Prologin.java
#{target}-cxxflags = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -ggdb3
#{target}-ldflags = -Wl,-rpath -Wl,$(JAVA_HOME)/jre/lib/amd64/server/ -L$(JAVA_HOME)/jre/lib/amd64/server/ -ljvm

# Evite de toucher a ce qui suit
EOF
@f.print <<-EOF
#{target}-dists = interface.hh
#{target}-srcs += interface.cc
include ../includes/rules.mk
EOF
@f.close
  end

end
