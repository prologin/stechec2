(*
** This file has been generated, if you wish to
** modify it in a permanent way, please refer
** to the script file : gen/generator_caml.rb
*)

open Api

let range m n = Array.init (n - m) (fun i -> m + i)

let times42 x = Array.make 42 x

let test_alert () = assert (false)

(*
** Called 10K times to test if things work well.
*)
let test () =  (* Pose ton code ici *)
  begin
    (* Check the types of constants. *)
    ignore (const_val : int);
    ignore (const_double : float);

    send_me_42 42;
    send_me_42_and_1337 42 1337;
    send_me_true true;
    send_me_tau 6.2831853;
    send_me_joseph_marchand "Joseph Marchand";
    send_me_13_ints 1 2 3 4 5 6 7 8 9 10 11 12 13;
    assert ((returns_42 ()) = 42);
    assert ((returns_true ()) = true);
    assert (abs_float (returns_tau () -. 6.2831853) < 0.0001);
    assert ((returns_joseph_marchand ()) = "Joseph Marchand");
    assert ((returns_val1 ()) = Val1);
    assert ((returns_range 1 100) = (range 1 100));
    assert ((returns_range 1 10000) = (range 1 10000));
    assert ((returns_sorted [|1;3;2;4;5;7;6|]) = [|1;2;3;4;5;6;7|]);
    assert ((returns_sorted (range 1 10000)) = (range 1 10000));
    assert ((returns_not [|true;false;false;true;false;false;true;false;false|]) =
        [|false;true;true;false;true;true;false;true;true|]);
    assert ((returns_inverse [|-0.5;1.0;12.5;42.0|]) = [|-2.0;1.0;0.08;0.023809523809523808|]);
    assert ((returns_reversed_enums [|Val1; Val2; Val2|]) = [|Val2; Val2; Val1|]);
    assert ((returns_upper [|"Alea"; "Jacta"; "Est"|])
            = [|"ALEA"; "JACTA"; "EST"|]);
    let simple = { field_i = 42;
                   field_bool = true;
                   field_double = 42.42;
                   field_string = "TTY" } in
    send_me_simple simple;
    send_me_42s { field_int = 42;
                  field_int_arr = times42 42;
                  field_str_arr = times42 simple };
    send_me_double_struct { field_one = 42.42; field_two = 42.42 };
    send_me_test_enum Val1 Val2;
    afficher_test_enum Val2;
    let l = send_me_struct_array (
        times42 { field_int = 42;
                  field_int_arr = times42 42;
                  field_str_arr = times42 simple }
    ) in
    assert (l = (times42 { field_int = 42;
                           field_int_arr = times42 42;
                           field_str_arr = times42 simple }));
    flush stderr; flush stdout (* Pour que vos sorties s'affichent *)
  end

(* /!\ Ne touche pas a ce qui suit /!\ *)
let _ =
  Callback.register "ml_test" test
