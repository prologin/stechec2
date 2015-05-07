# coding: utf-8
#
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2005, 2006 Prologin
#

require "pathname"
require "gen/file_generator"

# Methods from the C generator

def hs_get_array_types()
  def add_array_types(store, type)
    if type.is_array?
      add_array_types(store, type.type)
      store = store << [hs_c_typename(type), type.type]
    end
  end

  array_types = []
  @types.each do |t| add_array_types(array_types, t[1]) end
  for_each_struct(false) do |x|
    x['str_field'].each do |f|
      add_array_types(array_types, @types[f[1]])
    end
  end
  for_each_fun(false) do |fn|
    add_array_types(array_types, fn.ret)
    fn.args.each do |arg|
      add_array_types(array_types, arg.type)
    end
  end
  for_each_user_fun(false) do |f|
    add_array_types(array_types, f.ret)
    f.args.each do |arg|
      add_array_types(array_types, arg.type)
    end
  end
  array_types.uniq
end

def hs_cxx_typename(type)
  if type.is_array? then
    "std::vector<#{hs_cxx_typename(type.type)}>"
  elsif type.is_struct?
    "__internal__cxx__#{type.name}"
  elsif type.is_simple? && type.name == "string"
    "std::string"
  else
    type.name
  end
end

def hs_c_typename(type)
  if type.name == "string"
    "char*"
  elsif type.is_array?
    hs_c_typename(type.type) + "_array"
  else
    type.name
  end
end

def hs_c_typename_pointer(type)
  if type.is_array? || type.is_struct?
    hs_c_typename(type) + "*"
  else
    hs_c_typename(type)
  end
end

def hs_c_proto(fn)
  # Returns the prototype of a C function
  args = fn.args.map { |arg| "#{hs_c_typename_pointer(arg.type)} #{arg.name}" }.join(", ")
  "extern \"C\" #{hs_c_typename_pointer(fn.ret)} hs_#{fn.name}(#{(args.empty? ? "void" : args)})"
end

def hs_cxx_internal_proto(fn)
  # Returns the prototype of a CXX function
  args = fn.args.map { |arg| "#{hs_cxx_typename(arg.type)} #{arg.name}" }.join(", ")
  "extern \"C\" #{hs_cxx_typename(fn.ret)} api_#{fn.name}(#{(args.empty? ? "void" : args)})"
end

def hs_get_lang2cxx(type, bptr = false)
  ctype = bptr ? hs_c_typename_pointer(type) : hs_c_typename(type)
  cxxtype = hs_cxx_typename(type)
  if type.is_array? and type.name != "string"
    # We want to remove the "_array" suffix to get the underlying type
    # We also want to remove the "std::vector<...>"
    bptr ? "lang2cxx_array_ptr<#{ctype[0..-8]}, #{ctype[0..-2]}, #{cxxtype[12..-2]} >" : "lang2cxx_array<#{ctype[0..-7]}, #{ctype}, #{cxxtype[12..-2]} >"
  else
    "lang2cxx<#{ctype}, #{cxxtype} >"
  end
end

def hs_get_cxx2lang(type, bptr = false)
  ctype = bptr ? hs_c_typename_pointer(type) : hs_c_typename(type)
  cxxtype = hs_cxx_typename(type)
  if type.is_array? and type.name != "string"
    # We want to remove the "_array" suffix to get the underlying type
    # We also want to remove the "std::vector<...>"
    bptr ? "cxx2lang_array_ptr<#{ctype[0..-8]}, #{ctype[0..-2]}, #{cxxtype[12..-2]} >" : "cxx2lang_array<#{ctype[0..-7]}, #{ctype}, #{cxxtype[12..-2]} >"
  else
    "cxx2lang<#{ctype}, #{cxxtype} >"
  end

end

def hs_generated_filenames
  @haskell_hs_file = $conf['conf']['player_filename'].capitalize + '.hs'
  @haskell_capi_file = "CApi.hsc"
  @haskell_api_file = "Api.hs"
  @header_file = 'interface.hh'
  @header_c_file = 'interface_c.hh'
  @source_file = 'interface.cc'
end

# C generator with some Haskell specificity
class HaskellCFileGenerator < CxxProto

  def initialize
    super
    @lang = "C++ with Haskell extension"
  end

  def generate_header_c
    @f = File.open(@path + @header_c_file, 'w')
    print_banner "generator_haskell.rb"
    print_include "stdbool.h", true
    build_enums

    array_types = hs_get_array_types()

    # Forward declaration in case of interdependencies
    for_each_struct(false) { |s| @f.puts "struct #{s['str_name']};" }
    @f.puts ""

    # Declaration of the array structures.
    array_types.each do |name, type|
      @f.puts "typedef struct #{name} {"
      if type.is_struct?
        # [0..-7] removes the "_array"
        @f.puts "  struct #{name[0..-7]}* datas;"
      else
        @f.puts "  #{name[0..-7]}* datas;"
      end
      @f.puts "  size_t length;", "} #{name};", ""
    end

    build_structs_generic do |field, type|
      type = @types[type]
      if type.is_array? and not type.name == "string"
        "  struct #{hs_c_typename(type)} #{field};"
      else
        "  #{hs_c_typename(type)} #{field};"
      end
    end
  end

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_haskell.rb"
    print_include "vector", true
    print_include "string", true
    print_include "stdlib.h", true
    print_include @header_c_file, false

    for_each_struct false do |s|
      name = s['str_name']
      @f.puts "typedef struct __internal__cxx__#{name} {"
      s['str_field'].each do |f|
        @f.puts "  #{hs_cxx_typename(@types[f[1]])} #{f[0]};"
      end
      @f.puts "} __internal__cxx__#{name};"
    end
    for_each_fun false do |fn|
      @f.print hs_cxx_internal_proto(fn), ";\n"
    end
    for_each_user_fun false do |fn|
      @f.print cxx_proto(fn, "", 'extern "C"'); @f.puts ";"
    end
    @f.close
  end

  def generate_source
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_haskell.rb"
    print_include "HsFFI.h", false

    @f.puts "extern \"C\" {"
    for_each_user_fun false do |fn|
      @f.puts "  void hs_#{fn.name}(void);"
    end
    @f.puts "  extern void __attribute__((weak)) __stginit_#{$conf['conf']['player_filename'].capitalize} ( void );"
    @f.puts "}"
    print_include @header_file
    @f.puts

    @f.puts <<-EOF
extern \"C\" void haskell_init(){
  static bool done = false;
  if(!done){
    hs_init(0, 0);
    hs_add_root(__stginit_#{$conf['conf']['player_filename'].capitalize});
    done = true;
  }
}
EOF

    @f.puts <<-EOF
static std::vector<void*> __internal_need_free;

template<typename Lang, typename Cxx>
Cxx lang2cxx(Lang in)
{
  return in;
}

template<>
std::string lang2cxx<char*, std::string>(char* in)
{
  return in;
}

template<typename Lang, typename Lang_array, typename Cxx>
std::vector<Cxx> lang2cxx_array(Lang_array in)
{
  std::vector<Cxx> out(in.length);
  for (size_t i = 0; i < in.length; ++i)
    out[i] = lang2cxx<Lang, Cxx>(in.datas[i]);
  return out;
}

template<typename Lang, typename Lang_array, typename Cxx>
std::vector<Cxx> lang2cxx_array_ptr(Lang_array* in)
{
  std::vector<Cxx> out(in->length);
  for (size_t i = 0; i < in->length; ++i)
    out[i] = lang2cxx<Lang, Cxx>(in->datas[i]);
  return out;
}

template<typename Lang, typename Cxx>
Lang cxx2lang(Cxx in)
{
  return in;
}

template<>
char* cxx2lang<char*, std::string>(std::string in)
{
  size_t l = in.length();
  char* out = (char *) malloc(l + 1);
  __internal_need_free.push_back(out);
  for (int i = 0; i < l; i++) out[i] = in[i];
  out[l] = 0;
  return out;
}

template<typename Lang, typename Lang_array, typename Cxx>
Lang_array cxx2lang_array(const std::vector<Cxx>& in)
{
  Lang_array out = { NULL, in.size() };
  out.datas = (Lang *)malloc((out.length) * sizeof(Lang));
  __internal_need_free.push_back(out.datas);
  for (int i = 0; i < out.length; ++i)
    out.datas[i] = cxx2lang<Lang, Cxx>(in[i]);
  return out;
}

template<typename Lang, typename Lang_array, typename Cxx>
Lang_array* cxx2lang_array_ptr(const std::vector<Cxx>& in)
{
  static Lang_array out; out = { NULL, in.size() };
  out.datas = (Lang *)malloc((out.length) * sizeof(Lang));
  __internal_need_free.push_back(out.datas);
  for (int i = 0; i < out.length; ++i)
    out.datas[i] = cxx2lang<Lang, Cxx>(in[i]);
  return &out;
}
EOF

    for_each_struct false do |x|
      ctype = x['str_name']
      cxxtype = "__internal__cxx__#{ctype}"

      @f.puts <<-EOF
template<>
#{cxxtype} lang2cxx<#{ctype}, #{cxxtype}>(#{ctype} in)
{
  #{cxxtype} out;
EOF
      x['str_field'].each do |f|
        name = f[0]
        type = @types[f[1]]
        @f.puts "  out.#{name} = #{hs_get_lang2cxx(type)}(in.#{name});"
      end
      @f.puts <<-EOF
  return out;
}

template<>
#{ctype} cxx2lang<#{ctype}, #{cxxtype}>(#{cxxtype} in)
{
  #{ctype} out;
EOF
      x['str_field'].each do |f|
        name = f[0]
        type = @types[f[1]]
        @f.puts "  out.#{name} = #{hs_get_cxx2lang(type)}(in.#{name});"
      end
      @f.puts "  return out;", "}", ""

      @f.puts <<-EOF
template<>
#{cxxtype} lang2cxx<#{ctype}*, #{cxxtype}>(#{ctype}* in)
{
  #{cxxtype} out;
EOF
      x['str_field'].each do |f|
        name = f[0]
        type = @types[f[1]]
        @f.puts "  out.#{name} = #{hs_get_lang2cxx(type)}(in->#{name});"
      end
      @f.puts <<-EOF
  return out;
}

template<>
#{ctype}* cxx2lang<#{ctype}*, #{cxxtype}>(#{cxxtype} in)
{
  static #{ctype} out;
EOF
      x['str_field'].each do |f|
        name = f[0]
        type = @types[f[1]]
        @f.puts "  out.#{name} = #{hs_get_cxx2lang(type)}(in.#{name});"
      end
      @f.puts "  return &out;", "}", ""
    end

    for_each_fun false do |fn|
      @f.puts hs_c_proto(fn)
      buffer = "api_#{fn.name}(#{fn.args.map { |arg| "#{hs_get_lang2cxx(arg.type, true)}(#{arg.name})" }.join(", ")})"
      buffer = "return #{hs_get_cxx2lang(fn.ret, true)}(#{buffer})" if not fn.ret.is_nil?
      @f.puts "{", "  #{buffer};", "}", ""
    end

    for_each_user_fun false do |fn|
      @f.print <<-EOF
extern \"C\" void #{fn.name}(){
  haskell_init();
  hs_#{fn.name}();
  hs_perform_gc();
  for(void* ptr : __internal_need_free){
    free(ptr);
  }
  __internal_need_free.clear();
}
EOF
    end

    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "haskell"
    hs_generated_filenames

    generate_header
    generate_header_c
    generate_source
  end

end

# Haskell files

def haskell_hs_c_typename(type)
  if type.is_array?
    haskell_hs_c_typename(type.type) + "_array"
  elsif type.name == "void"
    "()"
  else
    "C" + type.name.capitalize
  end
end

def haskell_type(type)
  if type.is_array?
    "[" + haskell_type(type.type) + "]"
  elsif type.name == "void"
    "()"
  else
    type.name.capitalize
  end
end

class HaskellFileGenerator < FileGenerator
  def initialize
    super
    @lang = "Haskell"
  end

  def print_comment(str)
    @f.puts '-- ' + str if str
  end

  def print_multiline_doc_comment(str, prestr = '')
    return unless str
    str.each_line {|s| @f.print prestr + '-- | ', s }
    @f.puts
  end

  def print_multiline_comment(str, prestr = '')
    return unless str
    str.each_line {|s| @f.print prestr + '-- ', s }
    @f.puts
  end

  def build_enums
    for_each_enum false do |e|
      print_multiline_doc_comment(e['enum_summary'])
      @f.puts <<-EOF
data #{e['enum_name'].capitalize} =
    #{e['enum_field'].map{ |f| "#{f[0].capitalize} -- ^ #{f[1]} "}.join("\n  | ")}
  deriving(Show, Eq, Enum)
type C#{e['enum_name'].capitalize} = CInt

instance ApiStorable #{e['enum_name'].capitalize} where
  type ApiStorableType #{e['enum_name'].capitalize} = CInt
  toStorable x f = f (fromIntegral (fromEnum x))
  {-# INLINE toStorable #-}
  unStorable = return . toEnum . fromIntegral
  {-# INLINE unStorable #-}
  type ApiStorableBaseType #{e['enum_name'].capitalize} = ApiStorableType #{e['enum_name'].capitalize}
  toStorableBase = toStorable
  {-# INLINE toStorableBase #-}
  unStorableBase = unStorable
  {-# INLINE unStorableBase #-}
EOF
    end
  end

  def build_structs
    for_each_struct false do |s|
      # Haskell Struct
      print_multiline_doc_comment(s['str_summary'])
      @f.print "data #{s['str_name'].capitalize} = #{s['str_name'].capitalize} "
      if s['str_tuple']
        @f.puts s['str_field'].map { |f| haskell_type(@types[f[1]]) }.join(" ")
      else
        @f.puts "{"
        b = false
        s['str_field'].each do |f|
          @f.print "    " if !b
          @f.print "  , " if b
          @f.puts "  #{f[0]} :: #{haskell_type(@types[f[1]])} -- ^ #{f[2]}"
          b = true
        end
        @f.puts "}"
      end
      @f.puts "  deriving(Show, Eq)"
      # C Struct
      @f.print "data C#{s['str_name'].capitalize} = C#{s['str_name'].capitalize} "
      if s['str_tuple']
        @f.puts s['str_field'].map { |f| haskell_hs_c_typename(@types[f[1]]) }.join(" ")
      else
        @f.puts "{"
        b = false
        s['str_field'].each do |f|
          @f.print "    " if !b
          @f.print "  , " if b
          @f.puts "  c#{f[0]} :: #{haskell_hs_c_typename(@types[f[1]]) } -- ^ #{f[2]}"
          b = true
        end
        @f.puts "}"
      end

      # Storable instance
      @f.puts <<-EOF
instance Storable C#{s['str_name'].capitalize} where
  sizeOf    _ = (#size #{s['str_name']})
  {-# INLINE sizeOf #-}
  alignment _ = alignment (undefined :: CInt)
  {-# INLINE alignment #-}
  peek ptr = do
    #{s['str_field'].map { |f| "a#{f[0]} <- (#peek #{s['str_name']}, #{f[0]}) ptr" }.join("\n    ")}
    return $ C#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}" }.join(" ")}
  {-# INLINE peek #-}
  poke ptr (C#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}" }.join(" ")}) = do
    #{s['str_field'].map { |f| "(#poke #{s['str_name']}, #{f[0]}) ptr a#{f[0]}" }.join("\n    ")}
  {-# INLINE poke #-}
instance ApiStorable #{s['str_name'].capitalize} where
  type ApiStorableType #{s['str_name'].capitalize} = Ptr C#{s['str_name'].capitalize}
  toStorable (#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}" }.join(" ")}) f = #{s['str_field'].map{ |f| "toStorableBase a#{f[0]} $ \\a#{f[0]}' -> " }.join(" ")} do
    alloca $ \\ptr -> do
      poke ptr $ C#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}'" }.join(" ")}
      f ptr
  {-# INLINE toStorable #-}
  unStorable ptr = do
    (C#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}'" }.join(" ")}) <- peek ptr
    #{s['str_field'].map { |f| "a#{f[0]} <- unStorableBase a#{f[0]}'" }.join("\n    ")}
    return $ #{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}" }.join(" ")}
  {-# INLINE unStorable #-}
  type ApiStorableBaseType #{s['str_name'].capitalize} = C#{s['str_name'].capitalize}
  toStorableBase (#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}" }.join(" ")}) f =
    #{s['str_field'].map{ |f| "toStorableBase a#{f[0]} $ \\a#{f[0]}' -> " }.join(" ")} f (C#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}'" }.join(" ")})
  {-# INLINE toStorableBase #-}
  unStorableBase (C#{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}'" }.join(" ")}) = do
    #{s['str_field'].map { |f| "a#{f[0]} <- unStorableBase a#{f[0]}'" }.join("\n    ")}
    return $ #{s['str_name'].capitalize} #{s['str_field'].map { |f| "a#{f[0]}" }.join(" ")}
  {-# INLINE unStorableBase #-}
EOF
    end
  end

  def print_foreign_import(fn)
    @f.puts <<-EOF
#{fn.name} :: #{fn.args.map{ |arg| "#{haskell_type(arg.type)} -> " }.join(" ")}IO #{haskell_type(fn.ret)}
#{fn.name} #{fn.args.map{ |arg| arg.name }.join(" ")} = #{fn.args.map{ |arg| "toStorable #{arg.name} $ \\#{arg.name}' -> " }.join(" ")} (hs_#{fn.name} #{fn.args.map{ |arg| "#{arg.name}'" }.join(" ")}) >>= unStorable

foreign import ccall
  hs_#{fn.name} :: #{fn.args.map { |arg| "(ApiStorableType #{haskell_type(arg.type)}) -> " }.join} IO (ApiStorableType #{haskell_type(fn.ret)})
EOF
  end

  def build_arrays
    array_types = hs_get_array_types()
    array_types.each do |name, type|
      @f.print <<-EOF
data C#{name.capitalize} = C#{name.capitalize} { c#{name.capitalize}Ptr :: Ptr #{haskell_hs_c_typename(type)}, c#{name.capitalize}Size :: CSize }

instance Storable C#{name.capitalize} where
  sizeOf    _ = (#size #{name})
  {-# INLINE sizeOf #-}
  alignment _ = alignment (undefined :: CInt)
  {-# INLINE alignment #-}
  peek ptr = do
    length <- (#peek #{name}, length) ptr
    datas <- (#peek #{name}, datas) ptr
    return $ C#{name.capitalize} datas length
  {-# INLINE peek #-}
  poke ptr (C#{name.capitalize} datas length) = do
    (#poke #{name}, length) ptr length
    (#poke #{name}, datas) ptr datas
  {-# INLINE poke #-}

instance ApiStorable [#{haskell_type(type)}] where
  type ApiStorableType [#{haskell_type(type)}] = Ptr C#{name.capitalize}
  toStorable xs f = toStorableBase xs $ \\a -> alloca $ \\ptr -> do
    poke ptr a
    f ptr
  {-# INLINE toStorable #-}
  unStorable xa = do
    (C#{name.capitalize} xa' xl) <- peek xa
    xs <- peekArray (fromIntegral xl) xa'
    mapM unStorableBase xs
  {-# INLINE unStorable #-}
  type ApiStorableBaseType [#{haskell_type(type)}] = C#{name.capitalize}

  toStorableBase xs f = do
    let xl = length xs
    allocaArray xl $ \\xa ->
      foldl'
        (\\acc (i, c) -> toStorableBase c $ \\c' -> do
          poke (advancePtr xa i) c'
          acc
        )
        (f $ C#{name.capitalize} xa (fromIntegral xl))
        (zip [0..] xs)
  {-# INLINE toStorableBase #-}
  unStorableBase (C#{name.capitalize} xa xl) = do
    xs <- peekArray (fromIntegral xl) xa
    mapM unStorableBase xs
  {-# INLINE unStorableBase #-}
EOF
    end
  end

  def build_simple(name, conv_fn_to, conv_fn_from)
    @f.print <<-EOF
instance ApiStorable #{name.capitalize} where
  type ApiStorableType #{name.capitalize} = C#{name.capitalize}
  toStorable x f = f (#{conv_fn_to} x)
  {-# INLINE toStorable #-}
  unStorable = return . #{conv_fn_from}
  {-# INLINE unStorable #-}
  type ApiStorableBaseType #{name.capitalize} = ApiStorableType #{name.capitalize}
  toStorableBase = toStorable
  {-# INLINE toStorableBase #-}
  unStorableBase = unStorable
  {-# INLINE unStorableBase #-}
EOF
  end

  def build_export_list
    export_list = []
    for_each_enum false do |e|
      export_list << e['enum_name'].capitalize + "(..)"
    end
    for_each_struct false do |s|
      export_list << s['str_name'].capitalize + "(..)"
    end
    for_each_fun false do |fn|
      export_list << fn.name
    end
    for_each_constant false do |c|
      export_list << c['cst_name'].downcase
    end
    " " + export_list.join("\n           , ") + "\n           "
  end

  def generate_makefile
    target = $conf['conf']['player_lib']
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers sources, headers, ou changer
# des flags de compilation.
#{target}-srcs = #{@haskell_hs_file}
#{target}-dists = #{@haskell_hs_file}

# Évite de toucher à ce qui suit
#{target}-dists += #{@haskell_capi_file} #{@haskell_api_file} #{@header_file} #{@header_c_file} #{@source_file}
#{target}-srcs += #{@haskell_capi_file} #{@haskell_api_file} #{@header_file} #{@header_c_file} #{@source_file}
#{target}-hsc-srcs = $(filter %.hsc,$(#{target}-srcs))
#{target}-hs-srcs = $(filter %.hs,$(#{target}-srcs)) $(#{target}-hsc-src:.hsc=.hs)

doc: $(#{target}-hs-srcs)
	haddock -o doc -t Prologin --package-name=Prologin -html --pretty-html $(#{target}-hs-srcs)

STECHEC_LANG = haskell
include ../includes/rules.mk
    EOF
    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "haskell"
    hs_generated_filenames

    generate_makefile

    # Build C interface files
    HaskellCFileGenerator.new.build

    #
    # Generate the api hs file.
    #
    @f = File.open(@path + @haskell_api_file, 'w')
    print_banner "generator_haskell.rb"
    @f.puts "module Api (#{build_export_list}) where", ""
    @f.puts "import CApi", ""
    for_each_constant false do |x|
      print_multiline_doc_comment x['cst_comment']
      @f.puts "#{x['cst_name'].downcase}  :: Int"
      @f.puts "#{x['cst_name'].downcase} = #{x['cst_val']}"
    end
    @f.close

    #
    # Generate the api hs file.
    #
    @f = File.open(@path + @haskell_capi_file, 'w')
    @f.puts "{-# LANGUAGE ForeignFunctionInterface #-}"
    @f.puts "{-# LANGUAGE TypeFamilies #-}"
    @f.puts "{-# LANGUAGE FlexibleInstances #-}"
    @f.puts "{-# LANGUAGE UndecidableInstances #-}", ""
    @f.puts "{-# OPTIONS_HADDOCK hide #-}", ""
    print_banner "generator_haskell.rb"
    @f.puts "module CApi where", ""
    @f.puts "import Data.List (foldl')"
    @f.puts "import Foreign"
    @f.puts "import Foreign.C.Types", ""
    @f.puts "import Foreign.Marshal.Array", ""

    @f.puts "#include \"interface_c.hh\""

    @f.puts <<-EOF
type CBool = CChar

class ApiStorable a where
  type ApiStorableType a
  toStorable :: a -> (ApiStorableType a -> IO b) -> IO b
  unStorable :: ApiStorableType a -> IO a
  type ApiStorableBaseType a
  toStorableBase :: a -> (ApiStorableBaseType a -> IO b) -> IO b
  unStorableBase :: ApiStorableBaseType a -> IO a

instance ApiStorable () where
  type ApiStorableType () = ()
  toStorable () f = f ()
  {-# INLINE toStorable #-}
  unStorable _ = return ()
  {-# INLINE unStorable #-}
  type ApiStorableBaseType () = ()
  toStorableBase = toStorable
  {-# INLINE toStorableBase #-}
  unStorableBase = unStorable
  {-# INLINE unStorableBase #-}
EOF

    build_simple("int", "fromIntegral", "fromIntegral")
    build_simple("bool", "(\\bx -> if bx then 1 else 0)", "(/= 0)")

    # protos
    build_enums
    build_structs
    build_arrays
    for_each_fun false do |fn|
      print_multiline_doc_comment fn.summary
      print_foreign_import fn
    end

    @f.close

    #
    # Generate hs source file
    #
    @f = File.open(@path + @haskell_hs_file, 'w')
    @f.puts "{-# LANGUAGE ForeignFunctionInterface #-}"
    @f.puts
    print_banner "generator_haskell.rb"
    @f.puts "module #{$conf['conf']['player_filename'].capitalize} where", ""

    @f.puts "import Api", ""

    @f.puts
    for_each_user_fun false do |fn|
      print_multiline_doc_comment fn.summary
      @f.puts "#{fn.name.downcase} :: IO ()"
      @f.puts "#{fn.name.downcase} = return () -- A completer", ""
    end
    @f.puts
    for_each_user_fun false do |fn|
      @f.puts "hs_#{fn.name.downcase} = #{fn.name.downcase}"
      @f.puts "foreign export ccall hs_#{fn.name.downcase} :: IO ()"
    end

    @f.close
  end
end
