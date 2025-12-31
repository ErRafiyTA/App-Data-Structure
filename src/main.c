/**
 * MAIN.C - Point d'entrée de l'application
 * Structures de Données Premium - Raylib Edition
 */

#include "arbres/arbres.h"
#include "graphes/graphes.h"
#include "listes/listes.h"
#include "raylib.h"
#include "tableaux/tableaux.h"
#include "theme.h"
#include "ui_common.h"
#include "welcome.h"
#include <stddef.h>

// ==================== ÉTAT GLOBAL ====================

typedef struct {
  AppScreen current_screen;
  AppScreen next_screen;
  float transition_alpha;
  bool transitioning;

  // Écrans
  WelcomeScreen welcome;
  TableauxScreen tableaux;
  ListesScreen listes;
  ArbresScreen arbres;
  GraphesScreen graphes;
} AppState;

static AppState app;

// ==================== TRANSITIONS ====================

static void InitScreen(AppScreen screen) {
  switch (screen) {
  case SCREEN_WELCOME:
    WelcomeInit(&app.welcome);
    break;
  case SCREEN_TABLEAUX:
    TableauxInit(&app.tableaux);
    break;
  case SCREEN_LISTES:
    ListesInit(&app.listes);
    break;
  case SCREEN_ARBRES:
    ArbresInit(&app.arbres);
    break;
  case SCREEN_GRAPHES:
    GraphesInit(&app.graphes);
    break;
  }
}

static void StartTransition(AppScreen next) {
  if (app.current_screen != next) {
    app.next_screen = next;
    app.transitioning = true;
    app.transition_alpha = 0.0f;

    // Pré-initialiser l'écran cible AVANT la transition
    // Cela évite le problème de buffering GPU
    InitScreen(next);
  }
}

static void UpdateTransition(void) {
  if (!app.transitioning)
    return;

  float dt = GetFrameTime();

  // Phase 1: Fade Out (Ecran actuel vers Noir)
  if (app.current_screen != app.next_screen) {
    app.transition_alpha += dt * 4.0f; // Vitesse de fade

    if (app.transition_alpha >= 1.0f) {
      app.transition_alpha = 1.0f;
      // Changement d'écran effectif
      app.current_screen = app.next_screen;
    }
  }
  // Phase 2: Fade In (Noir vers Nouvel écran)
  else {
    app.transition_alpha -= dt * 4.0f; // Vitesse de fade

    if (app.transition_alpha <= 0.0f) {
      app.transition_alpha = 0.0f;
      app.transitioning = false; // Fin de la transition
    }
  }
}

static void DrawTransition(void) {
  if (!app.transitioning)
    return;

  unsigned char alpha = (unsigned char)(app.transition_alpha * 255);
  DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){5, 8, 15, alpha});
}

// ==================== MAIN ====================

int main(void) {
  // Initialisation Raylib
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
             "Structures de Donnees - Premium Edition");
  SetTargetFPS(TARGET_FPS);

  // Police par défaut de Raylib
  // InitCustomFont("src/fonts/Roboto-Regular.ttf", 32);

  // Initialisation de l'état
  app.current_screen = SCREEN_WELCOME;
  app.next_screen = SCREEN_WELCOME;
  app.transition_alpha = 0;
  app.transitioning = false;

  // Initialiser l'écran d'accueil
  WelcomeInit(&app.welcome);

  // Boucle principale
  while (!WindowShouldClose()) {
    // ==================== UPDATE ====================

    // Mise à jour selon l'écran actuel
    if (!app.transitioning) {
      AppScreen requested = app.current_screen;

      switch (app.current_screen) {
      case SCREEN_WELCOME:
        requested = WelcomeUpdate(&app.welcome);
        break;
      case SCREEN_TABLEAUX:
        requested = TableauxUpdate(&app.tableaux);
        break;
      case SCREEN_LISTES:
        requested = ListesUpdate(&app.listes);
        break;
      case SCREEN_ARBRES:
        requested = ArbresUpdate(&app.arbres);
        break;
      case SCREEN_GRAPHES:
        requested = GraphesUpdate(&app.graphes);
        break;
      }

      if (requested != app.current_screen) {
        StartTransition(requested);
      }
    }

    UpdateTransition();

    // ==================== DRAW ====================

    BeginDrawing();
    ClearBackground(COLOR_BG_DARK);

    switch (app.current_screen) {
    case SCREEN_WELCOME:
      WelcomeDraw(&app.welcome);
      break;
    case SCREEN_TABLEAUX:
      TableauxDraw(&app.tableaux);
      break;
    case SCREEN_LISTES:
      ListesDraw(&app.listes);
      break;
    case SCREEN_ARBRES:
      ArbresDrawTree(&app.arbres);
      break;
    case SCREEN_GRAPHES:
      GraphesDraw(&app.graphes);
      break;
    }

    // Overlay de transition
    DrawTransition();

    // FPS (debug)
    DrawFPS(WINDOW_WIDTH - 90, 10);

    EndDrawing();
  }

  // Nettoyage
  WelcomeUnload(&app.welcome);
  TableauxUnload(&app.tableaux);
  ListesUnload(&app.listes);
  ArbresUnload(&app.arbres);
  GraphesUnload(&app.graphes);
  UnloadCustomFont();

  CloseWindow();

  return 0;
}
