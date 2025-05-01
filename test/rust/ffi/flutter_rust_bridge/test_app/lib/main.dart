import 'package:flutter/material.dart';
import 'package:flutter_svg/svg.dart';
import 'package:test_app/src/rust/api/simple.dart';
import 'package:test_app/src/rust/frb_generated.dart';
import 'package:test_app/src/julien.dart';
import 'package:test_app/src/counter.dart';
import 'package:test_app/src/profile.dart';

Future<void> main() async {
  await RustLib.init();
  runApp(const MyApp());
}

const String svgString = '''
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100">
<path d="M50,3l12,36h38l-30,22l11,36l-31-21l-31,21l11-36l-30-22h38z"
fill="#FF0" stroke="#FC0" stroke-width="2"/>
</svg>
''';

String computeString() {
    return greet(name: helloWorld());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(title: const Text('A')),
        body: Center(
          child: Column(
          children: [
            SvgPicture.string(getProfile(), width: 500, height: 250),
            Text(
              computeString(),
            ),  
            Counter()
          ],
        ),
      ),
    ));
  }
}
