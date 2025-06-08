import 'package:flutter/material.dart';
import 'package:minisvg/src/minisvg.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    String m = mini();
    return MaterialApp(
      title: 'Flutter Demo',
      home: const Text("foo:"),
    );
  }
}
