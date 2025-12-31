/**
 * THEME.H - Couleurs et styles de l'application
 * Thème sombre premium - COULEURS TRÈS VISIBLES
 */

#ifndef THEME_H
#define THEME_H

#include "raylib.h"

// ==================== COULEURS PRINCIPALES ====================
// Fond sombre mais pas noir absolu
#define COLOR_BG_DARK (Color){15, 20, 30, 255}
#define COLOR_BG_LIGHT (Color){25, 35, 50, 255}
// Couleur de la barre de navigation (sombre/noir bleuté)
#define COLOR_NAVBAR (Color){10, 15, 25, 255}

// Panneaux/boutons : gris foncé visible
#define COLOR_SECONDARY (Color){50, 60, 80, 255}
#define COLOR_PANEL (Color){40, 50, 70, 255}

// Accents - TRÈS LUMINEUX
#define COLOR_NEON_ORANGE (Color){255, 160, 60, 255}
#define COLOR_NEON_BLUE (Color){80, 180, 255, 255}
#define COLOR_NEON_GREEN (Color){80, 220, 120, 255}
#define COLOR_NEON_PURPLE (Color){180, 130, 255, 255}
#define COLOR_NEON_RED (Color){255, 80, 80, 255}

// Texte - BLANC et GRIS CLAIR
#define COLOR_TEXT_MAIN (Color){240, 245, 250, 255}
#define COLOR_TEXT_DIM (Color){180, 190, 200, 255}
#define COLOR_TEXT_WHITE (Color){255, 255, 255, 255}

// Bordures - TRÈS VISIBLES
#define COLOR_BORDER (Color){100, 120, 150, 255}
#define COLOR_BORDER_HOVER (Color){80, 180, 255, 255}

// ==================== DIMENSIONS ====================
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define NAVBAR_HEIGHT 40
#define TARGET_FPS 60

// UI
#define BUTTON_HEIGHT 40
#define BUTTON_PADDING 15
#define PANEL_PADDING 20
#define BORDER_RADIUS 6

// Nodes (pour listes/arbres/graphes)
#define NODE_WIDTH 80
#define NODE_HEIGHT 50
#define NODE_RADIUS 25

// ==================== POLICES ====================
#define FONT_SIZE_TITLE 48
#define FONT_SIZE_SUBTITLE 24
#define FONT_SIZE_NORMAL 16
#define FONT_SIZE_SMALL 14

// ==================== ANIMATIONS ====================
#define ANIM_SPEED_FAST 0.15f
#define ANIM_SPEED_NORMAL 0.08f
#define ANIM_SPEED_SLOW 0.03f

// ==================== ÉTATS APPLICATION ====================
typedef enum {
  SCREEN_WELCOME,
  SCREEN_TABLEAUX,
  SCREEN_LISTES,
  SCREEN_ARBRES,
  SCREEN_GRAPHES
} AppScreen;

#endif // THEME_H
