/**
 * ARBRES.H - Module Arbres Binaires et N-aires
 */

#ifndef ARBRES_H
#define ARBRES_H

#include "../theme.h"
#include "../ui_common.h"
#include <stdbool.h>

// ==================== CONSTANTES ====================
#define MAX_CHILDREN 10
#define NODE_DIAM 50
#define H_SPACING 50
#define V_SPACING 100

// ==================== STRUCTURES DE DONNÉES ====================

// Nœud générique (supporte binaire et N-aire)
typedef struct NoeudGenerique {
  char data[32];
  struct NoeudGenerique *children[MAX_CHILDREN];
  int child_count;
  // Layout (calculated)
  float x, y;
  float subtree_width;
} NoeudGenerique;

// Arbre générique
typedef struct {
  NoeudGenerique *racine;
  int taille;
} ArbreGenerique;

// ==================== PARCOURS ====================

typedef enum {
  PARCOURS_PREORDRE,
  PARCOURS_INORDRE,
  PARCOURS_POSTORDRE,
  PARCOURS_LARGEUR
} TypeParcours;

extern const char *PARCOURS_NOMS[4];

// ==================== ÉCRAN ARBRES ====================

typedef struct {
  // Arbre
  ArbreGenerique arbre;
  bool is_binary; // true = binaire (max 2), false = N-aire

  // UI - Mode toggle
  Button btn_back;
  Button btn_mode_binaire;
  Button btn_mode_naire;
  Button btn_ordonner; // [NEW] Reorder tree as BST

  // UI - Generation
  InputBox input_taille;
  InputBox input_profondeur;
  InputBox input_max_fils; // N-ary only
  Button btn_generer;

  // Data Type Dropdown
  int selected_data_type; // 0=Int, 1=Float, 2=Char, 3=String
  bool dropdown_open;

  Button btn_supprimer_mode; // Activate deletion mode
  bool is_delete_mode;
  Button btn_modifier_mode; // Activate modification mode
  bool is_modify_mode;
  Button btn_recherche;
  bool popup_search_open;
  char search_text[32];
  int search_cursor;
  Button popup_btn_chercher;
  Button popup_btn_search_annuler;
  Button btn_vider;

  // UI - Traversal
  Button btn_preordre;
  Button btn_inordre;
  Button btn_postordre;
  Button btn_largeur;
  Button btn_stop;
  Button btn_speed_down;
  Button btn_speed_up;
  float anim_speed;

  // UI - N-ary to Binary conversion (N-ary only)
  Button btn_convertir;
  bool is_converting;
  float convert_progress; // 0.0 to 1.0
  int mario_anim_frame;
  Texture2D mario_texture; // Mario sprite loaded from file

  // Popup dialog for adding node
  bool popup_open;
  char popup_text[32];
  int popup_cursor;
  NoeudGenerique *popup_parent;
  Button popup_btn_gauche;
  Button popup_btn_droite;
  Button popup_btn_ajouter;
  Button popup_btn_annuler;
  // Modification popup
  bool popup_is_modify;
  NoeudGenerique *popup_modify_target;
  Button popup_btn_valider_modif;

  // Animation state
  NoeudGenerique **parcours_result;
  int parcours_size;
  int parcours_current_idx;
  float parcours_timer;
  bool parcours_animating;
  TypeParcours parcours_type;

  // Highlighted node
  NoeudGenerique *highlighted_node;

  // Node positions for click detection
  struct {
    NoeudGenerique *node;
    float x, y;
  } node_positions[500];
  int node_count;

  // Zoom and pan
  float zoom;
  Vector2 offset;
  Vector2 drag_start;
  bool dragging;

  // Status
  char status_message[128];
  float status_timer;

  // Traversal result popup
  bool show_result_popup;
  char result_text[512];
  TypeParcours result_type;

  // Root input state (for initial tree creation)
  bool show_root_input;
  char root_input_text[32];
  int root_input_cursor;

  // Search state
  bool is_searching;
  bool search_found;

} ArbresScreen;

// ==================== FONCTIONS ====================

void ArbresInit(ArbresScreen *screen);
AppScreen ArbresUpdate(ArbresScreen *screen);
void ArbresDrawTree(ArbresScreen *screen);
void ArbresUnload(ArbresScreen *screen);

// Helpers
NoeudGenerique *CreerNoeudGen(const char *data);
void DetruireArbre(NoeudGenerique *node);
void CalculerLayout(NoeudGenerique *node, float x_start, int depth,
                    bool is_binary);
void GenererArbreAleatoire(ArbresScreen *screen, int taille, int profondeur,
                           int max_fils);

#endif // ARBRES_H
