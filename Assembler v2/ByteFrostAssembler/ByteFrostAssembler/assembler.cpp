#include <unordered_map>
#include <iostream>
#include <string>

int main(int argc, char ** argv) {
	std::unordered_map<int, int> test;
	test[0] = -1;
	test[1] = 4;
	test[2] = 5;

	for (const auto & [key, val] : test)
		std::cout << "key: " << std::to_string(key) << " | value: " << std::to_string(val) << std::endl;
}