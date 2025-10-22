import 'dart:developer' as developer;
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/models/root.dart';
import 'package:ui/src/rust/api/bridge.dart';
import 'package:ui/src/rust/api/bridge.dart' as bridge;
import 'package:ui/src/screens/settings/slidervalues.dart';

class PointsSlider extends StatefulWidget {
  const PointsSlider({super.key});

  @override
  State<PointsSlider> createState() => _PointsSliderState();
}

class _PointsSliderState extends State<PointsSlider> {
  int? selectedValue;
  final SliderValues _sliderValues = SliderValues();

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) {
      readModel();
      RootModel rootModel = Provider.of<RootModel>(context, listen: false);
      developer.log("E=${rootModel.statistics().distanceEnd}");
      _sliderValues.init([0, 5, 10, 15, 20, 25], 10);
      setState(() {});
    });
  }

  void readModel() {
    RootModel rootModel = Provider.of<RootModel>(context, listen: false);
    Parameters p = rootModel.parameters();
    _sliderValues.setValue(p.profileOptions.npoints.toDouble());
  }

  void updateModel() {
    RootModel rootModel = Provider.of<RootModel>(context, listen: false);
    Parameters oldParameters = rootModel.parameters();
    ProfileOptions newProfileOptions = ProfileOptions(
      elevationIndicators: oldParameters.profileOptions.elevationIndicators,
      npoints: BigInt.from(selectedValue!.toInt()),
    );

    developer.log("set parameters on root model to update all segments");
    bridge.Parameters newParameters = bridge.Parameters(
      speed: oldParameters.speed,
      startTime: oldParameters.startTime,
      segmentLength: oldParameters.segmentLength,
      segmentOverlap: oldParameters.segmentOverlap,
      maxStepSize: oldParameters.maxStepSize,
      smoothWidth: oldParameters.smoothWidth,
      profileOptions: newProfileOptions,
      debug: oldParameters.debug,
    );
    rootModel.setParameters(newParameters);
  }

  void onChanged(double newValue) {
     _sliderValues.setValue(newValue);
    setState(() {
      selectedValue = newValue.toInt();
    });
    updateModel();
  }

  @override
  Widget build(BuildContext context) {
    return SliderValuesWidget(
      values: _sliderValues,
      onChanged: onChanged,
      formatLabel:
          (value) => "${(value).toInt()} points",
    );
  }
}

class PointCountChooser extends StatelessWidget {
  const PointCountChooser({super.key});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Padding(
        padding: const EdgeInsets.symmetric(
          horizontal: 20.0,
        ), // Add margin inside the parent
        child: ConstrainedBox(
          constraints: const BoxConstraints(maxWidth: 300),
          child: PointsSlider(),
        ),
      ),
    );
  }
}
