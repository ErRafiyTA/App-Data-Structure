/**
 * LISTES.H - Module Listes Chaînées
 */

#ifndef LISTES_H
#define LISTES_H

#include "../theme.h"
#include "../ui_common.h"
#include <stdbool.h>

// ==================== STRUCTURES DE DONNÉES ====================

// Nœud liste simplement chaînée
// Nœud liste simplement chaînée
typedef struct NodeSimple {
  char data[32];
  struct NodeSimple *next;
} NodeSimple;

// Liste simplement chaînée
typedef struct {
  NodeSimple *head;
  int count;
} ListeSimple;

// Nœud liste doublement chaînée
typedef struct NodeDouble {
  char data[32];
  struct NodeDouble *prev;
  struct NodeDouble *next;
} NodeDouble;

// Liste doublement chaînée
typedef struct {
  NodeDouble *head;
  NodeDouble *tail;
  int count;
} ListeDouble;

// ==================== OPÉRATIONS LISTE SIMPLE ====================

void ListeSimple_Init(ListeSimple *liste);
void ListeSimple_Free(ListeSimple *liste);

void ListeSimple_InsererDebut(ListeSimple *liste, const char *valeur);
void ListeSimple_InsererFin(ListeSimple *liste, const char *valeur);
void ListeSimple_InsererPosition(ListeSimple *liste, const char *valeur,
                                 int pos);

bool ListeSimple_SupprimerDebut(ListeSimple *liste);
bool ListeSimple_SupprimerFin(ListeSimple *liste);
bool ListeSimple_SupprimerPosition(ListeSimple *liste, int pos);
bool ListeSimple_SupprimerValeur(ListeSimple *liste, const char *valeur);

NodeSimple *ListeSimple_Rechercher(ListeSimple *liste, const char *valeur);
int ListeSimple_GetPosition(ListeSimple *liste, const char *valeur);
void ListeSimple_Vider(ListeSimple *liste);

// ==================== OPÉRATIONS LISTE DOUBLE ====================

void ListeDouble_Init(ListeDouble *liste);
void ListeDouble_Free(ListeDouble *liste);

void ListeDouble_InsererDebut(ListeDouble *liste, const char *valeur);
void ListeDouble_InsererFin(ListeDouble *liste, const char *valeur);
void ListeDouble_InsererPosition(ListeDouble *liste, const char *valeur,
                                 int pos);

bool ListeDouble_SupprimerDebut(ListeDouble *liste);
bool ListeDouble_SupprimerFin(ListeDouble *liste);
bool ListeDouble_SupprimerPosition(ListeDouble *liste, int pos);
bool ListeDouble_SupprimerValeur(ListeDouble *liste, const char *valeur);

NodeDouble *ListeDouble_Rechercher(ListeDouble *liste, const char *valeur);
void ListeDouble_Vider(ListeDouble *liste);

// ==================== ÉCRAN LISTES ====================

typedef enum { LISTE_TYPE_SIMPLE, LISTE_TYPE_DOUBLE } ListeType;

// Animation types
typedef enum {
  ANIM_NONE = 0,
  ANIM_INSERT_START,
  ANIM_INSERT_END,
  ANIM_INSERT_POS,
  ANIM_DELETE_START,
  ANIM_DELETE_END,
  ANIM_DELETE_VAL,
  ANIM_DELETE_POS,
  ANIM_SEARCH,
  ANIM_SORT // Tri animé
} ListeAnimType;

typedef struct {
  // Type de liste actuel
  ListeType type;

  // Listes
  ListeSimple liste_simple;
  ListeDouble liste_double;

  // UI - Navigation
  Button btn_back;
  Button btn_type_simple;
  Button btn_type_double;

  // UI - Insertion (groupe vert)
  Button btn_inserer_debut;
  Button btn_inserer_fin;
  Button btn_inserer_position;

  // UI - Suppression (groupe rouge)
  Button btn_supprimer_debut;
  Button btn_supprimer_fin;
  Button btn_supprimer_valeur;
  Button btn_supprimer_position;

  // UI - Recherche (bleu) - 2 inputs séparés avec titres
  // UI - Recherche (bleu) - boutons + inputs
  Button btn_rechercher_val;
  InputBox input_recherche_val;
  Button btn_rechercher_pos;
  InputBox input_recherche_pos;

  // UI - Tri (violet) - boutons verticaux + stop
  Button btn_tri_insertion; // Tri insertion (<=10)
  Button btn_tri_quicksort; // QuickSort (>10)
  Button btn_stop_tri;      // Arrêter le tri

  // UI - Génération aléatoire (cyan)
  Button btn_generer_aleatoire;
  InputBox input_taille_aleatoire;
  int random_data_type; // 0=int, 1=float, 2=char, 3=string
  bool type_dropdown_open;

  // UI - Vider
  Button btn_vider;

  // Inputs pour INSERTION (avec placeholder)
  InputBox input_position_insert;

  // Inputs pour SUPPRESSION (avec placeholder)
  InputBox input_valeur_delete;
  InputBox input_position_delete;

  // Nœud d'entrée (dans la zone de visualisation)
  char input_node_text[32];
  bool input_node_active;
  float input_node_x;
  float input_node_y;

  // Nœud surligné (recherche)
  int highlighted_index;
  float highlight_timer;

  // Scroll vertical
  float scroll_y;
  float target_scroll_y;

  // Animation
  ListeAnimType anim_type;
  float anim_progress;
  int anim_target_index;
  int anim_value;

  // État du tri animé
  bool sort_running;
  int sort_i;
  int sort_pass;
  int sort_total;
  bool sort_swapped;
  int swap_idx1;
  int swap_idx2;
  float swap_progress;
  bool swap_animating;
  float sort_progress; // 0.0 to 1.0 progress percentage for progress bar

  // État QuickSort
  bool quicksort_running;
  int qs_stack[256];
  int qs_stack_top;
  int qs_low;
  int qs_high;
  int qs_pivot_idx;
  int qs_i;
  int qs_j;
  int qs_phase;
  int qs_partitions_done;
  int qs_total_partitions;

  // Popup résultat
  bool popup_visible;
  char popup_title[64];
  char popup_message[256];
  float popup_timer;

  // Message status
  char status_message[128];
  float status_timer;

  // Animation Recherche "Personnage"
  bool search_active;
  char search_target_val[32]; // Valeur cherchée
  int search_current_node_idx;
  Vector2 search_char_pos;
  float search_jump_progress; // 0.0 -> 1.0
  bool search_victory;        // Pose victoire
  float search_victory_timer;

  // Caméra pour Zoom
  Camera2D camera;
  float target_zoom;
  Vector2 target_offset;

  bool search_mode_is_pos; // True = Recherche par Position, False = Par Valeur

  // Manuel Popup (création liste vide)
  Button btn_manual;
  bool manual_popup_active;
  InputBox manual_size_input;
  int manual_type_selected; // 0=Int, 1=Float, 2=Char, 3=Str

  // Inline node editing
  int editing_node_index;     // Index of node being edited, -1 if none
  char editing_node_text[64]; // Text buffer for editing
  bool editing_node_active;   // True if editing mode is active

} ListesScreen;

// Fonctions écran
void ListesInit(ListesScreen *screen);
AppScreen ListesUpdate(ListesScreen *screen);
void ListesDraw(ListesScreen *screen);
void ListesUnload(ListesScreen *screen);

#endif // LISTES_H
