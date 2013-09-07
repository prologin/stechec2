<?php
/*
 * This file has been generated, if you wish to
 * modify it in a permanent way, please refer
 * to the script file : gen/generator_php.rb
 */

require('api.php');

/*
 * Called 10K times to test if things work well.
 */
function test()
{
    send_me_42(42);
    send_me_42_and_1337(42, 1337);
    send_me_true(true);

    assert("returns_42() === 42");
    assert("returns_true() === true");
    assert("returns_range(1, 100) === range(1, 99)");
    assert("returns_range(1, 10000) === range(1, 9999)");
    assert("returns_sorted(array(1, 3, 2, 4, 5, 7, 6)) === range(1, 7)");
    assert("returns_sorted(range(1, 1000)) === range(1, 1000)");

    send_me_test_enum(VAL1, VAL2);

    $struct1 = array("field_i" => 42, "field_bool" => true);
    send_me_simple($struct1);
    send_me_42s(array(
        "field_int" => 42,
        "field_int_arr" => array_fill(0, 42, 42),
        "field_str_arr" => array_fill(0, 42, $struct1)
    ));
    $l1 = send_me_struct_array(array_fill(0, 42, array(
        "field_int" => 42,
        "field_int_arr" => array_fill(0, 42, 42),
        "field_str_arr" => array_fill(0, 42, $struct1)
    )));

    $l2 = array_fill(0, 42, array(
        "field_int" => 42,
        "field_int_arr" => array_fill(0, 42, 42),
        "field_str_arr" => array_fill(0, 42, $struct1)
    ));

    assert('$l1 == $l2');
}

?>