#![allow(unused)]
pub mod api;
mod ffi;

use api::*;
use std::os::raw::{c_double, c_int, c_void};

/// Called 10K times to test if things work well.
fn test()
{
    assert!(CONST_VAL/4 == 10);
    assert!(CONST_DOUBLE_2/2. == 668.5);

    send_me_42(&42);
    send_me_42_and_1337(&42, &1337);
    send_me_true(&true);
    send_me_tau(&6.2831853);
    assert!(returns_42() == 42);
    assert!(returns_true() == true);
    assert!((returns_tau() - 6.2831).abs() < 0.001);

    let r = returns_range(&1, &100);
    assert!(r == (1..100).collect::<Vec<c_int>>());

    let r = returns_range(&1, &10000);
    assert!(r == (1..10000).collect::<Vec<c_int>>());

    let v = returns_sorted(&vec![1, 3, 2, 4, 5, 7, 6]);
    assert!(v == (1..8).collect::<Vec<c_int>>());

    let s = StructWithArray {
        field_int: 42,
        field_int_arr: vec![42; 42],
        field_str_arr: vec![SimpleStruct {
            field_i: 42,
            field_bool: true,
            field_double: 42.42,
        }; 42],
    };
    send_me_42s(&s);

    send_me_test_enum(&TestEnum::Val1, &TestEnum::Val2);

    let l = vec![StructWithArray {
            field_int: 42,
            field_int_arr: vec![42; 42],
            field_str_arr: vec![SimpleStruct {
                field_i: 42,
                field_bool: true,
                field_double: 42.42,
            }; 42],
        }; 42];

    let lr = send_me_struct_array(&l);
    assert!(l == lr);
}
