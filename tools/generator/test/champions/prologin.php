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
}

?>
