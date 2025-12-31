/**
 * UI_COMMON.C - Implémentation des composants UI
 */

#include "ui_common.h"
#include "raymath.h"
#include <math.h>
#include <string.h>

// ==================== POLICE GLOBALE ====================

static Font g_customFont = {0};
static bool g_fontLoaded = false;

void InitCustomFont(const char *fontPath, int fontSize) {
  g_customFont = LoadFontEx(fontPath, fontSize, NULL, 0);
  if (g_customFont.glyphCount > 0) {
    SetTextureFilter(g_customFont.texture, TEXTURE_FILTER_BILINEAR);
    g_fontLoaded = true;
  }
}

void UnloadCustomFont(void) {
  if (g_fontLoaded) {
    UnloadFont(g_customFont);
    g_fontLoaded = false;
  }
}

Font GetCustomFont(void) {
  return g_fontLoaded ? g_customFont : GetFontDefault();
}

void DrawTextCustom(const char *text, int x, int y, int fontSize, Color color) {
  if (g_fontLoaded) {
    // DrawTextEx pour police personnalisée
    float spacing = (float)fontSize / 10.0f;
    DrawTextEx(g_customFont, text, (Vector2){(float)x, (float)y},
               (float)fontSize, spacing, color);
  } else {
    // Fallback vers police par défaut
    DrawText(text, x, y, fontSize, color);
  }
}

// ==================== BOUTONS ====================

Button CreateButton(float x, float y, float width, float height,
                    const char *text, Color color) {
  Button btn = {0};
  btn.bounds = (Rectangle){x, y, width, height};
  btn.text = text;
  btn.color = color;
  btn.is_hovered = false;
  btn.is_pressed = false;
  btn.glow_alpha = 0.0f;
  return btn;
}

bool UpdateButton(Button *btn) {
  Vector2 mouse = GetMousePosition();
  btn->is_hovered = CheckCollisionPointRec(mouse, btn->bounds);

  // Animation glow - Using Raylib's Lerp
  float target = btn->is_hovered ? 1.0f : 0.0f;
  btn->glow_alpha = Lerp(btn->glow_alpha, target, ANIM_SPEED_FAST);

  // Détection clic
  if (btn->is_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    btn->is_pressed = true;
    return true;
  }
  btn->is_pressed = false;
  return false;
}

void DrawButton(Button *btn) {
  // Ombre/glow sous le bouton
  if (btn->glow_alpha > 0.01f) {
    Color glowColor = btn->color;
    glowColor.a = (unsigned char)(60 * btn->glow_alpha);
    DrawRectangleRounded((Rectangle){btn->bounds.x - 4, btn->bounds.y + 4,
                                     btn->bounds.width + 8, btn->bounds.height},
                         0.3f, 8, glowColor);
  }

  // Fond du bouton
  Color bgColor = btn->is_hovered ? COLOR_SECONDARY : COLOR_PANEL;
  DrawRectangleRounded(btn->bounds, 0.3f, 8, bgColor);

  // Bordure
  Color borderColor = btn->is_hovered ? btn->color : COLOR_BORDER;
  DrawRectRoundedLinesThick(btn->bounds, 0.3f, 8, 2.0f, borderColor);

  // Texte centré
  int textWidth = MeasureText(btn->text, FONT_SIZE_NORMAL);
  int textX = btn->bounds.x + (btn->bounds.width - textWidth) / 2;
  int textY = btn->bounds.y + (btn->bounds.height - FONT_SIZE_NORMAL) / 2;

  Color textColor = btn->is_hovered ? COLOR_TEXT_WHITE : COLOR_TEXT_MAIN;
  DrawTextCustom(btn->text, textX, textY, FONT_SIZE_NORMAL, textColor);
}

void DrawButtonEx(Button *btn, Color overrideColor, bool interactive) {
  // Glow effect on hover (only if interactive)
  if (interactive && btn->is_hovered) {
    btn->glow_alpha += (0.6f - btn->glow_alpha) * 0.2f;
  } else {
    btn->glow_alpha += (0.0f - btn->glow_alpha) * 0.2f;
  }

  // Draw Glow
  if (btn->glow_alpha > 0.01f) {
    DrawRectangleRounded((Rectangle){btn->bounds.x - 2, btn->bounds.y - 2,
                                     btn->bounds.width + 4,
                                     btn->bounds.height + 4},
                         0.3f, 8, Fade(overrideColor, btn->glow_alpha));
  }

  // Draw Button Body
  Color bodyColor = COLOR_PANEL;
  Color borderColor = overrideColor;

  if (interactive && btn->is_pressed) {
    bodyColor = Fade(overrideColor, 0.3f);
  } else if (interactive && btn->is_hovered) {
    bodyColor = Fade(overrideColor, 0.1f);
  }

  DrawRectangleRounded(btn->bounds, 0.3f, 8, bodyColor);
  DrawRectRoundedLinesThick(btn->bounds, 0.3f, 8, 2, borderColor);

  // Draw Text
  int fontSize = 14;
  int textWidth = MeasureText(btn->text, fontSize);
  int textHeight = fontSize;

  int textX = btn->bounds.x + (btn->bounds.width - textWidth) / 2;
  int textY = btn->bounds.y + (btn->bounds.height - textHeight) / 2;

  DrawText(btn->text, textX, textY, fontSize, COLOR_TEXT_WHITE);
}

// ==================== PANNEAUX ====================

void DrawPanel(Rectangle bounds, const char *title, Color borderColor) {
  // Fond
  DrawRectangleRounded(bounds, 0.02f, 8, COLOR_PANEL);

  // Bordure
  DrawRectRoundedLinesThick(bounds, 0.02f, 8, 1.0f, borderColor);

  // Titre
  if (title && strlen(title) > 0) {
    int titleWidth = MeasureText(title, FONT_SIZE_SMALL);
    DrawRectangle(bounds.x + 10, bounds.y - 8, titleWidth + 10, 16,
                  COLOR_BG_LIGHT);
    DrawTextCustom(title, bounds.x + 15, bounds.y - 7, FONT_SIZE_SMALL,
                   borderColor);
  }
}

void DrawPanelSimple(Rectangle bounds, Color bgColor, Color borderColor) {
  DrawRectangleRounded(bounds, 0.02f, 8, bgColor);
  DrawRectRoundedLinesThick(bounds, 0.02f, 8, 1.0f, borderColor);
}

// ==================== TEXTE ====================

void DrawTooltip(const char *text) {
  Vector2 mouse = GetMousePosition();
  int width = MeasureText(text, 10) + 20;
  DrawRectangle(mouse.x + 10, mouse.y + 10, width, 25, Fade(BLACK, 0.8f));
  DrawText(text, mouse.x + 20, mouse.y + 15, 10, WHITE);
}

void DrawRectRoundedLinesThick(Rectangle rec, float roundness, int segments,
                               float thick, Color color) {
  // Version simplifiée : ignore l'épaisseur car la version installée de Raylib
  // ne la supporte pas
  DrawRectangleRoundedLines(rec, roundness, segments, color);
  (void)thick; // Eviter warning unused parameter
}

void DrawTextCentered(const char *text, int y, int fontSize, Color color) {
  int textWidth = MeasureText(text, fontSize);
  DrawText(text, (WINDOW_WIDTH - textWidth) / 2, y, fontSize, color);
}

void DrawTextGlow(const char *text, int x, int y, int fontSize, Color color,
                  float glowIntensity) {
  // Dessiner plusieurs couches pour effet glow
  if (glowIntensity > 0.01f) {
    Color glowColor = color;
    glowColor.a = (unsigned char)(40 * glowIntensity);

    for (int i = 3; i >= 1; i--) {
      DrawText(text, x - i, y, fontSize, glowColor);
      DrawText(text, x + i, y, fontSize, glowColor);
      DrawText(text, x, y - i, fontSize, glowColor);
      DrawText(text, x, y + i, fontSize, glowColor);
    }
  }

  // Texte principal
  DrawText(text, x, y, fontSize, color);
}

// ==================== INPUTS ====================

InputBox CreateInputBox(float x, float y, float width, float height,
                        int maxChars) {
  InputBox box = {0};
  box.bounds = (Rectangle){x, y, width, height};
  box.cursor = 0;
  box.is_focused = false;
  box.max_chars = maxChars < 256 ? maxChars : 255;
  memset(box.text, 0, sizeof(box.text));
  return box;
}

void UpdateInputBox(InputBox *box) {
  Vector2 mouse = GetMousePosition();

  // Focus au clic
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    box->is_focused = CheckCollisionPointRec(mouse, box->bounds);
  }

  if (!box->is_focused)
    return;

  // Saisie caractères
  int key = GetCharPressed();
  while (key > 0) {
    if ((key >= 32) && (key <= 125) && (box->cursor < box->max_chars)) {
      box->text[box->cursor] = (char)key;
      box->cursor++;
      box->text[box->cursor] = '\0';
    }
    key = GetCharPressed();
  }

  // Backspace
  if (IsKeyPressed(KEY_BACKSPACE) && box->cursor > 0) {
    box->cursor--;
    box->text[box->cursor] = '\0';
  }
}

void DrawInputBox(InputBox *box) {
  // Fond
  Color bgColor = box->is_focused ? COLOR_SECONDARY : COLOR_PANEL;
  DrawRectangleRounded(box->bounds, 0.2f, 8, bgColor);

  // Bordure
  Color borderColor = box->is_focused ? COLOR_NEON_BLUE : COLOR_BORDER;
  DrawRectRoundedLinesThick(box->bounds, 0.2f, 8, 2.0f, borderColor);

  // Clip text to stay inside the box
  BeginScissorMode((int)box->bounds.x + 5, (int)box->bounds.y,
                   (int)box->bounds.width - 10, (int)box->bounds.height);

  // Texte
  DrawText(box->text, box->bounds.x + 8,
           box->bounds.y + (box->bounds.height - FONT_SIZE_NORMAL) / 2,
           FONT_SIZE_NORMAL, COLOR_TEXT_MAIN);

  // Curseur clignotant
  if (box->is_focused && ((int)(GetTime() * 2) % 2 == 0)) {
    int textWidth = MeasureText(box->text, FONT_SIZE_NORMAL);
    DrawRectangle(box->bounds.x + 8 + textWidth + 2, box->bounds.y + 8, 2,
                  box->bounds.height - 16, COLOR_NEON_BLUE);
  }

  EndScissorMode();
}

// ==================== ANIMATIONS ====================
// Note: Lerp is provided by Raylib

float EaseOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }

float EaseInOutQuad(float t) {
  return t < 0.5f ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

// ==================== UTILITAIRES ====================

bool IsMouseInRect(Rectangle rect) {
  return CheckCollisionPointRec(GetMousePosition(), rect);
}

void DrawLineGlow(Vector2 start, Vector2 end, float thickness, Color color) {
  // Glow layers
  Color glowColor = color;
  glowColor.a = 30;
  DrawLineEx(start, end, thickness + 6.0f, glowColor);

  glowColor.a = 60;
  DrawLineEx(start, end, thickness + 3.0f, glowColor);

  // Main line
  DrawLineEx(start, end, thickness, color);
}

void DrawRoundedRectangle(Rectangle rect, float roundness, Color color) {
  DrawRectangleRounded(rect, roundness, 8, color);
}

void DrawRoundedRectangleLines(Rectangle rect, float roundness, float thickness,
                               Color color) {
  DrawRectRoundedLinesThick(rect, roundness, 8, thickness, color);
}

// ==================== NAVIGATION ====================

bool UpdateNavigationBar(AppScreen *targetScreen) {
  const char *labels[] = {"Accueil", "Tableaux", "Listes", "Arbres", "Graphes"};
  AppScreen screens[] = {SCREEN_WELCOME, SCREEN_TABLEAUX, SCREEN_LISTES,
                         SCREEN_ARBRES, SCREEN_GRAPHES};
  int count = 5;

  int btnWidth = 100;
  int btnHeight = 30;
  int startX = 20;
  int startY = (NAVBAR_HEIGHT - btnHeight) / 2;
  int spacing = 10;
  bool changed = false;

  for (int i = 0; i < count; i++) {
    bool isActive = (*targetScreen == screens[i]);
    Color btnColor = isActive ? COLOR_NEON_BLUE : COLOR_SECONDARY;

    Button btn = CreateButton(startX + (i * (btnWidth + spacing)), startY,
                              btnWidth, btnHeight, labels[i], btnColor);

    if (UpdateButton(&btn)) {
      *targetScreen = screens[i];
      changed = true;
    }
  }
  return changed;
}

void DrawNavigationBar(AppScreen activeScreen) {
  // Fond de la barre
  DrawRectangle(0, 0, WINDOW_WIDTH, NAVBAR_HEIGHT, COLOR_NAVBAR);
  DrawLine(0, NAVBAR_HEIGHT, WINDOW_WIDTH, NAVBAR_HEIGHT, COLOR_BORDER);

  // Boutons de navigation
  const char *labels[] = {"Accueil", "Tableaux", "Listes", "Arbres", "Graphes"};
  AppScreen screens[] = {SCREEN_WELCOME, SCREEN_TABLEAUX, SCREEN_LISTES,
                         SCREEN_ARBRES, SCREEN_GRAPHES};
  // Couleurs spécifiques par module "comme l'accueil"
  // Accueil: Blanc/Neutre, Tableaux: Orange, Listes: Bleu, Arbres: Vert,
  // Graphes: Violet
  Color moduleColors[] = {COLOR_TEXT_MAIN, COLOR_NEON_ORANGE, COLOR_NEON_BLUE,
                          COLOR_NEON_GREEN, COLOR_NEON_PURPLE};

  int count = 5;
  int btnWidth = 100;
  int btnHeight = 30;
  int startX = 20;
  int startY = (NAVBAR_HEIGHT - btnHeight) / 2;
  int spacing = 10;

  Vector2 mousePos = GetMousePosition();

  for (int i = 0; i < count; i++) {
    bool isActive = (activeScreen == screens[i]);
    Rectangle btnRect = {startX + (i * (btnWidth + spacing)), startY, btnWidth,
                         btnHeight};
    bool isHovered = CheckCollisionPointRec(mousePos, btnRect);

    // Couleur dynamique
    Color btnColor;
    if (isActive) {
      btnColor = moduleColors[i]; // Couleur pleine si actif
    } else if (isHovered) {
      btnColor = moduleColors[i]; // Couleur pleine mais peut-être un peu
                                  // dimmée? Non, full color on hover c'est bien
      btnColor.a = 200;           // Légère transparence
    } else {
      btnColor = COLOR_SECONDARY; // Gris par défaut
    }

    if (isActive) {
      // Effet de surbrillance pour l'onglet actif (ligne en bas)
      DrawRectangle(btnRect.x, NAVBAR_HEIGHT - 2, btnWidth, 2, moduleColors[i]);
    }

    // On utilise CreateButton juste pour l'affichage, la logique est dans
    // UpdateNavigationBar Mais on veut l'effet visuel ici
    Button btn = CreateButton(btnRect.x, btnRect.y, btnWidth, btnHeight,
                              labels[i], btnColor);

    // Override le status focus pour le rendu précis
    btn.is_hovered = isHovered;

    DrawButton(&btn);
  }
}
