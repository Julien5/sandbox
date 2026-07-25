import 'package:flutter/material.dart';
import 'package:nano/src/rust/frb_generated.dart';
import 'package:nano/src/screens/device_list_screen.dart';

Future<void> main() async {
  await RustLib.init();
  runApp(const NanoApp());
}

class NanoApp extends StatelessWidget {
  const NanoApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Nano MIDI',
      theme: ThemeData(
        colorSchemeSeed: Colors.indigo,
        useMaterial3: true,
      ),
      home: const DeviceListScreen(),
    );
  }
}
