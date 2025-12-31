#include "arbres.h"
// Fix N-ary generation
#include "../ui_common.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Noms des parcours
const char *PARCOURS_NOMS[4] = {"Pre-ordre", "In-ordre", "Post-ordre",
                                "Largeur"};

// ==================== HELPERS ====================

NoeudGenerique *CreerNoeudGen(const char *data) {
  NoeudGenerique *node = (NoeudGenerique *)malloc(sizeof(NoeudGenerique));
  memset(node, 0, sizeof(NoeudGenerique));
  strncpy(node->data, data, 31);
  node->data[31] = '\0';
  node->child_count = 0;
  return node;
}

void DetruireArbre(NoeudGenerique *node) {
  if (!node)
    return;
  for (int i = 0; i < node->child_count; i++) {
    DetruireArbre(node->children[i]);
  }
  free(node);
}

// ==================== N-ARY TO BINARY CONVERSION (LCRS) ====================
// Left-Child Right-Sibling representation:
// - First child of N-ary node becomes LEFT child of binary node
// - Next sibling becomes RIGHT child of binary node

static NoeudGenerique *ConvertNAryToBinaryHelper(NoeudGenerique *nary_node,
                                                 NoeudGenerique *sibling) {
  if (!nary_node)
    return NULL;

  // Create binary node with same data
  NoeudGenerique *bin_node = CreerNoeudGen(nary_node->data);
  bin_node->child_count = 0;

  // First child becomes left child (children[0])
  if (nary_node->child_count > 0 && nary_node->children[0]) {
    // Pass first child, with second child as its sibling
    NoeudGenerique *first_sibling =
        (nary_node->child_count > 1) ? nary_node->children[1] : NULL;
    bin_node->children[0] =
        ConvertNAryToBinaryHelper(nary_node->children[0], first_sibling);
    if (bin_node->children[0])
      bin_node->child_count = 1;
  }

  // Sibling becomes right child (children[1])
  if (sibling) {
    // Find next sibling of this sibling in the parent's children array
    // This is handled by the caller passing the next sibling
    NoeudGenerique *next_sibling = NULL;
    // We need to find the parent to get next sibling - simplify by not doing
    // this here
    bin_node->children[1] = ConvertNAryToBinaryHelper(sibling, NULL);
    if (bin_node->children[1])
      bin_node->child_count = 2;
  }

  return bin_node;
}

// Wrapper that properly handles sibling chains
static NoeudGenerique *ConvertChildrenToSiblingChain(NoeudGenerique **children,
                                                     int start, int count) {
  if (start >= count || !children[start])
    return NULL;

  NoeudGenerique *bin_node = CreerNoeudGen(children[start]->data);
  bin_node->child_count = 0;

  // First child of this N-ary node becomes left child
  if (children[start]->child_count > 0) {
    bin_node->children[0] = ConvertChildrenToSiblingChain(
        children[start]->children, 0, children[start]->child_count);
    if (bin_node->children[0])
      bin_node->child_count = 1;
  }

  // Next sibling becomes right child
  if (start + 1 < count) {
    bin_node->children[1] =
        ConvertChildrenToSiblingChain(children, start + 1, count);
    if (bin_node->children[1])
      bin_node->child_count = 2;
  }

  return bin_node;
}

NoeudGenerique *ConvertNAryToBinary(NoeudGenerique *nary_root) {
  if (!nary_root)
    return NULL;

  // Create binary root with same data
  NoeudGenerique *bin_root = CreerNoeudGen(nary_root->data);
  bin_root->child_count = 0;

  // First child of root becomes left child, its siblings form right chain
  if (nary_root->child_count > 0) {
    bin_root->children[0] = ConvertChildrenToSiblingChain(
        nary_root->children, 0, nary_root->child_count);
    if (bin_root->children[0])
      bin_root->child_count = 1;
  }

  return bin_root;
}

// ==================== LAYOUT ENGINE ====================

static float CalculerSubtreeWidth(NoeudGenerique *node, bool is_binary) {
  if (!node)
    return 0;

  int max_children = is_binary ? 2 : node->child_count;
  if (max_children == 0) {
    node->subtree_width = NODE_DIAM;
    return NODE_DIAM;
  }

  float total = 0;
  int actual_children = 0;

  if (is_binary) {
    // Binary: always reserve space for 2 children
    float left_w = (node->child_count > 0 && node->children[0])
                       ? CalculerSubtreeWidth(node->children[0], true)
                       : NODE_DIAM;
    float right_w = (node->child_count > 1 && node->children[1])
                        ? CalculerSubtreeWidth(node->children[1], true)
                        : NODE_DIAM;
    total = left_w + H_SPACING + right_w;
  } else {
    // N-ary: sum of all children
    for (int i = 0; i < node->child_count; i++) {
      if (node->children[i]) {
        total += CalculerSubtreeWidth(node->children[i], false);
        actual_children++;
      }
    }
    if (actual_children > 1) {
      total += H_SPACING * (actual_children - 1);
    }
  }

  node->subtree_width = (total > NODE_DIAM) ? total : NODE_DIAM;
  return node->subtree_width;
}

static void AssignerCoordonnees(NoeudGenerique *node, float x_center, int depth,
                                bool is_binary) {
  if (!node)
    return;

  node->x = x_center;
  node->y = depth * V_SPACING + 50;

  if (is_binary) {
    float left_w = (node->child_count > 0 && node->children[0])
                       ? node->children[0]->subtree_width
                       : NODE_DIAM;
    float right_w = (node->child_count > 1 && node->children[1])
                        ? node->children[1]->subtree_width
                        : NODE_DIAM;

    float total_w = left_w + H_SPACING + right_w;
    float start_x = x_center - total_w / 2;

    if (node->child_count > 0 && node->children[0]) {
      AssignerCoordonnees(node->children[0], start_x + left_w / 2, depth + 1,
                          true);
    }
    if (node->child_count > 1 && node->children[1]) {
      AssignerCoordonnees(node->children[1],
                          start_x + left_w + H_SPACING + right_w / 2, depth + 1,
                          true);
    }
  } else {
    // N-ary: distribute children evenly
    float total_w = node->subtree_width;
    float start_x = x_center - total_w / 2;
    float current_x = start_x;

    for (int i = 0; i < node->child_count; i++) {
      if (node->children[i]) {
        float child_w = node->children[i]->subtree_width;
        AssignerCoordonnees(node->children[i], current_x + child_w / 2,
                            depth + 1, false);
        current_x += child_w + H_SPACING;
      }
    }
  }
}

void CalculerLayout(NoeudGenerique *node, float x_start, int depth,
                    bool is_binary) {
  if (!node)
    return;
  CalculerSubtreeWidth(node, is_binary);
  // Start from center of subtree width, not from x_start
  float root_x = node->subtree_width / 2;
  AssignerCoordonnees(node, root_x, depth, is_binary);
}

// ==================== GENERATION ====================

// ==================== GENERATION EXACTE ====================

static int g_nodes_created = 0;

// Helper to generate value based on selected type
// Helper to count nodes
static int CountNodes(NoeudGenerique *node) {
  if (!node)
    return 0;
  int count = 1;
  for (int i = 0; i < node->child_count; i++) {
    count += CountNodes(node->children[i]);
  }
  return count;
}

// Helper to collect all node data into an array
static void CollectData(NoeudGenerique *node, char **array, int *index) {
  if (!node)
    return;
  // Store copy of data
  strncpy(array[*index], node->data, 31);
  (*index)++;
  for (int i = 0; i < node->child_count; i++) {
    CollectData(node->children[i], array, index);
  }
}

// Comparison functions for qsort
static int CompareInt(const void *a, const void *b) {
  const char *s1 = *(const char **)a;
  const char *s2 = *(const char **)b;
  return atoi(s1) - atoi(s2);
}

static int CompareFloat(const void *a, const void *b) {
  const char *s1 = *(const char **)a;
  const char *s2 = *(const char **)b;
  float f1 = strtof(s1, NULL);
  float f2 = strtof(s2, NULL);
  if (f1 < f2)
    return -1;
  if (f1 > f2)
    return 1;
  return 0;
}

static int CompareString(const void *a, const void *b) {
  const char *s1 = *(const char **)a;
  const char *s2 = *(const char **)b;
  return strcmp(s1, s2);
}

// Helper to build balanced BST from sorted array
static NoeudGenerique *BuildBalancedBST(char **array, int start, int end) {
  if (start > end)
    return NULL;

  int mid = (start + end) / 2;
  NoeudGenerique *node = CreerNoeudGen(array[mid]);

  // Left child -> child[0]
  node->children[0] = BuildBalancedBST(array, start, mid - 1);
  // Right child -> child[1]
  node->children[1] = BuildBalancedBST(array, mid + 1, end);

  // In binary mode, child_count is implicitly 2 (or 1 if only left, but usually
  // we init both slots) Actually CreerNoeudGen inits children to NULL. We
  // should set child_count = 2 if we are strictly binary
  node->child_count = 2;

  return node;
}

static void ReorderTreeAsBST(ArbresScreen *screen) {
  if (!screen->arbre.racine)
    return;

  int total_nodes = CountNodes(screen->arbre.racine);
  if (total_nodes < 1)
    return;

  // Allocate array of strings
  char **dataArray = (char **)malloc(total_nodes * sizeof(char *));
  for (int i = 0; i < total_nodes; i++) {
    dataArray[i] = (char *)malloc(32 * sizeof(char));
  }

  // Collect
  int index = 0;
  CollectData(screen->arbre.racine, dataArray, &index);

  // Sort
  if (screen->selected_data_type == 0) { // INT
    qsort(dataArray, total_nodes, sizeof(char *), CompareInt);
  } else if (screen->selected_data_type == 1) { // FLOAT
    qsort(dataArray, total_nodes, sizeof(char *), CompareFloat);
  } else { // CHAR or STRING
    qsort(dataArray, total_nodes, sizeof(char *), CompareString);
  }

  // Destroy old tree
  DetruireArbre(screen->arbre.racine);

  // Build new balanced tree
  screen->arbre.racine = BuildBalancedBST(dataArray, 0, total_nodes - 1);
  screen->arbre.taille = total_nodes;
  screen->is_binary = true; // BST implies binary

  // Cleanup
  for (int i = 0; i < total_nodes; i++) {
    free(dataArray[i]);
  }
  free(dataArray);
}

static void GenererValeurSelonType(char *buffer, int max_len, int type) {
  if (type == 0) { // INT (0-99)
    snprintf(buffer, max_len, "%d", GetRandomValue(0, 99));
  } else if (type == 1) { // FLOAT (0.0-99.9)
    float val = (float)GetRandomValue(0, 999) / 10.0f;
    snprintf(buffer, max_len, "%.1f", val);
  } else if (type == 2) { // CHAR (A-Z)
    char c = (char)GetRandomValue('A', 'Z');
    snprintf(buffer, max_len, "%c", c);
  } else if (type == 3) { // STRING (Random 4 letters)
    for (int i = 0; i < 4; i++) {
      buffer[i] = (char)GetRandomValue('A', 'Z');
    }
    buffer[4] = '\0';
  } else {
    snprintf(buffer, max_len, "?");
  }
}

// Structure for a candidate slot where a new node can be attached
typedef struct {
  NoeudGenerique *parent;
  int child_index;
  int depth;
} CandidateSlot;

void GenererArbreAleatoire(ArbresScreen *screen, int taille, int profondeur,
                           int max_fils) {
  // Clear existing
  if (screen->arbre.racine) {
    DetruireArbre(screen->arbre.racine);
    screen->arbre.racine = NULL;
  }

  screen->arbre.taille = 0;
  if (taille <= 0)
    return;

  // Create Root
  char val[32];
  GenererValeurSelonType(val, sizeof(val), screen->selected_data_type);
  screen->arbre.racine = CreerNoeudGen(val);
  screen->arbre.taille = 1;

  if (taille == 1)
    return;

  // Implement "Random Candidate" Algorithm for Exact Count
  // We maintain a list of all potential slots where a child can be added.
  // This ensures we can always pick a valid spot (if any exist) and reach exact
  // count.

  // Max candidates: rough estimate. Even for large trees, frontier isn't HUGE.
  // But let's be safe with dynamic array or large static.
  // For 1000 nodes, max frontier is bounded but can be large.
  // Safety: Allow up to 5000 candidates.
  CandidateSlot *candidates =
      (CandidateSlot *)malloc(sizeof(CandidateSlot) * 5000);
  int candidate_count = 0;

  // Initial candidates: children of root
  int actual_max_fils = screen->is_binary ? 2 : max_fils;
  // For N-ary, start with ONLY the first child (index 0).
  // Sibling slots (1, 2, ...) are added dynamically when the previous sibling
  // is filled. For Binary, we can start with both (0 and 1) candidates.
  int initial_candidates_count = screen->is_binary ? 2 : 1;

  // Add initial slots from root
  for (int i = 0; i < initial_candidates_count; i++) {
    if (candidate_count < 5000) {
      candidates[candidate_count].parent = screen->arbre.racine;
      candidates[candidate_count].child_index = i;
      candidates[candidate_count].depth = 1; // Children of root are at depth 1
      candidate_count++;
    }
  }

  // Generate remaining nodes
  while (screen->arbre.taille < taille && candidate_count > 0) {
    // Pick a random candidate
    int pick = GetRandomValue(0, candidate_count - 1);
    CandidateSlot slot = candidates[pick];

    // Remove picked candidate by moving last one here
    candidates[pick] = candidates[candidate_count - 1];
    candidate_count--;

    // Helper: Check constraints (depth)
    if (slot.depth < profondeur) {
      // Create the node
      GenererValeurSelonType(val, sizeof(val), screen->selected_data_type);
      NoeudGenerique *new_node = CreerNoeudGen(val);

      // Attach to parent
      slot.parent->children[slot.child_index] = new_node;

      // Update parent's child_count logic
      if (screen->is_binary) {
        // Binary specific: update child count to max index + 1
        if (slot.child_index >= slot.parent->child_count) {
          slot.parent->child_count = slot.child_index + 1;
        }
      } else {
        // N-ary: Append to end (slot.child_index) should match current count
        // for sequential fill? Wait, for N-ary candidates, we only add "next
        // available" slot or "all empty" slots? The slot logic above assumed
        // static slots [0..max]. For N-ary, usually we just append. But our
        // candidate approach allows sparse filling? NO, for N-ary layout,
        // children must be packed [0, 1, 2...]. Sparse support is weak in
        // layout. Let's enforce PACKED children for N-ary to avoid holes.
        // Actually, for N-ary, simply: "Pick a node from frontier that has <
        // max_fils children".

        // Re-thinking N-ary strategy:
        // Candidate = "Node that can accept more children".
        // Instead of "Slots", let's store "Nodes needing children".
        // BUT strict slots handles Binary Left/Right well.
        // Let's stick to slots but handle N-ary carefully:
        // For N-ary, we only add the NEXT slot as candidate. When filled, we
        // add the next one.
        slot.parent->child_count = slot.child_index + 1;
      }

      screen->arbre.taille++;

      // Valid node creation. Now add ITS potential children as candidates
      if (slot.depth + 1 < profondeur) {
        if (screen->is_binary) {
          // Add Left and Right slots
          if (candidate_count < 5000) {
            candidates[candidate_count++] =
                (CandidateSlot){new_node, 0, slot.depth + 1};
          }
          if (candidate_count < 5000) {
            candidates[candidate_count++] =
                (CandidateSlot){new_node, 1, slot.depth + 1};
          }
        } else {
          // N-ary: Add ONLY the first child slot [0].
          // Logic: When [0] is filled (later loop), we will add [1] as
          // candidate. This ensures N-ary children are always packed 0..N.
          if (candidate_count < 5000) {
            candidates[candidate_count++] =
                (CandidateSlot){new_node, 0, slot.depth + 1};
          }
        }
      }

      // If N-ary, and we just filled slot K, we can now open slot K+1 for this
      // parent
      if (!screen->is_binary && slot.child_index + 1 < actual_max_fils) {
        if (candidate_count < 5000) {
          // Add next sibling slot
          candidates[candidate_count++] =
              (CandidateSlot){slot.parent, slot.child_index + 1, slot.depth};
        }
      }
    } else {
      // Depth limit reached for this specific candidate.
      // We removed it from the pool.
      // If the pool becomes empty but we haven't reached target size, we have a
      // problem. In that specific fallback case (below), we'll do a brute force
      // scan.
    }
  }

  // FORCE FILL: If we ran out of candidates due to depth constraints but still
  // need nodes
  while (screen->arbre.taille < taille) {
    // Brute force find any node that has space
    // BFS traversal to find first node with < max_fils children
    // This ignores depth limit to ensure we meet the node count requirement
    NoeudGenerique *queue[5000];
    int head = 0;
    int tail = 0;
    queue[tail++] = screen->arbre.racine;

    NoeudGenerique *parent = NULL;
    while (head < tail) {
      NoeudGenerique *curr = queue[head++];
      if (curr->child_count < actual_max_fils) {
        parent = curr;
        break;
      }
      for (int i = 0; i < curr->child_count; i++) {
        queue[tail++] = curr->children[i];
      }
    }

    if (parent) {
      GenererValeurSelonType(val, sizeof(val), screen->selected_data_type);
      NoeudGenerique *new_node = CreerNoeudGen(val);
      // Append to next available slot
      parent->children[parent->child_count] = new_node;
      parent->child_count++;
      screen->arbre.taille++;
    } else {
      // Tree is full (shouldn't happen with dynamic allocation unless
      // max_fils=0)
      break;
    }
  }
  free(candidates);
}

// ==================== PARCOURS ====================

static void CollecterPreOrdre(NoeudGenerique *node, NoeudGenerique **result,
                              int *idx) {
  if (!node)
    return;
  result[(*idx)++] = node;
  for (int i = 0; i < node->child_count; i++) {
    CollecterPreOrdre(node->children[i], result, idx);
  }
}

static void CollecterInOrdre(NoeudGenerique *node, NoeudGenerique **result,
                             int *idx, bool is_binary) {
  if (!node)
    return;
  if (is_binary && node->child_count > 0 && node->children[0]) {
    CollecterInOrdre(node->children[0], result, idx, true);
  }
  result[(*idx)++] = node;
  if (is_binary && node->child_count > 1 && node->children[1]) {
    CollecterInOrdre(node->children[1], result, idx, true);
  }
}

static void CollecterPostOrdre(NoeudGenerique *node, NoeudGenerique **result,
                               int *idx) {
  if (!node)
    return;
  for (int i = 0; i < node->child_count; i++) {
    CollecterPostOrdre(node->children[i], result, idx);
  }
  result[(*idx)++] = node;
}

static void CollecterLargeur(NoeudGenerique *root, NoeudGenerique **result,
                             int *idx) {
  if (!root)
    return;
  NoeudGenerique *queue[500];
  int front = 0, rear = 0;
  queue[rear++] = root;

  while (front < rear) {
    NoeudGenerique *node = queue[front++];
    result[(*idx)++] = node;
    for (int i = 0; i < node->child_count; i++) {
      if (node->children[i])
        queue[rear++] = node->children[i];
    }
  }
}

// ==================== SUPPRESSION ====================

static NoeudGenerique *TrouverParent(NoeudGenerique *racine,
                                     NoeudGenerique *cible) {
  if (!racine || !cible || racine == cible)
    return NULL;

  for (int i = 0; i < racine->child_count; i++) {
    if (racine->children[i] == cible)
      return racine;
    NoeudGenerique *res = TrouverParent(racine->children[i], cible);
    if (res)
      return res;
  }
  return NULL;
}

static void SupprimerNoeud(ArbresScreen *screen, NoeudGenerique *cible) {
  if (!cible || !screen->arbre.racine)
    return;

  // Cas racine
  if (cible == screen->arbre.racine) {
    DetruireArbre(screen->arbre.racine);
    screen->arbre.racine = NULL;
    screen->arbre.taille = 0;
    screen->show_root_input = true;
    screen->root_input_text[0] = '\0';

    // IMPORTANT: Turn off delete mode when tree is empty!
    screen->is_delete_mode = false;

    snprintf(screen->status_message, sizeof(screen->status_message),
             "Racine supprimee - Arbre vide");
    screen->status_timer = 2.0f;
    return;
  }

  // Trouver parent
  NoeudGenerique *parent = TrouverParent(screen->arbre.racine, cible);
  if (parent) {
    // Retirer de la liste des enfants du parent
    int idx = -1;
    for (int i = 0; i < MAX_CHILDREN; i++) {
      // Search up to MAX_CHILDREN relative to usage, but for finding index we
      // safeguard loop Standard loop uses child_count, but let's be safe if
      // count is inconsistent during debug
      if (i < MAX_CHILDREN && parent->children[i] == cible) {
        idx = i;
        break;
      }
    }

    if (idx != -1) {
      if (screen->is_binary) {
        // Binary Mode: DO NOT SHIFT. Just clear the slot.
        // This preserves Left vs Right positioning.
        parent->children[idx] = NULL;

        // Détruire le sous-arbre cible
        DetruireArbre(cible);

        // Update child_count based on slots usage for binary
        // If [1] (Right) exists, count must be 2 to iterate up to it.
        // If only [0] (Left) exists, count is 1.
        // If neither, count is 0.
        if (parent->children[1] != NULL) {
          parent->child_count = 2;
        } else if (parent->children[0] != NULL) {
          parent->child_count = 1;
        } else {
          parent->child_count = 0;
        }
      } else {
        // N-ary Mode: Shift elements to keep list packed
        for (int i = idx; i < parent->child_count - 1; i++) {
          parent->children[i] = parent->children[i + 1];
        }
        parent->children[parent->child_count - 1] = NULL;
        parent->child_count--;

        // Détruire le sous-arbre
        DetruireArbre(cible);
      }

      // Decrement visible node count (approximation)
      if (screen->arbre.taille > 0)
        screen->arbre.taille--;

      snprintf(screen->status_message, sizeof(screen->status_message),
               "Noeud supprime");
      screen->status_timer = 2.0f;
    }
  }
}

// ==================== RECHERCHE ====================

// Collects the path taken during search. Returns true if found.
static bool CollecterRecherche(NoeudGenerique *node, const char *valeur,
                               NoeudGenerique **result, int *idx) {
  if (!node)
    return false;

  // Add current node to path
  result[(*idx)++] = node;

  if (strcmp(node->data, valeur) == 0)
    return true;

  for (int i = 0; i < node->child_count; i++) {
    if (CollecterRecherche(node->children[i], valeur, result, idx))
      return true;
  }
  return false;
}

// Old static bool RechercherValeur removed as it is replaced by
// CollecterRecherche logic

// ==================== ÉCRAN INIT ====================

void ArbresInit(ArbresScreen *screen) {
  memset(screen, 0, sizeof(ArbresScreen));

  screen->is_binary = true;
  screen->arbre.racine = NULL;
  screen->arbre.taille = 0;

  // Navigation button (top left)
  // screen->btn_back = Remove;

  // Mode buttons (top right)
  screen->btn_mode_binaire =
      CreateButton(WINDOW_WIDTH - 220, 10 + NAVBAR_HEIGHT, 100, 28, "Binaire",
                   COLOR_NEON_BLUE);
  screen->btn_mode_naire = CreateButton(WINDOW_WIDTH - 115, 10 + NAVBAR_HEIGHT,
                                        100, 28, "N-Aire", COLOR_NEON_PURPLE);

  // ========= UI ELEMENTS - positions set dynamically in Draw =========
  // Inputs
  screen->input_taille = CreateInputBox(0, 0, 55, 26, 4);
  screen->input_profondeur = CreateInputBox(0, 0, 50, 26, 2);
  screen->input_max_fils = CreateInputBox(0, 0, 50, 26, 2);

  // Buttons
  screen->btn_generer = CreateButton(0, 0, 80, 28, "Generer", COLOR_NEON_GREEN);
  screen->btn_preordre = CreateButton(0, 0, 65, 28, "Prefixe", COLOR_NEON_BLUE);
  screen->btn_inordre = CreateButton(0, 0, 55, 28, "Infixe", COLOR_NEON_BLUE);
  screen->btn_postordre =
      CreateButton(0, 0, 70, 28, "Postfixe", COLOR_NEON_BLUE);
  screen->btn_largeur = CreateButton(0, 0, 60, 28, "Larg", COLOR_NEON_BLUE);

  // Speed controls
  screen->btn_speed_down = CreateButton(0, 0, 28, 28, "-", COLOR_NEON_PURPLE);
  screen->btn_speed_up = CreateButton(0, 0, 28, 28, "+", COLOR_NEON_PURPLE);
  screen->anim_speed = 0.5f;

  // STOP and Vider (far right - fixed position)
  // Supprimer toggle button
  screen->btn_supprimer_mode =
      CreateButton(WINDOW_WIDTH - 240, 52 + NAVBAR_HEIGHT, 70, 28, "Supprimer",
                   COLOR_NEON_RED);
  screen->is_delete_mode = false;
  // Modify toggle button
  screen->btn_modifier_mode =
      CreateButton(WINDOW_WIDTH - 405, 52 + NAVBAR_HEIGHT, 70, 28, "Modifier",
                   COLOR_NEON_ORANGE);
  screen->is_modify_mode = false;

  // Bouton Recherche
  screen->btn_recherche = CreateButton(WINDOW_WIDTH - 325, 52 + NAVBAR_HEIGHT,
                                       80, 28, "Rechercher", COLOR_NEON_BLUE);
  screen->popup_search_open = false;
  screen->search_text[0] = '\0';
  screen->search_cursor = 0;

  // Popup search buttons
  screen->popup_btn_chercher =
      CreateButton(0, 0, 80, 30, "CHERCHER", COLOR_NEON_GREEN);
  // Mode buttons (top right) - Aligned Right-to-Left
  // N-Aire (Rightmost)
  screen->btn_mode_naire = CreateButton(WINDOW_WIDTH - 115, 10 + NAVBAR_HEIGHT,
                                        100, 28, "N-Aire", COLOR_NEON_PURPLE);
  // Binaire (Left of N-Aire)
  screen->btn_mode_binaire =
      CreateButton(WINDOW_WIDTH - 225, 10 + NAVBAR_HEIGHT, 100, 28, "Binaire",
                   COLOR_NEON_BLUE);

  // Vider (Left of Binaire)
  screen->btn_vider = CreateButton(WINDOW_WIDTH - 305, 10 + NAVBAR_HEIGHT, 70,
                                   28, "Vider", COLOR_NEON_RED);

  // Ordonner (Left of Vider)
  screen->btn_ordonner = CreateButton(WINDOW_WIDTH - 405, 10 + NAVBAR_HEIGHT,
                                      90, 28, "Ordonner", COLOR_NEON_GREEN);

  // N-ary to Binary conversion button (only visible in N-ary mode)
  screen->btn_convertir =
      CreateButton(0, 0, 90, 28, "-> Binaire", COLOR_NEON_GREEN);
  screen->is_converting = false;
  screen->convert_progress = 0.0f;
  screen->mario_anim_frame = 0;
  screen->mario_texture = LoadTexture("assets/mario_push.png");

  // Popup buttons
  screen->popup_btn_gauche =
      CreateButton(0, 0, 70, 30, "GAUCHE", COLOR_NEON_BLUE);
  screen->popup_btn_droite =
      CreateButton(0, 0, 70, 30, "DROITE", COLOR_NEON_BLUE);
  screen->popup_btn_ajouter =
      CreateButton(0, 0, 80, 30, "AJOUTER", COLOR_NEON_GREEN);
  screen->popup_btn_annuler =
      CreateButton(0, 0, 70, 30, "ANNULER", COLOR_NEON_RED);
  screen->popup_btn_valider_modif =
      CreateButton(0, 0, 80, 30, "MODIFIER", COLOR_NEON_ORANGE);
  screen->popup_is_modify = false;
  screen->popup_modify_target = NULL;

  screen->zoom = 1.0f;
  // Camera offset to center the tree area (tree area starts at y=90)
  screen->offset =
      (Vector2){(WINDOW_WIDTH - 20) / 2 + 10, (WINDOW_HEIGHT - 140) / 2 + 90};
  screen->popup_open = false;
  screen->popup_parent = NULL;
  screen->parcours_result = NULL;

  // Start with root input prompt
  screen->show_root_input = true;
  screen->root_input_text[0] = '\0';
  screen->root_input_cursor = 0;

  // Init Data Type Dropdown
  screen->selected_data_type = 0; // Default: Int
  screen->dropdown_open = false;
}

// ==================== ÉCRAN UPDATE ====================

AppScreen ArbresUpdate(ArbresScreen *screen) {
  float dt = GetFrameTime();

  // Root input mode - click on RACINE node to open popup
  if (screen->show_root_input && !screen->arbre.racine && !screen->popup_open) {
    // Check for click on the visual RACINE node
    Rectangle treeRect = {10, 90 + NAVBAR_HEIGHT, WINDOW_WIDTH - 20,
                          WINDOW_HEIGHT - 130 - NAVBAR_HEIGHT};
    float cx = treeRect.x + treeRect.width / 2;
    float cy = treeRect.y + treeRect.height / 2 - 30;
    float radius = NODE_DIAM / 2;

    Vector2 mousePos = GetMousePosition();
    float dx = mousePos.x - cx;
    float dy = mousePos.y - cy;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        dx * dx + dy * dy < radius * radius) {
      // Click on RACINE node - open popup to enter root value
      // IMPORTANT: Ensure Delete Mode is OFF when creating/editing root
      screen->is_delete_mode = false;

      screen->popup_open = true;
      screen->popup_parent = NULL; // NULL means creating root
      screen->popup_text[0] = '\0';
      screen->popup_cursor = 0;
    }
  }

  // Navigation Bar
  AppScreen next_screen = SCREEN_ARBRES;
  if (UpdateNavigationBar(&next_screen)) {
    if (screen->parcours_result) {
      free(screen->parcours_result);
      screen->parcours_result = NULL;
    }
    return next_screen;
  }

  // Mode toggle - clear arbre when switching (like Listes)
  if (UpdateButton(&screen->btn_mode_binaire) && !screen->is_binary) {
    // Clear tree when switching to binary mode
    if (screen->arbre.racine) {
      DetruireArbre(screen->arbre.racine);
      screen->arbre.racine = NULL;
      screen->arbre.taille = 0;
    }
    screen->is_binary = true;
    screen->show_root_input = true;
    screen->root_input_text[0] = '\0';
    screen->root_input_cursor = 0;
    screen->popup_open = false;
    screen->parcours_animating = false;
    // Reset camera
    Rectangle treeRect = {10, 100 + NAVBAR_HEIGHT, WINDOW_WIDTH - 20,
                          WINDOW_HEIGHT - 140 - NAVBAR_HEIGHT};
    screen->offset = (Vector2){treeRect.x + treeRect.width / 2,
                               treeRect.y + treeRect.height / 2};
    snprintf(screen->status_message, sizeof(screen->status_message),
             "Mode: BINAIRE");
    screen->status_timer = 2.0f;
  }
  if (UpdateButton(&screen->btn_mode_naire) && screen->is_binary) {
    // Clear tree when switching to n-ary mode
    if (screen->arbre.racine) {
      DetruireArbre(screen->arbre.racine);
      screen->arbre.racine = NULL;
      screen->arbre.taille = 0;
    }
    screen->is_binary = false;
    screen->show_root_input = true;
    screen->root_input_text[0] = '\0';
    screen->root_input_cursor = 0;
    screen->popup_open = false;
    screen->parcours_animating = false;
    // Reset camera
    Rectangle treeRect = {10, 100 + NAVBAR_HEIGHT, WINDOW_WIDTH - 20,
                          WINDOW_HEIGHT - 140 - NAVBAR_HEIGHT};
    screen->offset = (Vector2){treeRect.x + treeRect.width / 2,
                               treeRect.y + treeRect.height / 2};
    snprintf(screen->status_message, sizeof(screen->status_message),
             "Mode: N-AIRE");
    screen->status_timer = 2.0f;
  }

  // Generation inputs - ONLY update when popup is not open (otherwise they
  // consume key events)
  if (!screen->popup_open && !screen->popup_search_open) {
    UpdateInputBox(&screen->input_taille);
    UpdateInputBox(&screen->input_profondeur);
    if (!screen->is_binary) {
      UpdateInputBox(&screen->input_max_fils);
    }

    // Dropdown Interaction
    // Define bounds based on mode (Same logic as Draw)
    Rectangle dropdownRect;
    if (screen->is_binary) {
      // Gen Frame starts at 15. Inputs end around...
      // Taille(55) + Prof(50) + Gaps...
      // Layout: [15] [Taille: 15+38=53..108] [118] [Prof: 118+30=148..198] [??]
      float x = 15 + 15;
      x += 38 + 55 + 10; // After Taille
      x += 30 + 50 + 10; // After Prof
      dropdownRect = (Rectangle){x, 52 + NAVBAR_HEIGHT, 90, 26};
    } else {
      // N-ary: [Taille] [Prof] [Fils] ...
      float x = 15 + 15;
      x += 38 + 45 + 10; // After Taille
      x += 30 + 40 + 10; // After Prof
      x += 25 + 40 + 10; // After Fils
      dropdownRect = (Rectangle){x, 52 + NAVBAR_HEIGHT, 90, 26};
    }

    // Handle Open/Close
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 mouse = GetMousePosition();
      if (screen->dropdown_open) {
        // Check options
        for (int i = 0; i < 4; i++) {
          Rectangle optRect = {dropdownRect.x,
                               dropdownRect.y + dropdownRect.height + i * 26,
                               dropdownRect.width, 26};
          if (CheckCollisionPointRec(mouse, optRect)) {
            screen->selected_data_type = i;
            screen->dropdown_open = false;
            return SCREEN_ARBRES; // Consumed
          }
        }
        // Click outside closes
        screen->dropdown_open = false;
      } else {
        if (CheckCollisionPointRec(mouse, dropdownRect)) {
          screen->dropdown_open = true;
          return SCREEN_ARBRES; // Consumed
        }
      }
    }
  }

  // Handle Delete Mode Toggle
  if (UpdateButton(&screen->btn_supprimer_mode)) {
    screen->is_delete_mode = !screen->is_delete_mode;
    screen->is_modify_mode = false; // Disable modify mode
    // If entering delete mode, close other popups
    if (screen->is_delete_mode) {
      screen->popup_search_open = false;
      screen->popup_open = false;
    }
  }

  // Handle Modify Mode Toggle
  if (UpdateButton(&screen->btn_modifier_mode)) {
    screen->is_modify_mode = !screen->is_modify_mode;
    screen->is_delete_mode = false; // Disable delete mode
    if (screen->is_modify_mode) {
      screen->popup_search_open = false;
      screen->popup_open = false;
    }
  }

  // Handle Search Button Click
  if (UpdateButton(&screen->btn_recherche)) {
    screen->popup_search_open = true;
    screen->is_delete_mode = false;
    screen->search_text[0] = '\0';
    screen->search_cursor = 0;
  }

  // Handle Search Popup Interaction
  if (screen->popup_search_open) {
    // Input text
    int key = GetCharPressed();
    while (key > 0) {
      if ((key >= 32) && (key <= 125) && (screen->search_cursor < 31)) {
        screen->search_text[screen->search_cursor] = (char)key;
        screen->search_text[screen->search_cursor + 1] = '\0';
        screen->search_cursor++;
      }
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
      if (screen->search_cursor > 0) {
        screen->search_cursor--;
        screen->search_text[screen->search_cursor] = '\0';
      }
    }

    // Buttons position computed in Draw or here for hit testing
    // We'll trust UpdateButton wrapper to handle hit test if bounds correct
    float px = WINDOW_WIDTH / 2 - 120;
    float py = WINDOW_HEIGHT / 2 - 60;
    screen->popup_btn_chercher.bounds = (Rectangle){px + 40, py + 70, 80, 30};
    screen->popup_btn_search_annuler.bounds =
        (Rectangle){px + 130, py + 70, 70, 30};

    bool confirm =
        IsKeyPressed(KEY_ENTER) || UpdateButton(&screen->popup_btn_chercher);
    if (confirm && strlen(screen->search_text) > 0) {
      // Init animation
      if (screen->parcours_result)
        free(screen->parcours_result);
      screen->parcours_result =
          (NoeudGenerique **)malloc(500 * sizeof(NoeudGenerique *));
      screen->parcours_size = 0;
      int idx = 0;

      screen->search_found =
          CollecterRecherche(screen->arbre.racine, screen->search_text,
                             screen->parcours_result, &idx);

      screen->parcours_size = idx;
      screen->parcours_current_idx = 0;
      screen->parcours_timer = 0;
      screen->parcours_animating = true;
      screen->is_searching = true;              // Mark as search animation
      screen->parcours_type = PARCOURS_LARGEUR; // Dummy

      screen->popup_search_open = false;
      snprintf(screen->status_message, sizeof(screen->status_message),
               "Recherche de '%s'...", screen->search_text);
    }

    if (UpdateButton(&screen->popup_btn_search_annuler) ||
        IsKeyPressed(KEY_ESCAPE)) {
      screen->popup_search_open = false;
    }

    return SCREEN_ARBRES; // Block interaction
  }

  // Handle Click on Node for Deletion
  // Only if not clicking on UI (simple check: if mouse Y > 90 + navbar)
  // although `node_positions` check implies clicking on node.
  if (screen->is_delete_mode && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    for (int i = 0; i < screen->node_count; i++) {
      float dx = mousePos.x - screen->node_positions[i].x;
      float dy = mousePos.y - screen->node_positions[i].y;
      // Check distance (radius ~25)
      if (dx * dx + dy * dy < (NODE_DIAM / 2) * (NODE_DIAM / 2)) {
        SupprimerNoeud(screen, screen->node_positions[i].node);
        // Re-layout tree
        if (screen->arbre.racine) {
          CalculerLayout(screen->arbre.racine, 0, 0, screen->is_binary);
        }
        // IMPORTANT: Return immediately to prevent click from triggering add
        // popup
        return SCREEN_ARBRES;
      }
    }
  }

  // Generate button
  if (UpdateButton(&screen->btn_generer)) {
    int taille = atoi(screen->input_taille.text);
    int prof = atoi(screen->input_profondeur.text);
    int max_fils = atoi(screen->input_max_fils.text);
    if (taille < 1)
      taille = 10;
    if (prof < 1)
      prof = 3;
    if (max_fils < 1)
      max_fils = 3;
    if (max_fils > MAX_CHILDREN)
      max_fils = MAX_CHILDREN;

    GenererArbreAleatoire(screen, taille, prof, max_fils);
    CalculerLayout(screen->arbre.racine, 0, 0, screen->is_binary);
    // Center view on tree
    Rectangle treeRect = {10, 90 + NAVBAR_HEIGHT, WINDOW_WIDTH - 20,
                          WINDOW_HEIGHT - 130 - NAVBAR_HEIGHT};
    screen->offset.x = treeRect.x + treeRect.width / 2 -
                       screen->arbre.racine->x * screen->zoom;
    screen->offset.y = treeRect.y + 50; // Small offset from top
    screen->show_root_input = false;
    screen->highlighted_node = NULL; // Reset highlight from previous search

    if (screen->is_binary) {
      snprintf(screen->status_message, sizeof(screen->status_message),
               "Genere arbre binaire: %d noeuds, prof max %d",
               screen->arbre.taille, prof);
    } else {
      snprintf(screen->status_message, sizeof(screen->status_message),
               "Genere arbre n-aire: %d noeuds, prof %d, fils max %d",
               screen->arbre.taille, prof, max_fils);
    }
    screen->status_timer = 2.0f;
  }

  // Clear button - returns to root input mode
  if (UpdateButton(&screen->btn_vider)) {
    if (screen->arbre.racine) {
      DetruireArbre(screen->arbre.racine);
      screen->arbre.racine = NULL;
      screen->arbre.taille = 0;
    }
    screen->popup_open = false;
    screen->popup_parent = NULL;
    screen->parcours_animating = false;
    screen->show_root_input = true;
    screen->root_input_text[0] = '\0';
    screen->root_input_cursor = 0;
    snprintf(screen->status_message, sizeof(screen->status_message),
             "Arbre vide - Entrez une valeur racine");
    screen->status_timer = 2.0f;
  }

  // Handle Reorder Button
  if (screen->arbre.racine && UpdateButton(&screen->btn_ordonner)) {
    ReorderTreeAsBST(screen);
    // Recalculate layout after reordering
    if (screen->arbre.racine) {
      CalculerLayout(screen->arbre.racine, 0, 0, true);
    }
    snprintf(screen->status_message, sizeof(screen->status_message),
             "Arbre reordonne (BST equilibre)");
    screen->status_timer = 2.0f;
    return SCREEN_ARBRES;
  }

  // N-ary to Binary conversion button (only in N-ary mode with a tree)
  if (!screen->is_binary && screen->arbre.racine && !screen->is_converting) {
    if (UpdateButton(&screen->btn_convertir)) {
      screen->is_converting = true;
      screen->convert_progress = 0.0f;
      screen->mario_anim_frame = 0;
      snprintf(screen->status_message, sizeof(screen->status_message),
               "Conversion vers arbre binaire...");
      screen->status_timer = 3.0f;
    }
  }

  // Update conversion animation
  if (screen->is_converting) {
    screen->convert_progress += GetFrameTime() * 0.8f; // Animation speed
    screen->mario_anim_frame++;

    if (screen->convert_progress >= 1.0f) {
      // Animation complete - perform actual conversion
      NoeudGenerique *old_tree = screen->arbre.racine;
      screen->arbre.racine = ConvertNAryToBinary(old_tree);
      DetruireArbre(old_tree);

      // Switch to binary mode
      screen->is_binary = true;
      screen->is_converting = false;
      screen->convert_progress = 0.0f;

      // Recalculate layout
      if (screen->arbre.racine) {
        CalculerLayout(screen->arbre.racine, 0, 0, true);
      }

      snprintf(screen->status_message, sizeof(screen->status_message),
               "Arbre converti en binaire (LCRS)");
      screen->status_timer = 3.0f;
    }
  }

  // Traversal buttons
  if (!screen->parcours_animating && screen->arbre.racine) {
    TypeParcours types[4] = {PARCOURS_PREORDRE, PARCOURS_INORDRE,
                             PARCOURS_POSTORDRE, PARCOURS_LARGEUR};
    Button *btns[4] = {&screen->btn_preordre, &screen->btn_inordre,
                       &screen->btn_postordre, &screen->btn_largeur};

    for (int i = 0; i < 4; i++) {
      // Skip in-ordre for N-ary
      if (i == 1 && !screen->is_binary)
        continue;

      if (UpdateButton(btns[i])) {
        if (screen->parcours_result)
          free(screen->parcours_result);
        screen->parcours_result =
            (NoeudGenerique **)malloc(500 * sizeof(NoeudGenerique *));
        screen->parcours_size = 0;

        int idx = 0;
        switch (types[i]) {
        case PARCOURS_PREORDRE:
          CollecterPreOrdre(screen->arbre.racine, screen->parcours_result,
                            &idx);
          break;
        case PARCOURS_INORDRE:
          CollecterInOrdre(screen->arbre.racine, screen->parcours_result, &idx,
                           screen->is_binary);
          break;
        case PARCOURS_POSTORDRE:
          CollecterPostOrdre(screen->arbre.racine, screen->parcours_result,
                             &idx);
          break;
        case PARCOURS_LARGEUR:
          CollecterLargeur(screen->arbre.racine, screen->parcours_result, &idx);
          break;
        }
        screen->parcours_size = idx;
        screen->parcours_current_idx = 0;
        screen->parcours_timer = 0;
        screen->parcours_animating = true;
        screen->is_searching = false; // Regular traversal
        screen->parcours_type = types[i];
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Parcours %s", PARCOURS_NOMS[i]);
        screen->status_timer = 5.0f;
      }
    }
  }

  // Stop button
  if (UpdateButton(&screen->btn_stop)) {
    screen->parcours_animating = false;
    screen->highlighted_node = NULL;
    snprintf(screen->status_message, sizeof(screen->status_message),
             "Animation arretee");
    screen->status_timer = 2.0f;
  }

  // Speed control
  if (UpdateButton(&screen->btn_speed_down)) {
    screen->anim_speed += 0.1f;
    if (screen->anim_speed > 2.0f)
      screen->anim_speed = 2.0f;
  }
  if (UpdateButton(&screen->btn_speed_up)) {
    screen->anim_speed -= 0.1f;
    if (screen->anim_speed < 0.1f)
      screen->anim_speed = 0.1f;
  }

  // Update Animation
  if (screen->parcours_animating) {
    screen->parcours_timer += dt;
    if (screen->parcours_timer >= screen->anim_speed) {
      screen->parcours_timer = 0;
      if (screen->parcours_current_idx < screen->parcours_size) {
        screen->highlighted_node =
            screen->parcours_result[screen->parcours_current_idx];
        screen->parcours_current_idx++;
      } else {
        screen->parcours_animating = false;

        if (screen->is_searching) {
          screen->show_result_popup = true;
          // Reuse result type for title logic (or add explicit SEARCH type
          // later)
          snprintf(screen->result_text, sizeof(screen->result_text),
                   "La valeur '%s' %s dans l'arbre.", screen->search_text,
                   screen->search_found ? "EST PRESENTE"
                                        : "N'EST PAS PRESENTE");
          screen->is_searching = false; // Reset
        } else {
          // Traverse finished
          screen->show_result_popup = true;
          screen->result_type = screen->parcours_type;
          // Build result text from array
          screen->result_text[0] = '\0';
          for (int i = 0; i < screen->parcours_size; i++) {
            if (i > 0)
              strncat(screen->result_text, " -> ",
                      511 - strlen(screen->result_text));
            strncat(screen->result_text, screen->parcours_result[i]->data,
                    511 - strlen(screen->result_text));
          }
        }
      }
    }
  }

  // Popup input handling - MUST be outside parcours_animating block!
  if (screen->popup_open) {
    // Text input
    int key = GetCharPressed();
    while (key > 0) {
      if (screen->popup_cursor < 31 && key >= 32 && key <= 126) {
        screen->popup_text[screen->popup_cursor++] = (char)key;
        screen->popup_text[screen->popup_cursor] = '\0';
      }
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && screen->popup_cursor > 0) {
      screen->popup_text[--screen->popup_cursor] = '\0';
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      screen->popup_open = false;
      screen->popup_parent = NULL;
    }

    // Popup button positions (centered on screen)
    // Use GetScreenWidth/Height to handle resizing correctly
    float px = (float)GetScreenWidth() / 2 - 120;
    float py = (float)GetScreenHeight() / 2 - 60;

    // Special case: creating root (popup_parent is NULL)
    if (screen->popup_is_modify) {
      screen->popup_btn_valider_modif.bounds =
          (Rectangle){px + 40, py + 80, 80, 30};
      screen->popup_btn_annuler.bounds = (Rectangle){px + 130, py + 80, 70, 30};

      if ((IsKeyPressed(KEY_ENTER) ||
           UpdateButton(&screen->popup_btn_valider_modif)) &&
          strlen(screen->popup_text) > 0) {
        if (screen->popup_modify_target) {
          strncpy(screen->popup_modify_target->data, screen->popup_text, 31);
          screen->popup_modify_target->data[31] = '\0';
          CalculerLayout(screen->arbre.racine, 0, 0, screen->is_binary);
          snprintf(screen->status_message, sizeof(screen->status_message),
                   "Valeur modifiee: %s", screen->popup_text);
          screen->status_timer = 2.0f;
        }
        screen->popup_open = false;
        screen->popup_is_modify = false;
        screen->popup_modify_target = NULL;
      }
      if (UpdateButton(&screen->popup_btn_annuler)) {
        screen->popup_open = false;
        screen->popup_is_modify = false;
        screen->popup_modify_target = NULL;
      }
    } else if (screen->popup_parent == NULL) {
      screen->popup_btn_ajouter.bounds = (Rectangle){px + 40, py + 80, 80, 30};
      screen->popup_btn_annuler.bounds = (Rectangle){px + 140, py + 80, 70, 30};

      if ((IsKeyPressed(KEY_ENTER) ||
           UpdateButton(&screen->popup_btn_ajouter)) &&
          strlen(screen->popup_text) > 0) {
        screen->arbre.racine = CreerNoeudGen(screen->popup_text);
        screen->arbre.taille = 1;
        CalculerLayout(screen->arbre.racine, 0, 0, screen->is_binary);
        // Center the view
        Rectangle treeRect = {10, 90 + NAVBAR_HEIGHT,
                              (float)GetScreenWidth() - 20,
                              (float)GetScreenHeight() - 130 - NAVBAR_HEIGHT};
        screen->offset.x = treeRect.x + treeRect.width / 2 -
                           screen->arbre.racine->x * screen->zoom;
        screen->offset.y = treeRect.y + treeRect.height / 2 -
                           screen->arbre.racine->y * screen->zoom;
        screen->show_root_input = false;
        screen->popup_open = false;
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Racine '%s' creee", screen->popup_text);
        screen->status_timer = 3.0f;
      }
      if (UpdateButton(&screen->popup_btn_annuler)) {
        screen->popup_open = false;
      }
    } else if (screen->is_binary) {
      // Binary Mode - Update buttons
      screen->popup_btn_gauche.bounds = (Rectangle){px + 10, py + 80, 70, 30};
      screen->popup_btn_droite.bounds = (Rectangle){px + 90, py + 80, 70, 30};
      screen->popup_btn_annuler.bounds = (Rectangle){px + 170, py + 80, 70, 30};

      bool has_left = screen->popup_parent->child_count > 0 &&
                      screen->popup_parent->children[0] != NULL;
      bool has_right = screen->popup_parent->child_count > 1 &&
                       screen->popup_parent->children[1] != NULL;

      if (UpdateButton(&screen->popup_btn_gauche) && !has_left &&
          strlen(screen->popup_text) > 0) {
        NoeudGenerique *child = CreerNoeudGen(screen->popup_text);
        screen->popup_parent->children[0] = child;

        // Original logic requested by user
        if (screen->popup_parent->child_count == 0) {
          screen->popup_parent->children[0] = child;
          screen->popup_parent->child_count = 1;
        } else {
          screen->popup_parent->children[0] = child;
        }

        screen->arbre.taille++;
        CalculerLayout(screen->arbre.racine, 0, 0, screen->is_binary);
        screen->popup_open = false;
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Ajoute '%s' a gauche", screen->popup_text);
        screen->status_timer = 2.0f;
      }
      if (UpdateButton(&screen->popup_btn_droite) && !has_right &&
          strlen(screen->popup_text) > 0) {
        NoeudGenerique *child = CreerNoeudGen(screen->popup_text);
        screen->popup_parent->children[1] = child;

        // Original logic requested by user
        while (screen->popup_parent->child_count < 2) {
          screen->popup_parent->children[screen->popup_parent->child_count++] =
              NULL;
        }
        screen->popup_parent->children[1] = child;

        screen->arbre.taille++;
        CalculerLayout(screen->arbre.racine, 0, 0, screen->is_binary);
        screen->popup_open = false;
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Ajoute '%s' a droite", screen->popup_text);
        screen->status_timer = 2.0f;
      }
      if (UpdateButton(&screen->popup_btn_annuler)) {
        screen->popup_open = false;
        screen->popup_parent = NULL;
      }
    } else {
      // N-ary mode - Update buttons
      screen->popup_btn_ajouter.bounds = (Rectangle){px + 40, py + 80, 80, 30};
      screen->popup_btn_annuler.bounds = (Rectangle){px + 130, py + 80, 70, 30};

      if (UpdateButton(&screen->popup_btn_ajouter) &&
          strlen(screen->popup_text) > 0) {
        if (screen->popup_parent->child_count < MAX_CHILDREN) {
          NoeudGenerique *child = CreerNoeudGen(screen->popup_text);
          screen->popup_parent->children[screen->popup_parent->child_count++] =
              child;
          screen->arbre.taille++;
          CalculerLayout(screen->arbre.racine, 0, 0, screen->is_binary);
          screen->popup_open = false;
          snprintf(screen->status_message, sizeof(screen->status_message),
                   "Ajoute '%s'", screen->popup_text);
          screen->status_timer = 2.0f;
        }
      }
      if (UpdateButton(&screen->popup_btn_annuler)) {
        screen->popup_open = false;
        screen->popup_parent = NULL;
      }
    }

    return SCREEN_ARBRES; // Block all other interactions when popup open
  }

  // Status timer
  if (screen->status_timer > 0) {
    screen->status_timer -= dt;
  }

  // Zoom with wheel (centered on mouse)
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mouse = GetMousePosition();
    float oldZoom = screen->zoom;
    screen->zoom += wheel * 0.1f;
    if (screen->zoom < 0.3f)
      screen->zoom = 0.3f;
    if (screen->zoom > 2.0f)
      screen->zoom = 2.0f;
    // Adjust offset to zoom towards mouse position
    float zoomFactor = screen->zoom / oldZoom;
    screen->offset.x = mouse.x - (mouse.x - screen->offset.x) * zoomFactor;
    screen->offset.y = mouse.y - (mouse.y - screen->offset.y) * zoomFactor;
  }

  // Pan with left click drag on empty space OR right click drag
  Rectangle treeRect = {10, 90, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 130};
  Vector2 mousePos = GetMousePosition();
  bool mouseInTreeArea = CheckCollisionPointRec(mousePos, treeRect);

  // Check if we clicked on a node (only if no popup is open)
  bool clickedOnNode = false;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseInTreeArea &&
      !screen->show_result_popup) {
    float radius = (NODE_DIAM / 2) * screen->zoom;
    for (int i = 0; i < screen->node_count; i++) {
      float dx = mousePos.x - screen->node_positions[i].x;
      float dy = mousePos.y - screen->node_positions[i].y;
      if (dx * dx + dy * dy < radius * radius) {
        clickedOnNode = true;
        // Open popup for this node if not animating
        // Open popup for this node if not animating
        if (!screen->popup_open && !screen->parcours_animating &&
            !screen->is_delete_mode) {
          if (screen->is_modify_mode) {
            screen->popup_open = true;
            screen->popup_is_modify = true;
            screen->popup_modify_target = screen->node_positions[i].node;
            strncpy(screen->popup_text, screen->node_positions[i].node->data,
                    31);
            screen->popup_text[31] = '\0';
            screen->popup_cursor = strlen(screen->popup_text);
          } else {
            screen->popup_open = true;
            screen->popup_is_modify = false;
            screen->popup_parent = screen->node_positions[i].node;
            screen->popup_text[0] = '\0';
            screen->popup_cursor = 0;
          }
        }
        break;
      }
    }
  }

  // Start drag on left click (if not clicking node) or right click - ONLY
  // inside tree area and when no popup is open
  if (mouseInTreeArea && !screen->popup_open && !screen->show_result_popup &&
      ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !clickedOnNode) ||
       IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))) {
    screen->dragging = true;
    screen->drag_start = mousePos;
  }
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ||
      IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    screen->dragging = false;
  }
  if (screen->dragging && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
                           IsMouseButtonDown(MOUSE_RIGHT_BUTTON))) {
    Vector2 mouse = GetMousePosition();
    screen->offset.x += mouse.x - screen->drag_start.x;
    screen->offset.y += mouse.y - screen->drag_start.y;
    screen->drag_start = mouse;
  }

  return SCREEN_ARBRES;
}
// ==================== DRAWING WITH CAMERA OFFSET ====================

static void DrawTreeGrid(ArbresScreen *screen, Rectangle area) {
  int grid_size = 50;
  Color gridColor = {30, 40, 60, 255};

  // Calculate grid offset based on camera - grid should scroll with camera
  // fmod gives us the offset within a grid cell
  float ox = fmodf(screen->offset.x, (float)grid_size);
  float oy = fmodf(screen->offset.y, (float)grid_size);

  // Ensure positive offset
  if (ox < 0)
    ox += grid_size;
  if (oy < 0)
    oy += grid_size;

  // Draw vertical lines
  for (float x = area.x + ox; x < area.x + area.width; x += grid_size) {
    DrawLine((int)x, (int)area.y, (int)x, (int)(area.y + area.height),
             gridColor);
  }

  // Draw horizontal lines
  for (float y = area.y + oy; y < area.y + area.height; y += grid_size) {
    DrawLine((int)area.x, (int)y, (int)(area.x + area.width), (int)y,
             gridColor);
  }
}

static void DrawNodeWithOffset(NoeudGenerique *node, ArbresScreen *screen,
                               NoeudGenerique *highlighted, bool selected) {
  float zoom = screen->zoom;
  float r = (NODE_DIAM / 2) * zoom;

  // Apply camera offset
  float x = node->x * zoom + screen->offset.x;
  float y = node->y * zoom + screen->offset.y;

  // Glow if highlighted
  if (node == highlighted) {
    DrawCircle(x, y, r + 6, (Color){63, 185, 80, 80});
  }

  // Selection ring
  if (selected) {
    DrawCircleLines(x, y, r + 4, COLOR_NEON_ORANGE);
  }

  // Node circle
  Color fill = (node == highlighted) ? COLOR_NEON_GREEN : COLOR_PANEL;
  DrawCircle(x, y, r, fill);
  DrawCircleLines(x, y, r, COLOR_NEON_ORANGE);

  // Text
  int fontSize = (int)(14 * zoom);
  int textW = MeasureText(node->data, fontSize);
  DrawText(node->data, x - textW / 2, y - fontSize / 2, fontSize,
           COLOR_TEXT_WHITE);
}

static void DrawEdgesWithOffset(NoeudGenerique *node, ArbresScreen *screen) {
  if (!node)
    return;
  float zoom = screen->zoom;
  float r = (NODE_DIAM / 2) * zoom;

  float x1 = node->x * zoom + screen->offset.x;
  float y1 = node->y * zoom + screen->offset.y;

  for (int i = 0; i < node->child_count; i++) {
    if (node->children[i]) {
      float x2 = node->children[i]->x * zoom + screen->offset.x;
      float y2 = node->children[i]->y * zoom + screen->offset.y;
      DrawLineEx((Vector2){x1, y1 + r}, (Vector2){x2, y2 - r}, 2,
                 COLOR_NEON_BLUE);
      DrawEdgesWithOffset(node->children[i], screen);
    }
  }
}

static void CollectNodePositionsWithOffset(NoeudGenerique *node,
                                           ArbresScreen *screen) {
  if (!node || screen->node_count >= 500)
    return;

  // Store SCREEN coordinates for click detection
  float x = node->x * screen->zoom + screen->offset.x;
  float y = node->y * screen->zoom + screen->offset.y;

  screen->node_positions[screen->node_count].node = node;
  screen->node_positions[screen->node_count].x = x;
  screen->node_positions[screen->node_count].y = y;
  screen->node_count++;

  for (int i = 0; i < node->child_count; i++) {
    CollectNodePositionsWithOffset(node->children[i], screen);
  }
}

static void DrawNodesWithOffset(NoeudGenerique *node, ArbresScreen *screen,
                                NoeudGenerique *highlighted,
                                NoeudGenerique *selected) {
  if (!node)
    return;

  DrawNodeWithOffset(node, screen, highlighted, node == selected);

  for (int i = 0; i < node->child_count; i++) {
    DrawNodesWithOffset(node->children[i], screen, highlighted, selected);
  }
}

void ArbresDrawTree(ArbresScreen *screen) {
  // Background
  DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_BG_DARK,
                         COLOR_BG_LIGHT);

  // Navigation and mode buttons first
  DrawNavigationBar(SCREEN_ARBRES);
  DrawButton(&screen->btn_mode_binaire);
  DrawButton(&screen->btn_mode_naire);
  DrawButton(&screen->btn_vider);
  if (screen->arbre.racine) { // Only show reorder if tree exists
    DrawButton(&screen->btn_ordonner);
  }

  // Dynamic title - drawn AFTER navbar, positioned after Graphes button
  const char *title = screen->is_binary ? "ARBRE BINAIRE" : "ARBRE N-AIRE";
  Color titleColor = screen->is_binary ? COLOR_NEON_BLUE : COLOR_NEON_PURPLE;
  int titleX = 670; // 120px (4cm) after Graphes button
  int titleY = (NAVBAR_HEIGHT - 18) / 2;
  DrawText(title, titleX, titleY, 18, titleColor);

  // ============= CADRES COLORES AVEC HOVER =============
  Vector2 mousePos = GetMousePosition();
  Color modeColor = screen->is_binary ? COLOR_NEON_BLUE : COLOR_NEON_PURPLE;
  Color modeHover = screen->is_binary ? (Color){80, 180, 255, 30}
                                      : (Color){180, 130, 255, 30};

  // Constants for spacing
  // Constants for spacing
  const int FRAME_Y = 40 + NAVBAR_HEIGHT;
  const int FRAME_H = 50;
  const int BTN_Y = 52 + NAVBAR_HEIGHT;
  const int BTN_GAP = 15;   // 0.5cm between buttons
  const int FRAME_GAP = 60; // 2cm between frames

  if (screen->is_binary) {
    // ========== BINARY MODE LAYOUT ==========

    // Frame GENERATION: [Taille input] [Prof input] [Type Dropdown] [Generer]
    // Calculate width to contain all elements
    int genX = 15;
    int genW = 420; // Widened for Dropdown
    Rectangle frameGen = {genX, FRAME_Y, genW, FRAME_H};
    bool hoverGen = CheckCollisionPointRec(mousePos, frameGen);
    if (hoverGen)
      DrawRectangleRounded(frameGen, 0.1f, 8, modeHover);
    DrawRectRoundedLinesThick(frameGen, 0.1f, 8, 2, modeColor);
    int titleW = MeasureText("GENERATION", 10);
    DrawRectangle(genX + 15, FRAME_Y - 2, titleW + 10, 5, COLOR_BG_DARK);
    DrawText("GENERATION", genX + 20, FRAME_Y - 5, 10, modeColor);

    // Position inputs inside frame
    int x = genX + 15;
    DrawText("Taille", x, BTN_Y + 5, 10, COLOR_TEXT_DIM);
    x += 38;
    screen->input_taille.bounds = (Rectangle){x, BTN_Y, 55, 26};
    DrawInputBox(&screen->input_taille);
    x += 55 + 10;

    DrawText("Prof", x, BTN_Y + 5, 10, COLOR_TEXT_DIM);
    x += 30;
    screen->input_profondeur.bounds = (Rectangle){x, BTN_Y, 50, 26};
    DrawInputBox(&screen->input_profondeur);
    x += 50 + 10;

    // Dropdown Button
    Rectangle dropdownRect = {x, BTN_Y, 90, 26};
    DrawRectangleRounded(dropdownRect, 0.2f, 4, COLOR_PANEL);
    DrawRectRoundedLinesThick(dropdownRect, 0.2f, 4, 1, modeColor);
    const char *types[] = {"Int", "Float", "Char", "String"};
    DrawText(types[screen->selected_data_type], x + 10, BTN_Y + 7, 10,
             COLOR_TEXT_MAIN);
    DrawText("v", x + 75, BTN_Y + 7, 10, COLOR_TEXT_DIM);
    x += 90 + 15;

    screen->btn_generer.bounds = (Rectangle){x, BTN_Y, 80, 28};
    DrawButton(&screen->btn_generer);

    // Frame PARCOURS: [Prefixe] [Infixe] [Postfixe] [Larg] [- x +]
    int parcX = genX + genW + FRAME_GAP;
    int parcW = 400; // Contains buttons + speed controls
    Rectangle frameParc = {parcX, FRAME_Y, parcW, FRAME_H};
    bool hoverParc = CheckCollisionPointRec(mousePos, frameParc);
    if (hoverParc)
      DrawRectangleRounded(frameParc, 0.1f, 8, modeHover);
    DrawRectRoundedLinesThick(frameParc, 0.1f, 8, 2, modeColor);
    titleW = MeasureText("PARCOURS", 10);
    DrawRectangle(parcX + 15, FRAME_Y - 2, titleW + 10, 5, COLOR_BG_DARK);
    DrawText("PARCOURS", parcX + 20, FRAME_Y - 5, 10, modeColor);

    // Position buttons inside frame with 15px gaps
    x = parcX + 15;
    screen->btn_preordre.bounds = (Rectangle){x, BTN_Y, 60, 28};
    DrawButton(&screen->btn_preordre);
    x += 60 + BTN_GAP;

    screen->btn_inordre.bounds = (Rectangle){x, BTN_Y, 55, 28};
    DrawButton(&screen->btn_inordre);
    x += 55 + BTN_GAP;

    screen->btn_postordre.bounds = (Rectangle){x, BTN_Y, 65, 28};
    DrawButton(&screen->btn_postordre);
    x += 65 + BTN_GAP;

    screen->btn_largeur.bounds = (Rectangle){x, BTN_Y, 50, 28};
    DrawButton(&screen->btn_largeur);
    x += 50 + BTN_GAP;

    // Speed controls inside frame
    screen->btn_speed_down.bounds = (Rectangle){x, BTN_Y, 25, 28};
    DrawButton(&screen->btn_speed_down);
    x += 25 + 3;
    char speedStr[16];
    snprintf(speedStr, sizeof(speedStr), "%.1f", screen->anim_speed);
    DrawText(speedStr, x, BTN_Y + 7, 10, COLOR_TEXT_MAIN);
    x += 22;
    screen->btn_speed_up.bounds = (Rectangle){x, BTN_Y, 25, 28};
    DrawButton(&screen->btn_speed_up);

    // Frame OPERATION: [Rechercher] [Modifier] [Supprimer] [STOP]
    int opX = parcX + parcW + 20; // Small gap after PARCOURS
    int opW = 325;                // Widened to include Modifier
    Rectangle frameOp = {opX, FRAME_Y, opW, FRAME_H};
    bool hoverOp = CheckCollisionPointRec(mousePos, frameOp);
    if (hoverOp)
      DrawRectangleRounded(frameOp, 0.1f, 8, modeHover);
    DrawRectRoundedLinesThick(frameOp, 0.1f, 8, 2, modeColor);
    titleW = MeasureText("OPERATION", 10);
    DrawRectangle(opX + 15, FRAME_Y - 2, titleW + 10, 5, COLOR_BG_DARK);
    DrawText("OPERATION", opX + 20, FRAME_Y - 5, 10, modeColor);

    // Position operation buttons inside frame
    x = opX + 15;
    screen->btn_recherche.bounds = (Rectangle){x, BTN_Y, 75, 28};
    DrawButton(&screen->btn_recherche);
    x += 75 + BTN_GAP;

    screen->btn_modifier_mode.bounds = (Rectangle){x, BTN_Y, 70, 28};
    if (screen->is_modify_mode) {
      DrawRectangleRounded(screen->btn_modifier_mode.bounds, 0.2f, 8,
                           COLOR_NEON_ORANGE);
      DrawRectRoundedLinesThick(screen->btn_modifier_mode.bounds, 0.2f, 8, 2,
                                WHITE);
      DrawText("Modifier", screen->btn_modifier_mode.bounds.x + 8,
               screen->btn_modifier_mode.bounds.y + 8, 10, WHITE);
    } else {
      DrawButton(&screen->btn_modifier_mode);
    }
    x += 70 + BTN_GAP;

    screen->btn_supprimer_mode.bounds = (Rectangle){x, BTN_Y, 70, 28};
    if (screen->is_delete_mode) {
      DrawRectangleRounded(screen->btn_supprimer_mode.bounds, 0.2f, 8,
                           COLOR_NEON_RED);
      DrawRectRoundedLinesThick(screen->btn_supprimer_mode.bounds, 0.2f, 8, 2,
                                WHITE);
      DrawText("Supprimer", screen->btn_supprimer_mode.bounds.x + 5,
               screen->btn_supprimer_mode.bounds.y + 8, 10, WHITE);
    } else {
      DrawButton(&screen->btn_supprimer_mode);
    }
    x += 70 + BTN_GAP;

    screen->btn_stop.bounds = (Rectangle){x, BTN_Y, 50, 28};
    DrawButton(&screen->btn_stop);
    DrawText("STOP", x + 10, BTN_Y + 8, 10, WHITE);

  } else {
    // ========== N-ARY MODE LAYOUT ==========

    // Frame GENERATION: [Taille] [Prof] [Fils] [Type Dropdown] [Generer]
    int genX = 15;
    int genW = 460; // Widened to contain all
    Rectangle frameGen = {genX, FRAME_Y, genW, FRAME_H};
    bool hoverGen = CheckCollisionPointRec(mousePos, frameGen);
    if (hoverGen)
      DrawRectangleRounded(frameGen, 0.1f, 8, modeHover);
    DrawRectRoundedLinesThick(frameGen, 0.1f, 8, 2, modeColor);
    int titleW = MeasureText("GENERATION", 10);
    DrawRectangle(genX + 15, FRAME_Y - 2, titleW + 10, 5, COLOR_BG_DARK);
    DrawText("GENERATION", genX + 20, FRAME_Y - 5, 10, modeColor);

    // Position inputs inside frame
    int x = genX + 15;
    DrawText("Taille", x, BTN_Y + 5, 10, COLOR_TEXT_DIM);
    x += 38;
    screen->input_taille.bounds = (Rectangle){x, BTN_Y, 45, 26};
    DrawInputBox(&screen->input_taille);
    x += 45 + 10;

    DrawText("Prof", x, BTN_Y + 5, 10, COLOR_TEXT_DIM);
    x += 30;
    screen->input_profondeur.bounds = (Rectangle){x, BTN_Y, 40, 26};
    DrawInputBox(&screen->input_profondeur);
    x += 40 + 10;

    DrawText("Fils", x, BTN_Y + 5, 10, COLOR_TEXT_DIM);
    x += 25;
    screen->input_max_fils.bounds = (Rectangle){x, BTN_Y, 40, 26};
    DrawInputBox(&screen->input_max_fils);
    x += 40 + 10;

    // Dropdown Button
    Rectangle dropdownRect = {x, BTN_Y, 90, 26};
    DrawRectangleRounded(dropdownRect, 0.2f, 4, COLOR_PANEL);
    DrawRectRoundedLinesThick(dropdownRect, 0.2f, 4, 1, modeColor);
    const char *types[] = {"Int", "Float", "Char", "String"};
    DrawText(types[screen->selected_data_type], x + 10, BTN_Y + 7, 10,
             COLOR_TEXT_MAIN);
    DrawText("v", x + 75, BTN_Y + 7, 10, COLOR_TEXT_DIM);
    x += 90 + 15;

    screen->btn_generer.bounds = (Rectangle){x, BTN_Y, 75, 28};
    DrawButton(&screen->btn_generer);

    // Frame PARCOURS: [Prefixe] [Postfixe] [Larg] [- x +]  (NO Infixe for
    // N-ary!)
    int parcX = genX + genW + 20; // Reduced gap
    int parcW = 310;              // Wider to include speed controls
    Rectangle frameParc = {parcX, FRAME_Y, parcW, FRAME_H};
    bool hoverParc = CheckCollisionPointRec(mousePos, frameParc);
    if (hoverParc)
      DrawRectangleRounded(frameParc, 0.1f, 8, modeHover);
    DrawRectRoundedLinesThick(frameParc, 0.1f, 8, 2, modeColor);
    titleW = MeasureText("PARCOURS", 10);
    DrawRectangle(parcX + 15, FRAME_Y - 2, titleW + 10, 5, COLOR_BG_DARK);
    DrawText("PARCOURS", parcX + 20, FRAME_Y - 5, 10, modeColor);

    // Position buttons inside frame - NO Infixe in N-ary mode
    x = parcX + 15;
    screen->btn_preordre.bounds = (Rectangle){x, BTN_Y, 60, 28};
    DrawButton(&screen->btn_preordre);
    x += 60 + BTN_GAP;

    // Skip Infixe entirely for N-ary (not applicable)
    screen->btn_inordre.bounds = (Rectangle){-100, -100, 0, 0}; // Hide it

    screen->btn_postordre.bounds = (Rectangle){x, BTN_Y, 65, 28};
    DrawButton(&screen->btn_postordre);
    x += 65 + BTN_GAP;

    screen->btn_largeur.bounds = (Rectangle){x, BTN_Y, 50, 28};
    DrawButton(&screen->btn_largeur);
    x += 50 + BTN_GAP;

    // Speed controls inside frame
    screen->btn_speed_down.bounds = (Rectangle){x, BTN_Y, 25, 28};
    DrawButton(&screen->btn_speed_down);
    x += 25 + 3;
    char speedStr[16];
    snprintf(speedStr, sizeof(speedStr), "%.1f", screen->anim_speed);
    DrawText(speedStr, x, BTN_Y + 7, 10, COLOR_TEXT_MAIN);
    x += 22;
    screen->btn_speed_up.bounds = (Rectangle){x, BTN_Y, 25, 28};
    DrawButton(&screen->btn_speed_up);

    // Frame OPERATION: [Rechercher] [Modifier] [Supprimer] [STOP] [Convertir]
    int opX = parcX + parcW + 20;
    int opW = 405; // Widened for Modifier
    Rectangle frameOp = {opX, FRAME_Y, opW, FRAME_H};
    bool hoverOp = CheckCollisionPointRec(mousePos, frameOp);
    if (hoverOp)
      DrawRectangleRounded(frameOp, 0.1f, 8, modeHover);
    DrawRectRoundedLinesThick(frameOp, 0.1f, 8, 2, modeColor);
    titleW = MeasureText("OPERATION", 10);
    DrawRectangle(opX + 15, FRAME_Y - 2, titleW + 10, 5, COLOR_BG_DARK);
    DrawText("OPERATION", opX + 20, FRAME_Y - 5, 10, modeColor);

    x = opX + 15;
    screen->btn_recherche.bounds = (Rectangle){x, BTN_Y, 75, 28};
    DrawButton(&screen->btn_recherche);
    x += 75 + BTN_GAP;

    screen->btn_modifier_mode.bounds = (Rectangle){x, BTN_Y, 70, 28};
    if (screen->is_modify_mode) {
      DrawRectangleRounded(screen->btn_modifier_mode.bounds, 0.2f, 8,
                           COLOR_NEON_ORANGE);
      DrawRectRoundedLinesThick(screen->btn_modifier_mode.bounds, 0.2f, 8, 2,
                                WHITE);
      DrawText("Modifier", screen->btn_modifier_mode.bounds.x + 8,
               screen->btn_modifier_mode.bounds.y + 8, 10, WHITE);
    } else {
      DrawButton(&screen->btn_modifier_mode);
    }
    x += 70 + BTN_GAP;

    screen->btn_supprimer_mode.bounds = (Rectangle){x, BTN_Y, 70, 28};
    if (screen->is_delete_mode) {
      DrawRectangleRounded(screen->btn_supprimer_mode.bounds, 0.2f, 8,
                           COLOR_NEON_RED);
      DrawRectRoundedLinesThick(screen->btn_supprimer_mode.bounds, 0.2f, 8, 2,
                                WHITE);
      DrawText("Supprimer", screen->btn_supprimer_mode.bounds.x + 5,
               screen->btn_supprimer_mode.bounds.y + 8, 10, WHITE);
    } else {
      DrawButton(&screen->btn_supprimer_mode);
    }
    x += 70 + BTN_GAP;

    screen->btn_stop.bounds = (Rectangle){x, BTN_Y, 50, 28};
    DrawButton(&screen->btn_stop);
    DrawText("STOP", x + 10, BTN_Y + 8, 10,
             WHITE); // Force draw text if button texture fails
    x += 50 + BTN_GAP;

    // Conversion button in N-ary mode
    screen->btn_convertir.bounds = (Rectangle){x, BTN_Y, 90, 28};
    if (screen->arbre.racine && !screen->is_converting) {
      DrawButton(&screen->btn_convertir);
    }
  }

  // VIDER button is now at top, not here.
  // screen->btn_vider.bounds = ...;
  // DrawButton(&screen->btn_vider);

  // Calculate push offset for conversion animation
  float push_offset = 0;
  if (screen->is_converting) {
    push_offset = -screen->convert_progress *
                  (WINDOW_WIDTH + 100); // Slide left off screen
  }

  // Tree area (starts at y=90) - apply push offset during conversion
  Rectangle treeRect = {10 + push_offset, 90 + NAVBAR_HEIGHT, WINDOW_WIDTH - 20,
                        WINDOW_HEIGHT - 130 - NAVBAR_HEIGHT};
  DrawRectangleRec(treeRect, COLOR_BG_DARK);

  // Begin scissor mode to clip tree/grid to the treeRect area
  BeginScissorMode((int)treeRect.x, (int)treeRect.y, (int)treeRect.width,
                   (int)treeRect.height);

  DrawTreeGrid(screen, treeRect);

  // Collect node positions for click detection (with screen coordinates)

  // Collect node positions for click detection (with screen coordinates)
  screen->node_count = 0;
  if (screen->arbre.racine) {
    CollectNodePositionsWithOffset(screen->arbre.racine, screen);
  }

  // Draw tree with camera offset OR clickable RACINE node prompt
  if (screen->arbre.racine) {
    DrawEdgesWithOffset(screen->arbre.racine, screen);
    DrawNodesWithOffset(screen->arbre.racine, screen, screen->highlighted_node,
                        screen->popup_parent);
  } else if (screen->show_root_input) {
    // Draw clickable RACINE node centered in tree area
    float cx = treeRect.x + treeRect.width / 2;
    float cy = treeRect.y + treeRect.height / 2 - 30;
    float radius = NODE_DIAM / 2;

    // Draw the RACINE node (same style as normal nodes)
    Color nodeColor = COLOR_NEON_ORANGE;
    DrawCircle((int)cx, (int)cy, radius + 3, nodeColor);
    DrawCircle((int)cx, (int)cy, radius, COLOR_PANEL);

    // Draw "RACINE" text inside
    const char *racineText = "RACINE";
    int textW = MeasureText(racineText, 14);
    DrawText(racineText, (int)cx - textW / 2, (int)cy - 7, 14, nodeColor);

    // Animated arrow pointing to the node (bouncing up and down)
    float arrowOffset = sinf(GetTime() * 4.0f) * 8.0f; // Oscillate ±8 pixels
    float arrowY = cy - radius - 30 + arrowOffset;
    DrawTriangle((Vector2){cx, arrowY + 20}, // Point (bottom)
                 (Vector2){cx - 10, arrowY}, // Top left
                 (Vector2){cx + 10, arrowY}, // Top right
                 COLOR_NEON_GREEN);

    // Draw "Cliquez pour commencer" label below
    const char *label = "Cliquez pour commencer";
    int labelW = MeasureText(label, 14);
    DrawText(label, (int)cx - labelW / 2, (int)cy + radius + 20, 14,
             COLOR_NEON_GREEN);

    // Alternative text
    const char *altLabel = "ou utilisez 'Generer' pour un arbre aleatoire";
    int altW = MeasureText(altLabel, 11);
    DrawText(altLabel, (int)cx - altW / 2, (int)cy + radius + 45, 11,
             COLOR_TEXT_DIM);
  } else {
    DrawText("Cliquez 'Generer' pour creer un arbre", treeRect.x + 100,
             treeRect.y + treeRect.height / 2, 16, COLOR_TEXT_DIM);
  }

  // End scissor mode
  EndScissorMode();

  // Draw frame border (after scissor mode so it's not clipped)
  DrawRectangleLinesEx(treeRect, 1, COLOR_BORDER);

  // Draw conversion animation overlay (clean, professional style)
  if (screen->is_converting) {
    // "CONVERSION" text (centered, fixed position)
    DrawText("CONVERSION N-AIRE -> BINAIRE", WINDOW_WIDTH / 2 - 180, 100, 24,
             COLOR_NEON_GREEN);

    // Progress bar (fixed position)
    DrawRectangleRounded((Rectangle){WINDOW_WIDTH / 2 - 150, 140, 300, 20},
                         0.5f, 8, COLOR_PANEL);
    DrawRectangleRounded((Rectangle){WINDOW_WIDTH / 2 - 150, 140,
                                     300 * screen->convert_progress, 20},
                         0.5f, 8, COLOR_NEON_GREEN);

    // Percentage text
    char percent_text[16];
    snprintf(percent_text, sizeof(percent_text), "%.0f%%",
             screen->convert_progress * 100);
    int textW = MeasureText(percent_text, 16);
    DrawText(percent_text, WINDOW_WIDTH / 2 - textW / 2, 142, 16, WHITE);
    DrawText(percent_text, WINDOW_WIDTH / 2 - textW / 2, 142, 16, WHITE);
  }

  // Search Popup
  if (screen->popup_search_open) {
    float px = WINDOW_WIDTH / 2 - 120;
    float py = WINDOW_HEIGHT / 2 - 60;

    DrawRectangleRounded((Rectangle){px, py, 240, 110}, 0.1f, 8, COLOR_PANEL);
    DrawRectRoundedLinesThick((Rectangle){px, py, 240, 110}, 0.1f, 8, 2,
                              COLOR_NEON_BLUE);
    DrawText("RECHERCHE", px + 80, py + 10, 14, COLOR_NEON_BLUE);

    Rectangle inputRect = {px + 20, py + 35, 200, 30};
    DrawRectangleRec(inputRect, COLOR_BG_DARK);
    DrawRectangleLinesEx(inputRect, 1, COLOR_BORDER);
    DrawText(screen->search_text, inputRect.x + 5, inputRect.y + 8, 14,
             COLOR_TEXT_WHITE);

    // Cursor
    if ((int)(GetTime() * 2) % 2 == 0) {
      int cursorX = inputRect.x + 5 + MeasureText(screen->search_text, 14);
      DrawRectangle(cursorX, inputRect.y + 5, 2, 20, COLOR_NEON_BLUE);
    }

    DrawButton(&screen->popup_btn_chercher);
    DrawButton(&screen->popup_btn_search_annuler);
  }

  // Traversal result popup
  if (screen->show_result_popup) {
    float px = WINDOW_WIDTH / 2 - 300;
    float py = WINDOW_HEIGHT / 2 - 150;
    float pw = 600;
    float ph = 300;

    // Background
    DrawRectangleRounded((Rectangle){px, py, pw, ph}, 0.05f, 8, COLOR_PANEL);
    DrawRectRoundedLinesThick((Rectangle){px, py, pw, ph}, 0.05f, 8, 2,
                              COLOR_NEON_GREEN);

    // Title
    const char *title;
    if (strncmp(screen->result_text, "La valeur", 9) == 0) {
      title = "RESULTAT RECHERCHE";
    } else {
      title = PARCOURS_NOMS[screen->result_type];
    }
    int titleW = MeasureText(title, 24);
    DrawText(title, WINDOW_WIDTH / 2 - titleW / 2, py + 15, 24,
             COLOR_NEON_GREEN);

    // Subtitle
    DrawText("Resultat du parcours:", px + 20, py + 50, 16, COLOR_TEXT_DIM);

    // Result text - wrap if needed
    Rectangle textArea = {px + 20, py + 75, pw - 40, ph - 130};
    DrawRectangleRec(textArea, COLOR_BG_DARK);
    DrawRectangleLinesEx(textArea, 1, COLOR_BORDER);

    // Draw result text with wrapping
    BeginScissorMode((int)textArea.x, (int)textArea.y, (int)textArea.width,
                     (int)textArea.height);

    // Wrapping logic
    char wrappedText[4096];
    wrappedText[0] = '\0';
    int currentLineWidth = 0;
    int maxW = (int)textArea.width - 20;

    // Duplicate string for strtok
    char *dup = (char *)malloc(strlen(screen->result_text) + 1);
    strcpy(dup, screen->result_text);

    char *token = strtok(dup, " ");
    while (token != NULL) {
      int wordW = MeasureText(token, 14);
      int spaceW = MeasureText(" ", 14);

      if (currentLineWidth + wordW > maxW && currentLineWidth > 0) {
        strcat(wrappedText, "\n");
        currentLineWidth = 0;
      }

      if (currentLineWidth > 0) {
        strcat(wrappedText, " ");
        currentLineWidth += spaceW;
      }

      strcat(wrappedText, token);
      currentLineWidth += wordW;

      token = strtok(NULL, " ");
    }
    free(dup);

    DrawText(wrappedText, (int)textArea.x + 10, (int)textArea.y + 10, 14,
             COLOR_TEXT_MAIN);
    EndScissorMode();

    // OK button
    Rectangle okBtn = {WINDOW_WIDTH / 2 - 50, py + ph - 45, 100, 35};
    bool hoverOk = CheckCollisionPointRec(GetMousePosition(), okBtn);
    DrawRectangleRounded(okBtn, 0.3f, 8,
                         hoverOk ? COLOR_NEON_GREEN : COLOR_PANEL);
    DrawRectRoundedLinesThick(okBtn, 0.3f, 8, 2, COLOR_NEON_GREEN);
    int okW = MeasureText("OK", 18);
    DrawText("OK", (int)(okBtn.x + okBtn.width / 2 - okW / 2),
             (int)(okBtn.y + 8), 18, hoverOk ? BLACK : COLOR_NEON_GREEN);

    // Handle click to close
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoverOk) {
      screen->show_result_popup = false;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
      screen->show_result_popup = false;
    }
  }

  // Popup dialog
  if (screen->popup_open) {
    // Force update bounds in Draw as well (immediate mode style)
    float px = (float)GetScreenWidth() / 2 - 120;
    float py = (float)GetScreenHeight() / 2 - 60;

    // Update button positions for drawing
    if (screen->popup_parent == NULL) {
      screen->popup_btn_ajouter.bounds = (Rectangle){px + 40, py + 80, 80, 30};
      screen->popup_btn_annuler.bounds = (Rectangle){px + 140, py + 80, 70, 30};
    } else if (screen->is_binary) {
      screen->popup_btn_gauche.bounds = (Rectangle){px + 10, py + 80, 70, 30};
      screen->popup_btn_droite.bounds = (Rectangle){px + 90, py + 80, 70, 30};
      screen->popup_btn_annuler.bounds = (Rectangle){px + 170, py + 80, 70, 30};
    } else {
      screen->popup_btn_ajouter.bounds = (Rectangle){px + 40, py + 80, 80, 30};
      screen->popup_btn_annuler.bounds = (Rectangle){px + 130, py + 80, 70, 30};
    }

    DrawRectangleRounded((Rectangle){px, py, 240, 120}, 0.1f, 8, COLOR_PANEL);
    DrawRectRoundedLinesThick((Rectangle){px, py, 240, 120}, 0.1f, 8, 2,
                              COLOR_NEON_ORANGE);

    // Input box
    Rectangle inputRect = {px + 20, py + 35, 200, 30};
    DrawRectangleRec(inputRect, COLOR_BG_DARK);
    DrawRectangleLinesEx(inputRect, 1, COLOR_BORDER);
    DrawText(screen->popup_text, inputRect.x + 5, inputRect.y + 8, 14,
             COLOR_TEXT_WHITE);

    // Cursor blink
    if ((int)(GetTime() * 2) % 2 == 0) {
      int cursorX = inputRect.x + 5 + MeasureText(screen->popup_text, 14);
      DrawRectangle(cursorX, inputRect.y + 5, 2, 20, COLOR_NEON_ORANGE);
    }

    // Special case: creating root (popup_parent is NULL)
    if (screen->popup_is_modify) {
      DrawText("MODIFIER NOEUD", px + 40, py + 10, 14, COLOR_NEON_ORANGE);
      DrawButton(&screen->popup_btn_valider_modif);
      DrawButton(&screen->popup_btn_annuler);
    } else if (screen->popup_parent == NULL) {
      DrawText("CREER LA RACINE", px + 50, py + 10, 14, COLOR_NEON_GREEN);
      DrawButton(&screen->popup_btn_ajouter);
      DrawButton(&screen->popup_btn_annuler);
    } else if (screen->is_binary) {
      DrawText("AJOUTER UN NOEUD", px + 40, py + 10, 14, COLOR_TEXT_MAIN);
      bool has_left = screen->popup_parent->child_count > 0 &&
                      screen->popup_parent->children[0] != NULL;
      bool has_right = screen->popup_parent->child_count > 1 &&
                       screen->popup_parent->children[1] != NULL;

      if (has_left)
        screen->popup_btn_gauche.color = COLOR_PANEL;
      else
        screen->popup_btn_gauche.color = COLOR_NEON_BLUE;

      if (has_right)
        screen->popup_btn_droite.color = COLOR_PANEL;
      else
        screen->popup_btn_droite.color = COLOR_NEON_BLUE;

      DrawButton(&screen->popup_btn_gauche);
      DrawButton(&screen->popup_btn_droite);
    } else {
      DrawText("AJOUTER UN NOEUD", px + 40, py + 10, 14, COLOR_TEXT_MAIN);
      DrawButton(&screen->popup_btn_ajouter);
    }
    DrawButton(&screen->popup_btn_annuler);
  }

  // Status bar
  char infoStr[128];
  snprintf(infoStr, sizeof(infoStr), "Noeuds: %d | Zoom: %.0f%%",
           screen->arbre.taille, screen->zoom * 100);
  DrawText(infoStr, 20, WINDOW_HEIGHT - 30, 14, COLOR_TEXT_MAIN);

  if (screen->status_timer > 0) {
    float alpha = fminf(1.0f, screen->status_timer);
    Color msgColor = COLOR_NEON_GREEN;
    msgColor.a = (unsigned char)(alpha * 255);
    DrawText(screen->status_message, WINDOW_WIDTH - 300, WINDOW_HEIGHT - 30, 14,
             msgColor);
  }

  DrawText("Molette: zoom | Clic droit: deplacer | Clic sur noeud: ajouter",
           WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 30, 12, COLOR_TEXT_DIM);
  // Dropdown Options (Must be drawn last to float over everything)
  if (screen->dropdown_open) {
    Rectangle dropdownRect;
    if (screen->is_binary) {
      float x = 15 + 15 + 38 + 55 + 10 + 30 + 50 + 10;
      dropdownRect = (Rectangle){x, 52 + NAVBAR_HEIGHT, 90, 26};
    } else {
      float x = 15 + 15 + 38 + 45 + 10 + 30 + 40 + 10 + 25 + 40 + 10;
      dropdownRect = (Rectangle){x, 52 + NAVBAR_HEIGHT, 90, 26};
    }

    int optsCount = 4;
    const char *opts[] = {"Int", "Float", "Char", "String"};
    for (int i = 0; i < optsCount; i++) {
      Rectangle optRect = {dropdownRect.x,
                           dropdownRect.y + dropdownRect.height + i * 26,
                           dropdownRect.width, 26};
      DrawRectangleRec(optRect, COLOR_SECONDARY);
      DrawRectangleLinesEx(optRect, 1, COLOR_BORDER);
      bool hover = CheckCollisionPointRec(GetMousePosition(), optRect);
      if (hover) {
        DrawRectangleRec(optRect, (Color){80, 180, 255, 40});
      }
      DrawText(opts[i], optRect.x + 10, optRect.y + 7, 10, COLOR_TEXT_MAIN);
    }
  }
} // End ArbresDrawTree

void ArbresUnload(ArbresScreen *screen) {
  if (screen->arbre.racine) {
    DetruireArbre(screen->arbre.racine);
    screen->arbre.racine = NULL;
  }
  if (screen->parcours_result) {
    free(screen->parcours_result);
    screen->parcours_result = NULL;
  }
}
