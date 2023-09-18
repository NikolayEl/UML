#include<iostream>
#include<conio.h>
#include<thread>

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

#define MIN_ENGINE_CONSUMTION	3
#define MAX_ENGINE_CONSUMTION	30

class Engine
{
	const double DEFAULT_CONSUMPTION;
	double consumption;
	double consumption_per_second;
	bool is_started;
public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	void set_consumption_per_second(double consumption)
	{
		consumption_per_second = consumption * 3e-5;
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
	
	Engine(double consumption) :DEFAULT_CONSUMPTION(consumption<MIN_ENGINE_CONSUMTION ? MIN_ENGINE_CONSUMTION :
													consumption>MAX_ENGINE_CONSUMTION ? MAX_ENGINE_CONSUMTION :
													consumption)
	{
		this->consumption = DEFAULT_CONSUMPTION;
		set_consumption_per_second(consumption);
		is_started = false;
		std::cout << "Engine is ready:\t" << this << std::endl;
	}
	~Engine()
	{
		std::cout << "Engine is over:\t" << this << std::endl;
	}
	void info()const
	{
		std::cout << "Consumtion:\t" << consumption << " liters per 100km.\n";
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
	bool driver_inside;
	struct Threads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
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
	Car(double consumption, int volume, int max_speed) :engine(consumption), tank(volume),
		MAX_SPEED
		(
			max_speed<MAX_SPEED_LOWER_LEVEL ? MAX_SPEED_LOWER_LEVEL :
			max_speed>MAX_SPEED_UPPER_LEVEL ? MAX_SPEED_UPPER_LEVEL :
			max_speed
		)
	{
		driver_inside = false;
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
				if (driver_inside)get_out();
				else get_in();
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
				if (engine.started())stop();
				else start();
				break;
			case Escape:
				get_out();
			}
			if (tank.get_fuel_level() == 0)stop();
		} while (key != Escape);
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))
			std::this_thread::sleep_for(1s);
	}
	void panel()const
	{
		while (driver_inside)
		{
			system("cls");
			std::cout << "Level fuel:\t" << tank.get_fuel_level() << " Leters\n";
			std::cout << "Engine is:\t" << (engine.started() ? "started" : "stopped") << std::endl;
			std::this_thread::sleep_for(100ms);
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
	Car bmw(10, 40, 250);
	bmw.control();

}