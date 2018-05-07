#![allow(bad_style, unused)]
pub mod api;
use api::*;
use std::os::raw::{c_double, c_int, c_void};
use std::slice::from_raw_parts;

/// Called 10K times to test if things work well.
unsafe fn test()
{
    assert!(CONST_VAL/4 == 10);
    assert!(CONST_DOUBLE_2/2. == 668.5);

    send_me_42(42);
    send_me_42_and_1337(42, 1337);
    send_me_true(true);
    send_me_tau(6.2831853);
    assert!(returns_42() == 42);
    assert!(returns_true() == true);
    assert!((returns_tau() - 6.2831).abs() < 0.001);

    let r = returns_range(1, 100);
    {
        let rs = from_raw_parts(r.ptr, r.len);
        for i in 1..100 {
            assert!(rs[i - 1] == i as i32);
        }
    }
    r.free();

    let r = returns_range(1, 10000);
    {
        let rs = from_raw_parts(r.ptr, r.len);
        for i in 1..10000 {
            assert!(rs[i - 1] == i as i32);
        }
    }
    r.free();

    let vr = vec![1, 3, 2, 4, 5, 7, 6];
    let v = Array {
        ptr: vr.as_ptr(),
        len: vr.len(),
    };
    let v = returns_sorted(v);
    {
        let vs = from_raw_parts(v.ptr, v.len);
        for i in 0..7 {
            assert!(vs[i] == (i + 1) as i32);
        }
    }
    v.free();

    {
        let sr = vec![42; 42];
        let ss = vec![SimpleStruct {
            field_i: 42,
            field_bool: true,
            field_double: 42.42,
        }; 42];
        let s = StructWithArray {
            field_int: 42,
            field_int_arr: Array {
                ptr: sr.as_ptr(),
                len: sr.len(),
            },
            field_str_arr: Array {
                ptr: ss.as_ptr(),
                len: ss.len(),
            },
        };
        send_me_42s(s);
    }

    send_me_test_enum(TestEnum::Val1, TestEnum::Val2);

    let vi = (0..42).map(|_| vec![42; 42]).collect::<Vec<_>>();
    let vs = (0..42).map(|_| {
        (0..42).map(|_| SimpleStruct {
            field_i: 42,
            field_bool: true,
            field_double: 42.42,
        }).collect::<Vec<_>>()
    }).collect::<Vec<_>>();

    let l = (0..42).map(|i| {
        StructWithArray {
            field_int: 42,
            field_int_arr: Array {
                ptr: vi[i].as_ptr(),
                len: vi[i].len(),
            },
            field_str_arr: Array {
                ptr: vs[i].as_ptr(),
                len: vs[i].len(),
            },
        }
    }).collect::<Vec<_>>();
    let ll = Array {
        ptr: l.as_ptr(),
        len: l.len(),
    };
    let ll = send_me_struct_array(ll);
    assert!(ll.len == 42);
    {
        let lls = from_raw_parts(ll.ptr, ll.len);

        for l in lls
        {
            assert!(l.field_int == 42);
            assert!(l.field_int_arr.len == 42);
            assert!(l.field_str_arr.len == 42);
            {
                let is = from_raw_parts(l.field_int_arr.ptr, l.field_int_arr.len);
                let ss = from_raw_parts(l.field_str_arr.ptr, l.field_str_arr.len);
                for &i in is {
                    assert!(i == 42);
                }
                for s in ss
                {
                    assert!(s.field_i == 42);
                    assert!(s.field_bool == true);
                    assert!(s.field_double == 42.42);
                }
            }
            l.clone().field_int_arr.free();
            l.clone().field_str_arr.free();
        }
    }
    ll.free();

}
