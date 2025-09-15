import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:test_async/src/rust/api/simple.dart';

class StreamWidget extends StatefulWidget {
  const StreamWidget({super.key});

  @override
  State<StreamWidget> createState() => _StreamWidgetState();
}

class _StreamWidgetState extends State<StreamWidget> {
  late Stream<String> ticks;

  @override
  void initState() {
    super.initState();
    ticks = ticksink();
  }

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: <Widget>[
          const Text("Time since starting Rust stream"),
          StreamBuilder<String>(
            stream: ticks,
            builder: (context, snap) {
              final error = snap.error;
              String text = "nothing";
              if (error != null) {
                text = error.toString();
                developer.log("error: ${error.toString()}");
              }

              final data = snap.data;
              if (data != null) {
                text = data;
              }
              return Text('text=$text');
            },
          ),
        ],
      ),
    );
  }
}
