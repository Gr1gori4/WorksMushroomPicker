#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <list>

using namespace std;

enum State
{
	Enabling,
	Walk_before_forest,
	Search_mushroom,
	Define_type,
	Sit_down,
	Bring_hand,
	Cut,
	Open_compartment,
	Put,
	Close_compartment,
	Search_next_mushroom,
	Go_home
};

State state = Enabling;

struct Mushroom
{
	double x;
	double y;
	int type; // 1 - съедобный, 2 - несъедобный, 3 - галлюциогенный
	bool taken;
};

const int forest_size_x = 100;
const int forest_size_y = 100;
const int min_number_mushrooms = 4;
const int max_number_mushrooms = 25;
const int robot_step = 2;
int number_edible_mushrooms;
int number_inedible_mushrooms;
int number_hallucinogenic_mushrooms;
int current_x;
int current_y;
int сapacity_edible_compartment;
int сapacity_hallucinogenic_compartment;
int type_mushroom;
double battery_charge;
double safety_margin;
bool direction; // false - влево, true - вправо
vector <Mushroom> mushroom;
vector <int> index_search_mushroom;

void Robot_enabling();
void Robot_walk_before_forest();
void Robot_search_mushroom();
void Robot_define_type();
void Robot_sit_down();
void Robot_bring_hand();
void Robot_cut();
void Robot_open_compartment();
void Robot_put();
void Robot_close_compartment();
void Robot_go_home();
void Step(int, int);
void Search();

bool Check_safety_margin();
bool Check_battery_charge();
bool Check_forest();
bool Check_compartment();
bool Check();

int main()
{
	setlocale(LC_ALL, "Rus");
	srand(time(NULL));
	while (1)
	{
		switch (state)
		{
		case Enabling:
			Robot_enabling();
			break;
		case Walk_before_forest:
			Robot_walk_before_forest();
			break;
		case Search_mushroom:
			Robot_search_mushroom();
			break;
		case Define_type:
			Robot_define_type();
			break;
		case Sit_down:
			Robot_sit_down();
			break;
		case Bring_hand:
			Robot_bring_hand();
			break;
		case Cut:
			Robot_cut();
			break;
		case Open_compartment:
			Robot_open_compartment();
			break;
		case Put:
			Robot_put();
			break;
		case Close_compartment:
			Robot_close_compartment();
			break;
		case Search_next_mushroom:
			Robot_search_mushroom();
			break;
		case Go_home:
			Robot_go_home();
			break;	
		}
	}	
}

void Robot_enabling()
{
	srand(24543);

	number_edible_mushrooms = min_number_mushrooms + rand() % (max_number_mushrooms - min_number_mushrooms + 1);
	number_inedible_mushrooms = min_number_mushrooms + rand() % (max_number_mushrooms - min_number_mushrooms + 1);
	number_hallucinogenic_mushrooms = min_number_mushrooms + rand() % (max_number_mushrooms - min_number_mushrooms + 1);

	safety_margin = 10+(double)(rand() / (RAND_MAX / 90.0));
	mushroom.clear();

	for (int i = 0; i < number_edible_mushrooms + number_inedible_mushrooms + number_hallucinogenic_mushrooms; i++)
	{
		if(i <= number_edible_mushrooms-1)
			mushroom.push_back({ (double)(rand() / (RAND_MAX / 100.0)) , (double)(rand()) / ((double)(RAND_MAX / 100.0)), 1,false });
		else if (i > number_edible_mushrooms-1 && i < number_edible_mushrooms + number_inedible_mushrooms)
			mushroom.push_back({ (double)(rand() / (RAND_MAX / 100.0)) , (double)(rand()) / ((double)(RAND_MAX / 100.0)), 2,false });
		else
			mushroom.push_back({ (double)(rand() / (RAND_MAX / 100.0)) , (double)(rand()) / ((double)(RAND_MAX / 100.0)), 3,false });
	}

	battery_charge = 400.0;
	type_mushroom = 0;
	сapacity_edible_compartment = 10 + rand() % 11;
	сapacity_hallucinogenic_compartment = 10 + rand() % 11;
	current_x = 50;
	current_y = -30;
	direction = false;

	state = Walk_before_forest;
}

void Robot_walk_before_forest()
{
	while (Check() && !(current_x >= 0 && current_x <= forest_size_x) || !(current_y >= 0 && current_y <= forest_size_y))
	{
		if (current_x < current_y && (current_x < 0 || current_y < 0))
			Step(robot_step, 0);
		else if (current_x > current_y && (current_x < 0 || current_y < 0))
			Step(0, robot_step);
		else if (current_x > current_y && (current_x > 100 || current_y > 100))
			Step(-robot_step, 0);
		else
			Step(0, -robot_step);
	}
	if(Check())
		state = Search_mushroom;
}

void Robot_search_mushroom()
{
	while (Check() && state != Define_type)
	{
		if (current_y == 0 && !direction)
		{
			while (current_x != 0 && Check() && state != Define_type)
			{
				Step(-robot_step, 0);
				Search();
			}
			if (current_x == 0)
				direction = true;
		}		
		else if (direction)
		{ 
			while (current_x != forest_size_x && Check() && state != Define_type)
			{
				Step(robot_step, 0);
				Search();
			}
			if (current_x == forest_size_x)
			{
				direction = false;
				Step(0, robot_step);
				Step(0, robot_step);
			}
		}
		else
		{
			while (current_x != 0 && Check() && state != Define_type)
			{
				Step(-robot_step, 0);
				Search();
			}
			if (current_x == 0)
			{
				direction = true;
				Step(0, robot_step);
				Step(0, robot_step);
			}
		}
	}
}

void Robot_define_type()
{
	for(int i=0;i< index_search_mushroom.size();i++)
	{
		if (Check())
		{
			if (mushroom.at(index_search_mushroom.at(i)).type == 1)
			{
				cout << "Гриб съедобный" << endl;
				state = Sit_down;
			}
			else if (mushroom.at(index_search_mushroom.at(i)).type == 2)
			{
				cout << "Гриб несъедобный" << endl;
				state = Search_next_mushroom;
			}
			else if (mushroom.at(index_search_mushroom.at(i)).type == 3)
			{
				cout << "Гриб галлюциогенный" << endl;
				state = Sit_down;
			}
			battery_charge -= 0.2;
			safety_margin -= 0.002;
		}
	}
}

void Robot_sit_down()
{
	if (Check())
	{
		battery_charge -= 0.2;
		safety_margin -= 0.001;
		cout << "Робот присел" << endl;
		state = Bring_hand;
	}
}

void Robot_bring_hand()
{
	if(Check())
	{
		battery_charge -= 0.1;
		safety_margin -= 0.001;
		cout << "Робот подвел руку к грибу" << endl;
		state = Cut;
	}
}

void Robot_cut()
{
	if(Check())
	{
		battery_charge -= 0.1;
		safety_margin -= 0.001;
		cout << "Робот срезал гриб" << endl;
		state = Open_compartment;
	}
}

void Robot_open_compartment()
{
	for (int i = 0; i < index_search_mushroom.size(); i++)
	{
		if (Check())
		{
			if (mushroom.at(index_search_mushroom.at(i)).type == 1)
				cout << "Открыт отсек для съедобных грибов" << endl;
			else if (mushroom.at(index_search_mushroom.at(i)).type == 3)
				cout << "Открыт отсек для галлюциогенных грибов" << endl;
			battery_charge -= 0.1;
			safety_margin -= 0.001;
			state = Put;
		}
	}
}

void Robot_put()
{
	for (int i = 0; i < index_search_mushroom.size(); i++)
	{
		if (Check())
		{
			if (mushroom.at(index_search_mushroom.at(i)).type == 1 && сapacity_edible_compartment > 0)
			{
				сapacity_edible_compartment -= 1;
				battery_charge -= 0.1;
				safety_margin -= 0.001;
				cout << "Гриб положен" << endl;
			}
			else if (mushroom.at(index_search_mushroom.at(i)).type == 3 && сapacity_hallucinogenic_compartment > 0)
			{
				сapacity_hallucinogenic_compartment -= 1;
				battery_charge -= 0.1;
				safety_margin -= 0.001;
				cout << "Гриб положен" << endl;
			}
			else
				cout << "Отсек заполнен" << endl;
			type_mushroom = 0;
			state = Close_compartment;
		}
	}
}

void Robot_close_compartment()
{
	if (Check())
	{
		battery_charge -= 0.1;
		safety_margin -= 0.001;
		cout << "Отсек закрыт" << endl;
		state = Search_next_mushroom;
	}
}

void Step(int x, int y)
{
	current_x += x;
	current_y += y;
	battery_charge -= 0.25;
	safety_margin -=0.003;
	cout << current_x << " " << current_y;
}

void Search()
{
	index_search_mushroom.clear();
	for (int i = 0; i < mushroom.size(); i++)
	{
		if (!mushroom.at(i).taken && sqrt((mushroom.at(i).x - current_x) * (mushroom.at(i).x - current_x) + (mushroom.at(i).y - current_y) * (mushroom.at(i).y - current_y)) <= 1.1*robot_step)
		{
			mushroom.at(i).taken = true;
			index_search_mushroom.push_back(i);
			state = Define_type;
		}
	}
}

void Robot_go_home()
{
	cout << "Робот идёт домой" << endl;
	state=Enabling;
}

bool Check_safety_margin()
{
	if (safety_margin > 0)
	{
		cout << "\nиспр| ";
		return true;
	}
	else
	{
		cout << "\nнеиспр ";
		state = Go_home;
		return false;
	}
}

bool Check_battery_charge()
{
	if (battery_charge > 0)
	{
		cout << "дост| " ;
		return true;
	}
	else
	{
		cout << "недост| ";
		state = Go_home;
		return false;
	}
}

bool Check_forest()
{
	for(int i=0;i<mushroom.size();i++)
		if (!mushroom.at(i).taken)
		{
			cout << "гр.есть| ";
			return true;
		}
	cout << "гр.нет ";
	state = Go_home;
	return false;
}

bool Check_compartment()
{
	if (сapacity_edible_compartment!=0 || сapacity_hallucinogenic_compartment!=0)
	{
		cout << "отс.пуст| ";
		return true;
	}
	cout << "отс.полн| ";
	state = Go_home;
	return false;
}

bool Check()
{
	return Check_safety_margin() && Check_battery_charge() && Check_forest() && Check_compartment();
}