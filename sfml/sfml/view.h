#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;

void setPlayerCoordinateForView(float x, float y) 
{
	float tempX = x; float tempY = y;

	if (x < 400) tempX = 400;
	//if (y < 240) tempY = 240;//������� �������
	if (y > 480) tempY = 480;//������ �������.��� ����� �����

	view.setCenter(tempX, tempY);
}