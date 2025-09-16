import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'package:ui/src/futurerenderer.dart';
import 'package:ui/src/rust/api/bridge.dart' as bridge;

class SegmentData {
  Renderers renderers;
  List<bridge.Waypoint> tableWaypoints;
  SegmentData({required this.renderers, required this.tableWaypoints});
}

class RootModel extends ChangeNotifier {
  late bridge.Bridge _bridge;
  final Map<bridge.Segment,SegmentData> _segments = {};

  RootModel() {
    _bridge = bridge.Bridge.make();
  }

  @override
  void dispose() {
    developer.log("~RootModel");
    super.dispose();
  }

  Future<void> loadDemo() async {
    await _bridge.loadDemo();
  }

  bridge.Parameters parameters() {
    return _bridge.getParameters();
  }

  void setParameters(bridge.Parameters p) {
    _bridge.setParameters(parameters: p);
  }

  bridge.SegmentStatistics statistics() {
    return _bridge.statistics();
  }

  Map<bridge.Segment,SegmentData> segments() {
    return _segments;
  }

  void updateSegments() {
    assert(_segments.isEmpty);
    var segments = _bridge.segments();

    for (var segment in segments) {
      var t = ProfileRenderer(_bridge, segment);
      var m = MapRenderer(_bridge, segment);
      var y = YAxisRenderer(_bridge, segment);
      var W = _bridge.waypointsTable(segment: segment);
      _segments[segment]=SegmentData(renderers: Renderers(t, y, m), tableWaypoints: W);
    }
  }
}
