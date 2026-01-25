# Quick Setup - 5 Minutes

## Choose Your Installation Method

### ⭐ Easiest: Use Git URL (Recommended)

Add to your `conanfile.txt`:

```ini
[requires]
design-patterns-cpp14/1.0.24@

[generators]
CMakeDeps
CMakeToolchain
```

Then:
```bash
mkdir build && cd build
conan install .. --build=missing
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .
```

**That's it!** No authentication needed, no server setup. Conan handles everything.

---

### Alternative: Clone and Create

```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create .
```

Then in your project:
```ini
[requires]
design-patterns-cpp14/1.0.24
```

---

## Example CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject)

set(CMAKE_CXX_STANDARD 14)

# Conan-generated targets
find_package(design-patterns-cpp14 REQUIRED CONFIG)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE design-patterns-cpp14::design-patterns-cpp14)
```

## Example C++ Code

```cpp
#include <dp14/factory.h>
#include <memory>
#include <iostream>

class Animal {
public:
    using factory = dp14::factory<Animal, std::string>;
    
    explicit Animal(const std::string& name) : name_(name) {}
    virtual ~Animal() = default;
    virtual void speak() const = 0;
    
protected:
    std::string name_;
};

class Dog : public Animal {
public:
    DEFINE_KEY(Dog)
    explicit Dog(const std::string& name) : Animal(name) {}
    void speak() const override { 
        std::cout << name_ << ": Woof!\n"; 
    }
};

class Cat : public Animal {
public:
    DEFINE_KEY(Cat)
    explicit Cat(const std::string& name) : Animal(name) {}
    void speak() const override { 
        std::cout << name_ << ": Meow!\n"; 
    }
};

int main() {
    Animal::factory factory;
    Animal::factory::registrator<Dog> reg_dog(factory);
    Animal::factory::registrator<Cat> reg_cat(factory);
    
    auto dog = factory.create<Dog>("Buddy");
    auto cat = factory.create<Cat>("Whiskers");
    
    dog->speak();  // Output: Buddy: Woof!
    cat->speak();  // Output: Whiskers: Meow!
    
    return 0;
}
```

---

## Troubleshooting

**Q: "Conan can't find the package"**  
A: Make sure you have Conan 2.0+ and are using `design-patterns-cpp14/1.0.24@` (with the @ at the end)

**Q: "CMake can't find the toolchain file"**  
A: Run `conan install` first, then use the generated `conan_toolchain.cmake`

**Q: "Need a different C++ standard"**  
A: Conan respects your `CMakeLists.txt` settings. Just set `CMAKE_CXX_STANDARD` to what you need.

**Q: "Still having issues?"**  
A: Check [GITHUB_PACKAGES_TROUBLESHOOTING.md](GITHUB_PACKAGES_TROUBLESHOOTING.md) for detailed help

---

## Next Steps

- 📖 Read [GITHUB_PACKAGES.md](GITHUB_PACKAGES.md) for more options
- 💡 See the [example project](../examples/consumer_project/)
- 🐛 Report issues: https://github.com/makiolo/design-patterns-cpp14/issues
