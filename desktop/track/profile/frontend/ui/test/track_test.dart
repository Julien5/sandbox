import 'package:flutter_test/flutter_test.dart';
import 'package:ui/src/rust/frb_generated.dart';
import 'package:ui/src/rust/api/bridge.dart';

bool testSvg(String svg) {
  return svg.length > 2000;
}

void main() {
  test('add() should return the sum of two integers', () {
    int result = 5;
    expect(result, equals(5));
  });
  test("This is async", () async {
    await RustLib.init();
    Bridge bridge = await Bridge.create();
    var S = bridge.segments();
    Segment s;
    expect(S.length, equals(6));
    for (Segment segment in S) {
      print("segment id: ${segment.id}");
      String svg = await bridge.renderSegmentTrack(segment: segment);
      print("svg length: ${svg.length}");
      expect(testSvg(svg), true);
    }
  }, timeout: Timeout(Duration(seconds: 5)));
}
