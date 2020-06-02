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
////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
	std::vector<Object> obj;//вектор объектов карты
	float dx, dy, x, y, speed, moveTimer;//добавили переменную таймер для будущих целей
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;//враги могут быть разные, мы не будем делать другой класс для различающегося врага.всего лишь различим врагов по имени и дадим каждому свое действие в update в зависимости от имени
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
	{//ф-ция получения треугольника. его коорд. и размеры 
		return FloatRect(x, y, w, h);//ф-ция для проверки столкновений
	}

	virtual void update(float time) = 0;// Все потомки опред. эту функцию
};
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//добавляем тип перечисления - состояние объекта
	int playerScore, exp;//эта переменная может быть только у игрока
	bool isShoot;

	Player(Image& image, String Name, Level& lev, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H)
	{
		playerScore = 0; state = stay; obj = lev.GetAllObjects(); exp = 0; health = 100;// Инициализируем. получаем все объекты взаимодейст в перс
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
		if (Keyboard::isKeyPressed) {//если нажата клавиша
			if (Keyboard::isKeyPressed(Keyboard::Left)) {//а именно левая
				state = left; speed = 0.1;
				
				
				
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				state = right; speed = 0.1;
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//если нажата клавиша вверх и мы на земле, то можем прыгать
				state = jump; dy = -0.6; onGround = false;//увеличил высоту прыжка
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				state = down;
			}

			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				isShoot = true;
			}
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = 0; i < obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом 
			{
				if (obj[i].name == "solid")//если встретили препятствие
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
			
		control();//функция управления персонажем
		switch (state)//тут делаются различные действия в зависимости от состояния
		{
		case right:
		{
			dx = speed;
			CurrentFrame += 0.005 * time;
			if (CurrentFrame > 5) CurrentFrame -= 5;
			this->sprite.setTextureRect(IntRect(45 * int(CurrentFrame), 137, w, h));
			sprite.move(-0.1 * time, 0);
		
			break;//состояние идти вправо
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
			break;//состояние идти влево
		}
		case up: break;//будет состояние поднятия наверх (например по лестнице)
		case down: dx = 0; break;//будет состояние во время спуска персонажа (например по лестнице)
		CurrentFrame = 9;
		/*case space: 
		{
			
			CurrentFrame += 0.005 * time;
			if (CurrentFrame > 14) CurrentFrame -= 5;
			this->sprite.setTextureRect(IntRect(45 * int(CurrentFrame), 343, 40, 35));
			sprite.move(-0.1 * time, 0);
			
			break;
		}*/
		case stay: dx = 0; break;//и здесь тоже		
		}
		//state = stay;
		x += dx * time;
		checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
		y += dy * time;
		checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
		sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
		///////////////////////////////////////////////////////////////////////////////if (health <= 0) { life = false; }
		if (!isMove) { speed = 0; }
		//if (!onGround) { dy = dy + 0.0015*time; }//убираем и будем всегда притягивать к земле
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0015 * time;//постоянно притягиваемся к земле
	}

	void boom(Entity* enemy)
	{
		Vector2f center = enemy->sprite.getPosition();
		if (center.x > this->sprite.getPosition().x) {
			// центр врага правее, двигаем игрока влево
			state = left; speed = 100;
			dy = -0.5;
		}
		else {
			// центр врага левее, двигаем игрока вправо
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
		obj = lvl.GetObjects("solid");// нужные объекты для взаимодействия врага с картой
		if (name == "EasyEnemy") {
			sprite.setTextureRect(IntRect((w+frameMargin)*int(CurrentFrame)+paddingleft,paggengTop,w,h));
			dx = 0.1;//даем скорость.этот объект всегда двигается

		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = 0; i < obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				// if(obj[i].name == "solid") {//если встрет препятств с объектом им solid 
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
		{//для персонажа с таким именем логика будет такой
			//float CurrentFrame = 0;
			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
			checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
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

class Bullet :public Entity {//класс пули
public:
	float lifeDistance = 300;
	float startX;
	int direction;//направление пули

	Bullet(Image& image, String Name, Level& lvl, float X, float Y, int W, int H, int dir) :Entity(image, Name, X, Y, W, H) {//всё так же, только взяли в конце состояние игрока (int dir)
		obj = lvl.GetObjects("solid");//инициализируем .получаем нужные объекты для взаимодействия пули с картой
		startX = x = X;
		y = Y;
		direction = dir;
		speed = 0.8;
		w = h = 16;
		life = true;
		//выше инициализация в конструкторе
	}


	void update(float time)
	{
		switch (direction)
		{
		case 0: dx = -speed; dy = 0;   break;//интовое значение state = left
		case 1: dx = speed; dy = 0;   break;//интовое значение state = right
		case 2: dx = 0; dy = -speed;   break;//интовое значение state = up
		//case 3: dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
		//case 4: dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
		//case 5: dx = 0; dy = -speed;   break;//интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
		}

		x += dx * time;//само движение пули по х
		//y += dy * time;//по у

		if (x <= 0) x = 1;// задержка пули в левой стене, чтобы при проседании кадров она случайно не вылетела за предел карты и не было ошибки
		if (y <= 0) y = 1;

		for (int i = 0; i < obj.size(); i++) {//проход по объектам solid
			if (getRect().intersects(obj[i].rect)) //если этот объект столкнулся с пулей,
			{
				life = false;// то пуля умирает
			}
		}
		if (fabs(x - startX) > lifeDistance) { life = false; }
		sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
	}
};

class LifeBar //! не нужно наследовать от Entity, класс не зависти от него
{
public:
	Image image;
	Texture texture;  //! t => texture
	Sprite sprite;    //!!! s => sprite 
	int max;
	RectangleShape bar;

	LifeBar()  //! Убран вызов конструктора Entity. Убраны параметры консруктора, т.к. в классе они не сипользуются
	{
		image.loadFromFile("images/life.png");
		//image.createMaskFromColor(Color(50, 96, 166));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(783, 2, 15, 84));

		bar.setFillColor(Color(0, 0, 0));//черный прямоугольник накладывается сверху и появляется эффект отсутствия здоровья
		max = 100;
	}

	void update(int k)// k-текущее здоровье

	{
		if (k > 0)
			if (k < max)
				bar.setSize(Vector2f(10, (max - k) * 70 / max));//если не отрицательно и при этом меньше максимума, то устанавливаем новое значение (новый размер) для черного прямоугольника
	}

	void draw(RenderWindow& window)
	{
		Vector2f center = window.getView().getCenter();
		Vector2f size = window.getView().getSize();

		sprite.setPosition(center.x - size.x / 2 + 10, center.y - size.y / 2 + 10);//позиция на экране
		bar.setPosition(center.x - size.x / 2 + 14, center.y - size.y / 2 + 14);

		window.draw(sprite);//сначала рисуем полоску здоровья
		window.draw(bar);//поверх неё уже черный прямоугольник, он как бы покрывает её
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
		//////////////////////////////МЕНЮ///////////////////
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
				if (menuNum == 1) isMenu = false;//если нажали первую кнопку, то выходим из меню 
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


	// LifeBar *lifeBarPlayer; //экземпляр класса полоски здоровья
	//! /|\
	//!  |
	//!  |
	//!  Тут создаётся не экземпляр класса, а указатель на экземпляр. Это видно по наличию заёздочки
	//!  А тут создаётся экземпляр.
	//!	 |
	//!  |
	//! \|/ 
	LifeBar lifeBarPlayer;

	Level lvl; //экземпляр класса уровня
	lvl.LoadFromFile("map.tmx");//!!!!Загрузка файла карты

	Image heroImage;
	heroImage.loadFromFile("images/belka.gif");

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/shamaich.png");
	//easyEnemyImage.createMaskFromColor(Color(255, 255, 255));

	Image BulletImage;//изображение для пули
	BulletImage.loadFromFile("images/bullet.png");//загрузили картинку в объект изображения
	//BulletImage.createMaskFromColor(Color(0,0,0));//маска для пули по черному цвету

	std::list<Entity*>  entities;//создаю список, сюда буду кидать объекты.например врагов.
	std::list<Entity*>::iterator it;//итератор чтобы проходить по эл-там списка
	std::list<Entity*>::iterator it2;

	std::vector<Object> e = lvl.GetObjects("EasyEnemy");//все объекты врага на tmx карте хранятся в этом векторе

	for (int i = 0; i < e.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 80, 105));//и закидываем в список всех наших врагов с карты

	Object player = lvl.GetObject("player");//объект игрока на нашей карте.задаем координаты игроку в начале при помощи него


	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 45, 35);//передаем координаты прямоугольника player из карты в координаты нашего игрока

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
			if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, "Bullet", lvl, p.x, p.y, 16, 16, p.state)); }//если выстрелили, то появляется пуля. enum передаем как int 
		}
	
		p.update(time);// Player update function	

		for (it = entities.begin(); it != entities.end();)//говорим что проходимся от начала до конца
		{

			Entity* b = *it;//для удобства, чтобы не писать (*it)->
			//b->update(time);//вызываем ф-цию update для всех объектов (по сути для тех, кто жив)
			if (b->life == false) { it = entities.erase(it); delete b; }// если этот объект мертв, то удаляем его
			else it++;//и идем курсором (итератором) к след объекту. так делаем со всеми объектами списка
		}


		//for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
		//{
		//	if ((*it)->getRect().intersects(p.getRect()))//если прямоугольник спрайта объекта пересекается с игроком
		//	{
		//		if ((*it)->name == "EasyEnemy") {//и при этом имя объекта EasyEnemy,то..
		//			if ((*it)->dx > 0)//если враг идет вправо
		//			{
		//				std::cout << "(*it)->x" << (*it)->x << "\n";//коорд игрока
		//				std::cout << "p.x" << p.x << "\n\n";//коорд врага

		//				(*it)->x = p.x - (*it)->w; //отталкиваем его от игрока влево (впритык)
		//				(*it)->dx = 0;//останавливаем

		//				std::cout << "new (*it)->x" << (*it)->x << "\n";//новая коорд врага
		//				std::cout << "new p.x" << p.x << "\n\n";//новая коорд игрока (останется прежней)
		//			}
		//			if ((*it)->dx < 0)//если враг идет влево
		//			{
		//				(*it)->x = p.x + p.w; //аналогично - отталкиваем вправо
		//				(*it)->dx = 0;//останавливаем
		//			}
		//		}
		//	}
		//}

		std::vector<Entity*> bullets;
		for (auto& item: entities) {
			if (item->name == "Bullet")bullets.push_back(item);
		}

		for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
		{
			if ((*it)->name == "EasyEnemy") {
				if ((*it)->getRect().intersects(p.getRect()))//если прямоугольник спрайта объекта пересекается с игроком
				{
					//и при этом имя объекта EasyEnemy,то..

					if ((p.dy > 0) && (p.onGround == false)) { (*it)->dx = 0; p.dy = -0.2; (*it)->health = 0; }//если прыгнули на врага,то даем врагу скорость 0,отпрыгиваем от него чуть вверх,даем ему здоровье 0
					else {
						p.health -= 5; p.boom(*it);	//иначе враг подошел к нам сбоку и нанес урон

						//! После изменения здоровья игрока обновляем состояние полоски здоровья
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


		for (it = entities.begin(); it != entities.end(); it++) { (*it)->update(time); }//для всех элементов списка активируем ф-цию update


		//if (easyEnemy.life == false) { p.upExp(); }
		window.setView(view);
		window.clear(Color(77, 83, 140));
		lvl.Draw(window);//рисуем карту


		for (it = entities.begin(); it != entities.end(); it++) { window.draw((*it)->sprite); } // рисуем энтитис объекты


		lifeBarPlayer.draw(window);//рисуем полоску здоровья
		window.draw(p.sprite);
		window.display();

		sf::Time elapsed = clock.getElapsedTime();
		elapsed1 = elapsed.asSeconds() + elapsed1;

		// Если прошло больше или равно чем интервал спавна 
		if (elapsed1 >= k)
		{
			// Создаём врага и добавляем в конец массива
			// Все враги будут появлятся в одной и той же точке
			for (int i = 0; i < e.size(); i++)
				entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 80, 105));
			// Наинаем отсчёт вермени до создания врага заново
			elapsed1 = 0;
		}

		//std::cout << elapsed1 << std::endl;
		std::cout << p.health << std::endl;
	}

	return 0;
}

