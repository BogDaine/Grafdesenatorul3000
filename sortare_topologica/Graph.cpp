#define _USE_MATH_DEFINES
#include <climits>
#include "Graph.h"
#include "raylib\raymath.h"
#include <iostream>
#include <unordered_set>
#include "MyUtils.h"
#include <algorithm>
#include <stack>
#include <queue>

void Graph::generateRandomPath(const unsigned int& start, const unsigned int& finish)
{
	std::vector<Node*> unusedNodes = nodes;
	std::unordered_set<unsigned int> pathNodes;
	int randomNumber = 0;

	unusedNodes.erase(unusedNodes.begin() + start - 1);

	for (auto i = nodes[start - 1]; i != nodes[finish - 1];)
	{
		pathNodes.insert(i->getNumber());
		Node* u;
		if (unusedNodes.size() == 1)
			u = unusedNodes[0];
		else
		{
			randomNumber = random_number(0, unusedNodes.size() - 1);
			u = unusedNodes[randomNumber];
		}
		if (pathNodes.count(u->getNumber()))
			continue;


		unusedNodes.erase(unusedNodes.begin() + randomNumber);

		addEdge(i, u);
		i = u;
	}
}

void Graph::generateRandomCycle(const unsigned int& start)
{
	std::vector<Node*> unusedNodes = nodes;
	std::unordered_set<unsigned int> cycleNodes;
	int randomNumber = 0;

	bool firstLoop = true;

	//std::cout << "entered func\n";
	//std::cout << "i = " << start << std::endl << std::endl;
	//unusedNodes.erase(unusedNodes.begin() + start - 1);

	for (auto i = nodes[start - 1];;)
	{
		//std::cout << i->getNumber() << std::endl;

		firstLoop = false;
		cycleNodes.insert(i->getNumber());
		Node* u;

		if (unusedNodes.size() == 0)
		{
			//std::cout << "no more unused nodes!" << std::endl;
			break;
		}
		if (unusedNodes.size() == 1)
			u = unusedNodes[0];
		else
		{
			randomNumber = random_number(0, unusedNodes.size() - 1);
			u = unusedNodes[randomNumber];
		}

		//std::cout << u->getNumber() << std::endl << std::endl;
		if (u->getNumber() == start)
		{
			if (cycleNodes.size() < 3 && !oriented && cycleNodes.size() < 2 && oriented)
			{
				continue;
			}
			else
			{
				addEdge(i, u);
				break;
			}
		}

		unusedNodes.erase(unusedNodes.begin() + randomNumber);

		addEdge(i, u);
		i = u;
	}
}

void Graph::drawGrouped_color(std::vector<std::vector<unsigned int>>& groups)
{
}

void Graph::drawGrouped_nodes()
{
	auto adj = getAdjacencyList();
	std::vector <std::vector<unsigned int>> GroupsAdj;
	GroupsAdj.resize(group->size());
	std::vector<std::pair<unsigned int, unsigned int>> groupEdges;
	std::vector<Vector2> groupNodesPositions;
	groupNodesPositions.resize(group->size());

	for (unsigned int comp = 0; comp < GroupsAdj.size(); comp++)
	{
		for (const auto& node : group->operator[](comp))
		{
			for (const auto& neighbor : adj[node])
			{
				for (unsigned int neighborComp = 0; neighborComp < group->size(); neighborComp++)
				{
					if (neighborComp == comp)
						continue;
					auto currentComp = group->operator[](neighborComp);
					auto found = std::find(currentComp.begin(), currentComp.end(), neighbor);
					if (found != currentComp.end())
					{
						GroupsAdj[comp].push_back(neighborComp);
						break;
					}
				}
			}
		}
	}

	/*for (unsigned int i = 0; i < GroupsAdj.size(); i++)
	{
		std::cout << i << ": ";
		util::printVector(GroupsAdj[i]);
		std::cout << std::endl;
	}
		std::cout << std::endl;
		std::cout << std::endl;*/

	for (unsigned int i = 0; i < group->size(); i++)
	{
		Vector2 position = { 0,0 };
		std::string label = "";
		for (unsigned int node : group->operator[](i))
		{
			position.x += nodes[node]->getPos().x;
			position.y += nodes[node]->getPos().y;
			label += std::to_string(nodes[node]->getNumber()) + ", ";
		}
		position.x /= group->operator[](i).size();
		position.y /= group->operator[](i).size();
		groupNodesPositions[i] = position;
		if(group->operator[](i).size())
			DrawCircle(position.x, position.y,  25, nodes[group->operator[](i)[0]]->getColor());
		DrawText(label.c_str(), position.x, position.y + 23, 18, BLACK);
	}

	//Draw edges

	for (unsigned int currentGroup = 0; currentGroup < GroupsAdj.size(); currentGroup++)
	{
		for (const auto& nextGroup : GroupsAdj[currentGroup])
		{
			auto node1Pos = groupNodesPositions[currentGroup];
			auto node2Pos = groupNodesPositions[nextGroup];

			float x1;
			float y1;

			float x2;
			float y2;

			float angle = oriented ? 30 : 0;

			float theta = Vector2Angle({ node1Pos.x, node1Pos.y }, { node2Pos.x, node2Pos.y });
			x1 = node1Pos.x + 25 * cosf((theta + angle) * float(M_PI / 180));
			y1 = node1Pos.y + 25 * sinf((theta + angle) * float(M_PI / 180));
							  
			x2 = node2Pos.x - 25 * cosf((theta - angle) * float(M_PI / 180));
			y2 = node2Pos.y - 25 * sinf((theta - angle) * float(M_PI / 180));

			DrawLine(x1, y1, x2, y2, BLACK);
			if (oriented)
			{
				float nodesAngle = Vector2Angle(node1Pos, node2Pos) * float(M_PI / 180);
				float triAngle = 45 * float(M_PI / 180);
				//DrawText(std::to_string(nodesAngle).c_str(), 10, 23, 13, BLACK);
				Vector2 A, B;
				A.x = x2 - 12 * cosf(nodesAngle + triAngle);
				A.y = y2 - 12 * sinf(nodesAngle + triAngle);

				B.x = x2 - 12 * cosf(nodesAngle - triAngle);
				B.y = y2 - 12 * sinf(nodesAngle - triAngle);
				DrawTriangle({ x2, y2 }, A, B, BLACK);
			}

		}
	}


}

Graph::Graph(const bool& oriented)
	:oriented(oriented)
{

}

bool Graph::hasFlow() const
{
	return flow;
}

void Graph::resetGraph()
{
	hidden = true;
	drawingMode = graph_drawing_modes::NORMAL;
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		delete(nodes[i]);
	}
	edges.clear();
	numberOfNodes = 0;
	hidden = false;
}

Graph::Graph(const unsigned int& nNodes, const unsigned int& nPaths, const unsigned int& nCycles, const bool& oriented, const bool& fl, const bool& resizeNodes, const unsigned int& cRadius)
	: oriented(oriented), numberOfPaths(nPaths), numberOfCycles(nCycles), circleRadius(cRadius), flow(fl)
{
	float step = 360.0 / float(nNodes) * DEG2RAD;
	float theta = 180 * DEG2RAD;

	if (resizeNodes)
	{
		while (Vector2Distance({ WINDOW_WIDTH / 2 + cosf(theta) * cRadius, WINDOW_HEIGHT / 2 - sinf(theta) * cRadius },
			{ WINDOW_WIDTH / 2 + cosf(theta + step) * cRadius, WINDOW_HEIGHT / 2 - sinf(theta + step) * cRadius }) < Node::radius * 2)
		{
			Node::radius--;
			Node::fontSize--;
		}
	}

	//std::cout << std::endl;
	//std::cout << nNodes << std::endl;
	//std::cout << &nNodes  << " " << nNodes << std::endl;
	for (unsigned int i = 0; i < nNodes; i++)
	{
		addNodeNextNode({ WINDOW_WIDTH / 2 + cosf(theta) * cRadius, WINDOW_HEIGHT / 2 - sinf(theta) * cRadius });
		theta += step;
	}

	if (numberOfNodes > 1)
		for (unsigned int i = 0; i < nPaths; i++)
		{
			generateRandomPath(1, numberOfNodes);
		}

	if (numberOfNodes > 2 && !oriented || numberOfNodes > 1 && oriented)
	{
		int randomNumber;

		for (unsigned int i = 0; i < nCycles; i++)
		{
			randomNumber = random_number(1, nodes.size());
			generateRandomCycle(randomNumber);
			//std::cout << "random cycle " << randomNumber << std::endl;
		}
	}
}

void Graph::drawEdges()
{
	unsigned int index = 0;
	for (const auto& edge : edges)
	{
		drawEdge(std::get<0>(edge), std::get<1>(edge), index);
		index++;
	}
}
void Graph::drawNodes()
{
	for (const auto& aux : nodes)
	{
		aux->draw();
	}
}
void Graph::draw()
{
	if (hidden)
		return;

	switch (drawingMode)
	{
	case graph_drawing_modes::NORMAL:
		drawNodes();
		drawEdges();
		break;
	case graph_drawing_modes::GROUPED_COLOR:
		break;
	case graph_drawing_modes::GROUPED_NODE:
		drawGrouped_nodes();
		break;
	}
}

void Graph::group_Color(std::vector<std::vector<unsigned int>>& groups)
{
	for (const auto& it : groups)
	{
		Color randColor = random_color();
		for (const auto& node : it)
		{
			nodes[node]->setColor(randColor);
		}
	}
}

void Graph::group_Nodes(std::vector<std::vector<unsigned int>>& groups)
{

}

void Graph::resetColors()
{
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->setColor(nodes[i]->getOldColor());
	}
}

unsigned int** Graph::getMatrix(unsigned int& n)
{
	unsigned int** mat;
	matrixAlloc(mat, numberOfNodes, numberOfNodes);
	matrixZero(mat, numberOfNodes, numberOfNodes);
	n = numberOfNodes;

	for (auto edge : edges)
	{
		mat[std::get<0>(edge)][std::get<1>(edge)] = 1;
		if (!mat[std::get<1>(edge)][std::get<0>(edge)]) mat[std::get<1>(edge)][std::get<0>(edge)] = !oriented;
	}
	return mat;
}

inline std::vector<std::vector<unsigned int>> Graph::getAdjacencyList()
{
	std::vector<std::vector<unsigned int>> list;
	/*unsigned int** mat, n;
	mat = this->getMatrix(n);*/
	list.resize(numberOfNodes);
	for (auto edge : edges)
	{
		list[std::get<0>(edge)].push_back(std::get<1>(edge));
		if (!oriented)
			list[std::get<1>(edge)].push_back(std::get<0>(edge));

	}

	for (unsigned int i = 0; i < list.size(); i++)
	{
		std::sort(list[i].begin(), list[i].end());
	}

	return list;
}

void Graph::drawEdge(const unsigned int& node1, const unsigned int& node2, const unsigned int& index)
{
	auto node1Pos = nodes[node1]->getPos();
	auto node2Pos = nodes[node2]->getPos();

	float x1;
	float y1;

	float x2;
	float y2;

	float angle = oriented ? 30 : 0;

	float theta = Vector2Angle({ node1Pos.x, node1Pos.y }, { node2Pos.x, node2Pos.y });
	x1 = node1Pos.x + Node::radius * cosf((theta + angle) * float(M_PI / 180));
	y1 = node1Pos.y + Node::radius * sinf((theta + angle) * float(M_PI / 180));

	x2 = node2Pos.x - Node::radius * cosf((theta - angle) * float(M_PI / 180));
	y2 = node2Pos.y - Node::radius * sinf((theta - angle) * float(M_PI / 180));

	if (flow)
	{
		auto flow = std::get<3>(edges[index]);
		auto cap = std::get<2>(edges[index]);
		DrawText(std::to_string(flow).c_str(), (x1 + x2) / 2 - 20, (y1 + y2) / 2, 15, RED);
		DrawText(std::to_string(cap).c_str(), (x1 + x2) / 2, (y1 + y2) / 2, 15, BLACK);
	}

	Color color = (std::get<4>(edges[index])) ? CYAN : BLACK;

	DrawLine(x1, y1, x2, y2, color);
	if (oriented)
	{
		float nodesAngle = Vector2Angle(node1Pos, node2Pos) * float(M_PI / 180);
		float triAngle = 45 * float(M_PI / 180);
		//DrawText(std::to_string(nodesAngle).c_str(), 10, 23, 13, BLACK);
		Vector2 A, B;
		A.x = x2 - 12 * cosf(nodesAngle + triAngle);
		A.y = y2 - 12 * sinf(nodesAngle + triAngle);

		B.x = x2 - 12 * cosf(nodesAngle - triAngle);
		B.y = y2 - 12 * sinf(nodesAngle - triAngle);

		/*A.x = x2 - 12 * cosf(nodesAngle) - 12 * sinf(triAngle);
		A.y = y2 - 12 * sinf(nodesAngle) + 12 * cosf(triAngle);

		B.x = x2 - 12 * cosf(nodesAngle) + 12 * sinf(triAngle);
		B.y = y2 - 12 * sinf(nodesAngle) - 12 * cosf(triAngle);*/
		//Nu-i chiar asa, dar arata ca si cum ar fi triunghi 3D in cadranele I, II, III

		DrawTriangle({ x2, y2 }, A, B, BLACK);
		//DrawLine(x2, y2, A.x, A.y, BLACK);
		//DrawLine(x2, y2, B.x, B.y, BLACK);
	}


}

void Graph::dumpMatrixToFile(std::string fileName)
{
	unsigned int** mat;
	mat = getMatrix(numberOfNodes);
	printMatrixToFile(mat, numberOfNodes, numberOfNodes, fileName);
	matrixDealloc(mat, numberOfNodes, numberOfNodes);
}

void Graph::dumpAdjacencyListToFile(std::string fileName)
{
	std::ofstream f(fileName);
	auto list = this->getAdjacencyList();
	f << this->numberOfNodes << std::endl;
	for (unsigned int i = 0; i < list.size(); i++)
	{
		if (list[i].size() > 0)
		{
			f << i + 1 << ": ";
			for (auto it : list[i])
			{
				f << it + 1 << " ";
			}
			f << std::endl;
		}
	}
	f.close();
}

void Graph::addNodeNextNode(const Vector2& position)
{
	updatedSCC = true;
	updatedCC = true;
	nodes.push_back(new Node(++numberOfNodes, std::to_string(numberOfNodes), position));
	std::cout << "Node " << numberOfNodes << " pos: " << position.x << " " << position.y << std::endl;
}

bool Graph::addEdge(Node* node1, Node* node2, const unsigned int& cap)
{
	if (node1 != node2 && node1 != NULL && node2 != NULL)
	{
		if (oriented && !checkEdgeExistOriented(node1->getNumber() - 1, node2->getNumber() - 1) || !oriented && !checkEdgeExistUnoriented(node1->getNumber() - 1, node2->getNumber() - 1))
		{
			node1->setOldColor(SKYBLUE);
			node1->setColor(SKYBLUE);
			node2->setOldColor(SKYBLUE);
			node2->setColor(SKYBLUE);
			edges.push_back(std::make_tuple(node1->getNumber() - 1, node2->getNumber() - 1, cap, 0, false));
			//std::cout << "added edge: " << node1->getNumber() << " " << node2->getNumber() << std::endl;
			//std::cout << std::endl;
			//printEdges();
			//std::cout << std::endl;
			updatedSCC = true;
			updatedCC = true;
			return true;
		}
	}
	return false;
}

void Graph::printEdges()
{
	for (auto edge : edges)
		std::cout << "EDGE " << std::get<0>(edge) + 1 << " - " << std::get<1>(edge) + 1 << std::endl;
}

Node* Graph::checkNodeCollision(const Vector2& position, const Node* except)
{
	if (numberOfNodes <= 0) return NULL;

	for (unsigned int i = 0; i < numberOfNodes; i++)
	{

		if (nodes[i] != except)
		{
			auto compareNodePos = nodes[i]->getPos();
			if (Vector2Length({ position.x - compareNodePos.x, position.y - compareNodePos.y }) < 2 * Node::radius)
			{
				//std::cout << "overlaps: node" << i << std::endl;
				return nodes[i];
			}
		}

	}
	return NULL;
}

Node* Graph::checkNodeOverlap(const Vector2& position, const Node* except)
{
	if (numberOfNodes <= 0) return NULL;

	for (unsigned int i = 0; i < numberOfNodes; i++)
	{

		if (nodes[i] != except)
		{
			auto compareNodePos = nodes[i]->getPos();
			if (Vector2Length({ position.x - compareNodePos.x, position.y - compareNodePos.y }) < Node::radius)
			{
				//std::cout << "overlaps: node" << i << std::endl;
				return nodes[i];
			}
		}

	}
	return NULL;
}

bool Graph::checkEdgeExistOriented(const unsigned int& node1, const unsigned int& node2)
{
	//auto range = edges.equal_range(node1);
	//for (auto it = range.first; it != range.second; ++it)
	//{
	//	if (it->second == node2)
	//		return true;
	//}
	for (const auto& edge : edges)
	{
		if (std::get<0>(edge) == node1 && std::get<1>(edge) == node2)
			return true;
	}

	return false;
}

bool Graph::checkEdgeExistUnoriented(const unsigned int& node1, const unsigned int& node2)
{
	if (checkEdgeExistOriented(node1, node2) || checkEdgeExistOriented(node2, node1))
		return true;
	return false;
}



static void TarjanDFS(std::vector<std::vector<unsigned int>>& AdjacencyList,
	std::vector<int>& ids,
	const unsigned int& currentNode,
	std::vector<unsigned int>& lowlink,
	std::vector<bool>& onstack,
	std::stack<unsigned int>& stack,
	const unsigned int& id,
	unsigned int& maxid,
	unsigned int& SCCCount,
	std::vector<unsigned int>& result)
{
	stack.push(currentNode);
	onstack[currentNode] = true;
	ids[currentNode] = id;
	lowlink[currentNode] = id;

	if (maxid < id)
		maxid = id;
	//std::cout << std::endl << "onstack[" << currentNode << "] = " << onstack[currentNode] << std::endl;

	//std::cout << std::endl << "current node: " << currentNode << std::endl;

	for (auto& nextNode : AdjacencyList[currentNode])
	{
		if (ids[nextNode] == -1)
		{
			TarjanDFS(AdjacencyList, ids, nextNode, lowlink, onstack, stack, id + 1, maxid, SCCCount, result);
		}

		//std::cout << std::endl << "onstack[" << nextNode << "] = " << onstack[nextNode] << std::endl;
		if (onstack[nextNode])
		{
			//std::cout << std::endl << "lowlink[" << currentNode << "] = " << lowlink[currentNode] << std::endl;
			lowlink[currentNode] = std::min(lowlink[currentNode], lowlink[nextNode]);
		}
	}

	if (ids[currentNode] == lowlink[currentNode])
	{
		//std::cout << "CTC: ";
		//std::cout << currentNode << " ";
		SCCCount++;
		if (!stack.empty())
			for (auto node = stack.top(); !stack.empty();)
			{
				stack.pop();
				onstack[node] = false;
				lowlink[node] = ids[currentNode];

				result[node] = lowlink[currentNode];

				//std::cout << node << " ";

				if (node == currentNode)
					break;
			}
		//std::cout << std::endl;
	}
	//std::cout << "ids: ";
	//util::printVector(ids);
	//std::cout << std::endl;

}

int Graph::FindRoot()
{
	auto AdjacencyList = getAdjacencyList();
	//if (HasCycles(AdjacencyList), isOriented())
	//	return -1;
	//Nu este arbore

	std::vector<bool> visited(AdjacencyList.size(), false);
	bool dfsResult = true;

	unsigned int x, v;
	for (unsigned int x = 0; x < AdjacencyList.size(); x++)
	{
		if (!visited[x])
		{
			DFS_Root(x, AdjacencyList, visited, isOriented(), dfsResult);
			if (isOriented() && !dfsResult)
				return -1;
			v = x;
		}
	}

	visited.clear();
	visited.resize(AdjacencyList.size(), false);
	DFS_Root(v, AdjacencyList, visited, isOriented(), dfsResult);
	for (const auto node : visited)
	{
		if (!node)
			v = -1;
	}

	return v;


}

static bool BFS(std::vector<std::vector<unsigned int>> &adj, const unsigned int &s, const unsigned int &t, std::vector<bool> &visited, std::vector<int> &parents)
{
	parents.resize(0);
	parents.resize(adj.size(), -1);
	visited.resize(0);
	visited.resize(adj.size(), 0);
	std::queue<unsigned int> Q;
	Q.push(s);
	while (!Q.empty())
	{
		auto node = Q.front();
		visited[node] = true;
		Q.pop();
		for (const auto& nextNode : adj[node])
		{
			if (!visited[nextNode])
			{
				parents[nextNode] = node;
				Q.push(nextNode);
			}
		}
	}
	return visited[t];
}

unsigned int Graph::maxFlow(const unsigned int& s, const unsigned int& t)
{
	if (!numberOfNodes)
		return 0;
	std::vector<bool> visited;
	std::vector<int> parents;
	auto adj = getAdjacencyList();
	if (!BFS(adj, s, t, visited, parents))
		return 0;
	adj.clear();

	for (auto& e : edges)
	{
		std::get<4>(e) = false;
	}

	struct edge
	{
		unsigned int s;
		unsigned int t;
		unsigned int cap;
		unsigned int flow;
		edge* rev;
	};

	std::vector<std::vector<edge>> g(numberOfNodes);
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		g[std::get<0>(edges[i])].push_back(
			{
				std::get<0>(edges[i]),
				std::get<1>(edges[i]),
				std::get<2>(edges[i]),
				0,
				new edge
					(
						std::get<1>(edges[i]),
						std::get<0>(edges[i]),
						0,
						0,
						nullptr
					)
			}
		);
	}

	unsigned int max_flow = 0;

	std::vector<bool> vis(g.size(), false);
	while (true)
	{
		for (unsigned int i = 0; i < vis.size(); i++)
		{
			vis[i] = false;
		}
		std::vector<edge*> route;
		std::queue<unsigned int> q;
		q.push(s);
		std::vector<int> pred(g.size(), -1);
		while (!q.empty())
		{
			auto currentNode = q.front();
			//std::cout << currentNode << std::endl;
			vis[currentNode] = true;
			q.pop();
			for (unsigned int i = 0; i < g[currentNode].size(); i++)
			{
				auto& e = g[currentNode][i];
				if (vis[e.t] == false && e.s != e.t && e.cap > e.flow)
				{
					//std::cout << e.flow << std::endl;
					pred[e.t] = e.s;
					//vis[e.t] = true;
					route.push_back(&e);
					q.push(e.t);
				}
			}
		}
		//std::cout << "------------------------------------------------------\n";
		if (!vis[t])
			break;
		edge *min_edge = nullptr;
		unsigned int df = UINT_MAX;
		auto currentNode = t;
		for (auto node = pred[t]; node != -1; node = pred[node])
		{
			//std::cout << node << " ";
			for (auto& e : g[node])
			{
				if (e.t == currentNode)
				{
					df = __min(df, e.cap - e.flow);
					if(df == e.cap - e.flow)
						min_edge = &e;
					break;
				}
			}
			currentNode = node;
		}
		//std::cout << std::endl;
		//std::cout << std::endl;
		if(min_edge)
			std::cout << "min edge: " << min_edge->s << " - " << min_edge->t << std::endl;
		currentNode = t;
		for (auto node = pred[t]; node != -1; node = pred[node])
		{
			for (auto& e : g[node])
			{
				if (e.t == currentNode)
				{
					e.flow += df;
					e.rev->flow -= df;
					break;
				}
			}
			currentNode = node;
		}

		for (unsigned int i = 0; i < g.size(); i++)
		{
			for (auto& e : g[i])
			{
				for (unsigned int i = 0; i < edges.size(); i++)
				{
					if (std::get<0>(edges[i]) == e.s && std::get<1>(edges[i]) == e.t)
					{
						std::get<3>(edges[i]) = e.flow;
						if (min_edge)
						{
							if (&e == min_edge)
							{
								std::get<4>(edges[i]) = true;
							}
						}
						break;
					}
				}
			}
		}

		max_flow += df;
	}

	//std::queue<unsigned int> q;
	//q.push(s);
	//std::vector<bool> vis(g.size(), false);
	//while (!q.empty())
	//{
	//	auto currentNode = q.front();
	//	//std::cout << currentNode << std::endl;
	//	vis[currentNode] = true;
	//	q.pop();
	//	for (unsigned int i = 0; i < g[currentNode].size(); i++)
	//	{
	//		auto& e = g[currentNode][i];
	//		if (vis[e.t] == false && e.s != e.t && e.cap > e.flow)
	//		{
	//			vis[e.t] = true;
	//			q.push(e.t);
	//		}
	//	}
	//}
	//if (vis[t])
		//std::cout << "true" << std::endl;
	for (const auto &node_edges : g)
	{
		for(auto &e :node_edges)
		{
			bool isSpecial = false;
			if (vis[e.s] && !vis[e.t])
				isSpecial = true;
			for (auto& edge : edges)
			{
				if (std::get<0>(edge) == e.s && std::get<1>(edge) == e.t)
				{
					std::get<4>(edge) = isSpecial;
					if(std::get<4>(edge))
						std::cout << "edge " << std::get<0>(edge) << " - " << std::get<1>(edge) << " is special" << std::endl;
					break;
				}
			}
		}
	}

	for (unsigned int i = 0; i < g.size(); i++)
	{
		for (unsigned int j = 0; j < g[i].size(); j++)
		{
			delete(g[i][j].rev);
		}
	}
	return max_flow;
}

void Graph::DFS_Root(const unsigned int& currentNode, std::vector<std::vector<unsigned int>>& AdjacencyList, std::vector<bool>& visited, const bool& oriented, bool& result)
{
	visited[currentNode] = true;
	for (const auto& nextNode : AdjacencyList[currentNode])
	{
		if (!visited[nextNode])
		{
			DFS_Root(nextNode, AdjacencyList, visited, oriented, result);
		}
		else
			if (oriented)
			{
				result = false;
			}

	}
}

int Graph::StronglyConnectedComponents_tarjan(std::vector<std::vector<unsigned int>>& AdjacencyList, std::vector<unsigned int>& lowlink)
{
	std::vector<int> ids;
	std::vector<bool> onstack;
	std::vector<unsigned int> result;
	result.resize(AdjacencyList.size(), 0);
	lowlink.clear();

	std::stack<unsigned int> stack;

	unsigned int id = 0, SCCCount = 0;

	ids.resize(AdjacencyList.size(), -1);
	onstack.resize(AdjacencyList.size(), 0);
	lowlink.resize(AdjacencyList.size(), 0);
	//util::printVector(lowlink);
	bool stop = 0;

	for (unsigned int i = 0; i < AdjacencyList.size(); i++)
	{
		if (ids[i] == -1)
		{
			unsigned int maxid = id;
			TarjanDFS(AdjacencyList, ids, i, lowlink, onstack, stack, id, maxid, SCCCount, result);
			id = maxid + 1;
		}

	}

	//util::printVector(lowlink);
	//std::cout << std::endl;
	//util::printVector(ids);
	//std::cout << std::endl << SCCCount << std::endl;
	return SCCCount;
}

std::vector<std::vector<unsigned int>> Graph::TarjanOutputToGroup(std::vector<unsigned int>& lowlink, const unsigned int& SCCCount)
{
	std::vector<std::vector<unsigned int>> groups;
	std::unordered_map<unsigned int, unsigned int> sets;

	unsigned int index = 0;

	groups.resize(SCCCount);

	for (unsigned int i = 0; i < lowlink.size(); i++)
	{
		if (sets.count(lowlink[i]) == 0)
		{
			sets.insert(std::make_pair(lowlink[i], index++));
		}
		groups[sets[lowlink[i]]].push_back(i);
	}
	std::cout << "SCCCount: " << SCCCount << std::endl;
	util::printVector(lowlink);
	std::cout << std::endl;
	for (const auto& it : groups)
	{
		for (const auto& it2 : it)
			std::cout << it2 << " ";
		std::cout << std::endl;
	}

	return groups;
}

std::vector<std::vector<unsigned int>> Graph::AdjTransposed(std::vector<std::vector<unsigned int>>& AdjacencyList)
{
	std::vector<std::vector<unsigned int>> transpose;
	transpose.resize(AdjacencyList.size());

	for (unsigned int i = 0; i < AdjacencyList.size(); i++)
	{
		for (const auto& it : AdjacencyList[i])
		{
			transpose[it].push_back(i);
		}
	}
	return transpose;
}

static void KosarajuDFS1(const unsigned int& currentNode,std::vector<int>&visited, std::stack<unsigned int>& stack, std::vector<std::vector<unsigned int>>& AdjacencyList)
{
	visited[currentNode] = 1;

	for (const auto& node : AdjacencyList[currentNode])
	{
		if (visited[node] == -1)
		{
			KosarajuDFS1(node, visited, stack, AdjacencyList);
		}
	}
	stack.push(currentNode);
	//std::cout << currentNode << " ";
}

static void KosarajuDFS2(const unsigned int& currentNode, std::vector<int>& visited, std::vector<std::vector<unsigned int>>& transpose, std::vector<std::vector<unsigned int>>& result, const unsigned int& component)
{
	visited[currentNode] = 1;
	for (const auto& node : transpose[currentNode])
	{
		if (visited[node] == -1)
		{
			KosarajuDFS2(node, visited, transpose, result, component);
		}
	}
	result[component].push_back(currentNode);
	//std::cout << currentNode << " ";
}


int Graph::StronglyConnectedComponents_kosaraju(std::vector<std::vector<unsigned int>>& AdjacencyList, std::vector<std::vector<unsigned int>>& result)
{
	result.clear();
	std::vector<int> visited;
	visited.resize(AdjacencyList.size(), -1);
	auto transpose = AdjTransposed(AdjacencyList);
	std::stack<unsigned int> stack;

	for (unsigned int i = 0; i < AdjacencyList.size(); i++)
	{
		if (visited[i] == -1)
		{
			KosarajuDFS1(i, visited, stack, AdjacencyList);
		}
	}

	visited.clear();
	visited.resize(AdjacencyList.size(), -1);

	unsigned int SCCCount = 0;

	/*	std::cout << std::endl;
	for (unsigned int i = 0; i < transpose.size(); i++)
	{
		std::cout << i << ": ";
		util::printVector(transpose[i]);
		std::cout << std::endl;
	}*/

	while (!stack.empty())
	{
		int node = stack.top();
		stack.pop();
		//std::cout << "stack node: " << node << std::endl;
		if (visited[node] != -1)
			continue;
		//std::cout << std::endl;
		result.resize(result.size() + 1);
		KosarajuDFS2(node, visited, transpose, result, SCCCount);
		SCCCount++;
	}
	for(auto &comp : result)
	{
		std::sort(comp.begin(), comp.end());
	}
	return SCCCount;
}

int Graph::ConnectedComponents(std::vector<std::vector<unsigned int>>& AdjacencyList, std::vector<std::vector<unsigned int>>& result)
{
	std::vector<std::vector<unsigned int>> Adj_Bi;
	Adj_Bi = AdjacencyList;
	for (int i = 0; i < Adj_Bi.size(); i++)
	{
		for (auto j : Adj_Bi[i])
		{
			bool found = false;
			for (auto k : Adj_Bi[j])
			{
				if (k == i)
				{
					found = true;
					break;
				}
			}
			if (!found)
				Adj_Bi[j].push_back(i);
		}
	}

	std::queue<int> C;

	int* visited = new int[AdjacencyList.size()], cnt = 0, aux;
	*visited = {};
	result.clear();


	for (unsigned int i = 0; i < AdjacencyList.size(); i++)visited[i] = 0;

	for (unsigned int i = 0; i < AdjacencyList.size(); i++)
	{
		if (!visited[i])
		{
			C.push(i);
			result.resize(++cnt);
		}
		while (!C.empty())
		{
			auto currentNode = C.front();
			C.pop();

			if (!Adj_Bi[currentNode].size())
			{
				result[cnt - 1].push_back(currentNode);
				continue;
			}

			for (auto nextNode : Adj_Bi[currentNode])
			{
				if (!visited[nextNode])
				{
					C.push(nextNode);
					result[cnt - 1].push_back(nextNode);
					visited[nextNode] = cnt;
				}
			}
		}
	}
	//std::cout << "Graful are " << cnt << " componente conexe:" << std::endl;
	for (int i = 0; i < result.size(); i++)
	{
		std::sort(result[i].begin(), result[i].end());

		//for (auto x : result[i])
			//std::cout << x << " ";
		//std::cout << std::endl;
	}
	return cnt;
}

static void TSort(std::vector<std::vector<unsigned int>>& adj, std::vector<int>& visited, const unsigned int& currentNode, std::vector<int>& result)
{

	for (auto it : adj[currentNode])
	{
		if (visited[it] == -1)
		{
			visited[it] = 1;
			TSort(adj, visited, it, result);
			result.insert(result.begin(), it);
		}
	}
}

int Graph::TopSort(std::vector<std::vector<unsigned int>>& AdjacencyList, std::vector<int>& result)
{
	if (AdjacencyList.size() == 0)
		return false;
	if (Graph::HasCycles(AdjacencyList))
		return false;

	std::vector<int> visited;
	visited.resize(AdjacencyList.size(), -1);

	std::stack<unsigned int> st;
	st.push(0);

	bool stop = 0;
	unsigned int depth = 0;
	for (unsigned int i = 0; i < AdjacencyList.size(); i++)
	{
		if (visited[i] == -1)
		{
			visited[i] = depth;
			TSort(AdjacencyList, visited, i, result);
			result.insert(result.begin(), i);
		}
	}
	return 1;
}

int Graph::HasCycles(std::vector<std::vector<unsigned int>>& AdjacencyList, const bool& oriented)
{
	if (!AdjacencyList.size())
		return 0;
	if (!oriented)
	{
		unsigned int arcs = 0;
		for (const auto& arc : AdjacencyList)
		{
			arcs += arc.size();
		}
		if (arcs / 2 >= AdjacencyList.size())
			return 0;
		return 1;
	}
	std::vector<unsigned int> aux;
	int scccount = StronglyConnectedComponents_tarjan(AdjacencyList, aux);
	if (scccount < AdjacencyList.size())
		return AdjacencyList.size() - scccount;
	else return 0;
}
