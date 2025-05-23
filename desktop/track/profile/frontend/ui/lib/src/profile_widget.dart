import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:ui/src/backendmodel.dart';
import 'package:visibility_detector/visibility_detector.dart';

class TrackWidget extends StatefulWidget {
  final TrackData trackData;
  const TrackWidget({super.key, required this.trackData});
  @override
  State<TrackWidget> createState() => TrackWidgetState();
}

class TrackWidgetState extends State<TrackWidget> {
  double currentEpsilon = 0;
  @override
  void initState() {
    super.initState();
    developer.log("TrackWidgetState init state");
  }

  void ensureStart() {
    WidgetsBinding.instance.addPostFrameCallback((_) {
      start();
    });
  }

  void onVisibilityChanged(VisibilityInfo info) {
    var visiblePercentage = info.visibleFraction * 100;
    if (visiblePercentage >= 50) {
      start();
    }
    debugPrint('${info.key} => $visiblePercentage% visible');
  }

  void start() {
    if (!mounted) {
      return;
    }
    final renderings = Segment.of(context);
    if (widget.trackData == TrackData.track) {
      renderings.track.start();
    }
    if (widget.trackData == TrackData.waypoints) {
      renderings.waypoints.start();
    }
  }

  @override
  Widget build(BuildContext context) {
    final renderings = Segment.of(context);
    FutureRendering? future;
    if (widget.trackData == TrackData.track) {
      future = renderings.track;
    } else {
      future = renderings.waypoints;
    }
    return VisibilityDetector(
      key: Key('id:${future.id()}'),
      onVisibilityChanged: onVisibilityChanged,
      child: ListenableBuilder(
        listenable: future,
        builder: (context, _) {
          return LittleWidget(future: future!);
        },
      ),
    );
  }
}

class LittleWidget extends StatelessWidget {
  final FutureRendering future;

  const LittleWidget({super.key, required this.future});

  @override
  Widget build(BuildContext context) {
    Widget child = Text("START ${future.segment.id()}");
    if (future.done()) {
      developer.log("SVG DONE ${future.id()}");
      child = SvgPicture.string(future.result(), width: 600, height: 150);
    } 
    return SizedBox(width: 600.0, child: Column(children: [child]));
  }
}

class SegmentStack extends StatefulWidget {
  const SegmentStack({super.key});

  @override
  State<SegmentStack> createState() => _SegmentStackState();
}

class _SegmentStackState extends State<SegmentStack> {
  @override
  Widget build(BuildContext context) {
    final renderings = Segment.of(context);
    return Stack(
      children: <Widget>[
        TrackWidget(trackData: TrackData.track),
        TrackWidget(trackData: TrackData.waypoints),
        Text("ID:${renderings.id()}"),
      ],
    );
  }
}


