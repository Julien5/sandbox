import 'dart:io';

/// Reads a file from the given [filePath] and returns its content as a String.
String readFileAsStringSync(String filePath) {
  try {
    final file = File(filePath);
    return file.readAsStringSync();
  } catch (e) {
    throw Exception('Error reading file: $e');
  }
}

String getProfile() {
  return readFileAsStringSync('profile-0.svg');
}