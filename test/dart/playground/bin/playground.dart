import 'package:playground/playground.dart';

void main(List<String> arguments) {
  print('Hello world: ${calculate()}!');
  Person j = Person("julien");
  print('name: ${j.upper()}!');
  Singleton s1 = Singleton();
  Singleton s2 = Singleton();
  print('name: ${s1.p.name}!');
  print('name: ${s2.p.name}!');
}
