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
		const bool isOpen = serial.isOpen(); // OpenSiv3D v0.4.2 以前は serial.isOpened()

		if (enabled)
		{
			// 2 バイト (int16) のデータを書き込む
			const std::string n = "123 \n";

			serial.write(n.data(), n.size());
		}

		if (SimpleGUI::RadioButtons(index, options, Vec2{ Scene::Size().x - 250, Scene::Size().y - 375}))
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
