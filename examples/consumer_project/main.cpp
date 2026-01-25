#include <iostream>
#include <factory.h>
#include <memoize.h>

// Simple example using factory pattern from design-patterns-cpp14
class Shape {
public:
    using Factory = dp14::factory<Shape, std::string>;
    
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

class Circle : public Shape {
public:
    DEFINE_KEY(Circle)
    
    void draw() const override {
        std::cout << "Drawing Circle" << std::endl;
    }
};

class Square : public Shape {
public:
    DEFINE_KEY(Square)
    
    void draw() const override {
        std::cout << "Drawing Square" << std::endl;
    }
};

// Register implementations
namespace {
    Shape::Factory::registrator<Circle> circle_reg;
    Shape::Factory::registrator<Square> square_reg;
}

// Example using memoize pattern
class ExpensiveComputation {
public:
    static int fibonacci(int n) {
        static auto fib = dp14::memoize<std::function<int(int)>>(
            [](int x) -> int {
                if (x <= 1) return x;
                return fibonacci(x - 1) + fibonacci(x - 2);
            }
        );
        return fib(n);
    }
};

int main() {
    std::cout << "=== design-patterns-cpp14 Example ===" << std::endl;
    std::cout << std::endl;
    
    // Factory Pattern Example
    std::cout << "Factory Pattern Example:" << std::endl;
    auto circle = Shape::Factory::instance().create(Circle::KEY());
    auto square = Shape::Factory::instance().create(Square::KEY());
    
    if (circle) circle->draw();
    if (square) square->draw();
    
    std::cout << std::endl;
    
    // Memoize Pattern Example
    std::cout << "Memoize Pattern Example (Fibonacci):" << std::endl;
    for (int i = 0; i <= 10; ++i) {
        std::cout << "Fibonacci(" << i << ") = " 
                  << ExpensiveComputation::fibonacci(i) << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "✅ Successfully used design-patterns-cpp14 from GitHub Packages!" << std::endl;
    
    return 0;
}
