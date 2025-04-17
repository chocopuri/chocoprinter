#pragma once

struct command
{
	struct speed
	{
		int speed;
	};
	struct go
	{
		int x, y;
	};
	struct choco
	{
		int n;
	};
	std::variant<speed, go, choco> v;
};

class Serial_com {
public:
	const Array<SerialPortInfo> infos = System::EnumerateSerialPorts();
	const Array<String> options = infos.map([](const SerialPortInfo& info)
	{
		return info.port;
	}) << U"none";;

	Serial serial;
	size_t index = (options.size() - 1);

	void communication(bool enabled) {
		
	}

	command parse_command(const std::string& command)
	{
		std::istringstream iss{ command };

		std::string command_type;
		iss >> command_type;

		if (command_type == "speed")
		{
			int speed;
			iss >> speed;

			return {
				.v = command::speed{ speed }
			};
		}

		if (command_type == "go")
		{
			int x, y;
			iss >> x >> y;

			return {
				.v = command::go{ x, y }
			};
		}

		if (command_type == "choco")
		{
			int n;
			iss >> n;
			return {
				.v = command::choco{ n }
			};
		}
	}
	void print(const command::speed& cmd, bool enabled)
	{
		const bool isOpen = serial.isOpen(); // OpenSiv3D v0.4.2 以前は serial.isOpened()

		if (enabled)
		{
			// 2 バイト (int16) のデータを書き込む
			const std::string n = "speed " + std::to_string(cmd.speed) + "\n";
			serial.write(n.data(), n.size());
		}

		if (SimpleGUI::RadioButtons(index, options, Vec2{ Scene::Size().x - 250, Scene::Size().y - 375 }))
		{
			ClearPrint();

			if (index == (options.size() - 1))
			{
				serial = Serial{};
			}
			else
			{
				Print << U"Open {}"_fmt(infos[index].port);

				// シリアルポートをオープン
				if (serial.open(infos[index].port, 115200))
				{
					Print << U"Succeeded";
				}
				else
				{
					Print << U"Failed";
				}
			}
		}

		if (const size_t available = serial.available()) {
			std::string n(available, ' ');
			Print << U"1";
			if (serial.read(n.data(), n.size())) {
				Console << U"READ: " << Unicode::WidenAscii(n);
			}
		}
	}

	void print(const command::go& cmd, bool enabled)
	{
		std::cout << "go " << cmd.x << " " << cmd.y << std::endl;
		if (enabled)
		{
			// 2 バイト (int16) のデータを書き込む
			const std::string n = "go " + std::to_string(cmd.x) + " " + std::to_string(cmd.y) + "\n";
			std::cout << "go " << cmd.x << cmd.y << std::endl;
			serial.write(n.data(), n.size());
		}

		if (SimpleGUI::RadioButtons(index, options, Vec2{ Scene::Size().x - 250, Scene::Size().y - 375 }))
		{
			ClearPrint();

			if (index == (options.size() - 1))
			{
				serial = Serial{};
			}
			else
			{
				Print << U"Open {}"_fmt(infos[index].port);

				// シリアルポートをオープン
				if (serial.open(infos[index].port, 115200))
				{
					Print << U"Succeeded";
				}
				else
				{
					Print << U"Failed";
				}
			}
		}

		if (const size_t available = serial.available()) {
			std::string n(available, ' ');
			Print << U"1";
			if (serial.read(n.data(), n.size())) {
				Console << U"READ: " << Unicode::WidenAscii(n);
			}
		}
	}

	void print(const command::choco& cmd, bool enabled)
	{
		std::cout << "choco " << cmd.n << "inject" << std::endl;
		if (enabled)
		{
			// 2 バイト (int16) のデータを書き込む
			
			const std::string n = "speed " + std::to_string(cmd.n) + "inject";
			std::cout << "choco " << cmd.n << std::endl;
			serial.write(n.data(), n.size());
		}

		if (SimpleGUI::RadioButtons(index, options, Vec2{ Scene::Size().x - 250, Scene::Size().y - 375 }))
		{
			ClearPrint();

			if (index == (options.size() - 1))
			{
				serial = Serial{};
			}
			else
			{
				Print << U"Open {}"_fmt(infos[index].port);

				// シリアルポートをオープン
				if (serial.open(infos[index].port, 115200))
				{
					Print << U"Succeeded";
				}
				else
				{
					Print << U"Failed";
				}
			}
		}

		if (const size_t available = serial.available()) {
			std::string n(available, ' ');
			Print << U"1";
			if (serial.read(n.data(), n.size())) {
				Console << U"READ: " << Unicode::WidenAscii(n);
			}
		}
	}
};
