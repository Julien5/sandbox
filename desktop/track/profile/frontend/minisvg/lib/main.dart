import 'package:flutter/material.dart';
import 'package:minisvg/src/minisvg.dart' as minisvg;

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    minisvg.Element root = minisvg.rootElement();
    return MaterialApp(
      title: 'Flutter Demo',
      home: Scaffold(
        appBar: AppBar(title: const Text('Custom Painter Example')),
        body: Center(
          child: CustomPaint(
            size: const Size(200, 200), // Specify the size of the canvas
            painter: SvgPainter(root:root),
          ),
        ),
      ),
    );
  }
}

class SvgPainter extends CustomPainter {
  final minisvg.Element root;

  SvgPainter({required this.root});

  @override
  void paint(Canvas canvas, Size size) {
    // Example: Draw a red circle
    final paint =
        Paint()
          ..color = Colors.red
          ..style = PaintingStyle.fill;

    canvas.drawCircle(Offset(size.width / 2, size.height / 2), 50, paint);
    root.paintElement(canvas, size);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) {
    return false; // Return true if the painter should repaint
  }
}
