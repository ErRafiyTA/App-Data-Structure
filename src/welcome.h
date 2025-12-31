/**
 * WELCOME.H - Écran d'accueil avec effets visuels
 */

#ifndef WELCOME_H
#define WELCOME_H

#include "raylib.h"
#include "theme.h"
#include <stdbool.h>

// ==================== CODE RAIN (Matrix) ====================

#define MAX_CODE_RAIN 150

typedef struct {
  float x, y;
  float speed;
  char character;
  float brightness;
} CodeRainDrop;

// ==================== PARTICULES HEXAGONALES ====================

#define MAX_HEX_PARTICLES 40

typedef struct {
  Vector2 pos;
  Vector2 vel;
  float size;
  float rotation;
  float alpha_base;
  float pulse_phase;
} HexParticle;

// ==================== CARTES MODULES ====================

typedef struct {
  Rectangle bounds;
  const char *title;
  const char *desc;
  const char *icon;
  Color accent_color;
  float scale;
  float glow_alpha;
  float scan_pos;
  bool is_hovered;
} ModuleCard;

// ==================== ÉCRAN D'ACCUEIL ====================

typedef struct {
  // Effets visuels
  CodeRainDrop code_rain[MAX_CODE_RAIN];
  HexParticle hex_particles[MAX_HEX_PARTICLES];

  // Animation d'intro
  float intro_progress;
  float grid_alpha;
  float title_reveal;

  // Titre avec décodage
  char decoded_title[64];
  int decode_index;
  float decode_timer;

  // Cartes modules
  ModuleCard cards[4];

  // État
  bool intro_done;
  Vector2 mouse_pos;
} WelcomeScreen;

// Initialise l'écran d'accueil
void WelcomeInit(WelcomeScreen *screen);

// Met à jour l'écran d'accueil
AppScreen WelcomeUpdate(WelcomeScreen *screen);

// Dessine l'écran d'accueil
void WelcomeDraw(WelcomeScreen *screen);

// Libère les ressources
void WelcomeUnload(WelcomeScreen *screen);

#endif // WELCOME_H
