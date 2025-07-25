import 'dart:developer' as developer;
import 'dart:math';

import 'package:flutter/material.dart';
//import 'package:flutter_svg/flutter_svg.dart';
import 'package:ui/src/backendmodel.dart';
import 'package:ui/src/minisvg.dart';

class FutureRenderingWidget extends StatefulWidget {
  final FutureRenderer future;
  const FutureRenderingWidget({super.key, required this.future});

  @override
  State<FutureRenderingWidget> createState() => _FutureRenderingWidgetState();
}

class _FutureRenderingWidgetState extends State<FutureRenderingWidget> {
  Widget? svg;

  Widget buildWorker(Size parentSize) {
    Size wantedSize = parentSize;
    widget.future.setSize(wantedSize);
    if (widget.future.done()) {
      svg = MiniSvgWidget(svg: widget.future.result(), size: wantedSize);
    }
    if (!widget.future.done() && svg == null) {
      return Text("starting ${widget.future.trackData} ${widget.future.id()}");
    }

    if (!widget.future.done()) {
      return Stack(
        children: <Widget>[
          Text("updating ${widget.future.trackData} ${widget.future.id()}"),
          svg!,
        ],
      );
    }
    return svg!;
  }

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
      builder: (BuildContext context, BoxConstraints constraints) {
        double screenwidth = MediaQuery.sizeOf(context).width;
        double screenheight = MediaQuery.sizeOf(context).height;
        double w = min(screenwidth, constraints.maxWidth-100);
        double h = min(50+screenheight / 3, constraints.maxHeight);
        developer.log("wanted size: $constraints");
        Size size = Size(w, h);
        return buildWorker(size);
      },
    );
  }
}
