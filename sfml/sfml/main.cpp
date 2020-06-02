#include <SFML/Graphics.hpp>
#include "view.h"
#include <iostream>
#include <sstream>
#include "mission.h"
#include "iostream"
#include "level.h"
#include <vector>
#include <list>

#include "TinyXML/tinyxml.h"

using namespace sf;
////////////////////////////////////����� ����� ��������//////////////////////////
class Entity {
public:
	std::vector<Object> obj;//������ �������� �����
	float dx, dy, x, y, speed, moveTimer;//�������� ���������� ������ ��� ������� �����
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;//����� ����� ���� ������, �� �� ����� ������ ������ ����� ��� �������������� �����.����� ���� �������� ������ �� ����� � ����� ������� ���� �������� � update � ����������� �� �����
	Entity(Image& image, String Name, float X, float Y, int W, int H)
	{
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}
	FloatRect getRect()
	{//�-��� ��������� ������������. ��� �����. � ������� 
		return FloatRect(x, y, w, h);//�-��� ��� �������� ������������
	}

	virtual void update(float time) = 0;// ��� ������� �����. ��� �������
};
////////////////////////////////////////////////////����� ������////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//��������� ��� ������������ - ��������� �������
	int playerScore, exp;//��� ���������� ����� ���� ������ � ������
	bool isShoot;

	Player(Image& image, String Name, Level& lev, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H)
	{
		playerScore = 0; state = stay; obj = lev.GetAllObjects(); exp = 0; health = 100;// ��������������. �������� ��� ������� ����������� � ����
		if (name == "Player1") {
			sprite.setTextureRect(IntRect(0, 137, w, h)); 
		}
	}

	void upExp()
	{
		exp = exp + 25;
	}

	void control()
	{
		if (Keyboard::isKeyPressed) {//���� ������ �������
			if (Keyboard::isKeyPressed(Keyboard::Left)) {//� ������ �����
				state = left; speed = 0.1;
				
				
				
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				state = right; speed = 0.1;
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//���� ������ ������� ����� � �� �� �����, �� ����� �������
				state = jump; dy = -0.6; onGround = false;//�������� ������ ������
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				state = down;
			}

			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				isShoot = true;
			}
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//� ��� �������� ������������ � ������
	{
		for (int i = 0; i < obj.size(); i++)//���������� �� ��������
			if (getRect().intersects(obj[i].rect))//��������� ����������� ������ � �������� 
			{
				if (obj[i].name == "solid")//���� ��������� �����������
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w;  dx = -0.1; sprite.scale(-1, 1); }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; sprite.scale(-1, 1); }
				}
			}
	}
float CurrentFrame = 0;
	void update(float time)
	{
			
		control();//������� ���������� ����������
		switch (state)//��� �������� ��������� �������� � ����������� �� ���������
		{
		case right:
		{
			dx = speed;
			CurrentFrame += 0.005 * time;
			if (CurrentFrame > 5) CurrentFrame -= 5;
			this->sprite.setTextureRect(IntRect(45 * int(CurrentFrame), 137, w, h));
			sprite.move(-0.1 * time, 0);
		
			break;//��������� ���� ������
		}
		//
		case left:
		{
			//CurrentFrame = 0;
			dx = -speed;
			CurrentFrame += 0.005 * time;
			if (CurrentFrame > 5) CurrentFrame -= 4;
			this->sprite.setTextureRect(IntRect(45 * int(CurrentFrame),+ 137, -w, h));
			sprite.move(-0.1 * time, 0);
			break;//��������� ���� �����
		}
		case up: break;//����� ��������� �������� ������ (�������� �� ��������)
		case down: dx = 0; break;//����� ��������� �� ����� ������ ��������� (�������� �� ��������)
		CurrentFrame = 9;
		/*case space: 
		{
			
			CurrentFrame += 0.005 * time;
			if (CurrentFrame > 14) CurrentFrame -= 5;
			this->sprite.setTextureRect(IntRect(45 * int(CurrentFrame), 343, 40, 35));
			sprite.move(-0.1 * time, 0);
			
			break;
		}*/
		case stay: dx = 0; break;//� ����� ����		
		}
		//state = stay;
		x += dx * time;
		checkCollisionWithMap(dx, 0);//������������ ������������ �� �
		y += dy * time;
		checkCollisionWithMap(0, dy);//������������ ������������ �� Y
		sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
		///////////////////////////////////////////////////////////////////////////////if (health <= 0) { life = false; }
		if (!isMove) { speed = 0; }
		//if (!onGround) { dy = dy + 0.0015*time; }//������� � ����� ������ ����������� � �����
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0015 * time;//��������� ������������� � �����
	}

	void boom(Entity* enemy)
	{
		Vector2f center = enemy->sprite.getPosition();
		if (center.x > this->sprite.getPosition().x) {
			// ����� ����� ������, ������� ������ �����
			state = left; speed = 100;
			dy = -0.5;
		}
		else {
			// ����� ����� �����, ������� ������ ������
			state = right; speed = 100;
			dy = -0.5;
		}
	}
};


class Enemy :public Entity {
	float CurrentFrame;
	int paddingleft = 25;
	int paggengTop = 25;
	int frameMargin = 10;
public:
	Enemy(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		obj = lvl.GetObjects("solid");// ������ ������� ��� �������������� ����� � ������
		if (name == "EasyEnemy") {
			sprite.setTextureRect(IntRect((w+frameMargin)*int(CurrentFrame)+paddingleft,paggengTop,w,h));
			dx = 0.1;//���� ��������.���� ������ ������ ���������

		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//� ��� �������� ������������ � ������
	{
		for (int i = 0; i < obj.size(); i++)//���������� �� ��������
			if (getRect().intersects(obj[i].rect))//��������� ����������� ������ � ��������
			{
				// if(obj[i].name == "solid") {//���� ������ ��������� � �������� �� solid 
				if (Dy > 0) { y = obj[i].rect.top - h; dy = 0; onGround = true; }
				if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height; dy = 0; }
				if (Dx > 0) { x = obj[i].rect.left - w; dx = -0.1; sprite.scale(-1, 1); }
				if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; sprite.scale(-1, 1); }
				//}
			}
	}

	void update(float time)
	{
		if (name == "EasyEnemy")
		{//��� ��������� � ����� ������ ������ ����� �����
			//float CurrentFrame = 0;
			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//������ ����������� �������� ������ 3 ���
			checkCollisionWithMap(dx, 0);//������������ ������������ �� �
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
			if (true ) {
				if (health <= 0) { life = false; }
				///////////////////////////////////////////////////////////////////////////////////////////////////
				if (life == true) {
					CurrentFrame += 0.005 * time;
					if (CurrentFrame > 5) CurrentFrame -= 5;
					this->sprite.setTextureRect(IntRect((w + frameMargin) * int(CurrentFrame) + paddingleft, paggengTop, w, h));
					//sprite.move(-0.1 * time, 0);
				}
			}
		}
	}
};

class Bullet :public Entity {//����� ����
public:
	float lifeDistance = 300;
	float startX;
	int direction;//����������� ����

	Bullet(Image& image, String Name, Level& lvl, float X, float Y, int W, int H, int dir) :Entity(image, Name, X, Y, W, H) {//�� ��� ��, ������ ����� � ����� ��������� ������ (int dir)
		obj = lvl.GetObjects("solid");//�������������� .�������� ������ ������� ��� �������������� ���� � ������
		startX = x = X;
		y = Y;
		direction = dir;
		speed = 0.8;
		w = h = 16;
		life = true;
		//���� ������������� � ������������
	}


	void update(float time)
	{
		switch (direction)
		{
		case 0: dx = -speed; dy = 0;   break;//������� �������� state = left
		case 1: dx = speed; dy = 0;   break;//������� �������� state = right
		case 2: dx = 0; dy = -speed;   break;//������� �������� state = up
		//case 3: dx = 0; dy = -speed;   break;//������� �������� �� ������� ��������� � �����������, ���� ������ ��������� �����, ��� ������ ��� �� �����
		//case 4: dx = 0; dy = -speed;   break;//������� �������� �� ������� ��������� � �����������, ���� ������ ��������� �����, ��� ������ ��� �� �����
		//case 5: dx = 0; dy = -speed;   break;//������� �������� �� ������� ��������� � �����������, ���� ������ ��������� �����, ��� ������ ��� �� �����
		}

		x += dx * time;//���� �������� ���� �� �
		//y += dy * time;//�� �

		if (x <= 0) x = 1;// �������� ���� � ����� �����, ����� ��� ���������� ������ ��� �������� �� �������� �� ������ ����� � �� ���� ������
		if (y <= 0) y = 1;

		for (int i = 0; i < obj.size(); i++) {//������ �� �������� solid
			if (getRect().intersects(obj[i].rect)) //���� ���� ������ ���������� � �����,
			{
				life = false;// �� ���� �������
			}
		}
		if (fabs(x - startX) > lifeDistance) { life = false; }
		sprite.setPosition(x + w / 2, y + h / 2);//�������� ������� ����
	}
};

class LifeBar //! �� ����� ����������� �� Entity, ����� �� ������� �� ����
{
public:
	Image image;
	Texture texture;  //! t => texture
	Sprite sprite;    //!!! s => sprite 
	int max;
	RectangleShape bar;

	LifeBar()  //! ����� ����� ������������ Entity. ������ ��������� �����������, �.�. � ������ ��� �� ������������
	{
		image.loadFromFile("images/life.png");
		//image.createMaskFromColor(Color(50, 96, 166));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(783, 2, 15, 84));

		bar.setFillColor(Color(0, 0, 0));//������ ������������� ������������� ������ � ���������� ������ ���������� ��������
		max = 100;
	}

	void update(int k)// k-������� ��������

	{
		if (k > 0)
			if (k < max)
				bar.setSize(Vector2f(10, (max - k) * 70 / max));//���� �� ������������ � ��� ���� ������ ���������, �� ������������� ����� �������� (����� ������) ��� ������� ��������������
	}

	void draw(RenderWindow& window)
	{
		Vector2f center = window.getView().getCenter();
		Vector2f size = window.getView().getSize();

		sprite.setPosition(center.x - size.x / 2 + 10, center.y - size.y / 2 + 10);//������� �� ������
		bar.setPosition(center.x - size.x / 2 + 14, center.y - size.y / 2 + 14);

		window.draw(sprite);//������� ������ ������� ��������
		window.draw(bar);//������ �� ��� ������ �������������, �� ��� �� ��������� �
	}

};

class Menu
{
public:
	Menu(RenderWindow& window) {
		Texture menuTexture1, menuTexture2, menuTexture3, infaTexture, menuBackground;
		menuTexture1.loadFromFile("images/111.png");
		menuTexture2.loadFromFile("images/222.png");
		menuTexture3.loadFromFile("images/333.png");
		infaTexture.loadFromFile("images/infa.png");
		menuBackground.loadFromFile("images/menu1.jpg");
		Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), infa(infaTexture), menuBg(menuBackground);
		bool isMenu = 1;
		int menuNum = 0;
		menu1.setPosition(350, 150);
		menu2.setPosition(1100, 700);
		menu3.setPosition(350, 200);
		menuBg.setPosition(0, 0);
		infa.setPosition(0, 0);
		//////////////////////////////����///////////////////
		while (isMenu)
		{
			menu1.setColor(Color::White);
			menu2.setColor(Color::White);
			menu3.setColor(Color::White);
			menuNum = 0;
			window.clear(Color(129, 181, 221));

			if (IntRect(350, 150, 180, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Green); menuNum = 1; }
			if (IntRect(1100, 700, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Yellow); menuNum = 2; }
			if (IntRect(350, 200, 120, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Red); menuNum = 3; }

			if (Mouse::isButtonPressed(Mouse::Left))
			{
				if (menuNum == 1) isMenu = false;//���� ������ ������ ������, �� ������� �� ���� 
				if (menuNum == 2) { window.draw(infa); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
				if (menuNum == 3) { window.close(); isMenu = false; }

			}

			window.draw(menuBg);
			window.draw(menu1);
			window.draw(menu2);
			window.draw(menu3);

			window.display();
		}
		////////////////////////////////////////////////////
	}

};


int main()
{
	RenderWindow window(VideoMode(1376, 768), "Game");
	view.reset(FloatRect(0, 0, 1376, 768));
	Menu menu(window);


	// LifeBar *lifeBarPlayer; //��������� ������ ������� ��������
	//! /|\
	//!  |
	//!  |
	//!  ��� �������� �� ��������� ������, � ��������� �� ���������. ��� ����� �� ������� ��������
	//!  � ��� �������� ���������.
	//!	 |
	//!  |
	//! \|/ 
	LifeBar lifeBarPlayer;

	Level lvl; //��������� ������ ������
	lvl.LoadFromFile("map.tmx");//!!!!�������� ����� �����

	Image heroImage;
	heroImage.loadFromFile("images/belka.gif");

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/shamaich.png");
	//easyEnemyImage.createMaskFromColor(Color(255, 255, 255));

	Image BulletImage;//����������� ��� ����
	BulletImage.loadFromFile("images/bullet.png");//��������� �������� � ������ �����������
	//BulletImage.createMaskFromColor(Color(0,0,0));//����� ��� ���� �� ������� �����

	std::list<Entity*>  entities;//������ ������, ���� ���� ������ �������.�������� ������.
	std::list<Entity*>::iterator it;//�������� ����� ��������� �� ��-��� ������
	std::list<Entity*>::iterator it2;

	std::vector<Object> e = lvl.GetObjects("EasyEnemy");//��� ������� ����� �� tmx ����� �������� � ���� �������

	for (int i = 0; i < e.size(); i++)//���������� �� ��������� ����� �������(� ������ �� ������)
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 80, 105));//� ���������� � ������ ���� ����� ������ � �����

	Object player = lvl.GetObject("player");//������ ������ �� ����� �����.������ ���������� ������ � ������ ��� ������ ����


	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 45, 35);//�������� ���������� �������������� player �� ����� � ���������� ������ ������

	sf::Time micro = sf::microseconds(10000);
	sf::Time milli = sf::milliseconds(10);
	sf::Time second = sf::seconds(0.01f);
	double elapsed1 = 0;
	double k = 10;

	Clock clock;
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 800;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, "Bullet", lvl, p.x, p.y, 16, 16, p.state)); }//���� ����������, �� ���������� ����. enum �������� ��� int 
		}
	
		p.update(time);// Player update function	

		for (it = entities.begin(); it != entities.end();)//������� ��� ���������� �� ������ �� �����
		{

			Entity* b = *it;//��� ��������, ����� �� ������ (*it)->
			//b->update(time);//�������� �-��� update ��� ���� �������� (�� ���� ��� ���, ��� ���)
			if (b->life == false) { it = entities.erase(it); delete b; }// ���� ���� ������ �����, �� ������� ���
			else it++;//� ���� �������� (����������) � ���� �������. ��� ������ �� ����� ��������� ������
		}


		//for (it = entities.begin(); it != entities.end(); it++)//���������� �� ��-��� ������
		//{
		//	if ((*it)->getRect().intersects(p.getRect()))//���� ������������� ������� ������� ������������ � �������
		//	{
		//		if ((*it)->name == "EasyEnemy") {//� ��� ���� ��� ������� EasyEnemy,��..
		//			if ((*it)->dx > 0)//���� ���� ���� ������
		//			{
		//				std::cout << "(*it)->x" << (*it)->x << "\n";//����� ������
		//				std::cout << "p.x" << p.x << "\n\n";//����� �����

		//				(*it)->x = p.x - (*it)->w; //����������� ��� �� ������ ����� (�������)
		//				(*it)->dx = 0;//�������������

		//				std::cout << "new (*it)->x" << (*it)->x << "\n";//����� ����� �����
		//				std::cout << "new p.x" << p.x << "\n\n";//����� ����� ������ (��������� �������)
		//			}
		//			if ((*it)->dx < 0)//���� ���� ���� �����
		//			{
		//				(*it)->x = p.x + p.w; //���������� - ����������� ������
		//				(*it)->dx = 0;//�������������
		//			}
		//		}
		//	}
		//}

		std::vector<Entity*> bullets;
		for (auto& item: entities) {
			if (item->name == "Bullet")bullets.push_back(item);
		}

		for (it = entities.begin(); it != entities.end(); it++)//���������� �� ��-��� ������
		{
			if ((*it)->name == "EasyEnemy") {
				if ((*it)->getRect().intersects(p.getRect()))//���� ������������� ������� ������� ������������ � �������
				{
					//� ��� ���� ��� ������� EasyEnemy,��..

					if ((p.dy > 0) && (p.onGround == false)) { (*it)->dx = 0; p.dy = -0.2; (*it)->health = 0; }//���� �������� �� �����,�� ���� ����� �������� 0,����������� �� ���� ���� �����,���� ��� �������� 0
					else {
						p.health -= 5; p.boom(*it);	//����� ���� ������� � ��� ����� � ����� ����

						//! ����� ��������� �������� ������ ��������� ��������� ������� ��������
						lifeBarPlayer.update(p.health);
					}
				}
				for (auto bullet = bullets.begin(); bullet != bullets.end(); bullet++) {
					if ((*it)->getRect().intersects((*bullet)->getRect())) {
						if ((*it)->life) {
							(*it)->health -= 15;
							(*bullet)->life = false;
							bullets.erase(bullet);
							bullet--;
						}
					}
				}
			}
		}


		for (it = entities.begin(); it != entities.end(); it++) { (*it)->update(time); }//��� ���� ��������� ������ ���������� �-��� update


		//if (easyEnemy.life == false) { p.upExp(); }
		window.setView(view);
		window.clear(Color(77, 83, 140));
		lvl.Draw(window);//������ �����


		for (it = entities.begin(); it != entities.end(); it++) { window.draw((*it)->sprite); } // ������ ������� �������


		lifeBarPlayer.draw(window);//������ ������� ��������
		window.draw(p.sprite);
		window.display();

		sf::Time elapsed = clock.getElapsedTime();
		elapsed1 = elapsed.asSeconds() + elapsed1;

		// ���� ������ ������ ��� ����� ��� �������� ������ 
		if (elapsed1 >= k)
		{
			// ������ ����� � ��������� � ����� �������
			// ��� ����� ����� ��������� � ����� � ��� �� �����
			for (int i = 0; i < e.size(); i++)
				entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 80, 105));
			// ������� ������ ������� �� �������� ����� ������
			elapsed1 = 0;
		}

		//std::cout << elapsed1 << std::endl;
		std::cout << p.health << std::endl;
	}

	return 0;
}

