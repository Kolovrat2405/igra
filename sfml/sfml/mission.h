///////////////////////////////////////////////////////////// НОМЕР МИССИИ //////////////////////////////////////////////////////
int getCurrentMission(int x)
{
	int mission = 0;
	if ((x > 0) && (x < 600)) { mission = 0; }//Знакомство с игрой
	if (x > 400) { mission = 1; }//1 миссия
	if (x > 700) { mission = 2; }//2 миссия
	if (x > 2200) { mission = 3; }// и т.д.

	return mission;
}
///////////////////////////////////////////////////////////// ТЕКСТ МИССИИ //////////////////////////////////////////////////////
std::string getTextMission(int currentMission)
{
	std::string missionText = ""; //Текст миссии и его инециализация
	switch (currentMission)
	{
	case 0: missionText = "\nНачальный этап \nИнструкция"; break;
	case 1: missionText = "\nМиссия 1 \nВот твоя \nпервая миссия"; break;
	case 2: missionText = "\nМиссия 2 \nВот твоя \nвторая миссия"; break;
	case 3: missionText = "\nМиссия 3 \nВот твоя \nтретья миссия"; break;
	}
	return missionText;//ф-ция возвращает текст
}