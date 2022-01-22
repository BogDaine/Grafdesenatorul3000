#include "Graph.h"
#include "raylib\raylib.h"
#include "raylib\raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raylib\extras\raygui.h"
#include <thread>
#include <iostream>
#include <chrono>
#include "Button.h"


using namespace std::chrono_literals;

namespace util
{
	template <typename T>
	void printVector(std::vector<T>& v)
	{
		for (auto it : v)
			std::cout << it << " ";
	}
}

int Node::radius;
int Node::fontSize;
static int spinbox_value = 0;
static char* dummy_text = (char*)(malloc(128));

enum class app_modes
{
	IDLE,
	NODE_INSERT,
	EDGE_INSERT,
	NODE_RELOCATE,
	SELECT
};

void drawStuff(std::vector<Drawable*> stuff)
{
	for (auto i : stuff)
		i->draw();
}

void dragNode(Graph& g)
{
	Vector2 mousePos = GetMousePosition();
	Node* nodeToMove = g.checkNodeOverlap(mousePos);
	Vector2 oldPos;
	if (nodeToMove) oldPos = nodeToMove->getPos();
	if (nodeToMove)
	{
		nodeToMove->select();
		while (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{

			mousePos = GetMousePosition();
			if (!g.checkNodeCollision(mousePos, nodeToMove) && mousePos.x >= 0 && mousePos.x <= WINDOW_WIDTH &&
				mousePos.y >= 0 && mousePos.y <= WINDOW_HEIGHT)
			{
				nodeToMove->setPos(mousePos);
				oldPos = mousePos;
			}
			else
			{
				nodeToMove->setPos(oldPos);
			}
		}
		nodeToMove->deselect();
	}
}

void graphInitMenuLogic(RadioButton& rb1, RadioButton& rb2, RadioButton& rb3, Knob& nodesNumberKnob, Knob& pathsNumberKnob, Knob& cyclesNumberKnob, bool& oriented, bool& flow, bool& logicOK)
{
	while (true)
	{

		if (rb1.isPressed())
		{
			rb1.select();
			rb2.deselect();
			oriented = 1;
		}
		if (rb2.isPressed())
		{
			rb2.select();
			rb1.deselect();
			oriented = 0;
		}
		if (rb3.isPressed())
		{
			if (rb3.isSelected())
			{
				rb3.deselect();
				flow = false;
			}
			else
			{
				rb3.select();
				flow = true;
			}
		}

		nodesNumberKnob.grab();
		pathsNumberKnob.grab();
		cyclesNumberKnob.grab();

		if (IsKeyPressed(KEY_ENTER))
		{
			logicOK = true;
			break;
		}
		if (WindowShouldClose()) exit(0);

		std::this_thread::sleep_for(10ms);
	}
}

void graphInitMenu(bool& oriented, float* nNodes, float* nPaths, float* nCycles, bool& flow)
{
	RadioButton rb3({ WINDOW_WIDTH * 0.35, WINDOW_HEIGHT * 0.898 }, 6);
	RadioButton rb1({ WINDOW_WIDTH * 0.35, WINDOW_HEIGHT * 0.929 }, 6);
	RadioButton rb2({ WINDOW_WIDTH * 0.35, WINDOW_HEIGHT * 0.96 }, 6);
	Knob nodesNumberKnob({ 100, WINDOW_HEIGHT / 2 }, 20, nNodes);
	Knob pathsNumberKnob({ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 }, 20, nPaths);
	Knob cyclesNumberKnob({ WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2 }, 20, nCycles);

	std::vector<Drawable*> UI;

	UI.push_back(&rb1);
	UI.push_back(&rb2);
	UI.push_back(&rb3);
	UI.push_back(&nodesNumberKnob);
	UI.push_back(&pathsNumberKnob);
	UI.push_back(&cyclesNumberKnob);

	bool logicOK = false;

	std::thread menuLogic(graphInitMenuLogic, std::ref(rb1), std::ref(rb2),
		std::ref(rb3),
		std::ref(nodesNumberKnob), std::ref(pathsNumberKnob), std::ref(cyclesNumberKnob),
		std::ref(oriented), std::ref(flow), std::ref(logicOK));

	while (!logicOK)
	{
		//if (IsKeyPressed(KEY_ENTER)) break;
		if (WindowShouldClose()) exit(0);

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		drawStuff(UI);

		DrawText("ENTER pentru a continua. Default: graf neorientat", WINDOW_WIDTH * 0.38, WINDOW_HEIGHT * 0.05, 20, BLACK);
		DrawText("are flux:", WINDOW_WIDTH * 0.017, WINDOW_HEIGHT * 0.87, 20, BLACK);
		DrawText("pentru graf orientat:", WINDOW_WIDTH * 0.017, WINDOW_HEIGHT * 0.91, 20, BLACK);
		DrawText("pentru graf neorientat:", WINDOW_WIDTH * 0.017, WINDOW_HEIGHT * 0.95, 20, BLACK);



		std::string nNodesText = std::string("Nodes: ") + std::string(std::to_string(unsigned int(nodesNumberKnob.getValue())));
		std::string nPathsText = std::string("Paths: ") + std::string(std::to_string(unsigned int(pathsNumberKnob.getValue())));
		std::string nCyclesText = std::string("Cycles: ") + std::string(std::to_string(unsigned int(cyclesNumberKnob.getValue())));

		DrawText(nNodesText.c_str(), 60, WINDOW_HEIGHT / 2 - 45, 20, BLACK);
		DrawText(nPathsText.c_str(), WINDOW_WIDTH / 2 - 40, WINDOW_HEIGHT / 2 - 45, 20, BLACK);
		DrawText(nCyclesText.c_str(), WINDOW_WIDTH - 140, WINDOW_HEIGHT / 2 - 45, 20, BLACK);


		//DrawText("functii:", WINDOW_WIDTH-250,WINDOW_HEIGHT*0.5, 20, BLACK);
		DrawText("i: idle", WINDOW_WIDTH - 250, WINDOW_HEIGHT * 0.83, 20, BLACK);
		DrawText("n: inserare nod", WINDOW_WIDTH - 250, WINDOW_HEIGHT * 0.87, 20, BLACK);
		DrawText("e: inserare arc/muchie", WINDOW_WIDTH - 250, WINDOW_HEIGHT * 0.91, 20, BLACK);
		DrawText("r: repozitionare noduri", WINDOW_WIDTH - 250, WINDOW_HEIGHT * 0.95, 20, BLACK);


		EndDrawing();

		//std::cout << nPaths << std::endl;
	}
	std::cout << nodesNumberKnob.getValuePointer() << std::endl;
	menuLogic.join();
	std::cout << nodesNumberKnob.getValuePointer() << std::endl;
	std::cout << nNodes << std::endl;
}


void display(Graph& g, const std::string& modeName)
{
	std::vector<Drawable*> StuffToDraw;

	RectangularButton topSortButton({0,0}, 200, 26);
	topSortButton.setLabel("Sortare topologica");

	RectangularButton connectedComponentsButton({0,26}, 200, 26);
	connectedComponentsButton.setLabel("Componente conexe");
	RectangularButton connectedComponentsButtonV2({0,52}, 200, 26, GOLD);
	connectedComponentsButtonV2.setLabel("Componente conexe V2");

	RectangularButton stronglyConnectedComponentsButton({0,78}, 260, 26);
	stronglyConnectedComponentsButton.setLabel("Componente tare conexe");
	RectangularButton stronglyConnectedComponentsButtonV2({0,104}, 260, 26, GOLD);
	stronglyConnectedComponentsButtonV2.setLabel("Componente tare conexe V2");

	

	RectangularButton findRootButton({ 0,130 }, 200, 26);
	findRootButton.setLabel("Gasire radacina");

	RectangularButton maxFlowButton({ 0, 156 }, 200, 26);
	maxFlowButton.setLabel("Fluxul Maxim");

	RectangularButton resetViewButton({ 0,182 }, 200, 26);
	resetViewButton.setLabel("Resetare afisare");
	

	//RectangularButton resetButton({ 0, 104 }, 260, 26);
	//resetButton.setLabel("Resetare graf");

	StuffToDraw.push_back(&topSortButton);
	StuffToDraw.push_back(&connectedComponentsButton);
	StuffToDraw.push_back(&connectedComponentsButtonV2);
	StuffToDraw.push_back(&stronglyConnectedComponentsButton);
	StuffToDraw.push_back(&stronglyConnectedComponentsButtonV2);
	StuffToDraw.push_back(&findRootButton);
	StuffToDraw.push_back(&maxFlowButton);
	StuffToDraw.push_back(&resetViewButton);
	//StuffToDraw.push_back(&resetButton);
	StuffToDraw.push_back(&g);

	while (!WindowShouldClose())

	{
		if (modeName == "idle")
		{
			if (topSortButton.isPressed())
			{
				std::cout << "top sort!" << std::endl;
				std::vector<int> SortedNodes;
				auto AdjiacencyList = g.getAdjacencyList();
				if (!g.isOriented())
				{
					std::cout << "Eroare: Graful este neorientat!" << std::endl;
				}
				else
				if (Graph::TopSort(AdjiacencyList, SortedNodes))
				{
					util::printVector(SortedNodes);
					std::cout << std::endl;
				}
				else
				{
					std::cout << "Nodurile grafului nu au putut fi sortate.\nVerificati ca are noduri si ca nu este ciclic..." << std::endl;
				}
				SortedNodes.clear();
				g.dumpAdjacencyListToFile("adj.txt");
			}

			else if (connectedComponentsButton.isPressed())
			{
				auto adj = g.getAdjacencyList();
				Graph::ConnectedComponents(adj, g.getCC());

				g.group_Color(g.getCC());
				g.setDrawingMode(graph_drawing_modes::NORMAL);
			}

			else if (connectedComponentsButtonV2.isPressed())
			{

				auto adj = g.getAdjacencyList();
				

				Graph::ConnectedComponents(adj, g.getCC());

				g.group_Color(g.getCC());
				g.setGroup(&g.getCC());
				g.setDrawingMode(graph_drawing_modes::GROUPED_NODE);
			}

			else if (stronglyConnectedComponentsButton.isPressed())
			{
				if (g.isOriented())
				{
					auto adj = g.getAdjacencyList();
					Graph::StronglyConnectedComponents_kosaraju(adj, g.getSCC());

					g.group_Color(g.getSCC());
					g.setDrawingMode(graph_drawing_modes::NORMAL);
				}
				else std::cout << "Eroare: Graful este neorientat!" << std::endl;

			}
			else if (stronglyConnectedComponentsButtonV2.isPressed())
			{
				if (g.isOriented())
				{
					auto adj = g.getAdjacencyList();
					unsigned int SCCCount = Graph::StronglyConnectedComponents_kosaraju(adj, g.getSCC());
					g.group_Color(g.getSCC());

					g.setGroup(&g.getSCC());
					g.setDrawingMode(graph_drawing_modes::GROUPED_NODE);
				}
				else std::cout << "Eroare: Graful este neorientat!" << std::endl;
			}
			else if (findRootButton.isPressed())
			{
				auto result = g.FindRoot();
				if (result >= 0)
					std::cout << result + 1 << std::endl;
				else std::cout << "Graful nu este arbore" << std::endl;

			}
			else if (maxFlowButton.isPressed())
			{
				std::cout << "max flow: " << g.maxFlow(0, g.getNumberOfNodes() - 1) << std::endl;
			}
			else if (resetViewButton.isPressed())
			{
				g.resetColors();
				g.setDrawingMode(graph_drawing_modes::NORMAL);
			}


		}
		BeginDrawing();


		ClearBackground(RAYWHITE);
		
		drawStuff(StuffToDraw);
		if (g.hasFlow())
		{
			//GuiTextBox({ 200, 200, 30, 30 }, dummy_text, 4, GetMousePosition().x <= 230 && GetMousePosition().x >= 200 && GetMousePosition().y <= 230 && GetMousePosition().x >= 200);
			GuiSpinner({ 200, 200, 60, 30 }, dummy_text, &spinbox_value, 0, 999, 1);
		}
		DrawText(std::string(g.isOriented() ? "orientat" : "neorientat").c_str(), WINDOW_WIDTH - (g.isOriented() ? 100 : 130), WINDOW_HEIGHT * 0.95, 20, BLACK);
		DrawText(std::string("application mode: " + modeName).c_str(), 30, WINDOW_HEIGHT * 0.95, 20, BLACK);

		//if(g.getNumberOfNodes())DrawText(std::to_string(Vector2Angle(g.nodes[0]->getPos(), GetMousePosition())).c_str(),10, 10, 13, BLACK);
		EndDrawing();
	}
	exit(0);
}

Node* selectNode(Graph& g)
{
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
		Node* node = g.checkNodeOverlap(GetMousePosition());
		if (node)
		{
			node->select();
			//std::cout << "Node " << node->getNumber() << " selected" << std::endl;
			return node;
		}
	}
	return NULL;
}

app_modes checkModes(app_modes& mode, std::string& modeName, const unsigned int& numberOfNodes)
{
	if (IsKeyPressed(KEY_I))
	{
		mode = app_modes::IDLE;
		modeName = "idle";
		return mode;
	}
	if (IsKeyPressed(KEY_N))
	{
		mode = app_modes::NODE_INSERT;
		modeName = "inserting nodes";
		return mode;
	}
	if (IsKeyPressed(KEY_E))
	{
		if (numberOfNodes >= 2)
		{
			mode = app_modes::EDGE_INSERT;
			modeName = "inserting edges";
		}
		return mode;
	}
	if (IsKeyPressed(KEY_R))
	{
		mode = app_modes::NODE_RELOCATE;
		modeName = "relocating nodes";
		return mode;
	}
}

void doStuff(Graph& g, std::string& modeName)
{
	app_modes mode = app_modes::IDLE;


	Node* selectBuffer[2];
	selectBuffer[0] = NULL;
	selectBuffer[1] = NULL;
	//InitWindow(800, 600, "titlu");
	while (true)
	{

		switch (mode)
		case app_modes::IDLE:
		{
			checkModes(mode, modeName, g.getNumberOfNodes());
			break;

		case app_modes::NODE_INSERT:
			if (!IsMouseButtonUp(MOUSE_BUTTON_LEFT))
			{
				Vector2 mousePos = GetMousePosition();
				if (!g.checkNodeCollision(mousePos))
				{
					g.addNodeNextNode(mousePos);
					g.dumpMatrixToFile("graphMatrix.txt");
				}

			}
			checkModes(mode, modeName, g.getNumberOfNodes());
			break;

		case app_modes::EDGE_INSERT:
			while (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {}
			while (!selectBuffer[0])
			{
				checkModes(mode, modeName, g.getNumberOfNodes());
				if (mode == app_modes::EDGE_INSERT)
					selectBuffer[0] = selectNode(g);
				else break;
			}
			while ((!selectBuffer[1] || selectBuffer[0] == selectBuffer[1]) && selectBuffer[0] != NULL)
			{
				checkModes(mode, modeName, g.getNumberOfNodes());
				if (mode != app_modes::EDGE_INSERT)
				{
					selectBuffer[0]->deselect();
					selectBuffer[1] = NULL;
					break;
				}
				selectBuffer[1] = selectNode(g);
			}
			if (selectBuffer[0])selectBuffer[0]->deselect();
			if (selectBuffer[1]) selectBuffer[1]->deselect();

			if (g.addEdge(selectBuffer[0], selectBuffer[1], (unsigned int) spinbox_value))
			{
				g.dumpMatrixToFile("graphMatrix.txt");
			}

			selectBuffer[0] = NULL;
			selectBuffer[1] = NULL;

			mode = app_modes::IDLE;
			modeName = "idle";

			checkModes(mode, modeName, g.getNumberOfNodes());

			break;
		case app_modes::NODE_RELOCATE:
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				dragNode(g);
				//g.dumpMatrixToFile("graphMatrix.txt");
			}
			checkModes(mode, modeName, g.getNumberOfNodes());
			break;
		}

		std::this_thread::sleep_for(10ms);
		//display(g, modeName);

	}
}

void sayStuff(const std::string& stuff)
{
	for (;;)
		std::cout << stuff << std::endl;
}

void application()
{
	Node::radius = 15;
	Node::fontSize = 20;
	memset(dummy_text, 0, 128);

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Grafdesenatorul 3000");

	bool oriented = 0, flow = 0;
	float nNodes = 0, nPaths = 0, nCycles = 0;
	graphInitMenu(oriented, &nNodes, &nPaths, &nCycles, flow);

	//Graph g(oriented);
	//if(flow)
	//std::cout << "flow is true" << std::endl;
	Graph g(nNodes, (nPaths), (nCycles), oriented, flow, false);
	std::string modeName = "idle";

	std::thread graphThread(doStuff, std::ref(g), std::ref(modeName));
	display(g, modeName);

}

int main()
{
	SetTargetFPS(60);
	SetExitKey(0);
	application();
	return 0;
}