# include <Siv3D.hpp> // Siv3D v0.6.15

class CommandSender
{
	Array<std::string> commands;
	Serial& serial;

public:
	CommandSender(Serial& serial)
		: serial{ serial }
	{
	}

	void pushMove(bool isBlack, const Vec3& position, float speed, bool isInject)
	{
		commands.emplace_back(std::format(
			"move {} {} {} {} {} {}\n",
			isBlack ? "black" : "white",
			position.x, position.y, position.z,
			speed,
			isInject ? "inject" : "stop"
		)
		);
	}

	void pushHome()
	{
		commands.emplace_back("home\n");
	}

	void clear()
	{
		commands.clear();
	}

	bool send()
	{
		if (not serial.isOpen())
			return false;

		if (commands.isEmpty())
			return false;


		if (not sendText("start\n"))
			return false;

		for (const auto& command : commands)
		{
			if (not sendText(command))
				return false;
		}

		if (not sendText("end\n"))
			return false;


		return true;
	}

private:

	bool sendText(std::string_view text)
	{
		return serial.write(text.data(), text.size()) == text.size();
	}
};

Array<String> GetSerialPortOptions()
{
	Array<String> options = System::EnumerateSerialPorts().map([](const SerialPortInfo& info)
	{
		return U"[{}] {}"_fmt(info.port, info.description);
	});

	options << U"None";
	return options;
}


void Main()
{
	Serial serial; // シリアルポートの設定。適宜変更してください。
	serial.open(U"COM4", 115200);

	CommandSender sender{ serial };

	Print << GetSerialPortOptions();

	sender.pushHome();
	sender.pushMove(true, Vec3(100, 200, 300), 1.0f, true);
	sender.pushMove(false, Vec3(400, 500, 600), 2.0f, false);


	while (System::Update())
	{

		Print << sender.send() << serial.available();
	}
};
