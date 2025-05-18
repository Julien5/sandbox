int calculate() {
  return 6 * 7;
}

class Person {
  final String name;
  const Person(this.name);
  String upper() {
    return name.toUpperCase();
  }
}

class Singleton {
  // see https://dart.dev/language/constructors#factory-constructors
  static final Singleton _singleton = Singleton._internal(Person("joe"));

  factory Singleton() {
    return _singleton;
  }

  final Person p;
  Singleton._internal(this.p);
}
