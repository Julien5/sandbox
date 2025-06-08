import 'dart:developer' as developer show log;
import 'dart:math' as math;
import 'dart:ui' as ui;
import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:ui/src/backendmodel.dart';

class FutureRenderingWidget extends StatefulWidget {
  final FutureRenderer future;
  const FutureRenderingWidget({super.key, required this.future});

  @override
  State<FutureRenderingWidget> createState() => _FutureRenderingWidgetState();
}

class _FutureRenderingWidgetState extends State<FutureRenderingWidget> {
  Widget? svg;

  Widget grayBackground() {
    return Container(
      width: 1420.0,
      height: 400.0,
      decoration: BoxDecoration(color: Colors.grey.withAlpha(150)),
    );
  }

  @override
  Widget build(BuildContext context) {
    if (widget.future.done()) {
      /*svg = SvgPicture.string(
        widget.future.result(),
        fit: BoxFit.contain,
        width: 1420,
        height: 400,
      );*/
      svg = NoAntialiasingWidget(svgData: widget.future.result());
    }
    if (!widget.future.done() && svg == null) {
      return Text("starting ${widget.future.trackData} ${widget.future.id()}");
    }

    if (!widget.future.done()) {
      return Stack(
        children: <Widget>[
          grayBackground(),
          Text("updating ${widget.future.trackData} ${widget.future.id()}"),
          svg!,
        ],
      );
    }
    return svg!;
  }
}

class SvgPainter extends CustomPainter {
  final ImageInfo picture;
  SvgPainter(this.picture);

  @override
  void paint(Canvas canvas, Size size) {
    // Decode the SVG string into a DrawableRoot

    final paint = Paint()..isAntiAlias = true;
  
    canvas.saveLayer(Rect.fromLTWH(0, 0, 1400, 400), paint);
    //canvas.drawPicture(picture);
    canvas.drawImage(picture.image, Offset(0,0),paint);
    // Draw a circle
    canvas.drawCircle(Offset(size.width / 2, size.height / 2), 50, paint);
    canvas.restore();
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) {
    return true; // Return true if the SVG data changes
  }
}
Future<ImageInfo> loadSVG(String rawSvg, int pixelWidth, int pixelHeight) async {
  try {
    final pictureInfo = await vg.loadPicture(
      SvgStringLoader(rawSvg),
      null,
      clipViewbox: false,
      antialiasing: false
    );

    final recorder = ui.PictureRecorder();
    final paint = Paint()..isAntiAlias = false;
    final canvas = Canvas(recorder);

    canvas.scale(2.0);
    
    canvas.saveLayer(Rect.fromLTWH(0, 0, pixelWidth.toDouble(), pixelHeight.toDouble()), paint);
    canvas.drawPicture(pictureInfo.picture);
    canvas.restore();

    final image = await recorder.endRecording().toImage(
      pixelWidth,
      pixelHeight,
    );

    return ImageInfo(image: image);
  } catch (e) {
    throw Exception('Failed to render SVG: $e');
  }
}


class NoAntialiasingWidget extends StatefulWidget {
  final String svgData;
  const NoAntialiasingWidget({super.key, required this.svgData});

  @override
  State<NoAntialiasingWidget> createState() => _NoAntialiasingWidgetState();
}

class _NoAntialiasingWidgetState extends State<NoAntialiasingWidget> {
  ImageInfo? picture;

  @override
  void initState() {
    super.initState();
    _loadPicture();
  }

  Future<void> _loadPicture() async {
    picture = await loadSVG(widget.svgData,1420,400);
    //developer.log("[aa] ${pictureInfo.picture}");
    setState(() {
      picture = picture;
    });
  }

  @override
  Widget build(BuildContext context) {
    if (picture == null) {
      return const Center(child: CircularProgressIndicator());
    }
    return CustomPaint(
      size: const Size(200, 200),
      painter: SvgPainter(picture!),
    );
  }
}
