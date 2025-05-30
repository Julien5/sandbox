import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/backendmodel.dart';
import 'package:ui/src/counter.dart';
import 'package:ui/src/segment_stack.dart';

class SegmentsWidget extends StatefulWidget {
  final SegmentsProvider? segmentsProvider;
  const SegmentsWidget({super.key, this.segmentsProvider});

  @override
  State<SegmentsWidget> createState() => SegmentsWidgetState();
}

class SegmentsWidgetState extends State<SegmentsWidget> {
  final List<RenderingsProvider> _segments = [];
  SegmentsProvider? provider;

  @override
  void initState() {
    super.initState();
  }

  void buildSegments() {
    var segmentsProvider = widget.segmentsProvider;
    var S = segmentsProvider!.segments();
    if (S.length != _segments.length) {
      _segments.clear();
      for (var segment in S) {
        var provider = RenderingsProvider(
          renderings: segmentsProvider.createRenderings(segment),
          child: SegmentStack(),
        );
        _segments.add(provider);
      }
    } else {
      for (var segment in _segments) {
        segment.renderings.waypoints.reset();
      }
    }
  }

  void makeMorePoints() {
    var backend = widget.segmentsProvider!;
    backend.decrementDelta();
  }

  void makeLessPoints() {
    var backend = widget.segmentsProvider!;
    backend.incrementDelta();
  }

  @override
  Widget build(BuildContext context) {
    developer.log("[segments] [build] #segments=${_segments.length}");
    buildSegments();
    if (_segments.isEmpty) {
      return Text("segments is empty");
    }
    return Column(
      children: [
        Row(
          children: [
            PressButton(label: "more", onCounterPressed: makeMorePoints),
            PressButton(label: "less", onCounterPressed: makeLessPoints),
          ],
        ),
        Expanded(
          child: ListView.separated(
            itemCount: _segments.length,
            separatorBuilder: (context, index) => const Divider(),
            itemBuilder: (context, index) {
              return _segments[index];
            },
          ),
        ),
      ],
    );
  }
}

class SegmentConsumer extends StatelessWidget {
  const SegmentConsumer({super.key});

  @override
  Widget build(BuildContext ctx) {
   return Consumer<SegmentsProvider>(
      builder: (context, segmentsProvider, child) {
        return SegmentsWidget(segmentsProvider:segmentsProvider);
      });
  }
}