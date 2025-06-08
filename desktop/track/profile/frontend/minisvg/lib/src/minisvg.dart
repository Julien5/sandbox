import 'dart:developer' as developer;
import 'dart:io';
import 'dart:ui';

import 'package:flutter/widgets.dart';
import 'package:xml/xml.dart';

class Transform {
  double sx = 1.0;
  double sy = 1.0;
  double tx = 0.0;
  double ty = 0.0;
}

class Node {
  late Transform T;
  List<Element> children = [];

  late XmlElement e;
  Node(XmlElement pe) : e = pe {
    T = Transform(); //parse transform attributes if any
    if (e.attributes.isNotEmpty) {
      for (var attr in e.attributes) {
        switch (attr.name.local) {
          case 'transform':
            break;
          default:
            // Handle other attributes if necessary
            break;
        }
      }
    }
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

abstract class Element extends Node {
  Element(super.e);
  void paintElement(Canvas canvas, Size size);
}

class GroupElement extends Element {
  GroupElement(super.e) {
    // set super.T;
    for (var child in e.children) {
      if (child is XmlElement) {
        children.add(Node.fromXml(child));
      }
    }
    // unset super.T;
  }

  @override
  void paintElement(Canvas canvas, Size size) {
    developer.log('group paint ${super.e.name.local}');
    for (var child in children) {
      child.paintElement(canvas, size);
    }
  }
}

class PathElement extends Element {
  PathElement(super.e);

  @override
  void paintElement(Canvas canvas, Size size) {
    developer.log('path paint ${super.e.name.local}');
  }
}

class TextElement extends Element {
  TextElement(super.e);

  @override
  void paintElement(Canvas canvas, Size size) {
    developer.log('text paint ${super.e.name.local}');
  }
}

String mini() {
  /// read xml from file
  String xml = File('track-0.svg').readAsStringSync();
  XmlDocument doc = XmlDocument.parse(xml);
  Element root = Node.fromXml(doc.rootElement);
  Canvas canvas = Canvas(PictureRecorder());
  Size size = Size(100, 100); // Example size, adjust as needed
  root.paintElement(canvas, size);
  return "hi";
}
