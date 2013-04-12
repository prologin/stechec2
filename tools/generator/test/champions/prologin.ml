(*
** This file has been generated, if you wish to
** modify it in a permanent way, please refer
** to the script file : gen/generator_caml.rb
*)

open Api;;

let range m n = Array.init (n - m) (fun i -> m + i)

let times42 x = Array.make 42 x

(*
** Called 10K times to test if things work well.
*)
let test () =  (* Pose ton code ici *)
    send_me_42 42;
    send_me_42_and_1337 42 1337;
    send_me_true true;
    assert ((returns_42 ()) = 42);
    assert ((returns_true ()) = true);
    assert ((returns_range 1 100) = (range 1 100));
    assert ((returns_range 1 10000) = (range 1 10000));
    assert ((returns_sorted [|1;3;2;4;5;7;6|]) = [|1;2;3;4;5;6;7|]);
    assert ((returns_sorted (range 1 10000)) = (range 1 10000));
    send_me_42s { field_int = 42;
                  field_int_arr = times42 42;
                  field_str_arr = times42 { field_i = 42; field_bool = true } };
    send_me_test_enum Val1 Val2;
    let l = send_me_struct_array (
        times42 { field_int = 42;
                  field_int_arr = times42 42;
                  field_str_arr = times42 { field_i = 42; field_bool = true } }
    ) in
    assert (l = (times42 { field_int = 42;
                           field_int_arr = times42 42;
                           field_str_arr = times42 { field_i = 42; field_bool = true } }));
    flush stderr; flush stdout;; (* Pour que vos sorties s'affichent *)

(* /!\ Ne touche pas a ce qui suit /!\ *)
Callback.register "ml_test" test;;
