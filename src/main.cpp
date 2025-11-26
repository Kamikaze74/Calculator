#include "Calculator.cpp"

int main() {
    Calculator calc;
    std::string expression = "3 + 4 * (2 - 1) * -5 / 2.5";
    std::cout << "Das Ergebnis von " << expression << " ist: " << calc.calculate(expression) << std::endl;
    return 0;
}