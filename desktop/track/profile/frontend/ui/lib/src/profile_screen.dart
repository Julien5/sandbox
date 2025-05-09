import 'package:flutter/material.dart';
import 'package:ui/src/profile_widget.dart';
import 'package:ui/src/counter.dart';
import 'package:ui/src/rust/api/frontend.dart';
import 'dart:developer' as developer;

class ProfileScreen extends StatefulWidget {
  const ProfileScreen({super.key});

  @override
  State<ProfileScreen> createState() => _ProfileScreenState();
}

class _ProfileScreenState extends State<ProfileScreen> {
  final GlobalKey<ProfileWidgetsState> _profilesKey =
      GlobalKey<ProfileWidgetsState>();
  Frontend? frontend;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      await _initialize();
    });
  }

  Future<void> _initialize() async {
    developer.log("start loadCircle");    
    await _profilesKey.currentState?.loadCircle();
    while (_profilesKey.currentState == null || _profilesKey.currentState!.isLoading()) {
       await Future.delayed(const Duration(milliseconds: 100));
       developer.log("waiting");
    }
    _initializeFrontend();
  }

  Future<void> _initializeFrontend() async {
    Frontend instance = await Frontend.create();
    setState(() {
      frontend = instance;
    });
    developer.log("make track and points");
    _makeTrack();
    _makeMorePoints();
  }

  void _makeTrack() {
    if (frontend == null) {
      developer.log(name: '_makeTrack', "frontend is null");
      return;
    }
    developer.log(name: '_makeTrack', "frontend is not null");
    Frontend f = frontend!;
    f.changeParameter(eps: -10.0);
    _profilesKey.currentState?.loadTrackProfile(f);
  }

  void _makeMorePoints() {
    if (frontend == null) {
      developer.log(name: '_makeMorePoints', "frontend is null");
      return;
    }
    developer.log(name: '_makeMorePoints', "frontend is not null");
    Frontend f = frontend!;
    f.changeParameter(eps: -10.0);
    _profilesKey.currentState?.loadTrackProfile(f);
    _profilesKey.currentState?.loadWaypointsProfile(f);
  }

  void _makeLessPoints() {
    if (frontend == null) {
      developer.log(name: '_makeLessPoints', "frontend is null");
      return;
    }
    developer.log(name: '_makeLessPoints', "frontend is not null");
    Frontend f = frontend!;
    f.changeParameter(eps: 10.0);
    _profilesKey.currentState?.loadWaypointsProfile(f);
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Container(
          padding: const EdgeInsets.all(16.0),
          decoration: BoxDecoration(
            border: Border.all(color: Colors.blue, width: 2.0),
            borderRadius: BorderRadius.circular(8.0),
          ),
          child: ProfileWidgets(key: _profilesKey),
        ),
        const SizedBox(height: 60),
        Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            PressButton(label: "more", onCounterPressed: _makeMorePoints),
            const SizedBox(width: 32),
            PressButton(label: "less", onCounterPressed: _makeLessPoints),
          ],
        ),
      ],
    );
  }
}
