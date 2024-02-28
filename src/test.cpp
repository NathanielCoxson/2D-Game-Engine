#include <iostream>
#include <fstream>

int main() {
	std::ifstream infile;
	infile.open("input.txt");
	int number;
	infile >> number;
	std::cout << number;
	return 0;
}