#pragma once

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
