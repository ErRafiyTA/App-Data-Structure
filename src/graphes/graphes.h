/**
 * GRAPHES.H - Module Graphes
 */

#ifndef GRAPHES_H
#define GRAPHES_H

#include "../theme.h"
#include "../ui_common.h"
#include <stdbool.h>

// ==================== STRUCTURES DE DONNÉES ====================

#define MAX_NODES 50
#define MAX_EDGES 200

typedef struct {
  int id;
  char label[32]; // Custom label (any text)
  float x, y;
  bool active;
} GraphNode;

typedef struct {
  int from, to;
  int weight;
  bool active;
} GraphEdge;

typedef struct {
  GraphNode nodes[MAX_NODES];
  GraphEdge edges[MAX_EDGES];
  int node_count;
  int edge_count;
  bool directed;
} Graph;

// ==================== OPÉRATIONS GRAPHE ====================

void Graph_Init(Graph *g, bool directed);
void Graph_Clear(Graph *g);

int Graph_AddNode(Graph *g, float x, float y, const char *label);
bool Graph_RemoveNode(Graph *g, int node_id);

bool Graph_AddEdge(Graph *g, int from, int to, int weight);
bool Graph_RemoveEdge(Graph *g, int from, int to);

bool Graph_HasEdge(Graph *g, int from, int to);
int Graph_GetEdgeWeight(Graph *g, int from, int to);

// ==================== ALGORITHMES ====================

// BFS - retourne l'ordre de parcours, taille dans result_size
int *Graph_BFS(Graph *g, int start_id, int *result_size);

// DFS - retourne l'ordre de parcours
int *Graph_DFS(Graph *g, int start_id, int *result_size);

// Dijkstra - retourne le chemin le plus court de start à end
int *Graph_Dijkstra(Graph *g, int start_id, int end_id, int *path_length,
                    int *total_cost);
int *Graph_BellmanFord(Graph *g, int start_id, int end_id, int *path_length,
                       int *total_cost);

// Floyd-Warshall matrix result
typedef struct {
  int costs[MAX_NODES][MAX_NODES];
  int next[MAX_NODES][MAX_NODES];
  int count;
  int node_ids[MAX_NODES];
} FloydResult;

FloydResult Graph_FloydWarshall(Graph *g);

// ==================== ÉCRAN GRAPHES ====================

typedef enum {
  GRAPH_TOOL_SELECT,
  GRAPH_TOOL_ADD_NODE,
  GRAPH_TOOL_ADD_EDGE,
  GRAPH_TOOL_DELETE,
  GRAPH_TOOL_MODIFY
} GraphTool;

typedef struct {
  // Graphe
  Graph graph;

  // UI
  Button btn_back;
  Button btn_clear;
  Button btn_directed;
  Button btn_undirected;

  Button btn_tool_select;
  Button btn_tool_add_node;
  Button btn_tool_add_edge;
  Button btn_tool_delete;
  Button btn_tool_modify;

  Button btn_bellman; // Bellman-Ford
  Button btn_floyd;   // Floyd-Warshall
  Button btn_dijkstra;

  // Outil actif
  GraphTool current_tool;

  // Sélection pour arête
  int edge_from;
  bool edge_selecting;

  // Nœud sélectionné/déplacé
  int selected_node;
  bool dragging_node;

  // Résultat algo
  int *algo_result;
  int algo_result_size;
  int algo_current_idx;
  float algo_timer;
  bool algo_animating;
  char algo_name[32];

  // Chemin Dijkstra surligné
  int *dijkstra_path;
  int dijkstra_path_length;
  int dijkstra_cost;

  // Status
  char status_message[128];
  float status_timer;

  // ======= POPUPS =======
  // Popup poids arête
  bool popup_weight_open;
  bool popup_weight_is_modify;
  int popup_edge_from;
  int popup_edge_to;
  char popup_weight_text[16];
  int popup_weight_cursor;

  // Popup algorithme (départ/arrivée)
  bool popup_algo_open;
  int popup_algo_step;      // 0 = demande départ, 1 = demande arrivée
  char popup_algo_type[16]; // "BFS", "DFS", "DIJKSTRA"
  char popup_start_text[8];
  char popup_end_text[8];
  int popup_cursor;

  // Popup résultat algorithme
  bool popup_result_open;
  char popup_result_title[64];
  char popup_result_text[512];
  int popup_result_shortest_idx; // Index of shortest path line for green color

  // Popup ajout noeud (label)
  bool popup_node_open;
  bool popup_node_is_modify;
  int popup_node_target_id;
  float popup_node_x;
  float popup_node_y;
  char popup_node_text[32];
  int popup_node_cursor;

} GraphesScreen;

// Fonctions écran
void GraphesInit(GraphesScreen *screen);
AppScreen GraphesUpdate(GraphesScreen *screen);
void GraphesDraw(GraphesScreen *screen);
void GraphesUnload(GraphesScreen *screen);

#endif // GRAPHES_H
