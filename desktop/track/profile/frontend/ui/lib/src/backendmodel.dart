import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:ui/src/rust/api/frontend.dart';

enum TrackData { track, waypoints }

class FutureRendering with ChangeNotifier {
  final FSegment segment;
  final TrackData trackData;
  final Frontend _frontend;
  Future<String>? _future;
  String? _result;

  FutureRendering({
    required Frontend frontend,
    required this.segment,
    required this.trackData,
  }) : _frontend = frontend;

  void start() {
    developer.log("START rendering for ${segment.id()} $trackData");
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

  void reset() {
    _future = null;
    _result = null;
    developer.log(
      "[future rendering] notify for id ${segment.id()} and $trackData",
    );
    notifyListeners();
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
    developer.log("DONE rendering for ${segment.id()} $trackData");
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



class Renderings {
  final FutureRendering track;
  final FutureRendering waypoints;
  const Renderings({
    required this.track,
    required this.waypoints,
  });
  BigInt id() {
    return track.segment.id();
  }
}

class RenderingsProvider extends InheritedWidget {
  final Renderings renderings;
  const RenderingsProvider({super.key, required super.child, required this.renderings});

  @override
  bool updateShouldNotify(covariant InheritedWidget oldWidget) {
    // i dont understand this.

    assert(false);
    return true;
  }
  static RenderingsProvider of(BuildContext context) {
    final RenderingsProvider? ret = context.dependOnInheritedWidgetOfExactType<RenderingsProvider>();
    assert(ret != null);
    return ret!;
  }
}

class SegmentsProvider extends ChangeNotifier {
  Frontend? _frontend;

  SegmentsProvider(  Frontend f  ){_frontend=f;}


  void incrementDelta() {
    _frontend!.changeParameter(eps: 10.0);
    notifyListeners();
  }

  void decrementDelta() {
    _frontend!.changeParameter(eps: -10.0);
     notifyListeners();
  }

  List<FSegment> segments() {
    return _frontend!.segments();
  }

  String renderSegmentWaypointsSync(FSegment segment) {
    return _frontend!.renderSegmentWaypointsSync(segment: segment);
  }

  FutureRendering _renderSegmentWaypoints(FSegment segment) {
    return FutureRendering(
      frontend: _frontend!,
      segment: segment,
      trackData: TrackData.waypoints,
    );
  }

  FutureRendering _renderSegmentTrack(FSegment segment) {
    return FutureRendering(
      frontend: _frontend!,
      segment: segment,
      trackData: TrackData.track,
    );
  }

  Renderings createRenderings(FSegment segment) {
    return Renderings(
      track: _renderSegmentTrack(segment),
      waypoints: _renderSegmentWaypoints(segment),
    );
  }
}
