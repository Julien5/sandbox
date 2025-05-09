import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:ui/src/rust/api/frontend.dart';
import 'dart:developer' as developer;

class ProfileWidget extends StatefulWidget {
  const ProfileWidget({super.key});

  @override
  State<ProfileWidget> createState() => ProfileWidgetState();
}

class ProfileWidgetState extends State<ProfileWidget> {
  String? svgData;
  String? errorMessage;
  bool isLoading = true;

  @override
  void initState() {
    super.initState();
  }

  void loadCircle() async {
    setState(() {
      isLoading = true;
      svgData = null;
      errorMessage = null;
    });
    try {
      developer.log("start loading circle");
      final data = await svgCircle();
      developer.log("loaded circle");
      setState(() {
        svgData = data;
        isLoading = false;
      });
    } catch (e) {
      setState(() {
        errorMessage = 'Error: $e';
        isLoading = false;
      });
    }
  }

  Future<void> loadTrackProfile(Frontend frontend) async {
    try {
      final data = await frontend.svgTrack();
      setState(() {
        svgData = data;
        isLoading = false;
      });
    } catch (e) {
      setState(() {
        errorMessage = 'Error: $e';
        isLoading = false;
      });
    }
  }

  Future<void> loadWaypointsProfile(Frontend frontend) async {
    try {
      final data = await frontend.svgWaypoints();
      setState(() {
        svgData = data;
        isLoading = false;
      });
    } catch (e) {
      setState(() {
        errorMessage = 'Error: $e';
        isLoading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 500.0,
      height: 250.0,
      child: Builder(
        builder: (context) {
          if (isLoading) {
            return const Center(child: CircularProgressIndicator());
          } else if (errorMessage != null) {
            return Center(child: Text(errorMessage!));
          } else if (svgData != null) {
            return SvgPicture.string(svgData!, width: 500, height: 250);
          } else {
            return const Center(child: Text('No data available'));
          }
        },
      ),
    );
  }
}

class ProfileWidgets extends StatefulWidget {
  const ProfileWidgets({super.key});

  @override
  State<ProfileWidgets> createState() => ProfileWidgetsState();
}

class ProfileWidgetsState extends State<ProfileWidgets> {
  final GlobalKey<ProfileWidgetState> key1 = GlobalKey<ProfileWidgetState>();
  final GlobalKey<ProfileWidgetState> key2 = GlobalKey<ProfileWidgetState>();

  @override
  void initState() {
    super.initState();
    loadCircle();
  }

  Future<void> loadTrackProfile(Frontend frontend) async {
    key1.currentState?.loadTrackProfile(frontend);
  }

  Future<void> loadWaypointsProfile(Frontend frontend) async {
    key2.currentState?.loadWaypointsProfile(frontend);
  }

  Future<void> loadCircle() async {
    key1.currentState?.loadCircle();
  }

  bool isLoading() {
    if (key1.currentState == null) {
      return true;
    }
    if (key2.currentState == null) {
      return true;
    }
    return key1.currentState!.isLoading;
  }

  @override
  Widget build(BuildContext context) {
    return Stack(
      children: <Widget>[
        Container(width: 500, height: 250, color: const Color.fromARGB(255, 232, 238, 238)),
        ProfileWidget(key: key1),
        ProfileWidget(key: key2),
      ],
    );
  }
}
