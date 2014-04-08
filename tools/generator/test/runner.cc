#include "cxx/prologin.hh"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>

extern "C" {

void api_send_me_42(int n)
{
    assert(n == 42);
}

void api_send_me_42_and_1337(int n1, int n2)
{
    assert(n1 == 42);
    assert(n2 == 1337);
}

void api_send_me_true(bool b)
{
    assert(b);
}

int api_returns_42()
{
    return 42;
}

bool api_returns_true()
{
    return true;
}

std::vector<int> api_returns_range(int m, int n)
{
    std::vector<int> v(n - m);
    for (int i = m; i < n; ++i)
        v[i - m] = i;
    return v;
}

std::vector<int> api_returns_sorted(std::vector<int> l)
{
    std::sort(l.begin(), l.end());
    return l;
}

void api_send_me_simple(simple_struct s)
{
    assert(s.field_i == 42);
    assert(s.field_bool);
}

void api_send_me_42s(struct_with_array s)
{
    assert(s.field_int == 42);
    assert(s.field_int_arr.size() == 42);
    for (auto i : s.field_int_arr)
        assert(i == 42);
    assert(s.field_str_arr.size() == 42);
    for (auto& s2 : s.field_str_arr)
    {
        assert(s2.field_i == 42);
        assert(s2.field_bool);
    }
}

void api_send_me_test_enum(test_enum v1, test_enum v2)
{
    assert(v1 == VAL1);
    assert(v2 == VAL2);
}

std::vector<struct_with_array> api_send_me_struct_array(std::vector<struct_with_array> l)
{
    assert(l.size() == 42);
    for (auto& s : l)
        api_send_me_42s(s);
    return l;
}

}

// Printers
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr)
{
  os << "[";
  typename std::vector<T>::const_iterator it;
  for (it = arr.begin(); it != arr.end(); ++it)
  {
    if (it != arr.begin())
      os << ", ";
    os << *it;
  }
  os << "]";
  return os;
}


// todo avoir un ostringstream a la place de std::string

std::string convert_to_string(int i){
  std::ostringstream s;
  s << i;
  std::string result = s.str();
  return result;
}
std::string convert_to_string(std::string i){
  return i;
}
std::string convert_to_string(bool i){
  return i?"true":"false";
}
std::string convert_to_string(std::vector<int> in){
  if (in.size()){
    std::string s = "[" + convert_to_string(in[0]);
    for (int i = 1, l = in.size(); i < l; i++){
      s = s + ", " + convert_to_string(in[i]);
    }
    return s + "]";
  }else{
    return "[]";
  }
}
std::string convert_to_string(test_enum in){
  switch (in)
  {
    case VAL1: return "\"val1\"";
    case VAL2: return "\"val2\"";
  }
  return "bad value";
}
std::string convert_to_string(std::vector<test_enum> in){
  if (in.size()){
    std::string s = "[" + convert_to_string(in[0]);
    for (int i = 1, l = in.size(); i < l; i++){
      s = s + ", " + convert_to_string(in[i]);
    }
    return s + "]";
  }else{
    return "[]";
  }
}
std::string convert_to_string(simple_struct in){
  std::string field_i = convert_to_string(in.field_i);
  std::string field_bool = convert_to_string(in.field_bool);
  std::string out = "{";
  out += "field_i:" + field_i;
  out += ", ";
  out += "field_bool:" + field_bool;
  return out + "}";
}

std::string convert_to_string(std::vector<simple_struct> in){
  if (in.size()){
    std::string s = "[" + convert_to_string(in[0]);
    for (int i = 1, l = in.size(); i < l; i++){
      s = s + ", " + convert_to_string(in[i]);
    }
    return s + "]";
  }else{
    return "[]";
  }
}
std::string convert_to_string(struct_with_array in){
  std::string field_int = convert_to_string(in.field_int);
  std::string field_int_arr = convert_to_string(in.field_int_arr);
  std::string field_str_arr = convert_to_string(in.field_str_arr);
  std::string out = "{";
  out += "field_int:" + field_int;
  out += ", ";
  out += "field_int_arr:" + field_int_arr;
  out += ", ";
  out += "field_str_arr:" + field_str_arr;
  return out + "}";
}

std::string convert_to_string(std::vector<struct_with_array> in){
  if (in.size()){
    std::string s = "[" + convert_to_string(in[0]);
    for (int i = 1, l = in.size(); i < l; i++){
      s = s + ", " + convert_to_string(in[i]);
    }
    return s + "]";
  }else{
    return "[]";
  }
}

///
// Affiche le contenu d'une valeur de type test_enum
//
std::ostream& operator<<(std::ostream& os, test_enum v)
{
  switch (v) {
  case VAL1: os << "VAL1"; break;
  case VAL2: os << "VAL2"; break;
  }
  return os;
}
extern "C" void api_afficher_test_enum(test_enum v)
{
  std::cerr << v << std::endl;
}

///
// Affiche le contenu d'une valeur de type simple_struct
//
std::ostream& operator<<(std::ostream& os, simple_struct v)
{
  os << "{ ";
  os << "field_i" << "=" << v.field_i;
  os << ", ";
  os << "field_bool" << "=" << v.field_bool;
  os << " }";
  return os;
}
extern "C" void api_afficher_simple_struct(simple_struct v)
{
  std::cerr << v << std::endl;
}

///
// Affiche le contenu d'une valeur de type struct_with_array
//
std::ostream& operator<<(std::ostream& os, struct_with_array v)
{
  os << "{ ";
  os << "field_int" << "=" << v.field_int;
  os << ", ";
  os << "field_int_arr" << "=" << v.field_int_arr;
  os << ", ";
  os << "field_str_arr" << "=" << v.field_str_arr;
  os << " }";
  return os;
}
extern "C" void api_afficher_struct_with_array(struct_with_array v)
{
  std::cerr << v << std::endl;
}

int main(int argc, char *argv[])
{
    int count = 10000;
    if (argc > 1)
        count = std::strtol(argv[1], NULL, 0);
    for (int i = 0; i < count; ++i)
        test();
}
