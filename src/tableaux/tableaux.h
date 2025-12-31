/**
 * TABLEAUX.H - Interface du module Tableaux (Version améliorée)
 */

#ifndef TABLEAUX_H
#define TABLEAUX_H

#include "../theme.h"
#include "../ui_common.h"
#include "sorting.h"

// ==================== ÉTAT DU MODULE ====================

typedef enum {
  TAB_STATE_IDLE,     // Attente de données
  TAB_STATE_HAS_DATA, // Données générées, prêt à trier
  TAB_STATE_SORTING,  // Tri en cours
  TAB_STATE_DONE      // Tri terminé
} TableauxState;

// Mode de saisie
typedef enum {
  INPUT_MODE_NONE,
  INPUT_MODE_RANDOM,
  INPUT_MODE_MANUAL
} InputMode;

// Historique pour le graphique
typedef struct {
  int sizes[20];
  double times[SORT_COUNT][20];
  int count;
} PerformanceHistory;

// Structure principale du module
typedef struct {
  // État
  TableauxState state;
  DataType data_type; // Type de données actuel
  InputMode input_mode;

  // Données génériques
  GenericData *original_data; // Données originales
  GenericData
      *display_sorted; // Données triées pour affichage (algo sélectionné)
  int data_size;

  // Gestionnaire de tri parallèle
  SortManager sort_manager;
  int selected_sort;        // Algorithme sélectionné (-1 = tous)
  double global_start_time; // Temps de départ global
  double elapsed_time;      // Temps écoulé global

  // Timers individuels par algorithme
  double sort_start_times[SORT_COUNT];
  double sort_elapsed[SORT_COUNT];
  bool sort_running[SORT_COUNT];
  bool sort_done[SORT_COUNT];
  double sort_times[SORT_COUNT];

  // Progress tracking pour graphique temps réel
  double sort_progress_times[SORT_COUNT][100]; // Historique du temps écoulé
  float sort_progress_values[SORT_COUNT][100]; // Progression 0.0 à 1.0
  int sort_progress_count[SORT_COUNT];         // Nombre de points enregistrés

  // UI - Boutons principaux
  Button btn_back;
  Button btn_generate; // Génération aléatoire
  Button btn_manual;   // Saisie manuelle
  Button btn_sort_all; // Lancer tous les tris
  Button btn_stop;     // Arrêter les tris
  Button btn_graph;    // Afficher graphique
  Button btn_save;     // Sauvegarder résultats

  // UI - Dropdown type de données
  Button btn_type_dropdown; // Bouton principal du dropdown
  bool type_dropdown_open;  // État ouvert/fermé du dropdown

  // UI - Boutons tri individuel
  Button btn_sort[SORT_COUNT];

  // UI - Input
  InputBox input_size;

  // Mode saisie manuelle
  bool manual_input_active;
  int manual_current_index; // Index en cours d'édition
  int manual_filled_count;  // Nombre total de champs remplis
  int manual_target_size;
  InputBox manual_input;
  long long *manual_int_values;
  double *manual_float_values;
  char *manual_char_values;
  char **manual_string_values;
  float manual_scroll_y; // Scroll position for manual input fields
  char status_message[128];
  float status_timer;

  // Graphique performance
  PerformanceHistory history;
  bool show_graph;

  // Scroll pour affichage des données
  float scroll_before_y; // Scroll zone "Avant"
  float scroll_after_y;  // Scroll zone "Après"

} TableauxScreen;

// ==================== FONCTIONS ====================

// Initialise l'écran
void TableauxInit(TableauxScreen *screen);

// Met à jour l'écran (retourne l'écran suivant)
AppScreen TableauxUpdate(TableauxScreen *screen);

// Dessine l'écran
void TableauxDraw(TableauxScreen *screen);

// Libère les ressources
void TableauxUnload(TableauxScreen *screen);

#endif // TABLEAUX_H
