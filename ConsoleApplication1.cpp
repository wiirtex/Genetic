#include <vector>
#include <iostream>
#include <windows.h>
#include <wincon.h>
#include <iomanip>
#include <set>
#include <time.h>

using namespace std;
typedef double dd;


vector<vector<int>> pole; // Игровое поле - то, где всё происходит
int y = 30, x = 15, n = 2, sitnost = 10; // границы поля, количество точек, заспавненных изначально, сытность каждого кусочка еды

int randNapr() { //задаёт случайное направление для движения живой точки
	int t = rand() % 3;	
	return t - 1;	// по х: влево (-1), вправо(1) или никуда (0)
}

struct Point { // Структура точек. Используется для ЕДЫ
	int x_f, y_f;

};
bool operator<(const Point& left, const Point& right) { //компаратор для того, чтобы вся ЕДА могла храниться в сете.  
	if (left.x_f < right.x_f)	//Сортируется по х координате ЕДЫ
		return true;
	return false;
}

int newNpr(int food_coordinate, int alivePoint_coordinate) {	//Выдаёт новое направление по 1 координате. 
	if (food_coordinate != alivePoint_coordinate)
		return (food_coordinate - alivePoint_coordinate) / abs(food_coordinate - alivePoint_coordinate);
	return 0;
}

set<Point> food;	// сет для хранения точек, где есть еда

class AlivePoint {		// класс живых точек
private:
	int x_p, y_p;		//координаты живой точки
	int speed;			// пока что скорость живой точки
	int lifeTime, maxLifeTime;	//сколько прожила точка и сколько ей осталось
	int napr_x, napr_y; // направление двжения точки по каждой координате


	void setOnPole() {	//метод, чтобы установить точку на поле

		pole[x_p][y_p] = 1;
	}
	void runFromPole() {
		pole[x_p][this->y_p] = 0;	//метод, чтобы убрать точку с поля
	}


public:
	AlivePoint(int x_coordinate, int y_coordinate, int speed_, int maxlifetime_) { //конструктор
		this->x_p = x_coordinate;
		this->y_p = y_coordinate;
		this->speed = speed_;
		this->maxLifeTime = maxlifetime_;
		this->lifeTime = 0;
		setOnPole();
		this->napr_x = randNapr();
		this->napr_y = randNapr();
	};
	int getLifeTime() {		//сколько клетке осталось жить
		return maxLifeTime - lifeTime;
	}
	void deletE() {		//убирает клетку с поля надеюсь, навсегда
		runFromPole();
		x_p = -10e5;
	}
	int newHode() {		//метод, чтобы клетка сделала новый ход
		if (getLifeTime() == 1) {		// если клетка умерла бы на этом ходе, то на просто умирает
			runFromPole();
			return 1;
		}
		else {
			runFromPole();
			lifeTime++;
			x_p += napr_x;
			y_p += napr_y;
			x_p = (x_p + x) % x;
			y_p = (y_p + y) % y;
			if (pole[x_p][y_p] == 2) {		//если клетка напоролась на ЕДУ, то она её ест
				lifeTime -= sitnost;		//продлевает себе жизнь
				Point e;
				e.x_f = x_p;
				e.y_f = y_p;
				food.erase(food.find(e));		//и убирает ЕДУ с поля
			}
			setOnPole();
			return 0;
		}
	}
	bool alive() {		//метод, чтобы понять, а жива ли ещё клетка
		if (x_p >= 0) {			//если ещё на поле, то жива
			return 1;
		}
		return 0;
	}
	void findFood() {		//метод, чтобы клетке найти ближайшую еду
		int minz = 10e5;
		Point minp;
		minp.x_f = -10;
		for (auto i : food)			//для каждой ЕДЫ находим расстояние по теореме пифагора
		{
			if (sqrt((i.x_f - x_p) * (i.x_f - x_p) + (i.y_f - y_p) * (i.y_f - y_p)) < minz) {
				minp = i;
				minz = sqrt((i.x_f - x_p) * (i.x_f - x_p) + (i.y_f - y_p) * (i.y_f - y_p));
			}
		}
		if (minp.x_f < 0) {			//если не нашли никакой ЕДЫ, то просто начинаем случайно танцевать
			napr_x = randNapr();
			napr_y = randNapr();
		}
		else {						//или же устремляемся к ЕДЕ
			napr_x = newNpr(minp.x_f, x_p);
			napr_y = newNpr(minp.y_f, y_p);
		}
	}
protected:
	//Надо бы, наверное, что-то допилить, когда-нибудь будет, честно
};

vector<AlivePoint> points;			//массив клеток. Каюсь, не придумал, как их засунуть во что-нибудь типо сета или подобного
									// из-за этого умершие клетки всё равно остаются "живыми", просто очень далеко за картой

void poleSet() {					//инициализация стартового игрового поля размерами х на у
	for (int i = 0; i < x; i++) {
		vector<int> e(y);
		pole.push_back(e);
	}
}

void polePrint() {					//функция для вывода поля в консоль. Работает некрасиво, лучше научиться каким-нибдуь движкам, но пофиг
	for (int i = 0; i < y; i++)
	{
		cout << setfill(' ') << setw(3) << i;		//циферки сверху
	}
	cout << endl;
	for (int j = 0; j < x; j++) {
		cout << setw(3) << j;						//циферки слева
		for (int i = 0; i < y; i++)
		{
			if (pole[j][i] == 0)					//если 0 - значит там ничего нет
				cout << setw(3) << " ";
			if (pole[j][i] == 1)					//1 - значит там живая клетка
				cout << setw(3) << "@";
			if (pole[j][i] == 2)					//2 - значит там ЕДА
				cout << setw(3) << "+";
		}
		cout << endl;
	}
}




void newFood() {			//функция для созидания
	Point e;
	e.x_f = rand() % x;
	e.y_f = rand() % y;
	food.insert(e);			
	pole[e.x_f][e.y_f] = 2;		//создаёт новую ЕДУ в случайном месте карты
}



void newShag() {				//для ВСЕХ клеток в массиве клеток проверяет:
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].alive()) {			//живы ли они
			points[i].findFood();			// и заставляет искать еду
			if (points[i].newHode())		//заставляет делать новый ход и, если клетка умерла, то удаляет её
				points[i].deletE();
		}
	}
}

int main()
{
	srand((int)time(0));
	poleSet();
	AlivePoint e(rand() % x, rand() % y, 1, 15);		//просто набор новых клеток
	AlivePoint p(rand() % x, rand() % y, 1, 15);
	//return e < p;
	/*for (int i = 0; i < n; i++)
	{
		AlivePoint e(rand() % x, rand() % y, 1, i + 4);
		points.push_back(e);
	}
	polePrint();
	int k = 1;
	for(int i = 0; i < 7;i++){
		newShag();
		polePrint();
	}*/
	newFood();
	newFood();
	points.push_back(e);
	points.push_back(p);
	int k = 0, t = 0;
	while (k < 100)				//делает картинку 100 раз
	{
		k++;
		if (t++ % 10 == 0) {		//каждый 10 ход добавляет куссочек ЕДЫ
			newFood();
		}
		newShag();
		polePrint();
		Sleep(250);

	}
}
