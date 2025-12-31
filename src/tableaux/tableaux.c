/**
 * TABLEAUX.C - Module Tableaux (Version finale)
 *
 * Saisie manuelle style Python:
 * - Champs progressifs (un nouveau apparaît après chaque validation)
 * - Scroll automatique vers le nouveau champ
 * - Bouton X pour fermer
 */

#include "tableaux.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TYPE_NAMES[] = {"Entiers", "Reels", "Caracteres", "Chaines"};

// ==================== HELPERS ====================

static void ClearSortedData(TableauxScreen *screen) {
  for (int i = 0; i < SORT_COUNT; i++) {
    if (screen->sort_manager.sorted[i]) {
      FreeGenericData(screen->sort_manager.sorted[i]);
      screen->sort_manager.sorted[i] = NULL;
    }
    screen->sort_done[i] = false;
    screen->sort_times[i] = 0;
    screen->sort_running[i] = false;
    screen->sort_elapsed[i] = 0;
  }
  if (screen->display_sorted) {
    FreeGenericData(screen->display_sorted);
    screen->display_sorted = NULL;
  }
}

static void ResetScreen(TableauxScreen *screen) {
  if (screen->original_data) {
    FreeGenericData(screen->original_data);
    screen->original_data = NULL;
  }
  ClearSortedData(screen);
  screen->data_size = 0;
  screen->state = TAB_STATE_IDLE;
  screen->selected_sort = -1;
  screen->elapsed_time = 0;
  screen->status_message[0] = '\0';
}

static Color GetAlgoColor(int algo) {
  switch (algo) {
  case SORT_BUBBLE:
    return COLOR_NEON_ORANGE;
  case SORT_INSERTION:
    return COLOR_NEON_BLUE;
  case SORT_SHELL:
    return (Color){255, 193, 7, 255};
  case SORT_QUICK:
    return COLOR_NEON_GREEN;
  default:
    return COLOR_TEXT_MAIN;
  }
}

// ==================== AFFICHAGE DONNÉES ====================

static void DrawDataPanel(GenericData *data, Rectangle bounds,
                          const char *title, float scroll_y,
                          Color borderColor) {
  DrawPanel(bounds, title, borderColor);

  if (!data || data->size == 0) {
    DrawText("Aucune donnee", bounds.x + 20, bounds.y + 50, 16, COLOR_TEXT_DIM);
    return;
  }

  char countStr[32];
  snprintf(countStr, sizeof(countStr), "(%d elements)", data->size);
  DrawText(countStr, bounds.x + bounds.width - 130, bounds.y + 5, 12,
           COLOR_TEXT_DIM);

  BeginScissorMode(bounds.x + 5, bounds.y + 30, bounds.width - 10,
                   bounds.height - 35);

  int x_offset = bounds.x + 10;
  int y_offset = bounds.y + 35 - (int)scroll_y;

  int char_width = (data->type == DATA_TYPE_CHAR)  ? 28
                   : (data->type == DATA_TYPE_INT) ? 95
                                                   : 85;
  int items_per_row = (bounds.width - 20) / char_width;
  if (items_per_row < 1)
    items_per_row = 1;
  int item_height = 22;

  char buffer[64];

  for (int i = 0; i < data->size; i++) {
    int row = i / items_per_row;
    int col = i % items_per_row;
    int x = x_offset + col * char_width;
    int y = y_offset + row * item_height;

    if (y < bounds.y - item_height || y > bounds.y + bounds.height)
      continue;

    switch (data->type) {
    case DATA_TYPE_INT:
      snprintf(buffer, sizeof(buffer), "%lld", data->data.int_data[i]);
      break;
    case DATA_TYPE_FLOAT:
      snprintf(buffer, sizeof(buffer), "%.2f", data->data.float_data[i]);
      break;
    case DATA_TYPE_CHAR:
      snprintf(buffer, sizeof(buffer), "%c", data->data.char_data[i]);
      break;
    case DATA_TYPE_STRING:
      snprintf(buffer, sizeof(buffer), "%.8s", data->data.string_data[i]);
      break;
    }
    DrawText(buffer, x, y, 12, COLOR_TEXT_MAIN);
  }
  EndScissorMode();
}

// ==================== GRAPHIQUE ====================

static void DrawPerformanceGraph(TableauxScreen *screen, Rectangle bounds) {
  DrawPanel(bounds, "Courbes de Tri", COLOR_NEON_BLUE);

  // Vérifier si au moins un tri est terminé ou en cours
  bool hasData = false;
  double maxTime = 0.001;
  for (int a = 0; a < SORT_COUNT; a++) {
    if (screen->sort_done[a]) {
      hasData = true;
      if (screen->sort_times[a] > maxTime) {
        maxTime = screen->sort_times[a];
      }
    } else if (screen->sort_running[a] && screen->sort_elapsed[a] > maxTime) {
      maxTime = screen->sort_elapsed[a];
      hasData = true;
    }
  }

  if (!hasData) {
    DrawText("Lancez un tri pour voir les courbes", bounds.x + 40,
             bounds.y + bounds.height / 2, 16, COLOR_TEXT_DIM);
    return;
  }

  maxTime *= 1.15;

  float margin = 50;
  float graphX = bounds.x + margin;
  float graphY = bounds.y + 50;
  float graphW = bounds.width - margin - 150;
  float graphH = bounds.height - 100;

  // Point d'origine (0,0) en bas à gauche
  Vector2 origin = {graphX, graphY + graphH};

  // Axes
  DrawLineEx((Vector2){graphX, graphY + graphH},
             (Vector2){graphX + graphW, graphY + graphH}, 2.0f, COLOR_TEXT_DIM);
  DrawLineEx((Vector2){graphX, graphY}, (Vector2){graphX, graphY + graphH},
             2.0f, COLOR_TEXT_DIM);

  // Titre axe Y (vertical)
  const char *yTitle = "Progression";
  int yTitleLen = strlen(yTitle);
  for (int i = 0; i < yTitleLen; i++) {
    char c[2] = {yTitle[i], '\0'};
    DrawText(c, bounds.x + 8, graphY + i * 12, 12, COLOR_NEON_BLUE);
  }

  // Titre axe X
  DrawText("Temps (ms)", graphX + graphW / 2 - 30, graphY + graphH + 18, 12,
           COLOR_NEON_BLUE);

  // Labels axes
  DrawText("0", graphX - 12, graphY + graphH + 5, 12, COLOR_TEXT_DIM);
  char maxLabel[32];
  snprintf(maxLabel, sizeof(maxLabel), "%.0f ms", maxTime * 1000.0);
  DrawText(maxLabel, graphX + graphW - 30, graphY + graphH + 5, 12,
           COLOR_TEXT_DIM);

  // Dessiner 4 courbes polynomiales partant de l'origine
  // Chaque courbe va de (0,0) à (temps_tri, niveau_algo) avec courbure
  float levelSpacing = graphH / (SORT_COUNT + 1);

  for (int a = 0; a < SORT_COUNT; a++) {
    Color algoColor = GetAlgoColor(a);
    float targetY = graphY + (a + 1) * levelSpacing;

    double currentTime = 0;
    bool done = false;
    if (screen->sort_done[a]) {
      currentTime = screen->sort_times[a];
      done = true;
    } else if (screen->sort_running[a]) {
      currentTime = screen->sort_elapsed[a];
    }

    if (currentTime > 0) {
      float endX = graphX + (float)(currentTime / maxTime) * graphW;
      Vector2 endPoint = {endX, targetY};

      // Dessiner une courbe exponentielle avec plusieurs segments
      int segments = 25;
      Vector2 prevPoint = origin;
      float k = 2.5f; // Paramètre de courbure exponentielle
      float expK = expf(k);

      for (int s = 1; s <= segments; s++) {
        float t = (float)s / segments;

        // Courbe exponentielle: y monte rapidement puis ralentit
        // x avance linéairement, y suit une progression exponentielle
        float expProgress = (expf(k * t) - 1.0f) / (expK - 1.0f);

        float x = origin.x + t * (endPoint.x - origin.x);
        float y = origin.y + expProgress * (endPoint.y - origin.y);

        Vector2 currentPoint = {x, y};

        // Glow effect
        Color glowColor = algoColor;
        glowColor.a = 40;
        DrawLineEx(prevPoint, currentPoint, 6.0f, glowColor);
        glowColor.a = 80;
        DrawLineEx(prevPoint, currentPoint, 4.0f, glowColor);
        // Ligne principale
        DrawLineEx(prevPoint, currentPoint, 2.5f, algoColor);

        prevPoint = currentPoint;
      }

      // Point final
      if (done) {
        DrawCircleV(endPoint, 7, algoColor);
        DrawCircleV(endPoint, 4, COLOR_TEXT_WHITE);
      } else {
        float pulse = 0.7f + 0.3f * sinf(GetTime() * 5);
        DrawCircleV(endPoint, 5 * pulse, algoColor);
      }
    }
  }

  // Point d'origine
  DrawCircleV(origin, 4, COLOR_TEXT_WHITE);

  // Légende avec temps
  float legendX = bounds.x + bounds.width - 145;
  float legendY = bounds.y + 50;
  DrawText("Algorithmes:", legendX, legendY, 12, COLOR_TEXT_DIM);
  legendY += 20;
  for (int a = 0; a < SORT_COUNT; a++) {
    DrawRectangle(legendX, legendY + a * 22, 12, 12, GetAlgoColor(a));

    char legendLabel[64];
    if (screen->sort_done[a]) {
      snprintf(legendLabel, sizeof(legendLabel), "%s: %.1f ms", SORT_NAMES[a],
               screen->sort_times[a] * 1000.0);
    } else if (screen->sort_running[a]) {
      snprintf(legendLabel, sizeof(legendLabel), "%s: ...", SORT_NAMES[a]);
    } else {
      snprintf(legendLabel, sizeof(legendLabel), "%s", SORT_NAMES[a]);
    }
    DrawText(legendLabel, legendX + 18, legendY + a * 22, 11, COLOR_TEXT_MAIN);
  }
}

// ==================== SAISIE MANUELLE (Style Python) ====================

static void DrawManualInputDialog(TableauxScreen *screen) {
  DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 230});

  // Grande fenêtre
  Rectangle dialogRect = {60, 40, WINDOW_WIDTH - 120, WINDOW_HEIGHT - 80};
  DrawRectangleRounded(dialogRect, 0.015f, 8, COLOR_PANEL);
  DrawRectRoundedLinesThick(dialogRect, 0.015f, 8, 3, COLOR_NEON_BLUE);

  // Bouton X en haut à droite
  Rectangle closeBtn = {dialogRect.x + dialogRect.width - 45, dialogRect.y + 10,
                        35, 35};
  Vector2 mouse = GetMousePosition();
  bool closeHovered = CheckCollisionPointRec(mouse, closeBtn);

  DrawRectangleRounded(closeBtn, 0.3f, 4,
                       closeHovered ? COLOR_NEON_RED
                                    : (Color){80, 40, 40, 255});
  DrawText("X", closeBtn.x + 12, closeBtn.y + 7, 22, COLOR_TEXT_WHITE);

  // Titre
  char titleBuf[80];
  snprintf(titleBuf, sizeof(titleBuf), "SAISIE MANUELLE - %s",
           TYPE_NAMES[screen->data_type]);
  DrawText(titleBuf, dialogRect.x + 20, dialogRect.y + 15, 24,
           COLOR_NEON_ORANGE);

  // Compteur
  char counterBuf[40];
  snprintf(counterBuf, sizeof(counterBuf), "%d / %d elements",
           screen->manual_current_index, screen->manual_target_size);
  DrawText(counterBuf, dialogRect.x + dialogRect.width - 200, dialogRect.y + 20,
           18, COLOR_NEON_GREEN);

  // Barre de progression en haut
  float progress =
      (float)screen->manual_current_index / screen->manual_target_size;
  Rectangle progressBg = {dialogRect.x + 20, dialogRect.y + 55,
                          dialogRect.width - 40, 10};
  DrawRectangleRounded(progressBg, 0.5f, 4, COLOR_SECONDARY);
  Rectangle progressFill = {progressBg.x, progressBg.y,
                            progressBg.width * progress, progressBg.height};
  DrawRectangleRounded(progressFill, 0.5f, 4, COLOR_NEON_GREEN);

  // Zone scrollable des champs
  Rectangle fieldsRect = {dialogRect.x + 20, dialogRect.y + 80,
                          dialogRect.width - 40, dialogRect.height - 180};
  DrawRectangleRounded(fieldsRect, 0.01f, 4, COLOR_SECONDARY);
  DrawRectRoundedLinesThick(fieldsRect, 0.01f, 4, 1, COLOR_BORDER);

  BeginScissorMode(fieldsRect.x, fieldsRect.y, fieldsRect.width,
                   fieldsRect.height);

  // Layout 3 colonnes
  int cols = 3;
  int fieldWidth = (fieldsRect.width - 40) / cols;
  int fieldHeight = 45;
  int rowHeight = fieldHeight + 5;

  // Utiliser le scroll manuel
  int scrollOffset = (int)screen->manual_scroll_y;

  // Afficher tous les champs remplis + le champ courant
  int maxDisplay = (screen->manual_current_index >= screen->manual_filled_count)
                       ? screen->manual_current_index
                       : screen->manual_filled_count - 1;

  for (int i = 0; i <= maxDisplay && i < screen->manual_target_size; i++) {
    int row = i / cols;
    int col = i % cols;
    int xPos = fieldsRect.x + 10 + col * (fieldWidth + 5);
    int yPos = fieldsRect.y + 10 + row * rowHeight - scrollOffset;

    // Skip if outside visible area
    if (yPos < fieldsRect.y - fieldHeight ||
        yPos > fieldsRect.y + fieldsRect.height)
      continue;

    // Cadre du champ - taille adaptée au contenu
    Rectangle fieldBg = {xPos, yPos, fieldWidth - 5, fieldHeight - 5};

    bool isCurrent = (i == screen->manual_current_index);
    bool isFilled =
        (i < screen->manual_filled_count && i != screen->manual_current_index);

    if (isCurrent) {
      // Champ actif (orange)
      DrawRectangleRounded(fieldBg, 0.15f, 4, (Color){30, 25, 20, 255});
      DrawRectRoundedLinesThick(fieldBg, 0.15f, 4, 2, COLOR_NEON_ORANGE);
    } else if (isFilled) {
      // Champ rempli (vert)
      DrawRectangleRounded(fieldBg, 0.15f, 4, (Color){20, 35, 25, 255});
      DrawRectRoundedLinesThick(fieldBg, 0.15f, 4, 1, COLOR_NEON_GREEN);
    }

    // Numéro compact
    char numBuf[16];
    snprintf(numBuf, sizeof(numBuf), "%d:", i + 1);
    DrawText(numBuf, fieldBg.x + 5, fieldBg.y + 12, 14, COLOR_NEON_BLUE);

    // Valeur ou input
    if (isFilled) {
      char valBuf[256]; // Buffer plus grand pour valeurs complètes
      switch (screen->data_type) {
      case DATA_TYPE_INT:
        snprintf(valBuf, sizeof(valBuf), "%lld", screen->manual_int_values[i]);
        break;
      case DATA_TYPE_FLOAT:
        snprintf(valBuf, sizeof(valBuf), "%.2f",
                 screen->manual_float_values[i]);
        break;
      case DATA_TYPE_CHAR:
        snprintf(valBuf, sizeof(valBuf), "'%c'", screen->manual_char_values[i]);
        break;
      case DATA_TYPE_STRING:
        snprintf(valBuf, sizeof(valBuf), "%s",
                 screen->manual_string_values[i]); // Pas de truncation
        break;
      }
      // Tronquer l'affichage si trop long (mais valeur conservée)
      int maxDisplayLen = (fieldBg.width - 45) / 7; // ~7 pixels par char
      if ((int)strlen(valBuf) > maxDisplayLen && maxDisplayLen > 3) {
        valBuf[maxDisplayLen - 2] = '.';
        valBuf[maxDisplayLen - 1] = '.';
        valBuf[maxDisplayLen] = '\0';
      }
      DrawText(valBuf, fieldBg.x + 35, fieldBg.y + 12, 14, COLOR_NEON_GREEN);
    } else if (isCurrent) {
      // Champ de saisie actif
      Rectangle inputArea = {fieldBg.x + 35, fieldBg.y + 5, fieldBg.width - 45,
                             28};
      DrawRectangleRounded(inputArea, 0.2f, 4, (Color){15, 15, 20, 255});

      // Texte saisi
      DrawText(screen->manual_input.text, inputArea.x + 5, inputArea.y + 5, 14,
               COLOR_TEXT_WHITE);

      // Curseur clignotant
      if ((int)(GetTime() * 2) % 2 == 0) {
        int textW = MeasureText(screen->manual_input.text, 14);
        DrawRectangle(inputArea.x + 5 + textW, inputArea.y + 4, 2, 18,
                      COLOR_NEON_ORANGE);
      }
    }
  }

  EndScissorMode();

  // Zone d'instructions en bas
  Rectangle hintRect = {dialogRect.x + 20,
                        dialogRect.y + dialogRect.height - 90,
                        dialogRect.width - 40, 70};
  DrawRectangleRounded(hintRect, 0.05f, 4, (Color){20, 25, 35, 255});

  DrawText("ENTREE : Valider et ajouter suivant", hintRect.x + 20,
           hintRect.y + 12, 14, COLOR_TEXT_MAIN);
  DrawText("MOLETTE : Faire defiler | CLIC : Modifier un champ",
           hintRect.x + 20, hintRect.y + 32, 14, COLOR_TEXT_MAIN);

  // Info: Dernier champ auto-valide
  DrawText("Le dernier champ valide automatiquement la saisie", hintRect.x + 20,
           hintRect.y + 50, 12, COLOR_TEXT_DIM);
}

static bool UpdateManualInputCloseButton(TableauxScreen *screen) {
  Rectangle dialogRect = {60, 40, WINDOW_WIDTH - 120, WINDOW_HEIGHT - 80};
  Rectangle closeBtn = {dialogRect.x + dialogRect.width - 45, dialogRect.y + 10,
                        35, 35};

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, closeBtn)) {
      return true; // Fermer
    }

    // Bouton Terminer
    if (screen->manual_current_index >= screen->manual_target_size) {
      Rectangle hintRect = {dialogRect.x + 20,
                            dialogRect.y + dialogRect.height - 90,
                            dialogRect.width - 40, 70};
      Rectangle finishBtn = {hintRect.x + hintRect.width - 200, hintRect.y + 15,
                             180, 40};
      if (CheckCollisionPointRec(mouse, finishBtn)) {
        return true; // Terminer
      }
    }
  }

  return false;
}

// ==================== DROPDOWN TYPE ====================

static void DrawTypeDropdown(TableauxScreen *screen, float x, float y) {
  Rectangle mainBtn = {x, y, 120, 36};
  Color btnColor =
      screen->type_dropdown_open ? COLOR_NEON_PURPLE : COLOR_NEON_BLUE;
  DrawRectangleRounded(mainBtn, 0.3f, 6, btnColor);

  char label[32];
  snprintf(label, sizeof(label), "%s v", TYPE_NAMES[screen->data_type]);
  int textW = MeasureText(label, 14);
  DrawText(label, mainBtn.x + (mainBtn.width - textW) / 2, mainBtn.y + 10, 14,
           COLOR_TEXT_WHITE);

  if (screen->type_dropdown_open) {
    Rectangle dropdownBg = {x, y + 40, 120, 4 * 32};
    DrawRectangleRounded(dropdownBg, 0.1f, 4, COLOR_PANEL);
    DrawRectRoundedLinesThick(dropdownBg, 0.1f, 4, 2, COLOR_NEON_BLUE);

    for (int i = 0; i < 4; i++) {
      Rectangle itemRect = {x + 2, y + 42 + i * 32, 116, 30};
      Vector2 mouse = GetMousePosition();
      bool hovered = CheckCollisionPointRec(mouse, itemRect);

      if (i == (int)screen->data_type) {
        DrawRectangleRounded(itemRect, 0.2f, 4, COLOR_NEON_GREEN);
        DrawText(TYPE_NAMES[i], itemRect.x + 10, itemRect.y + 7, 13,
                 COLOR_BG_DARK);
      } else if (hovered) {
        DrawRectangleRounded(itemRect, 0.2f, 4, (Color){60, 80, 100, 255});
        DrawText(TYPE_NAMES[i], itemRect.x + 10, itemRect.y + 7, 13,
                 COLOR_TEXT_WHITE);
      } else {
        DrawText(TYPE_NAMES[i], itemRect.x + 10, itemRect.y + 7, 13,
                 COLOR_TEXT_MAIN);
      }
    }
  }
}

static void UpdateTypeDropdown(TableauxScreen *screen, float x, float y) {
  Rectangle mainBtn = {x, y, 120, 36};
  Vector2 mouse = GetMousePosition();

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (CheckCollisionPointRec(mouse, mainBtn)) {
      screen->type_dropdown_open = !screen->type_dropdown_open;
    } else if (screen->type_dropdown_open) {
      for (int i = 0; i < 4; i++) {
        Rectangle itemRect = {x + 2, y + 42 + i * 32, 116, 30};
        if (CheckCollisionPointRec(mouse, itemRect)) {
          screen->data_type = i;
          screen->type_dropdown_open = false;
          return;
        }
      }
      screen->type_dropdown_open = false;
    }
  }
}

// ==================== INITIALISATION ====================

void TableauxInit(TableauxScreen *screen) {
  memset(screen, 0, sizeof(TableauxScreen));

  screen->state = TAB_STATE_IDLE;
  screen->data_type = DATA_TYPE_INT;
  screen->selected_sort = -1;
  screen->type_dropdown_open = false;

  // Back removed
  screen->input_size = CreateInputBox(20, 60 + NAVBAR_HEIGHT, 120, 38, 8);
  strcpy(screen->input_size.text, "1000");
  screen->input_size.cursor = 4;

  screen->btn_type_dropdown = CreateButton(160, 60 + NAVBAR_HEIGHT, 120, 38,
                                           "Entiers v", COLOR_NEON_BLUE);
  screen->btn_generate = CreateButton(300, 60 + NAVBAR_HEIGHT, 120, 38,
                                      "Aleatoire", COLOR_NEON_GREEN);
  screen->btn_manual = CreateButton(440, 60 + NAVBAR_HEIGHT, 100, 38, "Manuel",
                                    COLOR_NEON_GREEN);

  int sortBtnX = 20;
  for (int i = 0; i < SORT_COUNT; i++) {
    screen->btn_sort[i] = CreateButton(sortBtnX + i * 155, 110 + NAVBAR_HEIGHT,
                                       145, 38, SORT_NAMES[i], GetAlgoColor(i));
  }
  // Boutons à l'extrémité droite
  screen->btn_stop = CreateButton(WINDOW_WIDTH - 300, 110 + NAVBAR_HEIGHT, 80,
                                  38, "Stop", COLOR_NEON_RED);
  screen->btn_graph = CreateButton(WINDOW_WIDTH - 210, 110 + NAVBAR_HEIGHT, 100,
                                   38, "Graphique", COLOR_NEON_BLUE);
  screen->btn_save = CreateButton(WINDOW_WIDTH - 100, 110 + NAVBAR_HEIGHT, 80,
                                  38, "Save", COLOR_NEON_GREEN);

  screen->manual_input = CreateInputBox(0, 0, 300, 40, 64);
  screen->manual_input_active = false;
  screen->show_graph = false;
  screen->history.count = 0;
}

// ==================== UPDATE ====================

AppScreen TableauxUpdate(TableauxScreen *screen) {
  float dt = GetFrameTime();

  // Mode saisie manuelle
  if (screen->manual_input_active) {
    // Bouton X ou Terminer
    if (UpdateManualInputCloseButton(screen)) {
      // Si terminé, créer les données
      if (screen->manual_filled_count >= screen->manual_target_size) {
        screen->original_data = (GenericData *)malloc(sizeof(GenericData));
        screen->original_data->type = screen->data_type;
        screen->original_data->size = screen->manual_target_size;

        switch (screen->data_type) {
        case DATA_TYPE_INT:
          screen->original_data->data.int_data = (long long *)malloc(
              screen->manual_target_size * sizeof(long long));
          memcpy(screen->original_data->data.int_data,
                 screen->manual_int_values,
                 screen->manual_target_size * sizeof(long long));
          break;
        case DATA_TYPE_FLOAT:
          screen->original_data->data.float_data =
              (double *)malloc(screen->manual_target_size * sizeof(double));
          memcpy(screen->original_data->data.float_data,
                 screen->manual_float_values,
                 screen->manual_target_size * sizeof(double));
          break;
        case DATA_TYPE_CHAR:
          screen->original_data->data.char_data =
              (char *)malloc(screen->manual_target_size * sizeof(char));
          memcpy(screen->original_data->data.char_data,
                 screen->manual_char_values,
                 screen->manual_target_size * sizeof(char));
          break;
        case DATA_TYPE_STRING:
          screen->original_data->data.string_data =
              (char **)malloc(screen->manual_target_size * sizeof(char *));
          for (int i = 0; i < screen->manual_target_size; i++) {
            screen->original_data->data.string_data[i] =
                screen->manual_string_values[i];
          }
          break;
        }

        screen->data_size = screen->manual_target_size;
        screen->state = TAB_STATE_HAS_DATA;
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Saisie terminee: %d elements", screen->data_size);
        screen->status_timer = 3.0f;
      } else {
        // Annulé via X
        if (screen->data_type == DATA_TYPE_STRING) {
          for (int i = 0; i < screen->manual_current_index; i++) {
            if (screen->manual_string_values[i]) {
              free(screen->manual_string_values[i]);
              screen->manual_string_values[i] = NULL;
            }
          }
        }
        snprintf(screen->status_message, sizeof(screen->status_message),
                 "Saisie annulee");
        screen->status_timer = 2.0f;
      }
      screen->manual_input_active = false;
      return SCREEN_TABLEAUX;
    }

    UpdateInputBox(&screen->manual_input);

    // Entrée valide
    if (IsKeyPressed(KEY_ENTER) &&
        screen->manual_current_index < screen->manual_target_size) {
      bool valid = false;

      switch (screen->data_type) {
      case DATA_TYPE_INT:
        if (strlen(screen->manual_input.text) > 0) {
          screen->manual_int_values[screen->manual_current_index] =
              atoll(screen->manual_input.text);
          valid = true;
        }
        break;
      case DATA_TYPE_FLOAT:
        if (strlen(screen->manual_input.text) > 0) {
          screen->manual_float_values[screen->manual_current_index] =
              atof(screen->manual_input.text);
          valid = true;
        }
        break;
      case DATA_TYPE_CHAR:
        if (strlen(screen->manual_input.text) > 0) {
          screen->manual_char_values[screen->manual_current_index] =
              screen->manual_input.text[0];
          valid = true;
        }
        break;
      case DATA_TYPE_STRING:
        if (strlen(screen->manual_input.text) > 0) {
          screen->manual_string_values[screen->manual_current_index] =
              strdup(screen->manual_input.text);
          valid = true;
        }
        break;
      }

      if (valid) {
        // Si on édite un nouveau champ (pas une ré-édition)
        if (screen->manual_current_index >= screen->manual_filled_count) {
          screen->manual_filled_count = screen->manual_current_index + 1;

          // Auto-submit si c'était le dernier champ
          if (screen->manual_filled_count >= screen->manual_target_size) {
            // Créer les données automatiquement
            screen->original_data = (GenericData *)malloc(sizeof(GenericData));
            screen->original_data->type = screen->data_type;
            screen->original_data->size = screen->manual_target_size;

            switch (screen->data_type) {
            case DATA_TYPE_INT:
              screen->original_data->data.int_data = (long long *)malloc(
                  screen->manual_target_size * sizeof(long long));
              memcpy(screen->original_data->data.int_data,
                     screen->manual_int_values,
                     screen->manual_target_size * sizeof(long long));
              break;
            case DATA_TYPE_FLOAT:
              screen->original_data->data.float_data =
                  (double *)malloc(screen->manual_target_size * sizeof(double));
              memcpy(screen->original_data->data.float_data,
                     screen->manual_float_values,
                     screen->manual_target_size * sizeof(double));
              break;
            case DATA_TYPE_CHAR:
              screen->original_data->data.char_data =
                  (char *)malloc(screen->manual_target_size * sizeof(char));
              memcpy(screen->original_data->data.char_data,
                     screen->manual_char_values,
                     screen->manual_target_size * sizeof(char));
              break;
            case DATA_TYPE_STRING:
              screen->original_data->data.string_data =
                  (char **)malloc(screen->manual_target_size * sizeof(char *));
              for (int i = 0; i < screen->manual_target_size; i++) {
                screen->original_data->data.string_data[i] =
                    screen->manual_string_values[i];
              }
              break;
            }

            screen->data_size = screen->manual_target_size;
            screen->state = TAB_STATE_HAS_DATA;
            screen->manual_input_active = false;
            snprintf(screen->status_message, sizeof(screen->status_message),
                     "Saisie terminee: %d elements", screen->data_size);
            screen->status_timer = 3.0f;
            return SCREEN_TABLEAUX;
          }

          screen->manual_current_index++; // Passer au suivant normalement
        } else {
          // Ré-édition d'un champ existant: retourner à la dernière position
          screen->manual_current_index = screen->manual_filled_count;
        }
        screen->manual_input.text[0] = '\0';
        screen->manual_input.cursor = 0;

        // Auto-scroll pour garder le champ courant visible (basé sur les
        // lignes) Avec 3 colonnes, ne scroller que quand on change de ligne
        int cols = 3;
        int rowHeight = 50; // fieldHeight + padding
        int currentRow = screen->manual_current_index / cols;
        float visibleHeight = 400; // Hauteur zone visible approximative
        float currentRowY = currentRow * rowHeight;

        // Scroller seulement si la nouvelle ligne est hors de la zone visible
        if (currentRowY >
            screen->manual_scroll_y + visibleHeight - rowHeight * 2) {
          screen->manual_scroll_y = currentRowY - visibleHeight + rowHeight * 2;
        }
      }
    }

    // ---------------------------------------------------------
    // NAVIGATION (Clic seulement, pas de retour arrière)
    // ---------------------------------------------------------

    int prev_index = screen->manual_current_index;

    // Scroll avec molette souris
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
      screen->manual_scroll_y -= wheel * 50;
      if (screen->manual_scroll_y < 0)
        screen->manual_scroll_y = 0;
      // Max scroll
      int fieldHeight = 50;
      float maxScroll = (screen->manual_filled_count + 1) * fieldHeight - 300;
      if (maxScroll < 0)
        maxScroll = 0;
      if (screen->manual_scroll_y > maxScroll)
        screen->manual_scroll_y = maxScroll;
    }

    // 2. Clic souris pour sélectionner un champ précédent (layout 3 colonnes)
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Rectangle dialogRect = {60, 40, WINDOW_WIDTH - 120, WINDOW_HEIGHT - 80};
      Rectangle fieldsRect = {dialogRect.x + 20, dialogRect.y + 80,
                              dialogRect.width - 40, dialogRect.height - 180};

      if (CheckCollisionPointRec(GetMousePosition(), fieldsRect)) {
        // Paramètres du layout 3 colonnes (doivent correspondre au draw)
        int cols = 3;
        int fieldWidth = (fieldsRect.width - 40) / cols;
        int fieldHeight = 45;
        int rowHeight = fieldHeight + 5;
        int scrollOffset = (int)screen->manual_scroll_y;

        int maxDisplay =
            (screen->manual_current_index >= screen->manual_filled_count)
                ? screen->manual_current_index
                : screen->manual_filled_count - 1;

        Vector2 mouse = GetMousePosition();
        for (int i = 0; i <= maxDisplay && i < screen->manual_target_size;
             i++) {
          int row = i / cols;
          int col = i % cols;
          int xPos = fieldsRect.x + 10 + col * (fieldWidth + 5);
          int yPos = fieldsRect.y + 10 + row * rowHeight - scrollOffset;

          // Skip si hors zone visible
          if (yPos < fieldsRect.y - fieldHeight ||
              yPos > fieldsRect.y + fieldsRect.height)
            continue;

          Rectangle fieldBg = {xPos, yPos, fieldWidth - 5, fieldHeight - 5};

          if (CheckCollisionPointRec(mouse, fieldBg)) {
            screen->manual_current_index = i;
            screen->manual_input.is_focused = true;
            // PAS de changement de scroll ici - on garde la position actuelle
            break;
          }
        }
      }
    }

    // 3. Rechargement de la valeur si changement d'index
    if (screen->manual_current_index != prev_index) {
      screen->manual_input.text[0] = '\0';

      switch (screen->data_type) {
      case DATA_TYPE_INT:
        snprintf(screen->manual_input.text, sizeof(screen->manual_input.text),
                 "%lld",
                 screen->manual_int_values[screen->manual_current_index]);
        break;
      case DATA_TYPE_FLOAT:
        snprintf(screen->manual_input.text, sizeof(screen->manual_input.text),
                 "%.2f",
                 screen->manual_float_values[screen->manual_current_index]);
        break;
      case DATA_TYPE_CHAR:
        snprintf(screen->manual_input.text, sizeof(screen->manual_input.text),
                 "%c",
                 screen->manual_char_values[screen->manual_current_index]);
        break;
      case DATA_TYPE_STRING:
        if (screen->manual_string_values[screen->manual_current_index]) {
          strncpy(screen->manual_input.text,
                  screen->manual_string_values[screen->manual_current_index],
                  sizeof(screen->manual_input.text) - 1);
        }
        break;
      }
      screen->manual_input.cursor = strlen(screen->manual_input.text);
    }

    return SCREEN_TABLEAUX;
  }

  AppScreen next_screen = SCREEN_TABLEAUX;
  if (UpdateNavigationBar(&next_screen)) {
    StopAllSorts(&screen->sort_manager);
    ResetScreen(screen);
    return next_screen;
  }

  if (UpdateButton(&screen->btn_graph)) {
    screen->show_graph = !screen->show_graph;
  }

  UpdateTypeDropdown(screen, 160, 60 + NAVBAR_HEIGHT);
  UpdateInputBox(&screen->input_size);

  if (UpdateButton(&screen->btn_generate) &&
      screen->state != TAB_STATE_SORTING && !screen->type_dropdown_open) {
    int size = atoi(screen->input_size.text);
    if (size > 0) {
      ResetScreen(screen);
      screen->data_size = size;
      screen->original_data = GenerateTypedData(size, screen->data_type);
      screen->state = TAB_STATE_HAS_DATA;
      snprintf(screen->status_message, sizeof(screen->status_message),
               "%d %s generes", size, TYPE_NAMES[screen->data_type]);
      screen->status_timer = 3.0f;
    }
  }

  if (UpdateButton(&screen->btn_manual) && screen->state != TAB_STATE_SORTING &&
      !screen->type_dropdown_open) {
    int size = atoi(screen->input_size.text);
    if (size > 0) {
      ResetScreen(screen);
      // Allouer dynamiquement les tableaux
      screen->manual_int_values = (long long *)malloc(size * sizeof(long long));
      screen->manual_float_values = (double *)malloc(size * sizeof(double));
      screen->manual_char_values = (char *)malloc(size * sizeof(char));
      screen->manual_string_values = (char **)malloc(size * sizeof(char *));
      for (int i = 0; i < size; i++)
        screen->manual_string_values[i] = NULL;

      screen->manual_target_size = size;
      screen->manual_current_index = 0;
      screen->manual_filled_count = 0;
      screen->manual_input.text[0] = '\0';
      screen->manual_input.cursor = 0;
      screen->manual_input.is_focused = true;
      screen->manual_input_active = true;
    }
  }

  if (!screen->type_dropdown_open) {
    for (int i = 0; i < SORT_COUNT; i++) {
      if (UpdateButton(&screen->btn_sort[i]) &&
          screen->state == TAB_STATE_HAS_DATA) {
        ClearSortedData(screen);
        screen->state = TAB_STATE_SORTING;
        screen->selected_sort = i;
        StartAllSorts(&screen->sort_manager, screen->original_data);
        screen->global_start_time = GetTime();

        for (int j = 0; j < SORT_COUNT; j++) {
          screen->sort_start_times[j] = screen->global_start_time;
          screen->sort_running[j] = true;
          screen->sort_done[j] = false;
          screen->sort_progress_count[j] = 0; // Reset progress tracking
        }
      }
    }
  }

  if (UpdateButton(&screen->btn_stop) && screen->state == TAB_STATE_SORTING) {
    StopAllSorts(&screen->sort_manager);
    for (int i = 0; i < SORT_COUNT; i++)
      screen->sort_running[i] = false;
    screen->state = TAB_STATE_HAS_DATA;
  }

  // Bouton sauvegarder les résultats
  if (UpdateButton(&screen->btn_save) && !screen->type_dropdown_open) {
    FILE *f = fopen("resultats_tri.txt", "w");
    if (f) {
      fprintf(f, "========================================\n");
      fprintf(f, "RESULTATS DES TRIS\n");
      fprintf(f, "========================================\n");
      fprintf(f, "Taille du tableau: %d elements\n", screen->data_size);
      fprintf(f, "Type de donnees: %s\n", TYPE_NAMES[screen->data_type]);
      fprintf(f, "\n--- TEMPS DE TRI ---\n");

      for (int i = 0; i < SORT_COUNT; i++) {
        if (screen->sort_done[i]) {
          fprintf(f, "%-15s : %.3f ms\n", SORT_NAMES[i],
                  screen->sort_times[i] * 1000.0);
        } else if (screen->sort_running[i]) {
          fprintf(f, "%-15s : En cours...\n", SORT_NAMES[i]);
        } else {
          fprintf(f, "%-15s : Non execute\n", SORT_NAMES[i]);
        }
      }

      // Sauvegarder le tableau AVANT tri
      fprintf(f, "\n--- DONNEES AVANT TRI ---\n");
      if (screen->original_data && screen->original_data->size > 0) {
        for (int i = 0; i < screen->original_data->size; i++) {
          switch (screen->original_data->type) {
          case DATA_TYPE_INT:
            fprintf(f, "%lld", screen->original_data->data.int_data[i]);
            break;
          case DATA_TYPE_FLOAT:
            fprintf(f, "%.2f", screen->original_data->data.float_data[i]);
            break;
          case DATA_TYPE_CHAR:
            fprintf(f, "%c", screen->original_data->data.char_data[i]);
            break;
          case DATA_TYPE_STRING:
            fprintf(f, "%s", screen->original_data->data.string_data[i]);
            break;
          }
          if (i < screen->original_data->size - 1)
            fprintf(f, ", ");
          if ((i + 1) % 10 == 0)
            fprintf(f, "\n");
        }
        fprintf(f, "\n");
      }

      // Sauvegarder le tableau APRES tri
      fprintf(f, "\n--- DONNEES APRES TRI ---\n");
      if (screen->display_sorted && screen->display_sorted->size > 0) {
        for (int i = 0; i < screen->display_sorted->size; i++) {
          switch (screen->display_sorted->type) {
          case DATA_TYPE_INT:
            fprintf(f, "%lld", screen->display_sorted->data.int_data[i]);
            break;
          case DATA_TYPE_FLOAT:
            fprintf(f, "%.2f", screen->display_sorted->data.float_data[i]);
            break;
          case DATA_TYPE_CHAR:
            fprintf(f, "%c", screen->display_sorted->data.char_data[i]);
            break;
          case DATA_TYPE_STRING:
            fprintf(f, "%s", screen->display_sorted->data.string_data[i]);
            break;
          }
          if (i < screen->display_sorted->size - 1)
            fprintf(f, ", ");
          if ((i + 1) % 10 == 0)
            fprintf(f, "\n");
        }
        fprintf(f, "\n");
      }

      fprintf(f, "\n========================================\n");
      fclose(f);

      snprintf(screen->status_message, sizeof(screen->status_message),
               "Resultats sauvegardes dans resultats_tri.txt");
      screen->status_timer = 3.0f;
    }
  }

  if (screen->state == TAB_STATE_SORTING) {
    double now = GetTime();
    screen->elapsed_time = now - screen->global_start_time;

    bool all_done = true;
    for (int i = 0; i < SORT_COUNT; i++) {
      if (screen->sort_manager.threads[i].completed && !screen->sort_done[i]) {
        screen->sort_done[i] = true;
        screen->sort_running[i] = false;
        screen->sort_times[i] = screen->sort_manager.threads[i].elapsed_time;
        if (i == screen->selected_sort) {
          screen->display_sorted =
              CopyGenericData(screen->sort_manager.threads[i].data);
        }
      }
      if (screen->sort_running[i]) {
        screen->sort_elapsed[i] = now - screen->sort_start_times[i];

        // Enregistrer un point de progression toutes les 50ms environ
        int pc = screen->sort_progress_count[i];
        if (pc < 100) {
          double lastT = (pc > 0) ? screen->sort_progress_times[i][pc - 1] : 0;
          if (screen->sort_elapsed[i] - lastT >= 0.05) { // 50ms
            screen->sort_progress_times[i][pc] = screen->sort_elapsed[i];
            // Estimer la progression (basée sur le temps relatif)
            float estimatedProgress =
                (float)(screen->sort_elapsed[i] * 0.5); // Approximation
            if (estimatedProgress > 0.95f)
              estimatedProgress = 0.95f;
            screen->sort_progress_values[i][pc] = estimatedProgress;
            screen->sort_progress_count[i]++;
          }
        }

        all_done = false;
      }
      if (!screen->sort_done[i])
        all_done = false;
    }

    if (all_done) {
      screen->state = TAB_STATE_DONE;
      if (screen->history.count < 20) {
        int idx = screen->history.count;
        screen->history.sizes[idx] = screen->data_size;
        for (int a = 0; a < SORT_COUNT; a++) {
          screen->history.times[a][idx] = screen->sort_times[a];
        }
        screen->history.count++;
      }
    }
  }

  if (screen->status_timer > 0)
    screen->status_timer -= dt;

  float wheel = GetMouseWheelMove();
  if (wheel != 0 && !screen->type_dropdown_open && !screen->show_graph) {
    Vector2 mouse = GetMousePosition();
    float panelWidth = (WINDOW_WIDTH - 50) / 2.0f;
    Rectangle beforeRect = {20, 175, panelWidth, 320};
    Rectangle afterRect = {beforeRect.x + beforeRect.width + 10, 175,
                           panelWidth, 320};

    if (CheckCollisionPointRec(mouse, beforeRect)) {
      screen->scroll_before_y -= wheel * 40;
      if (screen->scroll_before_y < 0)
        screen->scroll_before_y = 0;
    } else if (CheckCollisionPointRec(mouse, afterRect)) {
      screen->scroll_after_y -= wheel * 40;
      if (screen->scroll_after_y < 0)
        screen->scroll_after_y = 0;
    }
  }

  return SCREEN_TABLEAUX;
}

// ==================== DRAW ====================

void TableauxDraw(TableauxScreen *screen) {
  DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_BG_DARK,
                         COLOR_BG_LIGHT);

  // Navigation Bar
  DrawNavigationBar(SCREEN_TABLEAUX);

  DrawText("Taille:", 20, 45 + NAVBAR_HEIGHT, 12, COLOR_TEXT_DIM);
  DrawInputBox(&screen->input_size);
  DrawText("Type:", 160, 45 + NAVBAR_HEIGHT, 12, COLOR_TEXT_DIM);
  DrawButton(&screen->btn_generate);
  DrawButton(&screen->btn_manual);

  DrawText("Algorithmes (cliquez pour trier):", 20, 95 + NAVBAR_HEIGHT, 12,
           COLOR_TEXT_DIM);
  for (int i = 0; i < SORT_COUNT; i++)
    DrawButton(&screen->btn_sort[i]);
  DrawButton(&screen->btn_stop);

  screen->btn_graph.text = screen->show_graph ? "< Tableau" : "Graphique";
  screen->btn_graph.color =
      screen->show_graph ? COLOR_NEON_ORANGE : COLOR_NEON_BLUE;
  DrawButton(&screen->btn_graph);
  DrawButton(&screen->btn_save);

  if (screen->status_timer > 0) {
    float alpha = fminf(1.0f, screen->status_timer);
    Color msgColor = COLOR_NEON_GREEN;
    msgColor.a = (unsigned char)(alpha * 255);
    DrawText(screen->status_message, WINDOW_WIDTH - 300, WINDOW_HEIGHT - 22, 14,
             msgColor);
  }

  if (screen->show_graph) {
    DrawPerformanceGraph(screen,
                         (Rectangle){20, 190 + NAVBAR_HEIGHT, WINDOW_WIDTH - 40,
                                     WINDOW_HEIGHT - 210 - NAVBAR_HEIGHT});
  } else {
    float panelWidth = (WINDOW_WIDTH - 50) / 2.0f;
    Rectangle beforeRect = {20, 190 + NAVBAR_HEIGHT, panelWidth, 300};
    Rectangle afterRect = {beforeRect.x + beforeRect.width + 10,
                           190 + NAVBAR_HEIGHT, panelWidth, 300};

    DrawDataPanel(screen->original_data, beforeRect,
                  "AVANT - Donnees Initiales", screen->scroll_before_y,
                  COLOR_NEON_BLUE);
    DrawDataPanel(screen->display_sorted, afterRect, "APRES - Donnees Triees",
                  screen->scroll_after_y, COLOR_NEON_GREEN);

    Rectangle statusRect = {20, 510 + NAVBAR_HEIGHT, WINDOW_WIDTH - 40,
                            WINDOW_HEIGHT - 530 - NAVBAR_HEIGHT};
    DrawPanel(statusRect, "Resultats", COLOR_NEON_PURPLE);

    float colWidth = (statusRect.width - 40) / SORT_COUNT;

    for (int i = 0; i < SORT_COUNT; i++) {
      float x = statusRect.x + 20 + i * colWidth;
      float y = statusRect.y + 28;

      bool isSelected =
          (i == screen->selected_sort && screen->state != TAB_STATE_IDLE);
      if (isSelected) {
        // Correction de la hauteuret position pour ne pas sortir du cadre
        // StatusRect y=510, h=85 -> max Y = 595
        // y = 538.
        // Hauteur max dispo sous y: 595 - 530 = ~65px
        Rectangle selRect = {x - 8, y - 6, colWidth - 15, 60};
        DrawRectRoundedLinesThick(selRect, 0.15f, 4, 3, GetAlgoColor(i));
      }

      DrawText(SORT_NAMES[i], x, y, 14, GetAlgoColor(i));

      char timeStr[32];
      if (screen->sort_done[i]) {
        FormatTimeString(screen->sort_times[i], timeStr, sizeof(timeStr));
        DrawText(timeStr, x, y + 22, 18, COLOR_NEON_GREEN);
      } else if (screen->sort_running[i]) {
        FormatTimeString(screen->sort_elapsed[i], timeStr, sizeof(timeStr));
        DrawText(timeStr, x, y + 22, 18, (Color){255, 193, 7, 255});
        DrawText("...", x + 80, y + 24, 14, (Color){255, 193, 7, 255});
      } else {
        DrawText("--", x, y + 22, 18, COLOR_TEXT_DIM);
      }
    }

    if (screen->original_data) {
      char infoStr[80];
      snprintf(infoStr, sizeof(infoStr), "%d elements  |  Type: %s",
               screen->data_size, TYPE_NAMES[screen->data_type]);
      DrawText(infoStr, 20, WINDOW_HEIGHT - 22, 13, COLOR_TEXT_MAIN);
    }
  }

  DrawTypeDropdown(screen, 160, 60 + NAVBAR_HEIGHT);

  if (screen->manual_input_active) {
    DrawManualInputDialog(screen);
  }
}

void TableauxUnload(TableauxScreen *screen) {
  StopAllSorts(&screen->sort_manager);
  ResetScreen(screen);
}
