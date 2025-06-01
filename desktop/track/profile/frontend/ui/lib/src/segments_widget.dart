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

  @override
  void initState() {
    super.initState();
  }

  @override
  void didChangeDependencies() {
    SegmentsProvider p = Provider.of<SegmentsProvider>(context, listen: false);
    _initRenderingProviders(p);
    super.didChangeDependencies();
  }

  void _initRenderingProviders(SegmentsProvider segmentsProvider) {
    var segmentsProvider = widget.segmentsProvider;
    var S = segmentsProvider!.segments();
    if (S.length != _segments.length) {
      _segments.clear();
      for (var segment in S) {
        var w = segmentsProvider.createRenderings(segment, SegmentStack());
        w.trackRendering.start();
        w.waypointsRendering.start();
        _segments.add(w);
      }
    }
  }

  void makeMorePoints() {
    var backend = widget.segmentsProvider!;
    backend.decrementDelta();
    updateWaypoints();
  }

  void makeLessPoints() {
    var backend = widget.segmentsProvider!;
    backend.incrementDelta();
    updateWaypoints();
  }

  void updateWaypoints() {
    for (var segment in _segments) {
      segment.waypointsRendering.reset();
    }
  }

  @override
  Widget build(BuildContext context) {
    developer.log("[segments] [build] #segments=${_segments.length}");
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

class SegmentsConsumer extends StatelessWidget {
  const SegmentsConsumer({super.key});

  @override
  Widget build(BuildContext ctx) {
    return Consumer<SegmentsProvider>(
      builder: (context, segmentsProvider, child) {
        return SegmentsWidget(segmentsProvider: segmentsProvider);
      },
    );
  }
}
