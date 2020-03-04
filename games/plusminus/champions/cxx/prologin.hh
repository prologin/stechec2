/// This file has been generated, if you wish to
/// modify it in a permanent way, please refer
/// to the script file : gen/generator_cxx.rb

#ifndef PROLOGIN_HH_
#define PROLOGIN_HH_

#include <functional>

#include <string>

#include <vector>

/// All possible error types that can be returned by action functions
typedef enum error
{
    OK,           /* <- no error occurred */
    OUT_OF_BOUDS, /* <- guess is out of bounds */
} error;
// This is needed for old compilers
namespace std
{
template <>
struct hash<error>
{
    size_t operator()(const error& v) const
    {
        return hash<int>()(static_cast<int>(v));
    }
};
} // namespace std

/// Provide your guess
extern "C" error api_guess(int guess);
static inline error guess(int guess)
{
    return api_guess(guess);
}

/// Returns your team number
extern "C" int api_my_team();
static inline int my_team()
{
    return api_my_team();
}

/// Returns -1 if you are below the number, 1 if you are above, 0 if you found
/// it. Returns -2 when no valid guess has been received.
extern "C" int api_result();
static inline int result()
{
    return api_result();
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

// Les fonctions suivantes définissent les opérations de comparaison, d'égalité
// et de hachage sur les structures du sujet.

namespace std
{
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
} // namespace std

extern "C"
{

    /// Function called at the start of the game
    void init_game();

    /// Called when this is your turn to play
    void play_turn();

    /// Function called at the end of the game
    void end_game();
}
#endif
