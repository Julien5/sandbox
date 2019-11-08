#include <iostream>

class B {
public:
  virtual void foo() = 0;
};

class A : public B {
public:
  void foo() {
    std::cout << "A\n";
  };
};

void function(B& b) {
  b.foo();
}

int main(int,char**) {
  A a;
  function(a);
  return 0;
}
