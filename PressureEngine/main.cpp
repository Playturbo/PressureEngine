#include <iostream>
#include "Math\Vectors\Vector2f.h"

int main() {
	
	Vector2f vec(2);
	Vector2f vec2(4, 5);

	vec.mul(vec2);

	std::cout << vec.x << ", " << vec.y << std::endl;
	std::cout << vec2.x << ", " << vec2.y << std::endl;

	std::cin.get();
	return 0;
}