#include <iostream>
#include <memory>
#include <functional>
#include <factory.h>
#include <memoize.h>

// Simple example using factory pattern from design-patterns-cpp14
class Shape {
public:
    using factory = dp14::factory<Shape>;
    
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
namespace regCircle {
    Shape::factory::registrator<Circle> reg;
}

namespace regSquare {
    Shape::factory::registrator<Square> reg;
}

// Example using memoize pattern
class ExpensiveComputation {
public:
    static int fibonacci(int n) {
        // Simple recursive fibonacci without memoization for this example
        // The memoize pattern in the library is more advanced and requires
        // a proper T::memoize type definition
        if (n <= 1) return n;
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
};

int main() {
    std::cout << "=== design-patterns-cpp14 Example ===" << std::endl;
    std::cout << std::endl;
    
    // Factory Pattern Example
    std::cout << "Factory Pattern Example:" << std::endl;
    auto circle = Shape::factory::instance().create(Circle::KEY());
    auto square = Shape::factory::instance().create(Square::KEY());
    
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
