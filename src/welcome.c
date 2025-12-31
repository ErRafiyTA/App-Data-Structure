/**
 * WELCOME.C - Implémentation de l'écran d'accueil
 */

#include "welcome.h"
#include "raymath.h"
#include "ui_common.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Titre de l'application
static const char *FULL_TITLE = "STRUCTURES DE DONNEES";
static const char *SUBTITLE = "Oriented by Mr.KHOUKHI";
static const char *AUTHOR_TEXT = "[  REALISE PAR TAHA AMINE ER-RAFIY  ]";

// Caractères pour effet décodage
static const char DECODE_CHARS[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";

// ==================== HELPERS ====================

static float RandomFloat(float min, float max) {
  return min + (float)rand() / (float)RAND_MAX * (max - min);
}

static char RandomChar(void) { return 'A' + rand() % 26; }

// ==================== CODE RAIN ====================

static void InitCodeRain(CodeRainDrop *drops) {
  for (int i = 0; i < MAX_CODE_RAIN; i++) {
    drops[i].x = RandomFloat(0, WINDOW_WIDTH);
    drops[i].y = RandomFloat(0, WINDOW_HEIGHT);
    drops[i].speed = RandomFloat(50.0f, 200.0f);
    drops[i].character = RandomChar();
    drops[i].brightness = RandomFloat(0.2f, 0.6f);
  }
}

static void UpdateCodeRain(CodeRainDrop *drops, float dt) {
  for (int i = 0; i < MAX_CODE_RAIN; i++) {
    drops[i].y += drops[i].speed * dt;

    // Réinitialiser quand sort de l'écran
    if (drops[i].y > WINDOW_HEIGHT + 20) {
      drops[i].y = -20;
      drops[i].x = RandomFloat(0, WINDOW_WIDTH);
      drops[i].character = RandomChar();
    }

    // Changer le caractère occasionnellement
    if (rand() % 100 < 2) {
      drops[i].character = RandomChar();
    }
  }
}

static void DrawCodeRain(CodeRainDrop *drops, float alpha_mul) {
  for (int i = 0; i < MAX_CODE_RAIN; i++) {
    char text[2] = {drops[i].character, '\0'};
    unsigned char alpha =
        (unsigned char)(drops[i].brightness * 255 * alpha_mul);
    Color color = (Color){88, 166, 255, alpha};
    DrawText(text, (int)drops[i].x, (int)drops[i].y, 16, color);
  }
}

// ==================== PARTICULES HEXAGONALES ====================

static void InitHexParticles(HexParticle *particles) {
  for (int i = 0; i < MAX_HEX_PARTICLES; i++) {
    particles[i].pos =
        (Vector2){RandomFloat(0, WINDOW_WIDTH), RandomFloat(0, WINDOW_HEIGHT)};
    particles[i].vel =
        (Vector2){RandomFloat(-20.0f, 20.0f), RandomFloat(-20.0f, 20.0f)};
    particles[i].size = RandomFloat(15.0f, 40.0f);
    particles[i].rotation = RandomFloat(0, 360);
    particles[i].alpha_base = RandomFloat(0.15f, 0.35f);
    particles[i].pulse_phase = RandomFloat(0, 6.28f);
  }
}

static void UpdateHexParticles(HexParticle *particles, Vector2 mouse,
                               float dt) {
  for (int i = 0; i < MAX_HEX_PARTICLES; i++) {
    // Mouvement
    particles[i].pos.x += particles[i].vel.x * dt;
    particles[i].pos.y += particles[i].vel.y * dt;
    particles[i].rotation += 10.0f * dt;
    particles[i].pulse_phase += dt;

    // Rebond sur les bords
    if (particles[i].pos.x < 0 || particles[i].pos.x > WINDOW_WIDTH) {
      particles[i].vel.x *= -1;
      particles[i].pos.x = fmaxf(0, fminf(WINDOW_WIDTH, particles[i].pos.x));
    }
    if (particles[i].pos.y < 0 || particles[i].pos.y > WINDOW_HEIGHT) {
      particles[i].vel.y *= -1;
      particles[i].pos.y = fmaxf(0, fminf(WINDOW_HEIGHT, particles[i].pos.y));
    }

    // Attraction vers la souris
    float dx = mouse.x - particles[i].pos.x;
    float dy = mouse.y - particles[i].pos.y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < 200 && dist > 10) {
      float force = 50.0f / dist;
      particles[i].vel.x += dx / dist * force * dt;
      particles[i].vel.y += dy / dist * force * dt;
    }

    // Limiter la vitesse
    float speed = sqrtf(particles[i].vel.x * particles[i].vel.x +
                        particles[i].vel.y * particles[i].vel.y);
    if (speed > 60.0f) {
      particles[i].vel.x = particles[i].vel.x / speed * 60.0f;
      particles[i].vel.y = particles[i].vel.y / speed * 60.0f;
    }
  }
}

static void DrawHexagon(Vector2 center, float size, float rotation,
                        Color color) {
  Vector2 points[6];
  for (int i = 0; i < 6; i++) {
    float angle = (60.0f * i + rotation) * DEG2RAD;
    points[i] =
        (Vector2){center.x + cosf(angle) * size, center.y + sinf(angle) * size};
  }

  // Dessiner les lignes
  for (int i = 0; i < 6; i++) {
    DrawLineV(points[i], points[(i + 1) % 6], color);
  }
}

static void DrawHexParticles(HexParticle *particles, float time,
                             float alpha_mul) {
  for (int i = 0; i < MAX_HEX_PARTICLES; i++) {
    float pulse = 0.5f + 0.5f * sinf(particles[i].pulse_phase + time);
    float alpha = particles[i].alpha_base * (0.7f + 0.3f * pulse) * alpha_mul;
    Color color = (Color){255, 107, 53, (unsigned char)(alpha * 255)};
    DrawHexagon(particles[i].pos, particles[i].size, particles[i].rotation,
                color);
  }
}

// ==================== GRILLE DE FOND ====================

static void DrawBackgroundGrid(float alpha) {
  if (alpha < 0.01f)
    return;

  int spacing = 50;
  unsigned char a = (unsigned char)(15 * alpha);
  Color lineColor = (Color){48, 54, 61, a};

  // Lignes verticales
  for (int x = 0; x < WINDOW_WIDTH; x += spacing) {
    DrawLine(x, 0, x, WINDOW_HEIGHT, lineColor);
  }

  // Lignes horizontales
  for (int y = 0; y < WINDOW_HEIGHT; y += spacing) {
    DrawLine(0, y, WINDOW_WIDTH, y, lineColor);
  }
}

// ==================== TITRE ====================

static void UpdateTitleDecode(WelcomeScreen *screen, float dt) {
  if (screen->decode_index >= (int)strlen(FULL_TITLE))
    return;

  screen->decode_timer += dt;

  if (screen->decode_timer >= 0.05f) {
    screen->decode_timer = 0;

    // Ajouter un caractère
    screen->decoded_title[screen->decode_index] =
        FULL_TITLE[screen->decode_index];
    screen->decode_index++;
    screen->decoded_title[screen->decode_index] = '\0';
  }
}

static void DrawTitle(WelcomeScreen *screen, float time) {
  // --- Auteur au-dessus du titre (Effet Spécial) ---
  if (screen->grid_alpha > 0.5f) {
    int authWidth = MeasureText(AUTHOR_TEXT, 16);
    int authX = (WINDOW_WIDTH - authWidth) / 2;
    int authY = 80;

    // Effet pulsation couleur
    float pulse = (sinf(time * 3.0f) + 1.0f) * 0.5f; // 0.0 to 1.0
    unsigned char alpha = (unsigned char)(150 + 105 * pulse);
    Color authColor = (Color){0, 255, 100, alpha}; // Neon Green pulsing

    // Effet "Ghost" (double affichage décalé)
    DrawText(AUTHOR_TEXT, authX - 1, authY, 16, (Color){0, 100, 50, 100});
    DrawText(AUTHOR_TEXT, authX + 1, authY, 16, (Color){0, 100, 50, 100});

    DrawText(AUTHOR_TEXT, authX, authY, 16, authColor);

    // Lignes décoratives latérales
    DrawLine(authX - 40, authY + 8, authX - 10, authY + 8, authColor);
    DrawLine(authX + authWidth + 10, authY + 8, authX + authWidth + 40,
             authY + 8, authColor);
  }

  // Titre principal avec glow
  int titleWidth = MeasureText(screen->decoded_title, FONT_SIZE_TITLE);
  int titleX = (WINDOW_WIDTH - titleWidth) / 2;
  int titleY = 120;

  // Glow effect
  Color glowColor = (Color){255, 107, 53, 30};
  for (int i = 4; i >= 1; i--) {
    DrawText(screen->decoded_title, titleX + i, titleY, FONT_SIZE_TITLE,
             glowColor);
    DrawText(screen->decoded_title, titleX - i, titleY, FONT_SIZE_TITLE,
             glowColor);
    DrawText(screen->decoded_title, titleX, titleY + i, FONT_SIZE_TITLE,
             glowColor);
    DrawText(screen->decoded_title, titleX, titleY - i, FONT_SIZE_TITLE,
             glowColor);
  }

  // Titre principal
  DrawText(screen->decoded_title, titleX, titleY, FONT_SIZE_TITLE,
           COLOR_NEON_ORANGE);

  // Effet glitch occasionnel
  if (rand() % 100 < 3) {
    int offset = rand() % 4 - 2;
    Color glitchColor = (Color){88, 166, 255, 100};
    DrawText(screen->decoded_title, titleX + offset, titleY, FONT_SIZE_TITLE,
             glitchColor);
  }

  // Sous-titre avec effet laser
  if (screen->decode_index >= (int)strlen(FULL_TITLE)) {
    float subtitleAlpha = fminf(1.0f, screen->title_reveal);
    int subWidth = MeasureText(SUBTITLE, FONT_SIZE_SUBTITLE);
    int subX = (WINDOW_WIDTH - subWidth) / 2;
    int subY = titleY + 60;

    // Effet laser: ligne qui scanne de gauche à droite
    float laserPos = fmodf(time * 80.0f, (float)(subWidth + 40)) - 20;

    // Dessiner le glow laser derrière le texte
    if (laserPos >= -10 && laserPos <= subWidth + 10) {
      for (int i = 0; i < 3; i++) {
        Color laserGlow = (Color){0, 255, 100, (unsigned char)(30 - i * 10)};
        DrawRectangle(subX + (int)laserPos - 5 - i * 2, subY - 2, 10 + i * 4,
                      FONT_SIZE_SUBTITLE + 4, laserGlow);
      }
      // Ligne laser centrale
      Color laserCore = (Color){0, 255, 100, 200};
      DrawRectangle(subX + (int)laserPos - 1, subY, 2, FONT_SIZE_SUBTITLE,
                    laserCore);
    }

    // Texte du sous-titre avec couleur néon verte
    Color subColor = (Color){0, 255, 100, (unsigned char)(subtitleAlpha * 255)};
    DrawTextCentered(SUBTITLE, subY, FONT_SIZE_SUBTITLE, subColor);

    // Glow vert autour du texte
    Color glowGreen = (Color){0, 255, 100, (unsigned char)(40 * subtitleAlpha)};
    DrawText(SUBTITLE, subX - 1, subY, FONT_SIZE_SUBTITLE, glowGreen);
    DrawText(SUBTITLE, subX + 1, subY, FONT_SIZE_SUBTITLE, glowGreen);
  }
}

// ==================== CARTES MODULES ====================

static void InitModuleCards(ModuleCard *cards) {
  float cardWidth = 250;
  float cardHeight = 180;
  float startX = (WINDOW_WIDTH - (cardWidth * 4 + 30 * 3)) / 2;
  float cardY = 350;

  // Tableaux
  cards[0] = (ModuleCard){
      .bounds = {startX, cardY, cardWidth, cardHeight},
      .title = "TABLEAUX",
      .desc = "Algorithmes de Tri\nBubble - Insertion\nShell - Quick Sort",
      .icon = "[#]",
      .accent_color = COLOR_NEON_ORANGE,
      .scale = 1.0f,
      .glow_alpha = 0,
      .scan_pos = 0,
      .is_hovered = false};

  // Listes
  cards[1] = (ModuleCard){
      .bounds = {startX + cardWidth + 30, cardY, cardWidth, cardHeight},
      .title = "LISTES",
      .desc = "Listes Chainees\nSimple - Double\nOperations CRUD",
      .icon = "[->]",
      .accent_color = COLOR_NEON_BLUE,
      .scale = 1.0f,
      .glow_alpha = 0,
      .scan_pos = 0,
      .is_hovered = false};

  // Arbres
  cards[2] = (ModuleCard){
      .bounds = {startX + (cardWidth + 30) * 2, cardY, cardWidth, cardHeight},
      .title = "ARBRES",
      .desc = "Arbres Binaires\nParcours - BST\nN-aire",
      .icon = "[Y]",
      .accent_color = COLOR_NEON_GREEN,
      .scale = 1.0f,
      .glow_alpha = 0,
      .scan_pos = 0,
      .is_hovered = false};

  // Graphes
  cards[3] = (ModuleCard){
      .bounds = {startX + (cardWidth + 30) * 3, cardY, cardWidth, cardHeight},
      .title = "GRAPHES",
      .desc = "Graphes\nBFS - DFS\nDijkstra",
      .icon = "[O]",
      .accent_color = COLOR_NEON_PURPLE,
      .scale = 1.0f,
      .glow_alpha = 0,
      .scan_pos = 0,
      .is_hovered = false};
}

static int UpdateModuleCards(ModuleCard *cards, float dt) {
  Vector2 mouse = GetMousePosition();
  int clicked = -1;

  for (int i = 0; i < 4; i++) {
    // Calculer le rectangle avec scale
    Rectangle scaledBounds = cards[i].bounds;
    float scaleOffset = (cards[i].scale - 1.0f) * scaledBounds.width * 0.5f;
    scaledBounds.x -= scaleOffset;
    scaledBounds.y -= scaleOffset * (scaledBounds.height / scaledBounds.width);
    scaledBounds.width *= cards[i].scale;
    scaledBounds.height *= cards[i].scale;

    cards[i].is_hovered = CheckCollisionPointRec(mouse, cards[i].bounds);

    // Animation scale et glow
    float targetScale = cards[i].is_hovered ? 1.05f : 1.0f;
    float targetGlow = cards[i].is_hovered ? 1.0f : 0.0f;

    cards[i].scale = Lerp(cards[i].scale, targetScale, ANIM_SPEED_FAST);
    cards[i].glow_alpha =
        Lerp(cards[i].glow_alpha, targetGlow, ANIM_SPEED_FAST);

    // Animation scan
    if (cards[i].is_hovered) {
      cards[i].scan_pos += 200.0f * dt;
      if (cards[i].scan_pos > cards[i].bounds.height + 20) {
        cards[i].scan_pos = -20;
      }
    }

    // Détection clic
    if (cards[i].is_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      clicked = i;
    }
  }

  return clicked;
}

static void DrawModuleCard(ModuleCard *card) {
  Rectangle bounds = card->bounds;

  // Appliquer le scale (centré)
  float scaleOffset = (card->scale - 1.0f) * bounds.width * 0.5f;
  Rectangle scaledBounds = {
      bounds.x - scaleOffset,
      bounds.y - scaleOffset * (bounds.height / bounds.width),
      bounds.width * card->scale, bounds.height * card->scale};

  // Ombre glow
  if (card->glow_alpha > 0.01f) {
    Color glowColor = card->accent_color;
    glowColor.a = (unsigned char)(40 * card->glow_alpha);
    DrawRectangleRounded((Rectangle){scaledBounds.x - 6, scaledBounds.y + 6,
                                     scaledBounds.width + 12,
                                     scaledBounds.height},
                         0.08f, 8, glowColor);
  }

  // Fond
  DrawRectangleRounded(scaledBounds, 0.08f, 8, COLOR_PANEL);

  // Bordure
  Color borderColor = card->is_hovered ? card->accent_color : COLOR_BORDER;
  DrawRectRoundedLinesThick(scaledBounds, 0.08f, 8, 2.0f, borderColor);

  // Ligne de scan
  if (card->is_hovered && card->scan_pos > 0) {
    float scanY = scaledBounds.y + card->scan_pos;
    if (scanY < scaledBounds.y + scaledBounds.height) {
      Color scanColor = card->accent_color;
      scanColor.a = 100;
      DrawLineEx((Vector2){scaledBounds.x + 5, scanY},
                 (Vector2){scaledBounds.x + scaledBounds.width - 5, scanY},
                 2.0f, scanColor);
    }
  }

  // Icône
  DrawText(card->icon, scaledBounds.x + 20, scaledBounds.y + 20, 24,
           card->accent_color);

  // Titre
  DrawText(card->title, scaledBounds.x + 20, scaledBounds.y + 55, 20,
           COLOR_TEXT_WHITE);

  // Description (multiligne)
  Color descColor = card->is_hovered ? COLOR_TEXT_MAIN : COLOR_TEXT_DIM;

  // Parser les lignes
  char desc_copy[256];
  strncpy(desc_copy, card->desc, sizeof(desc_copy) - 1);
  desc_copy[sizeof(desc_copy) - 1] = '\0';

  int lineY = scaledBounds.y + 85;
  char *line = strtok(desc_copy, "\n");
  while (line != NULL) {
    DrawText(line, scaledBounds.x + 20, lineY, 14, descColor);
    lineY += 18;
    line = strtok(NULL, "\n");
  }
}

// ==================== ÉCRAN D'ACCUEIL PRINCIPAL ====================

void WelcomeInit(WelcomeScreen *screen) {
  srand((unsigned int)time(NULL));

  memset(screen, 0, sizeof(WelcomeScreen));

  InitCodeRain(screen->code_rain);
  InitHexParticles(screen->hex_particles);
  InitModuleCards(screen->cards);

  screen->intro_progress = 0;
  screen->grid_alpha = 0;
  screen->title_reveal = 0;
  screen->decode_index = 0;
  screen->decode_timer = 0;
  screen->decoded_title[0] = '\0';
  screen->intro_done = false;
}

AppScreen WelcomeUpdate(WelcomeScreen *screen) {
  float dt = GetFrameTime();
  screen->mouse_pos = GetMousePosition();

  // Animation d'intro
  if (!screen->intro_done) {
    screen->intro_progress += dt * 0.5f;

    // Fade in de la grille
    screen->grid_alpha = fminf(1.0f, screen->intro_progress);

    // Décodage du titre après 0.5s
    if (screen->intro_progress > 0.5f) {
      UpdateTitleDecode(screen, dt);
    }

    // Révélation du sous-titre
    if (screen->decode_index >= (int)strlen(FULL_TITLE)) {
      screen->title_reveal += dt * 2.0f;
    }

    // Fin de l'intro
    if (screen->intro_progress > 3.0f) {
      screen->intro_done = true;
    }
  }

  // Mise à jour des effets
  UpdateCodeRain(screen->code_rain, dt);
  UpdateHexParticles(screen->hex_particles, screen->mouse_pos, dt);

  // Mise à jour des cartes
  int clicked = UpdateModuleCards(screen->cards, dt);

  // Retourner l'écran sélectionné
  if (clicked >= 0) {
    switch (clicked) {
    case 0:
      return SCREEN_TABLEAUX;
    case 1:
      return SCREEN_LISTES;
    case 2:
      return SCREEN_ARBRES;
    case 3:
      return SCREEN_GRAPHES;
    }
  }

  return SCREEN_WELCOME;
}

void WelcomeDraw(WelcomeScreen *screen) {
  float time = GetTime();

  // Fond dégradé
  DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_BG_DARK,
                         COLOR_BG_LIGHT);

  // Grille
  DrawBackgroundGrid(screen->grid_alpha);

  // Code Rain (Matrix effect)
  DrawCodeRain(screen->code_rain, 0.3f);

  // Particules hexagonales
  DrawHexParticles(screen->hex_particles, time, 1.0f);

  // Titre
  DrawTitle(screen, time);

  // Cartes des modules
  for (int i = 0; i < 4; i++) {
    DrawModuleCard(&screen->cards[i]);
  }

  // Footer
  DrawTextCentered("Cliquez sur un module pour commencer", WINDOW_HEIGHT - 60,
                   14, COLOR_TEXT_DIM);

  // Version
  DrawText("v2.0 Raylib Edition", WINDOW_WIDTH - 150, WINDOW_HEIGHT - 30, 12,
           COLOR_TEXT_DIM);
}

void WelcomeUnload(WelcomeScreen *screen) {
  // Rien à libérer pour l'instant
  (void)screen;
}
