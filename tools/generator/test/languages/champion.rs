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
    send_me_joseph_marchand("Joseph Marchand");
    send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    assert_eq!(returns_42(), 42);
    assert_eq!(returns_true(), true);
    assert_eq!(returns_joseph_marchand(), "Joseph Marchand");
    assert_eq!(returns_val1(), TestEnum::Val1);
    assert!((returns_tau() - 6.2831).abs() < 0.001);

    let r = returns_range(1, 100);
    assert_eq!(r, (1..100).collect::<Vec<_>>());

    let r = returns_range(1, 10000);
    assert_eq!(r, (1..10000).collect::<Vec<_>>());

    let v = returns_sorted(&[1, 3, 2, 4, 5, 7, 6]);
    assert_eq!(v, (1..8).collect::<Vec<_>>());

    let ba = returns_not(&vec![true, false, false, true, false, false, true, false, false]);
    assert_eq!(ba, vec![false, true, true, false, true, true, false, true, true]);

    let bdo = &vec![-0.5, 1.0, 12.5, 42.0];
    let bdi = returns_inverse(bdo);
    for i in 0..bdi.len() {
        assert!(bdi[i] - (1. / bdo[i]) < 0.0001);
    }

    let ea = returns_reversed_enums(&vec![TestEnum::Val1, TestEnum::Val2, TestEnum::Val2]);
    assert_eq!(ea, vec![TestEnum::Val2, TestEnum::Val2, TestEnum::Val1]);

    let sa = ["Alea", "Jacta", "Est"];
    let sau = returns_upper(&sa);
    assert_eq!(sau, vec!["ALEA", "JACTA", "EST"]);

    let sa: Vec<_> = sa.iter().map(|s| s.to_string()).collect();
    let sau = returns_upper(&sa);
    assert_eq!(sau, vec!["ALEA", "JACTA", "EST"]);

    let simple = SimpleStruct {
        field_i: 42,
        field_bool: true,
        field_double: 42.42,
        field_string: String::from("TTY"),
    };
    send_me_simple(&simple);

    let s = StructWithArray {
        field_int: 42,
        field_int_arr: vec![42; 42],
        field_str_arr: vec![simple.clone(); 42],
        field_tup_arr: vec![(42, true); 42],
    };
    send_me_42s(&s);

    let float_struct = StructWithOnlyDouble {
        field_one: 42.42,
        field_two: 42.42,
    };
    send_me_double_struct(&float_struct);

    send_me_tuple_struct((42, true));

    send_me_test_enum(TestEnum::Val1, TestEnum::Val2);

    afficher_test_enum(TestEnum::Val2);

    let s_with_struct = StructWithStruct {
        field_integer: 42,
        field_struct: simple.clone(),
        field_tuple: (42, true),
    };
    send_me_struct_with_struct(&s_with_struct);

    send_me_tuple_with_struct((42, &simple, (42, true)));

    let l = vec![
        StructWithArray {
            field_int: 42,
            field_int_arr: vec![42; 42],
            field_str_arr: vec![simple.clone(); 42],
            field_tup_arr: vec![(42, true); 42],
        };
        42
    ];
    let lr = send_me_struct_array(&l);
    assert_eq!(l, lr);

    let l_with_refs: Vec<&StructWithArray> = l.iter().collect();
    let lr = send_me_struct_array(&l_with_refs);
    assert_eq!(l, lr);
}
