{-# LANGUAGE ForeignFunctionInterface #-}

module Prologin where

import Api

import Control.Applicative
import Control.Monad
import Control.Exception

assert' x = assert x ()

theStruct = Struct_with_array 42 (replicate 42 42) (replicate 42 (Simple_struct 42 True))

-- Called 10K times to test if things work well.
test :: IO ()
test = do
  send_me_42 42
  send_me_42_and_1337 42 1337
  send_me_true True
  send_me_tau 6.2831853

  assert' . (== 42) <$> returns_42
  assert' <$> returns_true
  assert' . (\x -> abs (x - 6.2831853) < 0.0001) <$> returns_tau
  assert' . (== [1..100]) <$> returns_range 1 100
  assert' . (== [1..10000]) <$> returns_range 1 10000
  assert' . (== [1..7]) <$> returns_sorted [1, 3, 2, 4, 5, 7, 6]
  assert' . (== [1..10000]) <$> returns_sorted [1..10000]

  send_me_42s $ theStruct
  send_me_test_enum Val1 Val2;

  assert' . (== (replicate 42 theStruct)) <$> send_me_struct_array (replicate 42 theStruct)
  return ()

hs_test = catch test (\e -> putStrLn $ show (e :: SomeException))
foreign export ccall hs_test :: IO ()

