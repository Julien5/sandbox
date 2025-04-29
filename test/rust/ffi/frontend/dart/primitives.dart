// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';
import 'package:path/path.dart' as path;

// C add function - int sum(int a, int b);
//
// Example of how to pass parameters into C and use the returned result
typedef AddFunc = Int64 Function(Int64 a, Int64 b);
typedef Add = int Function(int a, int b);

void main() {
  // Open the dynamic library
  var libraryPath = path.join(
    "/home/julien/delme/rust-targets/debug",
    'libbackend.so',
  );

  final dylib = DynamicLibrary.open(libraryPath);

  // calls int add(int a, int b);
  final addPointer = dylib.lookup<NativeFunction<AddFunc>>('add');
  final add = addPointer.asFunction<Add>();
  print('3 + 5 = ${add(3, 5)}');
}
