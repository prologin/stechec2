<?php

require('api.php');

function test_alert()
{
    assert("false");
}

/*
 * Called 10K times to test if things work well.
 */
function test()
{
    send_me_42(42);
    send_me_42_and_1337(42, 1337);
    send_me_true(true);
    send_me_tau(6.2831853);
    send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

    assert(returns_42() === 42);
    assert(returns_true() === true);
    assert(abs(returns_tau() - 6.2831853) < 0.0001);
    assert(returns_val1() === VAL1);
    assert(returns_range(1, 100) === range(1, 99));
    assert(returns_range(1, 10000) === range(1, 9999));
    assert(returns_sorted(array(1, 3, 2, 4, 5, 7, 6)) === range(1, 7));
    assert(returns_sorted(range(1, 1000)) === range(1, 1000));
    assert(returns_not(array(true, false, false, true, false, false, true, false, false)) ===
        array(false, true, true, false, true, true, false, true, true));

    $bdo = array(-0.5, 1.0, 12.5, 42.0);
    $bdi = returns_inverse($bdo);
    for ($i = 0; $i < 4; $i++)
        assert(bdi[i] - (1 / bdo[i]) < 0.0001);

    send_me_test_enum(VAL1, VAL2);
    afficher_test_enum(VAL2);

    $struct1 = array("field_i" => 42, "field_bool" => true, "field_double" => 42.42);
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

    assert($l1 == $l2);
}
