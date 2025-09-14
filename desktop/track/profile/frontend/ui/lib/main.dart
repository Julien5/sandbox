import 'dart:developer' as developer;
import 'package:flutter/material.dart';
import 'package:package_info_plus/package_info_plus.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/backendmodel.dart';
import 'package:ui/src/choose_data.dart';
import 'package:ui/src/eventwidget.dart';
import 'package:ui/src/routes.dart';
import 'package:ui/src/rust/frb_generated.dart';

import 'package:window_size/window_size.dart';
import 'dart:io';
import 'package:flutter/foundation.dart'; // Import kIsWeb

Future<void> main() async {
  developer.log("START");
  WidgetsFlutterBinding.ensureInitialized();
  if (!kIsWeb) {
    if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
      setWindowFrame(Rect.fromLTWH(1400, 150, 500, 900));
    }
  }
  await RustLib.init();
  var rootModel=RootModel();
  await rootModel.init();
  assert(rootModel.segmentsProvider!=null);
  PackageInfo packageInfo = await PackageInfo.fromPlatform();
  developer.log("frontend loaded");
  runApp(Application(packageInfo: packageInfo,rootModel:rootModel));
}

class Application extends StatelessWidget {
  final PackageInfo? packageInfo;
  final RootModel? rootModel;
  const Application({super.key, required this.packageInfo,required this.rootModel});

  @override
  Widget build(BuildContext context) {
    assert(rootModel!=null);
    assert(rootModel!.segmentsProvider!=null);
    return ChangeNotifierProvider.value(
      value: rootModel!,
      builder: (context, child) {
        assert(rootModel!.segmentsProvider!=null);
        return MaterialApp(
          title: "WPX",
          onGenerateRoute: RouteManager.generateRoute,
          initialRoute: RouteManager.home,
          home: Scaffold(
            appBar: AppBar(title: EventProvider(model:rootModel!.eventModel)),
            body: HomePage(),
          ),
          theme: ThemeData(
            pageTransitionsTheme: PageTransitionsTheme(
              builders: {
                TargetPlatform.android: ZoomPageTransitionsBuilder(),
                TargetPlatform.iOS: CupertinoPageTransitionsBuilder(),
                TargetPlatform.linux: CupertinoPageTransitionsBuilder(),
                //TargetPlatform.linux: ZoomPageTransitionsBuilder(),
                //TargetPlatform.linux:PredictiveBackPageTransitionsBuilder(),
              },
            ),
          ),
        );
      },
    );
  }
}
