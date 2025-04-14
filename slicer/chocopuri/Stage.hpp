#pragma once

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
