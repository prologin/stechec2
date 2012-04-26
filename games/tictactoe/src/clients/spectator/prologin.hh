///
// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_cxx.rb
//

#ifndef PROLOGIN_HH_
# define PROLOGIN_HH_

# include <vector>

# include <string>

///
// Enumeration containing all possible error types that can be returned by action functions
//
typedef enum error {
  OK, /* <- no error occurred */
  OUT_OF_BOUNDS, /* <- provided position is out of bounds */
  ALREADY_OCCUPIED, /* <- someone already played at the provided position */
  ALREADY_PLAYED, /* <- you already played this turn, you cheater! */
} error;


///
// Returns your team number
//
extern "C" int api_my_team();
static inline int my_team()
{
  return api_my_team();
}


///
// Returns the TicTacToe board
//
extern "C" std::vector<int> api_board();
static inline std::vector<int> board()
{
  return api_board();
}


///
// Play at the given position
//
extern "C" error api_play(int x, int y);
static inline error play(int x, int y)
{
  return api_play(x, y);
}


///
// Cancels the last played action
//
extern "C" bool api_cancel();
static inline bool cancel()
{
  return api_cancel();
}


///
// Affiche le contenu d'une valeur de type error
//
extern "C" void api_afficher_error(error v);
static inline void afficher_error(error v)
{
  api_afficher_error(v);
}



extern "C" {

///
// Function called at the start of the game
//
void init_game();

///
// Called when this is your turn to play
//
void play_turn();

///
// Function called at the end of the game
//
void end_game();

}
#endif
