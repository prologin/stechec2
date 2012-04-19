/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) 2012 Prologin
*/

#include "api.hh"

#include <iostream>
#include <sstream>
#include <vector>

// from api.cc
extern Api* api;

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
std::string convert_to_string(error in){
  switch (in)
  {
    case OK: return "\"ok\"";
    case OUT_OF_BOUNDS: return "\"out_of_bounds\"";
    case ALREADY_OCCUPIED: return "\"already_occupied\"";
    case ALREADY_PLAYED: return "\"already_played\"";
  }
  return "bad value";
}
std::string convert_to_string(std::vector<error> in){
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
// Returns your team number
//
extern "C" int api_my_team()
{
  return api->my_team();
}

///
// Returns the TicTacToe board
//
extern "C" std::vector<int> api_board()
{
  return api->board();
}

///
// Play at the given position
//
extern "C" error api_play(int x, int y)
{
  return api->play(x, y);
}

///
// Cancels the last played action
//
extern "C" bool api_cancel()
{
  return api->cancel();
}

///
// Affiche le contenu d'une valeur de type error
//
std::ostream& operator<<(std::ostream& os, error v)
{
  switch (v) {
  case OK: os << "OK"; break;
  case OUT_OF_BOUNDS: os << "OUT_OF_BOUNDS"; break;
  case ALREADY_OCCUPIED: os << "ALREADY_OCCUPIED"; break;
  case ALREADY_PLAYED: os << "ALREADY_PLAYED"; break;
  }
  return os;
}
extern "C" void api_afficher_error(error v)
{
  std::cerr << v << std::endl;
}

