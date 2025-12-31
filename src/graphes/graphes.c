/**
 * GRAPHES.C - Implémentation du module Graphes
 */

#include "graphes.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==================== OPÉRATIONS GRAPHE ====================

void Graph_Init(Graph *g, bool directed) {
  memset(g, 0, sizeof(Graph));
  g->directed = directed;
}

void Graph_Clear(Graph *g) {
  bool directed = g->directed;
  memset(g, 0, sizeof(Graph));
  g->directed = directed;
}

int Graph_AddNode(Graph *g, float x, float y, const char *label) {
  if (g->node_count >= MAX_NODES)
    return -1;

  // Trouver un ID libre
  int id = 0;
  for (int i = 0; i < MAX_NODES; i++) {
    bool used = false;
    for (int j = 0; j < g->node_count; j++) {
      if (g->nodes[j].active && g->nodes[j].id == i) {
        used = true;
        break;
      }
    }
    if (!used) {
      id = i;
      break;
    }
  }

  // Ajouter le nœud
  for (int i = 0; i < MAX_NODES; i++) {
    if (!g->nodes[i].active) {
      g->nodes[i].id = id;
      g->nodes[i].x = x;
      g->nodes[i].y = y;
      g->nodes[i].active = true;
      strncpy(g->nodes[i].label, label, 31);
      g->nodes[i].label[31] = '\0';
      g->node_count++;
      return id;
    }
  }

  return -1;
}

bool Graph_RemoveNode(Graph *g, int node_id) {
  // Supprimer le nœud
  for (int i = 0; i < MAX_NODES; i++) {
    if (g->nodes[i].active && g->nodes[i].id == node_id) {
      g->nodes[i].active = false;
      g->node_count--;

      // Supprimer les arêtes connectées
      for (int j = 0; j < MAX_EDGES; j++) {
        if (g->edges[j].active &&
            (g->edges[j].from == node_id || g->edges[j].to == node_id)) {
          g->edges[j].active = false;
          g->edge_count--;
        }
      }
      return true;
    }
  }
  return false;
}

bool Graph_AddEdge(Graph *g, int from, int to, int weight) {
  if (g->edge_count >= MAX_EDGES)
    return false;
  if (from == to)
    return false;

  // Vérifier que les nœuds existent
  bool from_exists = false, to_exists = false;
  for (int i = 0; i < MAX_NODES; i++) {
    if (g->nodes[i].active && g->nodes[i].id == from)
      from_exists = true;
    if (g->nodes[i].active && g->nodes[i].id == to)
      to_exists = true;
  }
  if (!from_exists || !to_exists)
    return false;

  // Vérifier si l'arête existe déjà
  if (Graph_HasEdge(g, from, to))
    return false;

  // Ajouter l'arête
  for (int i = 0; i < MAX_EDGES; i++) {
    if (!g->edges[i].active) {
      g->edges[i] =
          (GraphEdge){.from = from, .to = to, .weight = weight, .active = true};
      g->edge_count++;
      return true;
    }
  }

  return false;
}

bool Graph_RemoveEdge(Graph *g, int from, int to) {
  for (int i = 0; i < MAX_EDGES; i++) {
    if (g->edges[i].active && g->edges[i].from == from &&
        g->edges[i].to == to) {
      g->edges[i].active = false;
      g->edge_count--;
      return true;
    }
    // Pour graphe non-dirigé
    if (!g->directed && g->edges[i].active && g->edges[i].from == to &&
        g->edges[i].to == from) {
      g->edges[i].active = false;
      g->edge_count--;
      return true;
    }
  }
  return false;
}

bool Graph_HasEdge(Graph *g, int from, int to) {
  for (int i = 0; i < MAX_EDGES; i++) {
    if (g->edges[i].active) {
      if (g->edges[i].from == from && g->edges[i].to == to)
        return true;
      if (!g->directed && g->edges[i].from == to && g->edges[i].to == from)
        return true;
    }
  }
  return false;
}

int Graph_GetEdgeWeight(Graph *g, int from, int to) {
  for (int i = 0; i < MAX_EDGES; i++) {
    if (g->edges[i].active) {
      if (g->edges[i].from == from && g->edges[i].to == to)
        return g->edges[i].weight;
      if (!g->directed && g->edges[i].from == to && g->edges[i].to == from)
        return g->edges[i].weight;
    }
  }
  return -1;
}

bool Graph_HasNegativeWeights(Graph *g) {
  for (int i = 0; i < MAX_EDGES; i++) {
    if (g->edges[i].active && g->edges[i].weight < 0) {
      return true;
    }
  }
  return false;
}

// Helper: obtenir les voisins d'un nœud
static int GetNeighbors(Graph *g, int node_id, int *neighbors, int *weights) {
  int count = 0;
  for (int i = 0; i < MAX_EDGES; i++) {
    if (g->edges[i].active) {
      if (g->edges[i].from == node_id) {
        neighbors[count] = g->edges[i].to;
        weights[count] = g->edges[i].weight;
        count++;
      } else if (!g->directed && g->edges[i].to == node_id) {
        neighbors[count] = g->edges[i].from;
        weights[count] = g->edges[i].weight;
        count++;
      }
    }
  }
  return count;
}

// ==================== ALGORITHMES ====================

int *Graph_BFS(Graph *g, int start_id, int *result_size) {
  if (g->node_count == 0) {
    *result_size = 0;
    return NULL;
  }

  int *result = (int *)malloc(MAX_NODES * sizeof(int));
  bool visited[MAX_NODES] = {false};
  int queue[MAX_NODES];
  int front = 0, rear = 0;
  int count = 0;

  queue[rear++] = start_id;
  visited[start_id] = true;

  while (front < rear) {
    int current = queue[front++];
    result[count++] = current;

    int neighbors[MAX_NODES], weights[MAX_NODES];
    int neighbor_count = GetNeighbors(g, current, neighbors, weights);

    for (int i = 0; i < neighbor_count; i++) {
      if (!visited[neighbors[i]]) {
        visited[neighbors[i]] = true;
        queue[rear++] = neighbors[i];
      }
    }
  }

  *result_size = count;
  return result;
}

static void DFS_Helper(Graph *g, int node, bool *visited, int *result,
                       int *count) {
  visited[node] = true;
  result[(*count)++] = node;

  int neighbors[MAX_NODES], weights[MAX_NODES];
  int neighbor_count = GetNeighbors(g, node, neighbors, weights);

  for (int i = 0; i < neighbor_count; i++) {
    if (!visited[neighbors[i]]) {
      DFS_Helper(g, neighbors[i], visited, result, count);
    }
  }
}

int *Graph_DFS(Graph *g, int start_id, int *result_size) {
  if (g->node_count == 0) {
    *result_size = 0;
    return NULL;
  }

  int *result = (int *)malloc(MAX_NODES * sizeof(int));
  bool visited[MAX_NODES] = {false};
  int count = 0;

  DFS_Helper(g, start_id, visited, result, &count);

  *result_size = count;
  return result;
}

int *Graph_Dijkstra(Graph *g, int start_id, int end_id, int *path_length,
                    int *total_cost) {
  if (g->node_count == 0) {
    *path_length = 0;
    *total_cost = -1;
    return NULL;
  }

  int dist[MAX_NODES];
  int prev[MAX_NODES];
  bool visited[MAX_NODES] = {false};

  // CHECK FOR NEGATIVE WEIGHTS
  for (int i = 0; i < MAX_EDGES; i++) {
    if (g->edges[i].active && g->edges[i].weight < 0) {
      *path_length = 0;
      *total_cost = -99998; // Error code for negative weights in Dijkstra
      return NULL;
    }
  }

  // Initialisation
  for (int i = 0; i < MAX_NODES; i++) {
    dist[i] = INT_MAX;
    prev[i] = -1;
  }
  dist[start_id] = 0;

  // Dijkstra
  for (int iter = 0; iter < g->node_count; iter++) {
    // Trouver le nœud non visité avec la plus petite distance
    int u = -1;
    int min_dist = INT_MAX;

    for (int i = 0; i < MAX_NODES; i++) {
      if (g->nodes[i].active && !visited[g->nodes[i].id] &&
          dist[g->nodes[i].id] < min_dist) {
        min_dist = dist[g->nodes[i].id];
        u = g->nodes[i].id;
      }
    }

    if (u == -1 || u == end_id)
      break;

    visited[u] = true;

    // Relaxation
    int neighbors[MAX_NODES], weights[MAX_NODES];
    int neighbor_count = GetNeighbors(g, u, neighbors, weights);

    for (int i = 0; i < neighbor_count; i++) {
      int v = neighbors[i];
      int alt = dist[u] + weights[i];
      if (alt < dist[v]) {
        dist[v] = alt;
        prev[v] = u;
      }
    }
  }

  // Reconstruire le chemin
  if (dist[end_id] == INT_MAX) {
    *path_length = 0;
    *total_cost = -1;
    return NULL;
  }

  // Compter la longueur du chemin
  int len = 0;
  int current = end_id;
  while (current != -1) {
    len++;
    current = prev[current];
  }

  // Construire le chemin
  int *path = (int *)malloc(len * sizeof(int));
  current = end_id;
  for (int i = len - 1; i >= 0; i--) {
    path[i] = current;
    current = prev[current];
  }

  *path_length = len;
  *total_cost = dist[end_id];
  return path;
}

// Structure pour stocker tous les chemins
#define MAX_PATHS 50
#define MAX_PATH_LEN 20

typedef struct {
  int paths[MAX_PATHS][MAX_PATH_LEN];
  int path_lengths[MAX_PATHS];
  int path_costs[MAX_PATHS];
  int path_count;
  int shortest_idx;
} AllPaths;

// DFS helper pour trouver tous les chemins
static void FindAllPathsDFS(Graph *g, int current, int end_id,
                            int *current_path, int path_len, int current_cost,
                            bool *visited, AllPaths *result) {
  if (result->path_count >= MAX_PATHS)
    return;
  if (path_len >= MAX_PATH_LEN)
    return;

  current_path[path_len] = current;
  path_len++;

  if (current == end_id) {
    // Found a path - copy it
    for (int i = 0; i < path_len; i++) {
      result->paths[result->path_count][i] = current_path[i];
    }
    result->path_lengths[result->path_count] = path_len;
    result->path_costs[result->path_count] = current_cost;

    // Check if this is the shortest
    if (result->shortest_idx < 0 ||
        current_cost < result->path_costs[result->shortest_idx]) {
      result->shortest_idx = result->path_count;
    }
    result->path_count++;
    return;
  }

  visited[current] = true;

  // Get neighbors
  int neighbors[MAX_NODES], weights[MAX_NODES];
  int neighbor_count = GetNeighbors(g, current, neighbors, weights);

  for (int i = 0; i < neighbor_count; i++) {
    int next = neighbors[i];
    if (!visited[next]) {
      FindAllPathsDFS(g, next, end_id, current_path, path_len,
                      current_cost + weights[i], visited, result);
    }
  }

  visited[current] = false; // Backtrack
}

// Trouver tous les chemins entre deux noeuds
static AllPaths Graph_FindAllPaths(Graph *g, int start_id, int end_id) {
  AllPaths result = {0};
  result.shortest_idx = -1;

  if (g->node_count == 0)
    return result;

  bool visited[MAX_NODES] = {false};
  int current_path[MAX_PATH_LEN];

  FindAllPathsDFS(g, start_id, end_id, current_path, 0, 0, visited, &result);

  return result;
}

int *Graph_BellmanFord(Graph *g, int start_id, int end_id, int *path_length,
                       int *total_cost) {
  if (g->node_count == 0) {
    *path_length = 0;
    *total_cost = 0;
    return NULL;
  }

  int dist[MAX_NODES];
  int prev[MAX_NODES];

  // Init
  for (int i = 0; i < MAX_NODES; i++) {
    dist[i] = INT_MAX;
    prev[i] = -1;
  }
  dist[start_id] = 0;

  // Relaxation: |V| - 1 times
  for (int i = 0; i < g->node_count - 1; i++) {
    for (int j = 0; j < MAX_EDGES; j++) {
      if (g->edges[j].active) {
        int u = g->edges[j].from;
        int v = g->edges[j].to;
        int w = g->edges[j].weight;
        if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
          dist[v] = dist[u] + w;
          prev[v] = u;
        }
        // Bidirectional logic if undirected
        if (!g->directed) {
          int u2 = g->edges[j].to;
          int v2 = g->edges[j].from;
          if (dist[u2] != INT_MAX && dist[u2] + w < dist[v2]) {
            dist[v2] = dist[u2] + w;
            prev[v2] = u2;
          }
        }
      }
    }
  }

  // Check Negative Cycles
  for (int j = 0; j < MAX_EDGES; j++) {
    if (g->edges[j].active) {
      int u = g->edges[j].from;
      int v = g->edges[j].to;
      int w = g->edges[j].weight;
      if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
        // Negative cycle detected
        *path_length = 0;
        *total_cost = -99999; // Error code
        return NULL;
      }
      if (!g->directed) {
        int u2 = g->edges[j].to;
        int v2 = g->edges[j].from;
        if (dist[u2] != INT_MAX && dist[u2] + w < dist[v2]) {
          *path_length = 0;
          *total_cost = -99999;
          return NULL;
        }
      }
    }
  }

  // Reconstruct Path
  if (dist[end_id] == INT_MAX) {
    *path_length = 0;
    *total_cost = -1; // Unreachable
    return NULL;
  }

  // Path len
  int len = 0;
  int curr = end_id;
  while (curr != -1) {
    len++;
    curr = prev[curr];
    if (len > MAX_NODES)
      break; // Infinite loop safety
  }

  int *path = (int *)malloc(len * sizeof(int));
  curr = end_id;
  for (int i = len - 1; i >= 0; i--) {
    path[i] = curr;
    curr = prev[curr];
  }

  *path_length = len;
  *total_cost = dist[end_id];
  return path;
}

FloydResult Graph_FloydWarshall(Graph *g) {
  FloydResult res = {0};
  res.count = g->node_count;

  // Map real IDs to 0..count-1 indices for matrix
  int active_nodes[MAX_NODES];
  int idx = 0;
  for (int i = 0; i < MAX_NODES; i++) {
    if (g->nodes[i].active) {
      active_nodes[idx] = g->nodes[i].id;
      res.node_ids[idx] = g->nodes[i].id; // Store mapping
      idx++;
    }
  }
  res.count = idx;

  // Init Matrix
  for (int i = 0; i < res.count; i++) {
    for (int j = 0; j < res.count; j++) {
      if (i == j) {
        res.costs[i][j] = 0;
        res.next[i][j] = i;
      } else {
        res.costs[i][j] = 99999; // Infinity representation
        res.next[i][j] = -1;
      }
    }
  }

  // Fill Edges
  for (int e = 0; e < MAX_EDGES; e++) {
    if (g->edges[e].active) {
      int u = -1, v = -1;
      // Find matrix indices
      for (int i = 0; i < res.count; i++) {
        if (active_nodes[i] == g->edges[e].from)
          u = i;
        if (active_nodes[i] == g->edges[e].to)
          v = i;
      }

      if (u != -1 && v != -1) {
        res.costs[u][v] = g->edges[e].weight;
        res.next[u][v] = v;
        if (!g->directed) {
          res.costs[v][u] = g->edges[e].weight;
          res.next[v][u] = u;
        }
      }
    }
  }

  // Algo
  for (int k = 0; k < res.count; k++) {
    for (int i = 0; i < res.count; i++) {
      for (int j = 0; j < res.count; j++) {
        if (res.costs[i][k] != 99999 && res.costs[k][j] != 99999) {
          if (res.costs[i][k] + res.costs[k][j] < res.costs[i][j]) {
            res.costs[i][j] = res.costs[i][k] + res.costs[k][j];
            res.next[i][j] = res.next[i][k];
          }
        }
      }
    }
  }

  return res;
}

// ==================== VISUALISATION ====================

static GraphNode *FindNode(Graph *g, int id) {
  for (int i = 0; i < MAX_NODES; i++) {
    if (g->nodes[i].active && g->nodes[i].id == id) {
      return &g->nodes[i];
    }
  }
  return NULL;
}

// Find node by its label (returns node ID, or -1 if not found)
static int FindNodeByLabel(Graph *g, const char *label) {
  for (int i = 0; i < MAX_NODES; i++) {
    if (g->nodes[i].active && strcmp(g->nodes[i].label, label) == 0) {
      return g->nodes[i].id;
    }
  }
  return -1;
}

// Get label from node ID
static const char *GetNodeLabel(Graph *g, int id) {
  GraphNode *node = FindNode(g, id);
  return node ? node->label : "?";
}

static void DrawArrow(Vector2 from, Vector2 to, Color color) {
  // Raccourcir pour ne pas chevaucher les nœuds
  Vector2 dir = {to.x - from.x, to.y - from.y};
  float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
  if (len < 1)
    return;

  dir.x /= len;
  dir.y /= len;

  Vector2 start = {from.x + dir.x * NODE_RADIUS, from.y + dir.y * NODE_RADIUS};
  Vector2 end = {to.x - dir.x * NODE_RADIUS, to.y - dir.y * NODE_RADIUS};

  DrawLineEx(start, end, 2, color);

  // Pointe de flèche
  float angle = atan2f(dir.y, dir.x);
  float arrowSize = 10;

  Vector2 p1 = {end.x - arrowSize * cosf(angle - 0.4f),
                end.y - arrowSize * sinf(angle - 0.4f)};
  Vector2 p2 = {end.x - arrowSize * cosf(angle + 0.4f),
                end.y - arrowSize * sinf(angle + 0.4f)};

  DrawTriangle(end, p2, p1, color);
}

static void DrawEdge(Graph *g, GraphEdge *edge, bool highlighted) {
  GraphNode *from = FindNode(g, edge->from);
  GraphNode *to = FindNode(g, edge->to);
  if (!from || !to)
    return;

  Color color = highlighted ? COLOR_NEON_GREEN : COLOR_BORDER;

  Vector2 v_from = {from->x, from->y};
  Vector2 v_to = {to->x, to->y};

  if (g->directed) {
    DrawArrow(v_from, v_to, color);
  } else {
    // Ligne simple
    Vector2 dir = {v_to.x - v_from.x, v_to.y - v_from.y};
    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (len > 0) {
      dir.x /= len;
      dir.y /= len;
    }

    Vector2 start = {v_from.x + dir.x * NODE_RADIUS,
                     v_from.y + dir.y * NODE_RADIUS};
    Vector2 end = {v_to.x - dir.x * NODE_RADIUS, v_to.y - dir.y * NODE_RADIUS};

    DrawLineEx(start, end, 2, color);
  }

  // Poids
  // Poids
  // Toujours afficher le poids, même si c'est 1
  float mx = (from->x + to->x) / 2;
  float my = (from->y + to->y) / 2 - 12;

  char weightStr[16];
  snprintf(weightStr, sizeof(weightStr), "%d", edge->weight);
  int textW = MeasureText(weightStr, 12);

  DrawRectangle(mx - textW / 2 - 3, my - 2, textW + 6, 16, COLOR_PANEL);
  DrawText(weightStr, mx - textW / 2, my, 12, COLOR_TEXT_WHITE);
}

static void DrawGraphNode(GraphNode *node, bool selected, bool highlighted,
                          bool in_path) {
  Color bgColor = COLOR_PANEL;
  Color borderColor = COLOR_NEON_BLUE;

  if (in_path) {
    bgColor = (Color){63, 185, 80, 100};
    borderColor = COLOR_NEON_GREEN;
  } else if (highlighted) {
    bgColor = (Color){255, 107, 53, 100};
    borderColor = COLOR_NEON_ORANGE;
  } else if (selected) {
    borderColor = COLOR_NEON_PURPLE;
  }

  // Glow si surligné
  if (highlighted || in_path) {
    Color glowColor = borderColor;
    glowColor.a = 50;
    DrawCircle(node->x, node->y, NODE_RADIUS + 6, glowColor);
  }

  DrawCircle(node->x, node->y, NODE_RADIUS, bgColor);
  DrawCircleLines(node->x, node->y, NODE_RADIUS, borderColor);

  // Display node label (custom text)
  int textW = MeasureText(node->label, 16);
  DrawText(node->label, node->x - textW / 2, node->y - 8, 16, COLOR_TEXT_WHITE);
}

// ==================== ÉCRAN GRAPHES ====================

void GraphesInit(GraphesScreen *screen) {
  memset(screen, 0, sizeof(GraphesScreen));

  Graph_Init(&screen->graph, false);

  // Boutons navigation
  // screen->btn_back supprimé (remplacé par navbar)

  screen->btn_clear = CreateButton(WINDOW_WIDTH - 90, 50 + NAVBAR_HEIGHT, 70,
                                   28, "Vider", COLOR_NEON_RED);

  // Type (will be positioned in Draw)
  screen->btn_undirected =
      CreateButton(0, 0, 90, 28, "Non-Oriente", COLOR_NEON_BLUE);
  screen->btn_directed = CreateButton(0, 0, 70, 28, "Oriente", COLOR_NEON_BLUE);

  // Outils (will be positioned in Draw)
  screen->btn_tool_select =
      CreateButton(0, 0, 55, 28, "Select", COLOR_NEON_GREEN);
  screen->btn_tool_add_node =
      CreateButton(0, 0, 55, 28, "Noeud", COLOR_NEON_GREEN);
  screen->btn_tool_add_edge =
      CreateButton(0, 0, 50, 28, "Arete", COLOR_NEON_GREEN);
  screen->btn_tool_modify =
      CreateButton(0, 0, 55, 28, "Modif", COLOR_NEON_ORANGE);
  screen->btn_tool_delete = CreateButton(0, 0, 55, 28, "Suppr", COLOR_NEON_RED);

  // Algorithmes (will be positioned in Draw)
  screen->btn_bellman = CreateButton(0, 0, 60, 28, "Bellman", COLOR_NEON_BLUE);
  screen->btn_floyd = CreateButton(0, 0, 50, 28, "Floyd", COLOR_NEON_BLUE);
  screen->btn_dijkstra =
      CreateButton(0, 0, 65, 28, "Dijkstra", COLOR_NEON_BLUE);

  screen->current_tool = GRAPH_TOOL_SELECT;
  screen->selected_node = -1;
  screen->edge_from = -1;

  // Popup initialization
  screen->popup_weight_open = false;
  screen->popup_weight_text[0] = '1';
  screen->popup_weight_text[1] = '\0';
  screen->popup_weight_cursor = 1;
  screen->popup_weight_is_modify = false;

  screen->popup_algo_open = false;
  screen->popup_algo_step = 0;
  screen->popup_start_text[0] = '\0';
  screen->popup_end_text[0] = '\0';

  screen->popup_result_open = false;
  screen->popup_result_shortest_idx = -1;
}

AppScreen GraphesUpdate(GraphesScreen *screen) {
  float dt = GetFrameTime();

  // Navigation Bar
  AppScreen next_screen = SCREEN_GRAPHES;
  if (UpdateNavigationBar(&next_screen)) {
    if (screen->algo_result)
      free(screen->algo_result);
    if (screen->dijkstra_path)
      free(screen->dijkstra_path);
    return next_screen;
  }

  // Clear
  if (UpdateButton(&screen->btn_clear)) {
    Graph_Clear(&screen->graph);
    screen->selected_node = -1;
    if (screen->algo_result) {
      free(screen->algo_result);
      screen->algo_result = NULL;
    }
    if (screen->dijkstra_path) {
      free(screen->dijkstra_path);
      screen->dijkstra_path = NULL;
    }
  }

  // ===== POPUP RESULT - Display algorithm results =====
  if (screen->popup_result_open) {
    // Close on Enter only
    if (IsKeyPressed(KEY_ENTER)) {
      screen->popup_result_open = false;
    }
    return SCREEN_GRAPHES; // Block other interactions
  }

  // ===== POPUP NODE - Handle node label input =====
  if (screen->popup_node_open) {
    // Handle keyboard input for label
    int key = GetCharPressed();
    while (key > 0) {
      if (key >= 32 && key <= 126 && screen->popup_node_cursor < 30) {
        screen->popup_node_text[screen->popup_node_cursor++] = (char)key;
        screen->popup_node_text[screen->popup_node_cursor] = '\0';
      }
      key = GetCharPressed();
    }
    // Backspace
    if (IsKeyPressed(KEY_BACKSPACE) && screen->popup_node_cursor > 0) {
      screen->popup_node_cursor--;
      screen->popup_node_text[screen->popup_node_cursor] = '\0';
    }
    // Enter - confirm node creation or modification
    if (IsKeyPressed(KEY_ENTER) && strlen(screen->popup_node_text) > 0) {
      if (screen->popup_node_is_modify) {
        // Modify existing node label
        for (int i = 0; i < MAX_NODES; i++) {
          if (screen->graph.nodes[i].active &&
              screen->graph.nodes[i].id == screen->popup_node_target_id) {
            strncpy(screen->graph.nodes[i].label, screen->popup_node_text, 31);
            screen->graph.nodes[i].label[31] = '\0';
            break;
          }
        }
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Noeud modifie: '%s'", screen->popup_node_text);
      } else {
        // Add new node
        Graph_AddNode(&screen->graph, screen->popup_node_x,
                      screen->popup_node_y, screen->popup_node_text);
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Noeud '%s' ajoute", screen->popup_node_text);
      }
      screen->popup_node_open = false;
      screen->status_timer = 2.0f;
    }
    return SCREEN_GRAPHES; // Block other interactions while popup is open
  }

  // ===== POPUP WEIGHT - Handle edge weight input =====
  if (screen->popup_weight_open) {
    // Handle keyboard input for weight
    int key = GetCharPressed();
    while (key > 0) {
      if ((key >= '0' && key <= '9') ||
          (key == '-' && screen->popup_weight_cursor == 0)) {
        if (screen->popup_weight_cursor < 15) {
          screen->popup_weight_text[screen->popup_weight_cursor++] = (char)key;
          screen->popup_weight_text[screen->popup_weight_cursor] = '\0';
        }
      }
      key = GetCharPressed();
    }
    // Backspace
    if (IsKeyPressed(KEY_BACKSPACE) && screen->popup_weight_cursor > 0) {
      screen->popup_weight_cursor--;
      screen->popup_weight_text[screen->popup_weight_cursor] = '\0';
    }
    // Enter - confirm edge creation or modification
    if (IsKeyPressed(KEY_ENTER)) {
      int weight = atoi(screen->popup_weight_text);
      // Removed > 0 restriction for Bellman-Ford support
      // if (weight < 1) weight = 1;

      if (screen->popup_weight_is_modify) {
        // Update existing edge
        for (int i = 0; i < MAX_EDGES; i++) {
          if (screen->graph.edges[i].active) {
            bool match = false;
            if (screen->graph.directed) {
              match = (screen->graph.edges[i].from == screen->popup_edge_from &&
                       screen->graph.edges[i].to == screen->popup_edge_to);
            } else {
              match =
                  ((screen->graph.edges[i].from == screen->popup_edge_from &&
                    screen->graph.edges[i].to == screen->popup_edge_to) ||
                   (screen->graph.edges[i].from == screen->popup_edge_to &&
                    screen->graph.edges[i].to == screen->popup_edge_from));
            }

            if (match) {
              screen->graph.edges[i].weight = weight;
              break;
            }
          }
        }
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Poids modifie (poids %d)", weight);
      } else {
        // Add new edge
        Graph_AddEdge(&screen->graph, screen->popup_edge_from,
                      screen->popup_edge_to, weight);
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Arete ajoutee (poids %d)", weight);
      }
      screen->popup_weight_open = false;
      screen->status_timer = 2.0f;
    }
    return SCREEN_GRAPHES; // Block other interactions while popup is open
  }

  // ===== POPUP ALGO - Handle algorithm start/end input =====
  if (screen->popup_algo_open) {
    char *current_text = (screen->popup_algo_step == 0)
                             ? screen->popup_start_text
                             : screen->popup_end_text;
    int max_len = 7;

    // Handle keyboard input - accept any printable character
    int key = GetCharPressed();
    while (key > 0) {
      if (key >= 32 && key <= 126 && screen->popup_cursor < max_len) {
        current_text[screen->popup_cursor++] = (char)key;
        current_text[screen->popup_cursor] = '\0';
      }
      key = GetCharPressed();
    }
    // Backspace
    if (IsKeyPressed(KEY_BACKSPACE) && screen->popup_cursor > 0) {
      screen->popup_cursor--;
      current_text[screen->popup_cursor] = '\0';
    }
    // Enter - next step or confirm
    if (IsKeyPressed(KEY_ENTER) && strlen(current_text) > 0) {
      if (screen->popup_algo_step == 0) {
        // Check if start node exists
        int start_id =
            FindNodeByLabel(&screen->graph, screen->popup_start_text);
        if (start_id >= 0) {
          // Move to step 1 - ask for end node
          screen->popup_algo_step = 1;
          screen->popup_cursor = 0;
        } else {
          snprintf(screen->status_message, sizeof(screen->status_message),
                   "Noeud '%s' non trouve!", screen->popup_start_text);
          screen->status_timer = 2.0f;
          screen->popup_algo_open = false;
        }
      } else {
        // Find nodes by label
        int start_id =
            FindNodeByLabel(&screen->graph, screen->popup_start_text);
        int end_id = FindNodeByLabel(&screen->graph, screen->popup_end_text);

        if (start_id < 0 || end_id < 0) {
          snprintf(screen->status_message, sizeof(screen->status_message),
                   "Noeud non trouve!");
          screen->status_timer = 2.0f;
          screen->popup_algo_open = false;
        } else {
          // Run algorithm
          if (screen->dijkstra_path)
            free(screen->dijkstra_path);

          if (strcmp(screen->popup_algo_type, "BELLMAN") == 0) {
            screen->dijkstra_path = Graph_BellmanFord(
                &screen->graph, start_id, end_id, &screen->dijkstra_path_length,
                &screen->dijkstra_cost);
          } else if (strcmp(screen->popup_algo_type, "DIJKSTRA") == 0) {
            screen->dijkstra_path = Graph_Dijkstra(
                &screen->graph, start_id, end_id, &screen->dijkstra_path_length,
                &screen->dijkstra_cost);
          } else if (strcmp(screen->popup_algo_type, "FLOYD") == 0) {
            // Floyd doesn't use path dispatch here, it is handled separately
            // for Matrix view. But if we wanted path between A and B from
            // Floyd: For now let's just show matrix for Floyd regardless of
            // "End Node" input? Actually, Floyd button should probably NOT
            // verify End Node if we want Matrix. But the UI asks for End Node.
            // Let's special case Floyd: If "FLOYD", we ignore verifying end
            // node? Wait, user clicked Floyd -> Popup asks Start -> Popup asks
            // End. If we want Matrix, we should show it immediately after
            // Start/End or just show full matrix. Usually full matrix. Let's
            // modify FLOYD button action to NOT ask for start/end, OR handle
            // matrix display here. If user entered Start/End, we could show
            // path extracted from Floyd matrix. But request said "Matrice non
            // un chemin". So I should override the "ask for start/end" logic
            // for Floyd? Let's handle it here: show matrix.
            // Floyd: Handled in Draw
            // FloydResult res = Graph_FloydWarshall(&screen->graph);

            snprintf(screen->popup_result_title,
                     sizeof(screen->popup_result_title),
                     "Matrice des Distances (Floyd-Warshall)");
            screen->popup_result_text[0] = '\0';
            // We will handle rendering in GraphesDraw based on title being
            // "Matrice..."
            screen->popup_result_open = true;
            screen->popup_algo_open = false;
            // DO NOT continue to standard result logic
            return SCREEN_GRAPHES;
          }

          // Build result popup with labels (Standard for Dijkstra/Bellman)
          snprintf(screen->popup_result_title,
                   sizeof(screen->popup_result_title), "%s: %s -> %s",
                   screen->popup_algo_type, screen->popup_start_text,
                   screen->popup_end_text);

          AllPaths allPaths = {0};
          if (strcmp(screen->popup_algo_type, "DIJKSTRA") == 0 ||
              strcmp(screen->popup_algo_type, "BELLMAN") == 0) {
            allPaths = Graph_FindAllPaths(&screen->graph, start_id, end_id);
          }

          if (screen->dijkstra_path) {
            // Success
            snprintf(screen->popup_result_text,
                     sizeof(screen->popup_result_text),
                     "Chemin trouve!\nCout total: %d", screen->dijkstra_cost);
          } else {
            // Fail
            // Fail
            if (screen->dijkstra_cost == -99999)
              strcpy(screen->popup_result_text, "Cycle negatif detecte!");
            else if (screen->dijkstra_cost == -99998)
              strcpy(
                  screen->popup_result_text,
                  "Erreur: Poids negatifs interdits pour Dijkstra.\nUtilisez "
                  "Bellman-Ford.");
            else
              strcpy(screen->popup_result_text, "Aucun chemin trouve.");
          }

          if (allPaths.path_count > 0) {
            screen->popup_result_text[0] = '\0';
            screen->popup_result_shortest_idx = allPaths.shortest_idx;

            char header[64];
            snprintf(header, sizeof(header), "TOUS LES CHEMINS (%d trouves):\n",
                     allPaths.path_count);
            strcat(screen->popup_result_text, header);

            // Display each path with node labels
            for (int p = 0; p < allPaths.path_count && p < 10; p++) {
              char pathLine[256];
              int pos = 0;

              // Show path nodes using labels
              for (int i = 0; i < allPaths.path_lengths[p]; i++) {
                const char *label =
                    GetNodeLabel(&screen->graph, allPaths.paths[p][i]);
                if (i == allPaths.path_lengths[p] - 1) {
                  pos += snprintf(pathLine + pos, sizeof(pathLine) - pos, "%s",
                                  label);
                } else {
                  pos += snprintf(pathLine + pos, sizeof(pathLine) - pos,
                                  "%s->", label);
                }
              }
              pos += snprintf(pathLine + pos, sizeof(pathLine) - pos,
                              " (cout: %d)\n", allPaths.path_costs[p]);

              strcat(screen->popup_result_text, pathLine);
            }

            if (allPaths.path_count > 10) {
              strcat(screen->popup_result_text, "... et plus\n");
            }
          }

          screen->popup_result_open = true;
          screen->popup_algo_open = false;
        }
      }
    }
    return SCREEN_GRAPHES; // Block other interactions while popup is open
  }
  if (UpdateButton(&screen->btn_undirected)) {
    screen->graph.directed = false;
  }
  if (UpdateButton(&screen->btn_directed)) {
    screen->graph.directed = true;
  }

  // Outils
  if (UpdateButton(&screen->btn_tool_select))
    screen->current_tool = GRAPH_TOOL_SELECT;
  if (UpdateButton(&screen->btn_tool_add_node)) {
    // Open popup directly to add node
    Rectangle graphRect = {20, 110 + NAVBAR_HEIGHT, WINDOW_WIDTH - 40,
                           WINDOW_HEIGHT - 140 - NAVBAR_HEIGHT};
    // Offset position based on node count so nodes don't stack
    int nodeCount = screen->graph.node_count;
    int col = nodeCount % 5; // 5 nodes per row
    int row = nodeCount / 5;
    float offsetX = (col - 2) * 100; // -200 to +200
    float offsetY = (row - 2) * 80;  // Vertical offset
    screen->popup_node_x = graphRect.x + graphRect.width / 2 + offsetX;
    screen->popup_node_y = graphRect.y + graphRect.height / 2 + offsetY;
    // Keep within bounds
    if (screen->popup_node_x < graphRect.x + 50)
      screen->popup_node_x = graphRect.x + 50;
    if (screen->popup_node_x > graphRect.x + graphRect.width - 50)
      screen->popup_node_x = graphRect.x + graphRect.width - 50;
    if (screen->popup_node_y < graphRect.y + 50)
      screen->popup_node_y = graphRect.y + 50;
    if (screen->popup_node_y > graphRect.y + graphRect.height - 50)
      screen->popup_node_y = graphRect.y + graphRect.height - 50;
    screen->popup_node_text[0] = '\0';
    screen->popup_node_cursor = 0;
    screen->popup_node_open = true;
    screen->popup_node_is_modify = false; // Add mode
  }
  if (UpdateButton(&screen->btn_tool_add_edge)) {
    screen->current_tool = GRAPH_TOOL_ADD_EDGE;
    screen->edge_from = -1;
  }
  if (UpdateButton(&screen->btn_tool_modify))
    screen->current_tool = GRAPH_TOOL_MODIFY;
  if (UpdateButton(&screen->btn_tool_delete))
    screen->current_tool = GRAPH_TOOL_DELETE;

  // Algorithmes - open popup to ask for start/end
  if (UpdateButton(&screen->btn_bellman) && screen->graph.node_count > 0 &&
      !screen->popup_algo_open) {
    strcpy(screen->popup_algo_type, "BELLMAN");
    screen->popup_algo_step = 0; // Ask for start
    screen->popup_start_text[0] = '\0';
    screen->popup_end_text[0] = '\0';
    screen->popup_cursor = 0;
    screen->popup_algo_open = true;
  }

  if (UpdateButton(&screen->btn_floyd) && screen->graph.node_count > 0 &&
      !screen->popup_algo_open) {
    // For Floyd, we do NOT ask for start/end, we just show matrix immediately.
    // Trigger "FLOYD" which will be caught in standard update logic?
    // No, standard logic relies on popup confirmation.
    // Let's just set the special "Matrix" mode directly here!
    snprintf(screen->popup_result_title, sizeof(screen->popup_result_title),
             "Matrice des Distances (Floyd-Warshall)");
    screen->popup_result_text[0] = '\0';
    screen->popup_result_open = true; // Open the result popup immediately
  }

  if (UpdateButton(&screen->btn_dijkstra) && screen->graph.node_count > 0 &&
      !screen->popup_algo_open) {
    if (Graph_HasNegativeWeights(&screen->graph)) {
      snprintf(screen->status_message, sizeof(screen->status_message),
               "Dijkstra: Poids negatifs interdits. Utilisez Bellman-Ford.");
      screen->status_timer = 3.0f;
    } else {
      strcpy(screen->popup_algo_type, "DIJKSTRA");
      screen->popup_algo_step = 0;
      screen->popup_start_text[0] = '\0';
      screen->popup_end_text[0] = '\0';
      screen->popup_cursor = 0;
      screen->popup_algo_open = true;
    }
  }

  // Animation algo
  if (screen->algo_animating && screen->algo_result) {
    screen->algo_timer += dt;
    if (screen->algo_timer >= 0.5f) {
      screen->algo_timer = 0;
      screen->algo_current_idx++;
      if (screen->algo_current_idx >= screen->algo_result_size) {
        screen->algo_animating = false;
      }
    }
  }

  // Zone graphe
  Rectangle graphRect = {20, 110 + NAVBAR_HEIGHT, WINDOW_WIDTH - 40,
                         WINDOW_HEIGHT - 140 - NAVBAR_HEIGHT};
  Vector2 mouse = GetMousePosition();

  if (CheckCollisionPointRec(mouse, graphRect)) {
    // Clic gauche
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (screen->current_tool == GRAPH_TOOL_SELECT ||
          screen->current_tool == GRAPH_TOOL_DELETE ||
          screen->current_tool == GRAPH_TOOL_MODIFY) {
        // Trouver le nœud cliqué
        screen->selected_node = -1;
        bool clickedOnNode = false;
        for (int i = 0; i < MAX_NODES; i++) {
          if (screen->graph.nodes[i].active) {
            float dx = mouse.x - screen->graph.nodes[i].x;
            float dy = mouse.y - screen->graph.nodes[i].y;
            if (dx * dx + dy * dy < NODE_RADIUS * NODE_RADIUS) {
              screen->selected_node = screen->graph.nodes[i].id;
              clickedOnNode = true;

              if (screen->current_tool == GRAPH_TOOL_DELETE) {
                Graph_RemoveNode(&screen->graph, screen->selected_node);
                screen->selected_node = -1;
              } else if (screen->current_tool == GRAPH_TOOL_MODIFY) {
                // Modify Node Label
                screen->popup_node_is_modify = true;
                screen->popup_node_target_id = screen->selected_node;
                strncpy(screen->popup_node_text, screen->graph.nodes[i].label,
                        31);
                screen->popup_node_text[31] = '\0';
                screen->popup_node_cursor = strlen(screen->popup_node_text);
                screen->popup_node_open = true;
                screen->popup_node_x = WINDOW_WIDTH / 2 - 175;
                screen->popup_node_y = WINDOW_HEIGHT / 2 - 60;
              } else {
                screen->dragging_node = true;
              }
              break;
            }
          }
        }

        // Check Edges if no node clicked
        if (!clickedOnNode && (screen->current_tool == GRAPH_TOOL_MODIFY ||
                               screen->current_tool == GRAPH_TOOL_DELETE)) {
          for (int i = 0; i < MAX_EDGES; i++) {
            if (screen->graph.edges[i].active) {
              GraphNode *n1 =
                  FindNode(&screen->graph, screen->graph.edges[i].from);
              GraphNode *n2 =
                  FindNode(&screen->graph, screen->graph.edges[i].to);
              if (n1 && n2) {
                Vector2 p1 = {n1->x, n1->y};
                Vector2 p2 = {n2->x, n2->y};
                if (CheckCollisionPointLine(mouse, p1, p2, 5)) {
                  if (screen->current_tool == GRAPH_TOOL_DELETE) {
                    screen->graph.edges[i].active = false;
                    screen->graph.edge_count--;
                  } else {
                    // Modify Edge Weight
                    screen->popup_edge_from = screen->graph.edges[i].from;
                    screen->popup_edge_to = screen->graph.edges[i].to;
                    screen->popup_weight_is_modify = true;
                    snprintf(screen->popup_weight_text, 16, "%d",
                             screen->graph.edges[i].weight);
                    screen->popup_weight_cursor =
                        strlen(screen->popup_weight_text);
                    screen->popup_weight_open = true;
                  }
                  break;
                }
              }
            }
          }
        }
      } else if (screen->current_tool == GRAPH_TOOL_ADD_EDGE) {
        // Sélectionner nœuds pour arête
        for (int i = 0; i < MAX_NODES; i++) {
          if (screen->graph.nodes[i].active) {
            float dx = mouse.x - screen->graph.nodes[i].x;
            float dy = mouse.y - screen->graph.nodes[i].y;
            if (dx * dx + dy * dy < NODE_RADIUS * NODE_RADIUS) {
              if (screen->edge_from < 0) {
                screen->edge_from = screen->graph.nodes[i].id;
                snprintf(screen->status_message, sizeof(screen->status_message),
                         "Selectionnez le noeud destination");
                screen->status_timer = 2.0f;
              } else {
                // Open popup to ask for weight
                screen->popup_edge_from = screen->edge_from;
                screen->popup_edge_to = screen->graph.nodes[i].id;
                screen->popup_weight_text[0] = '\0'; // Start empty
                screen->popup_weight_cursor = 0;
                screen->popup_weight_open = true;
                screen->popup_weight_is_modify = false;
                screen->edge_from = -1;
              }
              break;
            }
          }
        }
      }
    }

    // Relâcher clic
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      screen->dragging_node = false;
    }

    // Déplacer nœud
    if (screen->dragging_node && screen->selected_node >= 0) {
      GraphNode *node = FindNode(&screen->graph, screen->selected_node);
      if (node) {
        node->x = mouse.x;
        node->y = mouse.y;
      }
    }
  }

  // Timer status
  if (screen->status_timer > 0) {
    screen->status_timer -= dt;
  }

  return SCREEN_GRAPHES;
}

void GraphesDraw(GraphesScreen *screen) {
  // Background
  DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_BG_DARK,
                         COLOR_BG_LIGHT);

  // Dynamic title
  const char *title =
      screen->graph.directed ? "GRAPHE ORIENTE" : "GRAPHE NON-ORIENTE";
  Color titleColor =
      screen->graph.directed ? COLOR_NEON_PURPLE : COLOR_NEON_BLUE;
  // Navigation buttons
  DrawNavigationBar(SCREEN_GRAPHES);

  // Draw dynamic title after navbar (at position 610 for 2cm gap after Graphes
  // button)
  int titleX = 670;
  int titleY = (NAVBAR_HEIGHT - 18) / 2;
  DrawText(title, titleX, titleY, 18, titleColor);

  // ============= CADRES COLORES AVEC HOVER (same as Arbres) =============
  Vector2 mousePos = GetMousePosition();
  Color modeColor =
      screen->graph.directed ? COLOR_NEON_PURPLE : COLOR_NEON_BLUE;
  Color modeHover = screen->graph.directed ? (Color){180, 130, 255, 30}
                                           : (Color){80, 180, 255, 30};

  // Constants for spacing (EXACTLY same as Arbres)
  const int FRAME_Y = 40 + NAVBAR_HEIGHT;
  const int FRAME_H = 50;
  const int BTN_Y = 52 + NAVBAR_HEIGHT;
  const int BTN_GAP = 15;
  const int FRAME_GAP = 60;

  // ========== Frame TYPE: [Non-Oriente] [Oriente] ==========
  int typeX = 15;
  int typeW = 210;
  Rectangle frameType = {typeX, FRAME_Y, typeW, FRAME_H};
  bool hoverType = CheckCollisionPointRec(mousePos, frameType);
  if (hoverType)
    DrawRectangleRounded(frameType, 0.1f, 8, modeHover);
  DrawRectRoundedLinesThick(frameType, 0.1f, 8, 2, modeColor);
  int labelW = MeasureText("TYPE", 10);
  DrawRectangle(typeX + 15, FRAME_Y - 2, labelW + 10, 5, COLOR_BG_DARK);
  DrawText("TYPE", typeX + 20, FRAME_Y - 5, 10, modeColor);

  // Position buttons inside TYPE frame
  int x = typeX + 15;
  screen->btn_undirected.bounds = (Rectangle){x, BTN_Y, 90, 28};
  DrawButton(&screen->btn_undirected);
  x += 90 + BTN_GAP;
  screen->btn_directed.bounds = (Rectangle){x, BTN_Y, 75, 28};
  DrawButton(&screen->btn_directed);

  // Active mode indicator
  Rectangle activeRect = screen->graph.directed ? screen->btn_directed.bounds
                                                : screen->btn_undirected.bounds;
  DrawRectRoundedLinesThick(activeRect, 0.3f, 8, 3, COLOR_NEON_GREEN);

  // ========== Frame OUTILS: [Select] [Noeud] [Arete] [Modif] [Suppr]
  // ==========
  int outilsX = typeX + typeW + FRAME_GAP;
  int outilsW = 410; // Widened for Modify button
  Rectangle frameOutils = {outilsX, FRAME_Y, outilsW, FRAME_H};
  bool hoverOutils = CheckCollisionPointRec(mousePos, frameOutils);
  if (hoverOutils)
    DrawRectangleRounded(frameOutils, 0.1f, 8, (Color){255, 150, 80, 30});
  DrawRectRoundedLinesThick(frameOutils, 0.1f, 8, 2, COLOR_NEON_ORANGE);
  labelW = MeasureText("OUTILS", 10);
  DrawRectangle(outilsX + 15, FRAME_Y - 2, labelW + 10, 5, COLOR_BG_DARK);
  DrawText("OUTILS", outilsX + 20, FRAME_Y - 5, 10, COLOR_NEON_ORANGE);

  // Position tool buttons inside frame
  x = outilsX + 15;
  screen->btn_tool_select.bounds = (Rectangle){x, BTN_Y, 55, 28};
  DrawButton(&screen->btn_tool_select);
  x += 55 + BTN_GAP;
  screen->btn_tool_add_node.bounds = (Rectangle){x, BTN_Y, 55, 28};
  DrawButton(&screen->btn_tool_add_node);
  x += 55 + BTN_GAP;
  screen->btn_tool_add_edge.bounds = (Rectangle){x, BTN_Y, 50, 28};
  DrawButton(&screen->btn_tool_add_edge);
  x += 50 + BTN_GAP;
  screen->btn_tool_modify.bounds = (Rectangle){x, BTN_Y, 55, 28};
  DrawButton(&screen->btn_tool_modify);
  x += 55 + BTN_GAP;
  screen->btn_tool_delete.bounds = (Rectangle){x, BTN_Y, 55, 28};
  DrawButton(&screen->btn_tool_delete);

  // Active tool indicator
  Button *tool_btns[] = {&screen->btn_tool_select, &screen->btn_tool_add_node,
                         &screen->btn_tool_add_edge, &screen->btn_tool_delete,
                         &screen->btn_tool_modify};
  int idx = -1;
  if (screen->current_tool == GRAPH_TOOL_SELECT)
    idx = 0;
  else if (screen->current_tool == GRAPH_TOOL_ADD_NODE)
    idx = 1;
  else if (screen->current_tool == GRAPH_TOOL_ADD_EDGE)
    idx = 2;
  else if (screen->current_tool == GRAPH_TOOL_DELETE)
    idx = 3;
  else if (screen->current_tool == GRAPH_TOOL_MODIFY)
    idx = 4;

  if (idx >= 0) {
    DrawRectRoundedLinesThick(tool_btns[idx]->bounds, 0.3f, 8, 3,
                              COLOR_NEON_GREEN);
  }

  // ========== Frame ALGORITHMES: [BFS] [DFS] [Dijkstra] ==========
  int algoX = outilsX + outilsW + FRAME_GAP;
  int algoW = 220;
  Rectangle frameAlgo = {algoX, FRAME_Y, algoW, FRAME_H};
  bool hoverAlgo = CheckCollisionPointRec(mousePos, frameAlgo);
  if (hoverAlgo)
    DrawRectangleRounded(frameAlgo, 0.1f, 8, (Color){100, 255, 150, 30});
  DrawRectRoundedLinesThick(frameAlgo, 0.1f, 8, 2, COLOR_NEON_GREEN);
  labelW = MeasureText("ALGORITHMES", 10);
  DrawRectangle(algoX + 15, FRAME_Y - 2, labelW + 10, 5, COLOR_BG_DARK);
  DrawText("ALGORITHMES", algoX + 20, FRAME_Y - 5, 10, COLOR_NEON_GREEN);

  // Position algo buttons inside frame (no inputs anymore - using popups)
  x = algoX + 10;
  screen->btn_bellman.bounds = (Rectangle){x, BTN_Y, 60, 28};
  DrawButton(&screen->btn_bellman);
  x += 60 + BTN_GAP;
  screen->btn_floyd.bounds = (Rectangle){x, BTN_Y, 50, 28};
  DrawButton(&screen->btn_floyd);
  x += 50 + BTN_GAP;
  screen->btn_dijkstra.bounds = (Rectangle){x, BTN_Y, 65, 28};

  if (Graph_HasNegativeWeights(&screen->graph)) {
    Color disabledColor = COLOR_TEXT_DIM; // Grey
    DrawButtonEx(&screen->btn_dijkstra, disabledColor, false);
    // We still draw it, but with disabled look.
    // UpdateButton still works, but we intercepted it in Update to show
    // message.
  } else {
    DrawButton(&screen->btn_dijkstra);
  }

  // ========== Vider button (top right) ==========
  DrawButton(&screen->btn_clear);
  DrawButton(&screen->btn_clear);

  // Zone graphe with mode-colored border
  Rectangle graphRect = {10, 100 + NAVBAR_HEIGHT, WINDOW_WIDTH - 20,
                         WINDOW_HEIGHT - 130 - NAVBAR_HEIGHT};
  DrawRectangleRec(graphRect, COLOR_BG_DARK);
  DrawRectangleLinesEx(graphRect, 2, modeColor);

  // Dessiner les arêtes
  for (int i = 0; i < MAX_EDGES; i++) {
    if (screen->graph.edges[i].active) {
      bool highlighted = false;

      // Vérifier si dans le chemin Dijkstra
      if (screen->dijkstra_path && screen->dijkstra_path_length > 1) {
        for (int j = 0; j < screen->dijkstra_path_length - 1; j++) {
          if ((screen->dijkstra_path[j] == screen->graph.edges[i].from &&
               screen->dijkstra_path[j + 1] == screen->graph.edges[i].to) ||
              (!screen->graph.directed &&
               screen->dijkstra_path[j] == screen->graph.edges[i].to &&
               screen->dijkstra_path[j + 1] == screen->graph.edges[i].from)) {
            highlighted = true;
            break;
          }
        }
      }

      DrawEdge(&screen->graph, &screen->graph.edges[i], highlighted);
    }
  }

  // Dessiner les nœuds
  for (int i = 0; i < MAX_NODES; i++) {
    if (screen->graph.nodes[i].active) {
      bool selected = (screen->graph.nodes[i].id == screen->selected_node);
      bool highlighted = false;
      bool in_path = false;

      // Animation algo
      if (screen->algo_animating && screen->algo_result &&
          screen->algo_current_idx < screen->algo_result_size) {
        if (screen->graph.nodes[i].id ==
            screen->algo_result[screen->algo_current_idx]) {
          highlighted = true;
        }
      }

      // Chemin Dijkstra
      if (screen->dijkstra_path) {
        for (int j = 0; j < screen->dijkstra_path_length; j++) {
          if (screen->dijkstra_path[j] == screen->graph.nodes[i].id) {
            in_path = true;
            break;
          }
        }
      }

      DrawGraphNode(&screen->graph.nodes[i], selected, highlighted, in_path);
    }
  }

  // Ligne temporaire pour création d'arête
  if (screen->current_tool == GRAPH_TOOL_ADD_EDGE && screen->edge_from >= 0) {
    GraphNode *from = FindNode(&screen->graph, screen->edge_from);
    if (from) {
      Vector2 mouse = GetMousePosition();
      DrawLineEx((Vector2){from->x, from->y}, mouse, 2, COLOR_TEXT_DIM);
    }
  }

  // Infos
  char infoStr[128];
  snprintf(infoStr, sizeof(infoStr), "Noeuds: %d | Aretes: %d",
           screen->graph.node_count, screen->graph.edge_count);
  DrawText(infoStr, 30, WINDOW_HEIGHT - 25, 14, COLOR_TEXT_MAIN);

  // Résultat algo
  if (screen->algo_result && screen->algo_result_size > 0) {
    char algoStr[256];
    snprintf(algoStr, sizeof(algoStr), "%s: ", screen->algo_name);
    for (int i = 0; i < screen->algo_result_size && i < 15; i++) {
      char num[16];
      if (i == screen->algo_current_idx && screen->algo_animating) {
        snprintf(num, sizeof(num), "[%d] ", screen->algo_result[i]);
      } else {
        snprintf(num, sizeof(num), "%d ", screen->algo_result[i]);
      }
      strcat(algoStr, num);
    }
    DrawText(algoStr, 250, WINDOW_HEIGHT - 25, 12, COLOR_NEON_BLUE);
  }

  // Status
  if (screen->status_timer > 0) {
    DrawText(screen->status_message, WINDOW_WIDTH - 350, WINDOW_HEIGHT - 25, 14,
             COLOR_NEON_GREEN);
  }

  // ===== POPUP NODE DRAWING =====
  if (screen->popup_node_open) {
    // Dim background
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 150});

    // Popup box
    int popW = 350, popH = 120;
    int popX = WINDOW_WIDTH / 2 - popW / 2;
    int popY = WINDOW_HEIGHT / 2 - popH / 2;

    DrawRectangleRounded((Rectangle){popX, popY, popW, popH}, 0.1f, 8,
                         COLOR_PANEL);
    DrawRectRoundedLinesThick((Rectangle){popX, popY, popW, popH}, 0.1f, 8, 2,
                              COLOR_NEON_GREEN);

    // Title
    const char *title =
        screen->popup_node_is_modify ? "Modifier le noeud" : "Nom du noeud";
    int tw = MeasureText(title, 16);
    DrawText(title, popX + popW / 2 - tw / 2, popY + 15, 16, COLOR_NEON_GREEN);

    // Input box
    Rectangle inputRect = {popX + 50, popY + 50, popW - 100, 30};
    DrawRectangleRec(inputRect, COLOR_BG_DARK);
    DrawRectangleLinesEx(inputRect, 2, COLOR_NEON_BLUE);
    DrawText(screen->popup_node_text, inputRect.x + 10, inputRect.y + 7, 16,
             COLOR_TEXT_WHITE);
    // Blinking cursor
    int cursorX = inputRect.x + 10 + MeasureText(screen->popup_node_text, 16);
    if (((int)(GetTime() * 2)) % 2 == 0) {
      DrawRectangle(cursorX + 2, inputRect.y + 6, 2, 18, COLOR_NEON_BLUE);
    }

    // Instructions
    DrawText("Entrez un nom (ex: Paris, 42, A), puis Entree", popX + 30,
             popY + 95, 10, COLOR_TEXT_DIM);
  }

  // ===== POPUP WEIGHT DRAWING =====
  if (screen->popup_weight_open) {
    // Dim background
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 150});

    // Popup box
    int popW = 300, popH = 120;
    int popX = WINDOW_WIDTH / 2 - popW / 2;
    int popY = WINDOW_HEIGHT / 2 - popH / 2;

    DrawRectangleRounded((Rectangle){popX, popY, popW, popH}, 0.1f, 8,
                         COLOR_PANEL);
    DrawRectRoundedLinesThick((Rectangle){popX, popY, popW, popH}, 0.1f, 8, 2,
                              COLOR_NEON_ORANGE);

    // Title
    char title[64];
    const char *fromLabel =
        GetNodeLabel(&screen->graph, screen->popup_edge_from);
    const char *toLabel = GetNodeLabel(&screen->graph, screen->popup_edge_to);
    snprintf(title, sizeof(title), "Poids de l'arete %s -> %s", fromLabel,
             toLabel);
    int tw = MeasureText(title, 16);
    DrawText(title, popX + popW / 2 - tw / 2, popY + 15, 16, COLOR_NEON_ORANGE);

    // Input box
    Rectangle inputRect = {popX + 50, popY + 50, popW - 100, 30};
    DrawRectangleRec(inputRect, COLOR_BG_DARK);
    DrawRectangleLinesEx(inputRect, 2, COLOR_NEON_BLUE);
    DrawText(screen->popup_weight_text, inputRect.x + 10, inputRect.y + 7, 16,
             COLOR_TEXT_WHITE);
    // Blinking cursor
    int cursorX = inputRect.x + 10 + MeasureText(screen->popup_weight_text, 16);
    if (((int)(GetTime() * 2)) % 2 == 0) {
      DrawRectangle(cursorX + 2, inputRect.y + 6, 2, 18, COLOR_NEON_BLUE);
    }

    // Instructions
    DrawText("Entrez le poids, puis Entree", popX + 15, popY + 95, 10,
             COLOR_TEXT_DIM);
  }

  // ===== POPUP ALGO DRAWING =====
  if (screen->popup_algo_open) {
    // Dim background
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 150});

    // Popup box
    int popW = 350, popH = 120;
    int popX = WINDOW_WIDTH / 2 - popW / 2;
    int popY = WINDOW_HEIGHT / 2 - popH / 2;

    DrawRectangleRounded((Rectangle){popX, popY, popW, popH}, 0.1f, 8,
                         COLOR_PANEL);
    DrawRectRoundedLinesThick((Rectangle){popX, popY, popW, popH}, 0.1f, 8, 2,
                              COLOR_NEON_GREEN);

    // Title
    char title[64];
    if (screen->popup_algo_step == 0) {
      snprintf(title, sizeof(title), "%s - Noeud de depart",
               screen->popup_algo_type);
    } else {
      snprintf(title, sizeof(title), "%s - Noeud d'arrivee",
               screen->popup_algo_type);
    }
    int tw = MeasureText(title, 16);
    DrawText(title, popX + popW / 2 - tw / 2, popY + 15, 16, COLOR_NEON_GREEN);

    // Input box
    Rectangle inputRect = {popX + 100, popY + 50, popW - 200, 30};
    DrawRectangleRec(inputRect, COLOR_BG_DARK);
    DrawRectangleLinesEx(inputRect, 2, COLOR_NEON_BLUE);
    char *text = (screen->popup_algo_step == 0) ? screen->popup_start_text
                                                : screen->popup_end_text;
    DrawText(text, inputRect.x + 10, inputRect.y + 7, 16, COLOR_TEXT_WHITE);

    // Instructions
    DrawText("Entrez le nom du noeud, puis Entree", popX + 20, popY + 95, 10,
             COLOR_TEXT_DIM);
  }

  // ===== POPUP RESULT DRAWING =====
  // ===== POPUP RESULT DRAWING =====
  if (screen->popup_result_open) {
    // Dim background
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 180});

    // Determine size based on content
    bool isMatrix = (strstr(screen->popup_result_title, "Matrice") != NULL);
    int popW = isMatrix ? 900 : 450;
    int popH = isMatrix ? 600 : 250;
    int popX = WINDOW_WIDTH / 2 - popW / 2;
    int popY = WINDOW_HEIGHT / 2 - popH / 2;

    DrawRectangleRounded((Rectangle){popX, popY, popW, popH}, 0.1f, 8,
                         COLOR_PANEL);
    DrawRectRoundedLinesThick((Rectangle){popX, popY, popW, popH}, 0.1f, 8, 3,
                              COLOR_NEON_GREEN);

    // Title
    int tw = MeasureText(screen->popup_result_title, 18);
    DrawText(screen->popup_result_title, popX + popW / 2 - tw / 2, popY + 15,
             18, COLOR_NEON_GREEN);

    if (isMatrix) {
      // Special render for Floyd Matrix
      FloydResult res = Graph_FloydWarshall(&screen->graph);

      int rows = res.count;
      int cols = res.count;
      int startX = popX + 60;
      int startY = popY + 60;
      int cellW = 50;
      int cellH = 30;

      // Draw Headers (Top and Left)
      for (int i = 0; i < rows; i++) {
        const char *label = GetNodeLabel(&screen->graph, res.node_ids[i]);
        // Top Header
        DrawText(label, startX + i * cellW + 15, startY - 20, 14,
                 COLOR_NEON_BLUE);
        // Left Header
        DrawText(label, startX - 40, startY + i * cellH + 5, 14,
                 COLOR_NEON_BLUE);
      }

      // Draw Matrix Grid
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
          int val = res.costs[i][j];
          char valStr[16];
          if (val > 10000)
            strcpy(valStr, "Inf");
          else
            snprintf(valStr, sizeof(valStr), "%d", val);

          Color c = COLOR_TEXT_WHITE;
          if (i == j)
            c = COLOR_TEXT_DIM;
          if (val > 10000)
            c = COLOR_NEON_RED;

          DrawText(valStr, startX + j * cellW + 10, startY + i * cellH + 5, 14,
                   c);
        }
      }
    } else {
      // Standard Text Render
      int lineY = popY + 45;
      int lineNum = 0;
      char textCopy[512];
      strncpy(textCopy, screen->popup_result_text, sizeof(textCopy) - 1);
      textCopy[sizeof(textCopy) - 1] = '\0';

      char *line = strtok(textCopy, "\n");
      while (line != NULL && lineY < popY + popH - 35) {
        // Line 0 is header, lines 1+ are paths. Shortest is at index
        // (shortest_idx + 1)
        Color lineColor = (lineNum == screen->popup_result_shortest_idx + 1 &&
                           screen->popup_result_shortest_idx >= 0)
                              ? COLOR_NEON_GREEN
                              : COLOR_TEXT_WHITE;
        DrawText(line, popX + 20, lineY, 12, lineColor);
        lineY += 15;
        lineNum++;
        line = strtok(NULL, "\n");
      }
    }

    // Instructions
    DrawText("Appuyez sur Entree pour fermer", popX + popW / 2 - 100,
             popY + popH - 25, 11, COLOR_TEXT_DIM);
  }
}

void GraphesUnload(GraphesScreen *screen) {
  if (screen->algo_result) {
    free(screen->algo_result);
    screen->algo_result = NULL;
  }
  if (screen->dijkstra_path) {
    free(screen->dijkstra_path);
    screen->dijkstra_path = NULL;
  }
}
