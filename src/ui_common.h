/**
 * UI_COMMON.H - Composants UI réutilisables
 */

#ifndef UI_COMMON_H
#define UI_COMMON_H

#include "raylib.h"
#include "theme.h"
#include <stdbool.h>

// ==================== POLICE GLOBALE ====================

// Initialise la police personnalisée (à appeler après InitWindow)
void InitCustomFont(const char *fontPath, int fontSize);

// Libère la police (à appeler avant CloseWindow)
void UnloadCustomFont(void);

// Retourne la police active (personnalisée ou défaut)
Font GetCustomFont(void);

// Dessine du texte avec la police personnalisée
void DrawTextCustom(const char *text, int x, int y, int fontSize, Color color);

// ==================== BOUTONS ====================

typedef struct {
  Rectangle bounds;
  const char *text;
  Color color;
  bool is_hovered;
  bool is_pressed;
  float glow_alpha;
} Button;

// Crée un bouton
Button CreateButton(float x, float y, float width, float height,
                    const char *text, Color color);

// Met à jour l'état du bouton (hover, clic)
bool UpdateButton(Button *btn);

// Dessine un bouton simple
void DrawButton(Button *btn);

// Wrapper pour compatibilité Raylib (épaisseur ignorée si non supportée)
void DrawRectRoundedLinesThick(Rectangle rec, float roundness, int segments,
                               float thick, Color color);

// Dessine un bouton avec couleur forcee et interaction (ou non)
void DrawButtonEx(Button *btn, Color overrideColor, bool interactive);

// ==================== PANNEAUX ====================

// Dessine un panneau avec bordure
void DrawPanel(Rectangle bounds, const char *title, Color borderColor);

// Dessine un panneau simple sans titre
void DrawPanelSimple(Rectangle bounds, Color bgColor, Color borderColor);

// ==================== TEXTE ====================

// Dessine du texte centré horizontalement
void DrawTextCentered(const char *text, int y, int fontSize, Color color);

// Dessine du texte avec effet glow
void DrawTextGlow(const char *text, int x, int y, int fontSize, Color color,
                  float glowIntensity);

// ==================== INPUTS ====================

typedef struct {
  Rectangle bounds;
  char text[256];
  int cursor;
  bool is_focused;
  int max_chars;
} InputBox;

// Crée une boîte de saisie
InputBox CreateInputBox(float x, float y, float width, float height,
                        int maxChars);

// Met à jour la boîte de saisie (gère le clavier)
void UpdateInputBox(InputBox *box);

// Dessine la boîte de saisie
void DrawInputBox(InputBox *box);

// ==================== ANIMATIONS ====================
// Note: Lerp() is provided by Raylib (raymath.h)

// Ease out quad
float EaseOutQuad(float t);

// Ease in out quad
float EaseInOutQuad(float t);

// ==================== UTILITAIRES ====================

// Vérifie si la souris est dans un rectangle
bool IsMouseInRect(Rectangle rect);

// Dessine une ligne avec glow
void DrawLineGlow(Vector2 start, Vector2 end, float thickness, Color color);

// Dessine un rectangle arrondi
void DrawRoundedRectangle(Rectangle rect, float roundness, Color color);

// Dessine un rectangle arrondi avec bordure
void DrawRoundedRectangleLines(Rectangle rect, float roundness, float thickness,
                               Color color);

// ==================== NAVIGATION ====================

// Met à jour la barre de navigation (gestion des clics)
// Modifie targetScreen si un bouton est cliqué
// Retourne true si un changement d'écran est demandé
bool UpdateNavigationBar(AppScreen *targetScreen);

// Dessine la barre de navigation
// activeScreen indique quel onglet allumer
void DrawNavigationBar(AppScreen activeScreen);

#endif // UI_COMMON_H
