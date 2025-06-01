
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/rust/api/frontend.dart';

enum TrackData { track, waypoints }

class FutureRenderer with ChangeNotifier {
  final FSegment segment;
  final TrackData trackData;
  final Frontend _frontend;

  Future<String>? _future;
  String? _result;

  FutureRenderer({
    required Frontend frontend,
    required this.segment,
    required this.trackData,
  }) : _frontend = frontend;

  void start() {
    _result = null;
    if (trackData == TrackData.track) {
      _future = _frontend.renderSegmentTrack(segment: segment);
    } else {
      _future = _frontend.renderSegmentWaypoints(segment: segment);
    }
    notifyListeners();
    _future!.then((value) => onCompleted(value));
  }

  BigInt id() {
    return segment.id();
  }

  bool started() {
    return _future != null;
  }

  bool needsStart() {
    return !started() && !done();
  }

  void onCompleted(String value) {
    _result = value;
    _future = null;
    notifyListeners();
  }

  bool done() {
    return _result != null;
  }

  String result() {
    assert(_result != null);
    return _result!;
  }
}

class TrackRendering extends FutureRenderer {
  TrackRendering(Frontend frontend, FSegment segment)
    : super(frontend: frontend, segment: segment, trackData: TrackData.track);
}

class WaypointsRendering extends FutureRenderer {
  double visibility = 0;
  WaypointsRendering(Frontend frontend, FSegment segment)
    : super(
        frontend: frontend,
        segment: segment,
        trackData: TrackData.waypoints,
      );

  void updateVisibility(double v) {
    visibility = v;
    _update();
  }

  void reset() {
    _future = null;
    _result = null;
    _update();
  }

  void _update() {
    if (visibility < 0.5) {
      return;
    }
    if (needsStart()) {
      start();
    }
  }
}

class SegmentsProvider extends ChangeNotifier {
  Frontend? _frontend;
  List<FSegment> _segments = [];

  SegmentsProvider(Frontend f) {
    _frontend = f;
    _updateSegments();
  }

  void incrementDelta() {
    _frontend!.changeParameter(eps: 10.0);
    _updateSegments();
  }

  void decrementDelta() {
    _frontend!.changeParameter(eps: -10.0);
    _updateSegments();
  }

  void _updateSegments() {
    _segments = _frontend!.segments();
    notifyListeners();
  }

  List<FSegment> segments() {
    return _segments;
  }

  String renderSegmentWaypointsSync(FSegment segment) {
    return _frontend!.renderSegmentWaypointsSync(segment: segment);
  }

  RenderingsProvider createRenderings(FSegment segment, Widget child) {
    TrackRendering track = TrackRendering(_frontend!, segment);
    WaypointsRendering waypoints = WaypointsRendering(_frontend!, segment);
    return RenderingsProvider(track, waypoints, child);
  }
}

class RenderingsProvider extends MultiProvider {
  final WaypointsRendering waypointsRendering;
  final TrackRendering trackRendering;

  RenderingsProvider(
    TrackRendering track,
    WaypointsRendering waypoints,
    Widget child, {
    super.key,
  }) : waypointsRendering = waypoints,
       trackRendering = track,
       super(
         providers: [
           ChangeNotifierProvider.value(value: track),
           ChangeNotifierProvider.value(value: waypoints),
         ],
         child: child,
       );
}
