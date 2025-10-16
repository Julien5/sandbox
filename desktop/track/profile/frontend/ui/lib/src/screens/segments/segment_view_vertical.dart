import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'mapview.dart';
import 'profileview.dart';

class SegmentViewVertical extends StatelessWidget {
  const SegmentViewVertical({super.key});

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
      builder: (context, constraints) {
        double profileHeight = constraints.maxWidth * (285.0 / 400);
        double mapHeight = constraints.maxWidth;
        developer.log("profileHeight= $profileHeight");
        Widget p = ProfileStack(profileHeight: profileHeight);
        /*Widget profile = Padding(
          padding: EdgeInsets.all(20.0),
          child: DecoratedBox(
            decoration: BoxDecoration(
              border: Border.all(color: Colors.blue),
              borderRadius: BorderRadius.circular(20),
            ),
            child: Center(child: p),
          ),
        );*/
        BoxConstraints box = BoxConstraints(maxWidth: 200, maxHeight: 200);
        ButtonStyle style = ButtonStyle(
          shape: WidgetStateProperty.all<RoundedRectangleBorder>(
            RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(10.0),
              side: BorderSide(color: Colors.blue),
            ),
          ),
          overlayColor: WidgetStateProperty.all(Colors.transparent), // No hover color
        );
        Widget profile = ElevatedButton(
          onPressed: () {
            developer.log("hi profile");
          },
          style: style,
          child: ConstrainedBox(constraints: box, child: p),
        );
        Widget map = ElevatedButton(
          onPressed: () {
            developer.log("hi map");
          },
          onLongPress: () {
            developer.log("edit map");
          },
          onHover: (b) {
            developer.log("hover map $b");
          },
          style: style,
          child: ConstrainedBox(constraints: box, child: MapConsumer()),
        );
        return Column(
          children: [
            Expanded(child: Center(child: profile)),
            Expanded(child: Center(child: map)),
          ],
        );
      },
    );
  }
}
