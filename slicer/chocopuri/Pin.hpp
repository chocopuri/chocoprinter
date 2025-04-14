#pragma once

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

inline void DrawEdge(const Route& from, const Route& to) {
	Line{ from.pos, to.pos }.stretched(-4).drawArrow(1, Vec2{ 15, 15 }, ColorF{ 0.11 });
}


class Pin {
	int32 row;
	int32 col;
	int32 s;
public:
	Array<Array<int32>> graph;
	Array<Route> vertices;
	Array<bool> enabled;
	int32 num = 0;
	int32 window_size_x = Scene::Size().x;
	int32 window_size_y = Scene::Size().y;

	Pin(int row, int col, int s)
		: row(row)
		, col(col)
		, s(s)
	{
		graph.push_back({ 1 });
		graph.push_back({ 0 });

		vertices.push_back({ 0, {s / row, s / col} });
		vertices.push_back({ 1, {s / row, s / col} });

		enabled.push_back({ 0 });
		enabled.push_back({ 0 });
		enabled.push_back({ 0 });
	}

	void Draw(bool e, bool& f) {
		Rect r = Rect{ s / row, s / col, s - s / col * 2 };
		int32 cursor_x = Round(Cursor::PosF().x / (window_size_y / col)) * (window_size_y / col);
		int32 cursor_y = Round(Cursor::PosF().y / (window_size_y / col)) * (window_size_y / col);

		if (r.leftClicked()) {
			graph.push_back({ num + 1 });
			vertices.push_back({ num, { cursor_x, cursor_y } });
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
