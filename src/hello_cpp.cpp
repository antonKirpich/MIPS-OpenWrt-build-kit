#include <cstdio>
#include <cstring>

class Greeter
{
public:
    Greeter(const char* name) : name_(name) {}

    void greet() const
    {
        std::printf("Hello, %s!\n", name_);
    }
private:
    const char* name_;
};

template<typename T>
T square(T x)
{
    return x * x;
}

int main(int argc, char** argv)
{
    const char* name = "MT7628";

    if (argc > 1)
    {
        name = argv[1];
    }

    Greeter g(name);
    g.greet();

    int value = 5;
    std::printf("Square(%d) = %d\n", value, square(value));

    return 0;
}