pub mod api;
mod ffi;
use api::*;

pub fn test_alert() {
    assert!(false);
}

/// Called 10K times to test if things work well.
pub fn test() {
    assert!(CONST_VAL / 4 == 10);
    assert!(CONST_DOUBLE == 42.42);

    send_me_42(42);
    send_me_42_and_1337(42, 1337);
    send_me_true(true);
    send_me_tau(6.2831853);
    send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    assert!(returns_42() == 42);
    assert!(returns_true() == true);
    assert!((returns_tau() - 6.2831).abs() < 0.001);
    assert!(returns_val1() == TestEnum::Val1);

    let r = returns_range(1, 100);
    assert!(r == (1..100).collect::<Vec<_>>());

    let r = returns_range(1, 10000);
    assert!(r == (1..10000).collect::<Vec<_>>());

    let v = returns_sorted(&[1, 3, 2, 4, 5, 7, 6]);
    assert!(v == (1..8).collect::<Vec<_>>());

    let ba = returns_not(&vec![true, false, false, true, false, false, true, false, false]);
    assert!(ba == vec![false, true, true, false, true, true, false, true, true]);

    let bdo = &vec![-0.5, 1.0, 12.5, 42.0];
    let bdi = returns_inverse(bdo);
    for i in 0..bdi.len() {
        assert!(bdi[i] - (1. / bdo[i]) < 0.0001);
    }

    let simple = SimpleStruct {
        field_i: 42,
        field_bool: true,
        field_double: 42.42,
    };
    send_me_simple(&simple);

    let s = StructWithArray {
        field_int: 42,
        field_int_arr: vec![42; 42],
        field_str_arr: vec![
            SimpleStruct {
                field_i: 42,
                field_bool: true,
                field_double: 42.42,
            };
            42
        ],
    };
    send_me_42s(&s);

    send_me_test_enum(TestEnum::Val1, TestEnum::Val2);

    afficher_test_enum(TestEnum::Val2);

    let l = vec![
        StructWithArray {
            field_int: 42,
            field_int_arr: vec![42; 42],
            field_str_arr: vec![
                SimpleStruct {
                    field_i: 42,
                    field_bool: true,
                    field_double: 42.42,
                };
                42
            ],
        };
        42
    ];

    let lr = send_me_struct_array(&l);
    assert!(l == lr);
}
