/// This file has been generated, if you wish to
/// modify it in a permanent way, please refer
/// to the script file : gen/generator_cxx.rb

#ifndef PROLOGIN_HH_
# define PROLOGIN_HH_

# include <functional>

# include <string>

# include <vector>

/// All possible error types that can be returned by action functions
typedef enum error {
  OK, /* <- no error occurred */
  OUT_OF_BOUNDS, /* <- provided position is out of bounds */
  ALREADY_OCCUPIED, /* <- someone already played at the provided position */
  ALREADY_PLAYED, /* <- you already played this turn, you cheater! */
} error;
// This is needed for old compilers
namespace std
{
  template <> struct hash<error> {
    size_t operator()(const error& v) const {
      return hash<int>()(static_cast<int>(v));
    }
  };
}

/// Position on the TicTacToe board
typedef struct position {
  int x;  /* <- X coordinate */
  int y;  /* <- Y coordinate */
} position;


/// Play at the given position
extern "C" error api_play(position pos);
static inline error play(position pos)
{
  return api_play(pos);
}


/// Returns your team number
extern "C" int api_my_team();
static inline int my_team()
{
  return api_my_team();
}


/// Returns the TicTacToe board
extern "C" std::vector<int> api_board();
static inline std::vector<int> board()
{
  return api_board();
}


/// Cancels the last played action
extern "C" bool api_cancel();
static inline bool cancel()
{
  return api_cancel();
}


/// Affiche le contenu d'une valeur de type error
extern "C" void api_afficher_error(error v);
static inline void afficher_error(error v)
{
  api_afficher_error(v);
}


/// Affiche le contenu d'une valeur de type position
extern "C" void api_afficher_position(position v);
static inline void afficher_position(position v)
{
  api_afficher_position(v);
}


// Les fonctions suivantes définissent les opérations de comparaison, d'égalité
// et de hachage sur les structures du sujet.

namespace std {
  template <typename T>
  struct hash<std::vector<T>>
  {
    std::size_t operator()(const std::vector<T>& v) const
    {
      std::size_t res = v.size();
      for (const auto& e : v)
        res ^= std::hash<T>()(e) + 0x9e3779b9 + (res << 6) + (res >> 2);
      return res;
    }
  };
}

inline bool operator==(const position& a, const position& b) {
  if (a.x != b.x) return false;
  if (a.y != b.y) return false;
  return true;
}

inline bool operator!=(const position& a, const position& b) {
  if (a.x != b.x) return true;
  if (a.y != b.y) return true;
  return false;
}

inline bool operator<(const position& a, const position& b) {
  if (a.x < b.x) return true;
  if (a.x > b.x) return false;
  if (a.y < b.y) return true;
  if (a.y > b.y) return false;
  return false;
}

inline bool operator>(const position& a, const position& b) {
  if (a.x > b.x) return true;
  if (a.x < b.x) return false;
  if (a.y > b.y) return true;
  if (a.y < b.y) return false;
  return false;
}

namespace std {
  template <>
  struct hash<position>
  {
    std::size_t operator()(const position& s) const
    {
      std::size_t res = 0;
      res ^= 0x9e3779b9 + (res << 6) + (res >> 2) + std::hash<int>()(s.x);
      res ^= 0x9e3779b9 + (res << 6) + (res >> 2) + std::hash<int>()(s.y);
      return res;
    }
  };
}


extern "C" {

/// Function called at the start of the game
void init_game();

/// Called when this is your turn to play
void play_turn();

/// Function called at the end of the game
void end_game();

}
#endif
