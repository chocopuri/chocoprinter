# include <Siv3D.hpp>  // OpenSiv3D 0.6.16

using VertexID = int32;

class Buttons {
public:
	double volume = 1.0;
	int window_size_x = Scene::Size().x;
	int window_size_y = Scene::Size().y;
	bool stop;
	bool run;
	bool grid = 1;
	bool choco_switch = 1;
	bool choco_switch_flag = 0;

	void Draw() {
		///実行ボタン
		run = SimpleGUI::Button(U"\U000F0493 実行", Vec2{ window_size_x - 250, window_size_y - 150 }, 200);
		//中止ボタン
		stop = SimpleGUI::Button(U"\U000F1398 中止", Vec2{ window_size_x - 250, window_size_y - 100 }, 200);
		//チョコの量
		SimpleGUI::Slider(U"チョコの量", volume, Vec2{ window_size_x - 250, window_size_y - 200 });
		//グリッドの表示
		if (SimpleGUI::Button(U"マス目の表示", Vec2{ window_size_x - 250, window_size_y - 250 }, 200)) {
			grid = !grid;
		}
		//チョコを出すかどうか
		if (SimpleGUI::Button(U"チョコON/OFF", Vec2{ window_size_x - 250, window_size_y - 300 }, 200)) {
			choco_switch = !choco_switch;
			choco_switch_flag = 1;
		}

	}
};

class Stage {
	//マスの数
public:
	int32 row;
	int32 col;
	int32 s = Scene::Size().y;

	void Draw(bool enabled) {
		//台の背景表示
		Rect{ s / row, s / col, s - s / col * 2 }.draw(Palette::White);

		//grid線の表示
		if (enabled) {
			for (int32 x = 1; x < row - 1; ++x)
			{
				for (int32 y = 1; y < col - 1; ++y)
				{
					Rect{ (x * s / row), (y * s / col), s / col }.drawFrame(2, 0, Palette::Skyblue);
				}
			}
		}

	}
};

struct Route
{
	int32 num;

	Vec2 pos;

	bool enabled = 0;

	void draw() const
	{
		pos.asCircle(4).draw(ColorF{ 0.95 }).drawFrame(2, ColorF{ 0.11 });
	}
};

void DrawEdge(const Route& from, const Route& to)
{
	Line{ from.pos, to.pos }.stretched(-4).drawArrow(1, Vec2{ 15, 15 }, ColorF{ 0.11 });
}

class Pin {
	public:
		Array<Array<VertexID>> graph;
		Array<Route> vertices;
		Array<bool> enabled;
		int32 row;
		int32 col;
		int32 s;
		int32 num = 0;
		int32 window_size_x = Scene::Size().x;
		int32 window_size_y = Scene::Size().y;

	void Draw(bool e, bool &f) {
		Rect r = Rect{ s / row, s / col, s - s / col * 2 };
		int32 cursor_x = Round(Cursor::PosF().x / (window_size_y / col)) * (window_size_y / col);
		int32 cursor_y = Round(Cursor::PosF().y / (window_size_y / col)) * (window_size_y / col);

		if (r.leftClicked()) {
			graph.push_back({ num + 1 });
			vertices.push_back({ num, { cursor_x, cursor_y }});
			if (f) {
				enabled.pop_back();
				enabled.push_back({ e });
				enabled.push_back({ e });
				f = 0;
			}
			else enabled.push_back({ e });
			num++;
		}

		for (const auto& v : vertices) {
			v.draw();
		}
		for (size_t from = 0; from < graph.size(); ++from)
		{
			for (const auto& to : graph[from])
			{
				if (enabled[from]) {
					DrawEdge(vertices[from], vertices[to]);
				}
			}
		}
		
	}
};

class Serial_com {
	public:
		const Array<SerialPortInfo> infos = System::EnumerateSerialPorts();
		const Array<String> options = infos.map([](const SerialPortInfo& info)
		{
			return U"{} ({})"_fmt(info.port, info.description);
		});

		Serial serial;
		size_t index = (options.size() - 1);

	void communication(bool enabled) {
		const bool isOpen = serial.isOpen(); // OpenSiv3D v0.4.2 以前は serial.isOpened()

		if (enabled)
		{
			// 2 バイト (int16) のデータを書き込む
			const int16 n = 12300;

			serial.write(n);
		}

		if (SimpleGUI::RadioButtons(index, options, Vec2{ 200, 60 }))
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
				if (serial.open(infos[index].port))
				{
					Print << U"Succeeded";
				}
				else
				{
					Print << U"Failed";
				}
			}
		}

		if (const size_t available = serial.available())
		{
			int16 n;

			if (serial.read(n)) // 2 バイト (int16) のデータを読み込んだら
			{
				Print << U"READ: " << n;
			}
		}
		}



};


void Main()
{
	Window::Resize(1536, 864);
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	Buttons buttons;
	int row = 32, col = 32; 
	Stage stage(row, col); //マス目のサイズ設定
	int32 s = Scene::Size().y;

	Pin p;
	p.row = row; p.col = col; p.s = s;

	p.graph.push_back({ 1 });
	p.graph.push_back({ 0 });

	p.vertices.push_back({ 0, {s / row, s / col} });
	p.vertices.push_back({ 1, {s / row, s / col} });

	
	p.enabled.push_back({ 0 });
	p.enabled.push_back({ 0 });
	p.enabled.push_back({ 0 });

	Serial_com serial;


	while (System::Update())
	{
		buttons.Draw();
		stage.Draw(buttons.grid);
		p.Draw(buttons.choco_switch, buttons.choco_switch_flag);
		serial.communication(buttons.run);
	}
}
