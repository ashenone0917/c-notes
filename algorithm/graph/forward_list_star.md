```cpp
// foward_list_star.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//链式向前星

#include <iostream>
#include <vector>
#include <queue>
struct Edge {
	uint32_t to_;
	uint32_t weight_;
	uint32_t next_;
};

std::vector<uint32_t> points;
std::vector<Edge> edges;
int cnt = 0;
void add_edge(uint32_t point, uint32_t weight, uint32_t to) {
	edges[cnt].weight_ = weight;
	edges[cnt].to_ = to;
	edges[cnt].next_ = points[point];
	points[point] = cnt++;
}


void bfs(uint32_t u) {
	std::vector<uint32_t> cost(0, points.size());
	std::vector<uint32_t> path(-1, points.size());
	std::vector<uint32_t> visit(0, points.size());
	std::queue<uint32_t> q;
	
	cost[u] = 0;
	path[u] = -1;
	visit[u] = 1;
	q.push(u);
	while (!q.empty()) {
		auto p = q.front();
		q.pop();
		for (uint32_t i = points[u]; ~i; i = edges[i].next_) {
			if (!visit[edges[i].to_]) {

			}
		}
	}
}

int main() {
	int point_cnt = 0, edge_cnt = 0;
	std::cout << "input point count : ";
	std::cin >> point_cnt;
	std::cout << "input edge count : ";
	std::cin >> edge_cnt;

	points.resize(point_cnt, -1);
	edges.resize(edge_cnt);
	for (int i = 0; i < edge_cnt; ++i) {
		uint32_t point = 0, weight = 0, to = 0;
		std::cin >> point >> weight >> to;
		add_edge(point, weight, to);
	}

	std::cout << "from   to   weight" << std::endl;
	for (int i = 0; i < point_cnt; ++i) {
		auto point = points[i];
		for (auto pos = point; ~pos;) {
			auto& edge = edges[pos];
			std::cout << i << "   " << edge.to_ << "   " << edge.weight_ << std::endl;
			pos = edge.next_;
		}
	}

	return 0;
}
```
