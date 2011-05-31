#include <iostream>
#include "../../delegate/Delegate2.h"

struct Test {
	static double sum(double a, double b) {
		std::cout << "static function sum(" << a << ", " << b << ")\n";
		return a + b;
	}

	double substract(double a, double b) {
		std::cout << "member function substract(" << a << ", " << b << ")\n";
		return a - b;
	}

	double multiply(double a, double b) const {
		std::cout << "const member function multiply(" << a << ", " << b << ")\n";
	}
};

double divide(double a, double b) {
	std::cout << "global function divide(" << a << ", " << b << ")\n";
	return a / b;
}

int main(void) {
	delegate::Delegate2<double, double, double> del(Test::sum);
	Test t;
	del.add(t, &Test::substract);
	del.add(t, &Test::multiply);
	del.add(divide);

	std::cout << "return value: " << del(2, 3) << '\n';

	std::cout << "remove all\n";
	del.remove(Test::sum);
	del.remove(t, &Test::substract);
	del.remove(t, &Test::multiply);
	del.remove(divide);

	std::cout << "call again, return value: " << del(2, 3) << '\n';

	return 0;
}
