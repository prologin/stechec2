============
Introduction
============

Stechec2 is client-server turn-based strategy game match maker used during the
French national computing contest: `Prologin <https://prologin.org/>`_.

Each year, we create a new game for the final and candidates are asked to
develop an AI in 36 hours for this specific game. The final ranking will be
determined at the end with a tournament where each AIs compete against each
other.

Hence, stechec2 must be very flexible to accommodate with different games, and
compatible with multiple programming languages such as: C, C++, Python, Java,
Ocaml, Haskell, Rust, etc. We also need to isolate matches to share resources
amongst untrusted code execution, and distribute them evenly over our own
infrastructure.
