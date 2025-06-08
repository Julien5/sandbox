import 'dart:developer' as developer;
import 'dart:io';
import 'dart:ui';

import 'package:flutter/foundation.dart';
import 'package:xml/xml.dart';

class Transform {
  static List<Transform> readAttribute(String s) {
    List<Transform> transforms = [];
    final transformRegex = RegExp(r'(translate\([^)]+\)|scale\([^)]+\))');

    // Match all transformations in order
    for (final match in transformRegex.allMatches(s)) {
      final transform = match.group(0)!;
      if (transform.startsWith('translate')) {
        transforms.add(Translate(transform));
      } else if (transform.startsWith('scale')) {
        transforms.add(Scale(transform));
      }
    }

    return transforms;
  }
}

class Scale extends Transform {
  double sx = 1.0;
  double sy = 1.0;
  Scale(String s) {
    developer.log(s);
    final scaleRegex = RegExp(r'scale\(([^,\s]+)[,\s]+([^)]+)\)');
    final scaleMatch = scaleRegex.firstMatch(s);
    assert(scaleMatch != null);
    sx = double.parse(scaleMatch!.group(1)!);
    sy = double.parse(scaleMatch.group(2)!);
  }
}

class Translate extends Transform {
  double tx = 0.0;
  double ty = 0.0;
  Translate(String s) {
    developer.log(s);
    final translateRegex = RegExp(r'translate\(([^,\s]+)[,\s]+([^)]+)\)');
    final translateMatch = translateRegex.firstMatch(s);
    assert(translateMatch != null);
    tx = double.parse(translateMatch!.group(1)!);
    ty = double.parse(translateMatch  .group(2)!);
  }
}

abstract class Element {
  List<Transform> T = [];
  List<Element> children = [];

  void paintElement(Canvas canvas, Size size);

  late XmlElement e;
  Element(XmlElement pe) : e = pe {
    if (e.attributes.isNotEmpty) {
      for (var attr in e.attributes) {
        switch (attr.name.local) {
          case 'transform':
            T = Transform.readAttribute(attr.value);
            break;
          default:
            // Handle other attributes if necessary
            break;
        }
      }
    }
  }

  void installTransforms(Canvas canvas) {
    canvas.save();
    for (var t in T) {
      if (t is Translate) {
        developer.log('install translate (${t.tx},${t.ty})');
        canvas.translate(t.tx, t.ty);
      }
      if (t is Scale) {
        developer.log('install scale (${t.sx},${t.sy})');
        canvas.scale(t.sx, t.sy);
      }
    }
  }

  void deinstallTransforms(Canvas canvas) {
    developer.log('desinstall transforms');
    canvas.restore();
  }

  static Element fromXml(XmlElement e) {
    if (e.name.local == "path") {
      return PathElement(e);
    } else if (e.name.local == "text") {
      return TextElement(e);
    } else if (e.name.local == "svg") {
      return GroupElement(e);
    } else if (e.name.local == "g") {
      return GroupElement(e);
    } else {
      throw Exception("Unknown element type: ${e.name}");
    }
  }
}

class GroupElement extends Element {
  GroupElement(super.e) {
    for (var child in e.children) {
      if (child is XmlElement) {
        children.add(Element.fromXml(child));
      }
    }
  }

  @override
  void paintElement(Canvas canvas, Size size) {
    developer.log('group paint ${super.e.name.local}');
    installTransforms(canvas);
    for (var child in children) {
      child.paintElement(canvas, size);
    }
    deinstallTransforms(canvas);
  }
}

class PathElement extends Element {
  PathElement(super.e);

  @override
  void paintElement(Canvas canvas, Size size) {
    installTransforms(canvas);
    developer.log('path paint ${super.e.name.local}');
    deinstallTransforms(canvas);
  }
}

class TextElement extends Element {
  TextElement(super.e);

  @override
  void paintElement(Canvas canvas, Size size) {
    installTransforms(canvas);
    developer.log('text paint ${super.e.name.local}');
    deinstallTransforms(canvas);
  }
}

String mini() {
  /// read xml from file
  String xml = File('test.svg').readAsStringSync();
  XmlDocument doc = XmlDocument.parse(xml);
  Element root = Element.fromXml(doc.rootElement);
  Canvas canvas = Canvas(PictureRecorder());
  Size size = Size(100, 100); // Example size, adjust as needed
  root.paintElement(canvas, size);
  return "hi";
}
