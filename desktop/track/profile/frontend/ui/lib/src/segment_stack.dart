import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/backendmodel.dart';
import 'package:ui/src/future_rendering_widget.dart';
import 'package:visibility_detector/visibility_detector.dart';

class SegmentStack extends StatefulWidget {
  const SegmentStack({super.key});

  @override
  State<SegmentStack> createState() => _SegmentStackState();
}

class _SegmentStackState extends State<SegmentStack> {
  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Stack(children: <Widget>[TrackConsumer(), WaypointsConsumer()]);
  }
}

class TrackConsumer extends StatelessWidget {
  const TrackConsumer({super.key});

  @override
  Widget build(BuildContext ctx) {
    return Consumer<TrackRendering>(
      builder: (context, trackRendering, child) {
        return FutureRenderingWidget(future: trackRendering);
      },
    );
  }
}

class WaypointsConsumer extends StatefulWidget {
  const WaypointsConsumer({super.key});

  @override
  State<WaypointsConsumer> createState() => _WaypointsConsumerState();
}

class _WaypointsConsumerState extends State<WaypointsConsumer> {
  double visibility = 0;

  void onVisibilityChanged(VisibilityInfo info) {
    developer.log("[on vis changed] ${info.visibleFraction}");
    if (!mounted) {
      return;
    }
    WaypointsRendering wp=Provider.of<WaypointsRendering>(context,listen:false);
    wp.updateVisibility(info.visibleFraction);
  }

  @override
  Widget build(BuildContext ctx) {
    return Consumer<WaypointsRendering>(
      builder: (context, waypointsRendering, child) {
        return VisibilityDetector(
          key: Key('id:${waypointsRendering.id()}'),
          onVisibilityChanged: onVisibilityChanged,
          child: FutureRenderingWidget(future: waypointsRendering),
        );
      },
    );
  }
}
