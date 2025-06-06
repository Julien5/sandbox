import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/backendmodel.dart';
import 'package:ui/src/future_rendering_widget.dart';
import 'package:visibility_detector/visibility_detector.dart';

class SegmentStack extends StatelessWidget {
  const SegmentStack({super.key});

  @override
  Widget build(BuildContext context) {
    return Stack(children: <Widget>[TrackConsumer(), WaypointsConsumer()]);
  }
}

class TrackConsumer extends StatelessWidget {
  const TrackConsumer({super.key});

  @override
  Widget build(BuildContext ctx) {
    return Consumer<TrackRenderer>(
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
    if (!mounted) {
      return;
    }
    WaypointsRenderer wp=Provider.of<WaypointsRenderer>(context,listen:false);
    developer.log("[waypoint consumer] id:${wp.id()} vis:${info.visibleFraction}");
    wp.updateVisibility(info.visibleFraction);
  }

  @override
  Widget build(BuildContext ctx) {
    return Consumer<WaypointsRenderer>(
      builder: (context, waypointsRendering, child) {
        // It would be more accurate to check visibility with a scroll controller
        // at the list view level. Because "Callbacks are not fired immediately 
        // on visibility changes."
        return VisibilityDetector(
          key: Key('id:${waypointsRendering.id()}'),
          onVisibilityChanged: onVisibilityChanged,
          child: FutureRenderingWidget(future: waypointsRendering),
        );
      },
    );
  }
}
