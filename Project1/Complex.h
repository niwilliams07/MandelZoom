class Complex {
public:
	double x;
	double y;

	Complex(double real, double imaginary) :
		x(real), y(imaginary) {};

	Complex operator+ (Complex that) {
		float real = x + that.x;
		float imaginary = y + that.y;
		return Complex(real, imaginary);
	}

	Complex operator* (Complex that) {
		float real = (x * that.x) - (y * that.y);
		float imaginary = (x * that.y) + (y * that.x);
		return Complex(real, imaginary);
	}
};

