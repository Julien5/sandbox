import 'package:flutter/material.dart';

class Counter extends StatefulWidget {
  const Counter({super.key});

  @override
  State<Counter> createState() => _CounterState();
}

class _CounterState extends State<Counter> {
  int counter = 0;
  @override
  Widget build(BuildContext context) {
    return ElevatedButton(onPressed: () => setState(() {
      counter=counter +2;
    }),
    child: Text('now at $counter'),
    );
  }
}