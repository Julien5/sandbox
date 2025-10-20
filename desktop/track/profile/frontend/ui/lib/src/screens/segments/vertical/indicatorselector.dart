import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/models/futurerenderer.dart';
import 'package:ui/src/models/root.dart';
import 'package:ui/src/rust/api/bridge.dart';

class ElevationIndicatorGroup extends StatefulWidget {
  const ElevationIndicatorGroup({super.key});

  @override
  State<ElevationIndicatorGroup> createState() =>
      _ElevationIndicatorGroupState();
}

class _ElevationIndicatorGroupState extends State<ElevationIndicatorGroup> {
  String? selectedValue = "none"; // Initially, "A" is selected
  static const String none = "none";
  static const String ticks = "ticks";
  static const String percent = "percent";

  @override
  void initState() {
    super.initState();
    readModel();
  }

  void readModel() {
    RootModel rootModel = Provider.of<RootModel>(context, listen: false);
    Parameters p = rootModel.parameters();
    for (var indicator in p.profileOptions.elevationIndicators) {
      if (indicator == ProfileIndication.numericSlope) {
        selectedValue = percent;
      }
      if (indicator == ProfileIndication.gainTicks) {
        selectedValue = ticks;
      }
      if (indicator == ProfileIndication.none) {
        selectedValue = none;
      }
    }
  }

  void updateModel() {
    RootModel rootModel = Provider.of<RootModel>(context, listen: false);
    Parameters p = rootModel.parameters();
    p.profileOptions.elevationIndicators.clear();
    if (selectedValue == none) {
      // nothing
    } else if (selectedValue == percent) {
      p.profileOptions.elevationIndicators.add(ProfileIndication.numericSlope);
    } else if (selectedValue == ticks) {
      p.profileOptions.elevationIndicators.add(ProfileIndication.gainTicks);
    }
    developer.log("set parameters on root model to update all segments");
    rootModel.setParameters(p);
  }

  void onChanged(String? newValue) {
    setState(() {
      selectedValue = newValue;
    });
    updateModel();
  }

  @override
  Widget build(BuildContext context) {
    ListTileControlAffinity left = ListTileControlAffinity.trailing;
    return RadioGroup<String>(
      groupValue: selectedValue,
      onChanged: onChanged,
      child: Column(
        mainAxisSize: MainAxisSize.min, // Center the column vertically
        children: [
          RadioListTile<String>(
            title: const Text("Elevation ticks"),
            value: ticks,
            controlAffinity: left,
          ),
          RadioListTile<String>(
            title: const Text("Average slope per intervals"),
            value: percent,
            controlAffinity: left,
          ),
          RadioListTile<String>(
            title: const Text("None"),
            value: none,
            controlAffinity: left,
          ),
        ],
      ),
    );
  }
}

class ElevationIndicatorChooser extends StatefulWidget {
  const ElevationIndicatorChooser({super.key});

  @override
  State<ElevationIndicatorChooser> createState() =>
      _ElevationIndicatorChooserState();
}

class _ElevationIndicatorChooserState extends State<ElevationIndicatorChooser> {
  // Track the selected value
  String? selectedValue = "A"; // Initially, "A" is selected

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Padding(
        padding: const EdgeInsets.symmetric(
          horizontal: 20.0,
        ), // Add margin inside the parent
        child: ConstrainedBox(
          constraints: const BoxConstraints(maxWidth: 300),
          child: ElevationIndicatorGroup(),
        ),
      ),
    );
  }
}
