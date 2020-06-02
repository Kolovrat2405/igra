#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "TinyXML/tinyxml.h"

sf::Color hexToColor(unsigned int hexValue)
{
	sf::Color rgbColor;
	rgbColor.r = ((hexValue >> 16) & 0xFF);  // Extract the RR byte
	rgbColor.g = ((hexValue >> 8) & 0xFF);   // Extract the GG byte
	rgbColor.b = ((hexValue) & 0xFF);        // Extract the BB byte

	return rgbColor;
}

struct Object
{
	int GetPropertyInt(std::string name);//����� �������� ������� � ����� ������
	float GetPropertyFloat(std::string name);
	std::string GetPropertyString(std::string name);

	std::string name;//�������� ���������� name ���� string
	std::string type;//� ����� ���������� type ���� string
	sf::Rect<float> rect;//��� Rect � �������� ����������
	std::map<std::string, std::string> properties;//������ ������������ ������. ���� - ��������� ���, �������� - ���������

	sf::Sprite sprite;//�������� ������
};

struct Layer//����
{
	int opacity;//�������������� ����
	std::vector<sf::Sprite> tiles;//���������� � ������ �����
};

class TileSet {
public:
	int tileWidth;
	int	tileHeight;
	sf::Texture texImage;
	std::vector<sf::Rect<int>> subRects;
	int firstTileID;
	bool loadFromFile(int firstTileID, std::string pathToTileset)
	{
		this->firstTileID = firstTileID;
		// ��������� �������
		TiXmlDocument tilesetFile(pathToTileset.c_str());
		if (!tilesetFile.LoadFile())//���� �� ������� ��������� �����
		{
			std::cout << "Loading tileset \"" << pathToTileset << "\" failed." << std::endl; //������ ������
			return false;
		}

		// source - ���� �� �������� � ���������� image
		TiXmlElement* image;
		image = tilesetFile.FirstChildElement("tileset")->FirstChildElement("image");
		std::string imagepath = image->Attribute("source");

		// �������� ��������� ��������
		sf::Image img;

		if (!img.loadFromFile(imagepath))
		{
			std::cout << "Failed to load tile sheet. Path: " << imagepath << std::endl; //���� �� ������� ��������� �������-������� ������ � �������
			return false;
		}

		const char* trans = image->Attribute("trans");
		if (trans != nullptr) {
			img.createMaskFromColor(hexToColor(strtol(trans, NULL, 16))); //��� ����� �����.������ ��� �����
		}
		texImage.loadFromImage(img);
		texImage.setSmooth(false);//�����������

		// �������� ���������� �������� � ����� ��������
		tileWidth = atoi(tilesetFile.FirstChildElement("tileset")->Attribute("tilewidth"));
		tileHeight = atoi(tilesetFile.FirstChildElement("tileset")->Attribute("tileheight"));
		int columns = texImage.getSize().x / tileWidth;
		int rows = texImage.getSize().y / tileHeight;

		// ������ �� ��������������� ����������� (TextureRect)
		subRects.clear();
		for (int y = 0; y < rows; y++)
			for (int x = 0; x < columns; x++)
			{
				sf::Rect<int> rect;

				rect.top = y * tileHeight;
				rect.height = tileHeight;
				rect.left = x * tileWidth;
				rect.width = tileWidth;

				subRects.push_back(rect);
			}
		return true;
	}
	sf::Rect<int>& getRect(int ID) {
		return subRects[ID - firstTileID];
	}
};

class Level//������� ����� - �������
{
public:
	bool LoadFromFile(std::string filename);//���������� false ���� �� ���������� ���������
	Object GetObject(std::string name);
	std::vector<Object> GetObjects(std::string name);//������ ������ � ��� �������
	std::vector<Object> GetAllObjects();//������ ��� ������� � ��� �������
	void Draw(sf::RenderWindow& window);//������ � ����
	sf::Vector2i GetTileSize();//�������� ������ �����
	TileSet& findTileSet(int tileGID);
private:
	int width, height, tileWidth, tileHeight;//� tmx ����� width height � ������,����� ������ �����
	sf::Rect<float> drawingBounds;//������ ����� ����� ������� ������
	std::vector<Object> objects;//������ ���� �������, ������� �� �������
	std::vector<TileSet> tileSet;
	std::vector<Layer> layers;
};

///////////////////////////////////////


int Object::GetPropertyInt(std::string name)//���������� ����� �������� � ����� ������
{
	return atoi(properties[name].c_str());
}

float Object::GetPropertyFloat(std::string name)
{
	return strtod(properties[name].c_str(), NULL);
}

std::string Object::GetPropertyString(std::string name)//�������� ��� � ���� ������.����� �������
{
	return properties[name];
}

bool Level::LoadFromFile(std::string filename)//���������-��������� � ������� ������ ��� ������ 
{
	TiXmlDocument levelFile(filename.c_str());//��������� ���� � TiXmlDocument
	// ��������� XML-�����
	if (!levelFile.LoadFile())//���� �� ������� ��������� �����
	{
		std::cout << "Loading level \"" << filename << "\" failed." << std::endl;//������ ������
		return false;
	}

	// �������� � ����������� map
	TiXmlElement* map;
	map = levelFile.FirstChildElement("map");

	// ������ �����: <map version="1.0" orientation="orthogonal"
	// width="10" height="10" tilewidth="34" tileheight="34">
	width = atoi(map->Attribute("width"));//��������� �� ����� ����� �� ��������
	height = atoi(map->Attribute("height"));//�� ��������, ������� �������� ��� ������ � 
	tileWidth = atoi(map->Attribute("tilewidth"));//������� ���������
	tileHeight = atoi(map->Attribute("tileheight"));


	TiXmlElement* tilesetElement;
	tilesetElement = map->FirstChildElement("tileset");
	while (tilesetElement)
	{
		int firstTileID = atoi(tilesetElement->Attribute("firstgid"));
		std::string tilesetPath = tilesetElement->Attribute("source");

		// ��������� �������
		TileSet ts;
		if (ts.loadFromFile(firstTileID, tilesetPath)) tileSet.push_back(ts);
		else return false;

		tilesetElement = tilesetElement->NextSiblingElement("tileset");
	}

	// ������ �� ������
	TiXmlElement* layerElement;
	layerElement = map->FirstChildElement("layer");
	while (layerElement)
	{
		Layer layer;

		// ���� ������������ opacity, �� ������ ������������ ����, ����� �� ��������� �����������
		if (layerElement->Attribute("opacity") != NULL)
		{
			float opacity = strtod(layerElement->Attribute("opacity"), NULL);
			layer.opacity = 255 * opacity;
		}
		else
		{
			layer.opacity = 255;
		}

		//  ��������� <data> 
		TiXmlElement* layerDataElement;
		layerDataElement = layerElement->FirstChildElement("data");

		if (layerDataElement == NULL)
		{
			std::cout << "Bad map. No layer information found." << std::endl;
		}

		//  ��������� <tile> - �������� ������ ������� ����
		TiXmlElement* tileElement;
		tileElement = layerDataElement->FirstChildElement("tile");

		if (tileElement == NULL)
		{
			std::cout << "Bad map. No tile information found." << std::endl;
			return false;
		}

		int x = 0;
		int y = 0;
		while (tileElement)
		{
			if (tileElement->Attribute("gid") != nullptr) {
				int tileGID = atoi(tileElement->Attribute("gid"));

				TileSet& currentTileSet = findTileSet(tileGID);

				// ������������� TextureRect ������� �����
				sf::Sprite sprite;
				sprite.setTexture(currentTileSet.texImage);
				sprite.setTextureRect(currentTileSet.getRect(tileGID));
				sprite.setPosition(x * tileWidth, y * tileHeight);
				//sprite.setColor(sf::Color(255, 255, 255, layer.opacity));

				layer.tiles.push_back(sprite);//���������� � ���� ������� ������
			}
			tileElement = tileElement->NextSiblingElement("tile");
			
			x++;
			if (x >= this->width) {
				y++;
				x = 0;
			}
		}

		layers.push_back(layer);

		layerElement = layerElement->NextSiblingElement("layer");
	}

	// ������ � ���������
	TiXmlElement* objectGroupElement;

	// ���� ���� ���� ��������
	if (map->FirstChildElement("objectgroup") != NULL)
	{
		objectGroupElement = map->FirstChildElement("objectgroup");
		while (objectGroupElement)
		{
			//  ��������� <object>
			TiXmlElement* objectElement;
			objectElement = objectGroupElement->FirstChildElement("object");

			while (objectElement)
			{
				// �������� ��� ������ - ���, ���, �������, � ��
				std::string objectType;
				if (objectElement->Attribute("type") != NULL)
				{
					objectType = objectElement->Attribute("type");
				}
				std::string objectName;
				if (objectElement->Attribute("name") != NULL)
				{
					objectName = objectElement->Attribute("name");
				}
				int x = atoi(objectElement->Attribute("x"));
				int y = atoi(objectElement->Attribute("y"));

				int width, height;

				sf::Sprite sprite;
//				sprite.setTexture(tilesetImage);
//				sprite.setTextureRect(sf::Rect<int>(0, 0, 0, 0));
				sprite.setPosition(x, y);

				if (objectElement->Attribute("width") != NULL)
				{
					width = atoi(objectElement->Attribute("width"));
					height = atoi(objectElement->Attribute("height"));
				}
/*				else
				{
					width = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].width;
					height = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].height;
					sprite.setTextureRect(subRects[atoi(objectElement->Attribute("gid")) - firstTileID]);
				}
*/
				// ��������� �������
				Object object;
				object.name = objectName;
				object.type = objectType;
				object.sprite = sprite;

				sf::Rect <float> objectRect;
				objectRect.top = y;
				objectRect.left = x;
				objectRect.height = height;
				objectRect.width = width;
				object.rect = objectRect;

				// "����������" �������
				TiXmlElement* properties;
				properties = objectElement->FirstChildElement("properties");
				if (properties != NULL)
				{
					TiXmlElement* prop;
					prop = properties->FirstChildElement("property");
					if (prop != NULL)
					{
						while (prop)
						{
							std::string propertyName = prop->Attribute("name");
							std::string propertyValue = prop->Attribute("value");

							object.properties[propertyName] = propertyValue;

							prop = prop->NextSiblingElement("property");
						}
					}
				}


				objects.push_back(object);

				objectElement = objectElement->NextSiblingElement("object");
			}
			objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
		}
	}
	else
	{
		std::cout << "No object layers found..." << std::endl;
	}

	return true;
}

TileSet& Level::findTileSet(int tileGID) {
	TileSet* res = nullptr;
	for (auto& ts : tileSet) {
		if (ts.firstTileID <= tileGID) res = &ts;
		else break;
	}
	if (res == nullptr) std::cout << "findTileSet: error" << std::endl;
	return *res;
}

Object Level::GetObject(std::string name)
{
	// ������ ������ ������ � �������� ������
	for (int i = 0; i < objects.size(); i++)
		if (objects[i].name == name)
			return objects[i];
}

std::vector<Object> Level::GetObjects(std::string name)
{
	// ��� ������� � �������� ������
	std::vector<Object> vec;
	for (int i = 0; i < objects.size(); i++)
		if (objects[i].name == name)
			vec.push_back(objects[i]);

	return vec;
}


std::vector<Object> Level::GetAllObjects()
{
	return objects;
};


sf::Vector2i Level::GetTileSize()
{
	return sf::Vector2i(tileWidth, tileHeight);
}

void Level::Draw(sf::RenderWindow& window)
{
	// ������ ��� ����� (������� �� ������!)
	for (int layer = 0; layer < layers.size(); layer++)
		for (int tile = 0; tile < layers[layer].tiles.size(); tile++)
			window.draw(layers[layer].tiles[tile]);
}

#endif
