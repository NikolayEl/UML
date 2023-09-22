#include<iostream>
#include<conio.h>
#include<thread>
#include<Windows.h>

using namespace std::chrono_literals;

#define Enter	13
#define Escape	27

#define MIN_TANK_VOLUME		20
#define MAX_TANK_VOLUME		120

class Tank
{
	const int VOLUME;
	double fuel_level;
public:
	int get_VOLUME() const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double fuel)
	{
		if (fuel < 0) return;
		if (fuel_level + fuel < VOLUME) fuel_level += fuel;
		else fuel_level = VOLUME;
	}
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0) fuel_level = 0;
		return fuel_level;
	}

	Tank(int volume):VOLUME(volume<MIN_TANK_VOLUME?MIN_TANK_VOLUME:volume>MAX_TANK_VOLUME?MAX_TANK_VOLUME:volume)
	{
		this->fuel_level = 0;
		std::cout << "Tank is ready\t" << this << std::endl;
	}
	~Tank()
	{
		std::cout << "Tank is over:\t" << this << std::endl;
	}

	void info()const
	{
		std::cout << "volume:\t" << VOLUME << " letters\n";
		std::cout << "Fuel level:\t" << get_fuel_level() << " liters\n";
	}
};

#define MIN_ENGINE_CONSUMPTION	3
#define MAX_ENGINE_CONSUMPTION	30

class Engine
{
	const double DEFAULT_CONSUMPTION;
	const double DEFAULT_CONSUMPTION_PER_SECOND;
	double consumption_per_second;
	bool is_started;
public:
	double get_DEFAULT_CONSUMPTION_PER_SECOND()const
	{
		return DEFAULT_CONSUMPTION_PER_SECOND;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	void set_consumption_per_second(int speed)
	{
		if (speed == 0)consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND;
		else if (speed < 60) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20 / 3;
		else if (speed < 100 && speed >= 60) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 14 /3;
		else if (speed < 140 && speed >=100) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20 /3;
		else if (speed < 200 && speed >=140) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 25 /3;
		else if (speed > 200) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 10;

	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	
	Engine(double DEFAULT_CONSUMPTION_PER_SECOND) :DEFAULT_CONSUMPTION(DEFAULT_CONSUMPTION_PER_SECOND<MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
													DEFAULT_CONSUMPTION_PER_SECOND>MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
													DEFAULT_CONSUMPTION_PER_SECOND),
		DEFAULT_CONSUMPTION_PER_SECOND(DEFAULT_CONSUMPTION * 3e-5)
	{
		//this->DEFAULT_CONSUMPTION_PER_SECOND = DEFAULT_CONSUMPTION*3e-5;
		set_consumption_per_second(0);
		is_started = false;
		std::cout << "Engine is ready:\t" << this << std::endl;
	}
	~Engine()
	{
		std::cout << "Engine is over:\t" << this << std::endl;
	}
	void info()const
	{
		std::cout << "Consumtion:\t" << DEFAULT_CONSUMPTION << " liters per 100km.\n";
		std::cout << "Consumtion:\t" << consumption_per_second << " liters per 1 second.\n";
	}
};
#define MAX_SPEED_LOWER_LEVEL	30
#define MAX_SPEED_UPPER_LEVEL	400
class Car
{
	Engine engine;
	Tank tank;
	int speed;
	const int MAX_SPEED;
	int acceleration;
	bool driver_inside;
	struct Threads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}threads;
public:
	int get_speed()const
	{
		return speed;
	}
	int get_MAX_SPEED()const
	{
		return MAX_SPEED;
	}
	Car(double DEFAULT_CONSUMPTION_PER_SECOND, int volume, int max_speed, int acceleration = 10) :engine(DEFAULT_CONSUMPTION_PER_SECOND), tank(volume),
		MAX_SPEED
		(
			max_speed<MAX_SPEED_LOWER_LEVEL ? MAX_SPEED_LOWER_LEVEL :
			max_speed>MAX_SPEED_UPPER_LEVEL ? MAX_SPEED_UPPER_LEVEL :
			max_speed
		)
	{
		driver_inside = false;
		this->acceleration = acceleration;
		this->speed = 0;
		std::cout << "Your car is ready to go " << this << std::endl;
	}
	~Car()
	{
		std::cout << "Car is over\t" << this << std::endl;
	}
	void get_in()
	{
		driver_inside = true;
		threads.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (threads.panel_thread.joinable())threads.panel_thread.join();
		system("cls");
		std::cout << "Outside" << std::endl;
	}
	void start()
	{
		if (driver_inside && tank.get_fuel_level())
		{
			speed = 0;
			engine.start();
			threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop()
	{
		engine.stop();
		if (threads.engine_idle_thread.joinable())threads.engine_idle_thread.join();
	}
	void control()
	{
		char key;
		do
		{
			key = 0;
			if(_kbhit()) //Функция kbhit возвращает только если нажата клавиша, в противном случае 0
				key = _getch();
			switch (key)
			{
			case Enter:
				if (driver_inside && speed == 0)get_out();
				else if(!driver_inside && speed == 0) get_in();
				break;
			case 'F':case 'f':
			{
				if (driver_inside)
				{
					std::cout << "Для начала нужно выйти из машины" << std::endl;
					std::this_thread::sleep_for(2s);
					break;
				}
				double fuel;
				std::cout << "Enter volume fuel: "; std::cin >> fuel;
				tank.fill(fuel);
				break;
			}
			case 'I':case 'i':
				if (engine.started())
				{
					speed = 0;
					stop();
				}
				else start();
				break;
			case 'W':case 'w': 
				accelerate(); 
				std::this_thread::sleep_for(1s);
				break;
			case 'S':case 's': 
				slow_down();
				std::this_thread::sleep_for(1s);
				break;

				break;
			case Escape:
				speed = 0;
				if (engine.started())stop();
				get_out();
			}
			if (tank.get_fuel_level() == 0)stop();
			if (speed < 1)engine.set_consumption_per_second(speed = 0);
			if (speed == 0 && threads.free_wheeling_thread.joinable())threads.free_wheeling_thread.join();
		} while (key != Escape);
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))
			std::this_thread::sleep_for(1s);
	}
	void free_wheeling()
	{
		while (--speed > 0)
		{
			std::this_thread::sleep_for(1s);
			engine.set_consumption_per_second(speed);
		}
	}
	void accelerate()
	{
		if (engine.started() && driver_inside)
		{
			speed += acceleration;
			if (speed > MAX_SPEED)speed = MAX_SPEED;
			if (!threads.free_wheeling_thread.joinable())threads.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
			//std::this_thread::sleep_for(1s);
		}
	}
	void slow_down()
	{
		if (driver_inside)
		{
			speed -= acceleration;
			if (speed < 1)speed = 0;
			//if (threads.free_wheeling_thread.joinable() && speed == 0)threads.free_wheeling_thread.join();
			//std::this_thread::sleep_for(1s);
		}
	}
	void panel()const
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		while (driver_inside)
		{
			system("cls");
			for (int i = 0; i < speed / 3; i++)
			{
				std::cout << "|";
			}
			std::cout << std::endl;
			std::cout << "Level fuel:\t" << tank.get_fuel_level() << " Leters";
			SetConsoleTextAttribute(hConsole, 0xCE);
			std::cout << (tank.get_fuel_level() <= 5 ? "\t\tLOW FUEL" : "") << std::endl;
			SetConsoleTextAttribute(hConsole, 0x07);
			std::cout << "Engine is:\t" << (engine.started() ? "started" : "stopped") << std::endl;
			SetConsoleTextAttribute(hConsole, 10);
			std::cout << "Speed:\t" << speed << " km/h\n";
			SetConsoleTextAttribute(hConsole, 0x07);
			std::this_thread::sleep_for(100ms);
			std::cout << "Consumption per second:\t" << engine.get_consumption_per_second() << " liters";
		}
	}
};

//#define TANK_CHEK
void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHEK
	Tank tank(150);
	tank.info();

	do
	{
		int fuel;
		std::cout << "Enter level fuel: "; std::cin >> fuel;
		tank.fill(fuel);
		tank.info();
	} while (true);
#endif // TANK_CHEK

	//Engine engine(10);
	//engine.info();
	Car bmw(25, 100, 300);
	bmw.control();

}