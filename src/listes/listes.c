

#include "listes.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==================== LISTE SIMPLEMENT CHAÎNÉE ====================

void ListeSimple_Init(ListeSimple *liste) {
  liste->head = NULL;
  liste->count = 0;
}

void ListeSimple_Free(ListeSimple *liste) { ListeSimple_Vider(liste); }

void ListeSimple_InsererDebut(ListeSimple *liste, const char *valeur) {
  NodeSimple *nouveau = (NodeSimple *)malloc(sizeof(NodeSimple));
  strncpy(nouveau->data, valeur, 31);
  nouveau->data[31] = '\0';
  nouveau->next = liste->head;
  liste->head = nouveau;
  liste->count++;
}

void ListeSimple_InsererFin(ListeSimple *liste, const char *valeur) {
  NodeSimple *nouveau = (NodeSimple *)malloc(sizeof(NodeSimple));
  strncpy(nouveau->data, valeur, 31);
  nouveau->data[31] = '\0';
  nouveau->next = NULL;

  if (liste->head == NULL) {
    liste->head = nouveau;
  } else {
    NodeSimple *current = liste->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = nouveau;
  }
  liste->count++;
}

void ListeSimple_InsererPosition(ListeSimple *liste, const char *valeur,
                                 int pos) {
  if (pos <= 0) {
    ListeSimple_InsererDebut(liste, valeur);
    return;
  }
  if (pos >= liste->count) {
    ListeSimple_InsererFin(liste, valeur);
    return;
  }

  NodeSimple *current = liste->head;
  for (int i = 0; i < pos - 1; i++) {
    current = current->next;
  }

  NodeSimple *nouveau = (NodeSimple *)malloc(sizeof(NodeSimple));
  strncpy(nouveau->data, valeur, 31);
  nouveau->data[31] = '\0';
  nouveau->next = current->next;
  current->next = nouveau;
  liste->count++;
}

bool ListeSimple_SupprimerDebut(ListeSimple *liste) {
  if (liste->head == NULL)
    return false;

  NodeSimple *temp = liste->head;
  liste->head = liste->head->next;
  free(temp);
  liste->count--;
  return true;
}

bool ListeSimple_SupprimerFin(ListeSimple *liste) {
  if (liste->head == NULL)
    return false;

  if (liste->head->next == NULL) {
    free(liste->head);
    liste->head = NULL;
    liste->count--;
    return true;
  }

  NodeSimple *current = liste->head;
  while (current->next->next != NULL) {
    current = current->next;
  }

  free(current->next);
  current->next = NULL;
  liste->count--;
  return true;
}

bool ListeSimple_SupprimerPosition(ListeSimple *liste, int pos) {
  if (liste->head == NULL || pos < 0 || pos >= liste->count)
    return false;

  if (pos == 0)
    return ListeSimple_SupprimerDebut(liste);

  NodeSimple *current = liste->head;
  for (int i = 0; i < pos - 1; i++) {
    current = current->next;
  }

  NodeSimple *temp = current->next;
  current->next = temp->next;
  free(temp);
  liste->count--;
  return true;
}

bool ListeSimple_SupprimerValeur(ListeSimple *liste, const char *valeur) {
  if (liste->head == NULL)
    return false;

  if (strcmp(liste->head->data, valeur) == 0) {
    return ListeSimple_SupprimerDebut(liste);
  }

  NodeSimple *current = liste->head;
  while (current->next != NULL && strcmp(current->next->data, valeur) != 0) {
    current = current->next;
  }

  if (current->next == NULL)
    return false;

  NodeSimple *temp = current->next;
  current->next = temp->next;
  free(temp);
  liste->count--;
  return true;
}

NodeSimple *ListeSimple_Rechercher(ListeSimple *liste, const char *valeur) {
  NodeSimple *current = liste->head;
  while (current != NULL) {
    if (strcmp(current->data, valeur) == 0)
      return current;
    current = current->next;
  }
  return NULL;
}

int ListeSimple_GetPosition(ListeSimple *liste, const char *valeur) {
  NodeSimple *current = liste->head;
  int pos = 0;
  while (current != NULL) {
    if (strcmp(current->data, valeur) == 0)
      return pos;
    current = current->next;
    pos++;
  }
  return -1;
}

void ListeSimple_Vider(ListeSimple *liste) {
  while (liste->head != NULL) {
    ListeSimple_SupprimerDebut(liste);
  }
}

// ==================== LISTE DOUBLEMENT CHAÎNÉE ====================

void ListeDouble_Init(ListeDouble *liste) {
  liste->head = NULL;
  liste->tail = NULL;
  liste->count = 0;
}

void ListeDouble_Free(ListeDouble *liste) { ListeDouble_Vider(liste); }

void ListeDouble_InsererDebut(ListeDouble *liste, const char *valeur) {
  NodeDouble *nouveau = (NodeDouble *)malloc(sizeof(NodeDouble));
  strncpy(nouveau->data, valeur, 31);
  nouveau->data[31] = '\0';
  nouveau->prev = NULL;
  nouveau->next = liste->head;

  if (liste->head != NULL) {
    liste->head->prev = nouveau;
  } else {
    liste->tail = nouveau;
  }

  liste->head = nouveau;
  liste->count++;
}

void ListeDouble_InsererFin(ListeDouble *liste, const char *valeur) {
  NodeDouble *nouveau = (NodeDouble *)malloc(sizeof(NodeDouble));
  strncpy(nouveau->data, valeur, 31);
  nouveau->data[31] = '\0';
  nouveau->next = NULL;
  nouveau->prev = liste->tail;

  if (liste->tail != NULL) {
    liste->tail->next = nouveau;
  } else {
    liste->head = nouveau;
  }

  liste->tail = nouveau;
  liste->count++;
}

void ListeDouble_InsererPosition(ListeDouble *liste, const char *valeur,
                                 int pos) {
  if (pos <= 0) {
    ListeDouble_InsererDebut(liste, valeur);
    return;
  }
  if (pos >= liste->count) {
    ListeDouble_InsererFin(liste, valeur);
    return;
  }

  NodeDouble *current = liste->head;
  for (int i = 0; i < pos; i++) {
    current = current->next;
  }

  NodeDouble *nouveau = (NodeDouble *)malloc(sizeof(NodeDouble));
  strncpy(nouveau->data, valeur, 31);
  nouveau->data[31] = '\0';
  nouveau->next = current;
  nouveau->prev = current->prev;
  current->prev->next = nouveau;
  current->prev = nouveau;
  liste->count++;
}

bool ListeDouble_SupprimerDebut(ListeDouble *liste) {
  if (liste->head == NULL)
    return false;

  NodeDouble *temp = liste->head;
  liste->head = liste->head->next;

  if (liste->head != NULL) {
    liste->head->prev = NULL;
  } else {
    liste->tail = NULL;
  }

  free(temp);
  liste->count--;
  return true;
}

bool ListeDouble_SupprimerFin(ListeDouble *liste) {
  if (liste->tail == NULL)
    return false;

  NodeDouble *temp = liste->tail;
  liste->tail = liste->tail->prev;

  if (liste->tail != NULL) {
    liste->tail->next = NULL;
  } else {
    liste->head = NULL;
  }

  free(temp);
  liste->count--;
  return true;
}

bool ListeDouble_SupprimerPosition(ListeDouble *liste, int pos) {
  if (liste->head == NULL || pos < 0 || pos >= liste->count)
    return false;

  if (pos == 0)
    return ListeDouble_SupprimerDebut(liste);
  if (pos == liste->count - 1)
    return ListeDouble_SupprimerFin(liste);

  NodeDouble *current = liste->head;
  for (int i = 0; i < pos; i++) {
    current = current->next;
  }

  current->prev->next = current->next;
  current->next->prev = current->prev;
  free(current);
  liste->count--;
  return true;
}

bool ListeDouble_SupprimerValeur(ListeDouble *liste, const char *valeur) {
  NodeDouble *current = liste->head;
  while (current != NULL) {
    if (strcmp(current->data, valeur) == 0) {
      if (current == liste->head)
        return ListeDouble_SupprimerDebut(liste);
      if (current == liste->tail)
        return ListeDouble_SupprimerFin(liste);

      current->prev->next = current->next;
      current->next->prev = current->prev;
      free(current);
      liste->count--;
      return true;
    }
    current = current->next;
  }
  return false;
}

NodeDouble *ListeDouble_Rechercher(ListeDouble *liste, const char *valeur) {
  NodeDouble *current = liste->head;
  while (current != NULL) {
    if (strcmp(current->data, valeur) == 0)
      return current;
    current = current->next;
  }
  return NULL;
}

void ListeDouble_Vider(ListeDouble *liste) {
  while (liste->head != NULL) {
    ListeDouble_SupprimerDebut(liste);
  }
}

// ==================== FONCTIONS DE DESSIN ====================

static void DrawCustomFrame(Rectangle rect, const char *title, Color color) {
  DrawRectRoundedLinesThick(rect, 0.1f, 8, 2, color);

  // Effacer la ligne derrière le titre
  int textW = MeasureText(title, 12);
  DrawRectangle((int)rect.x + 15, (int)rect.y - 2, textW + 10, 4,
                COLOR_BG_DARK);

  DrawText(title, (int)rect.x + 20, (int)rect.y - 6, 12, color);
}

// Labeled input box: label on top, input area below (per user image)
static void DrawLabeledInputBox(int x, int y, int w, int h, const char *label,
                                const char *value, Color borderColor,
                                bool focused) {
  int labelH = 16;
  int inputH = h - labelH;

  // Top zone: label
  Rectangle labelRect = {x, y, w, labelH};
  DrawRectangleRounded(labelRect, 0.2f, 4, COLOR_PANEL);
  DrawRectRoundedLinesThick(labelRect, 0.2f, 4, 1, borderColor);
  int labelW = MeasureText(label, 10);
  DrawText(label, x + (w - labelW) / 2, y + 3, 10, borderColor);

  // Bottom zone: input value
  Rectangle inputRect = {x, y + labelH, w, inputH};
  Color inputBg = focused ? (Color){50, 80, 50, 255} : COLOR_SECONDARY;
  DrawRectangleRounded(inputRect, 0.2f, 4, inputBg);
  DrawRectRoundedLinesThick(inputRect, 0.2f, 4, 1, borderColor);

  int valW = MeasureText(value, 14);
  int fontSize = 14;
  if (valW > w - 8) {
    fontSize = 10;
    valW = MeasureText(value, fontSize);
  }
  int textY = y + labelH + (inputH - fontSize) / 2;
  DrawText(value, x + (w - valW) / 2, textY, fontSize, COLOR_TEXT_WHITE);

  // Cursor if focused
  if (focused && (int)(GetTime() * 2) % 2 == 0) {
    int cursorX = x + (w + valW) / 2 + 2;
    DrawRectangle(cursorX, textY, 2, fontSize, COLOR_NEON_GREEN);
  }
}

static void DrawNodeSimple(int x, int y, const char *data, bool highlighted,
                           int index) {
  int dataWidth = (int)(NODE_WIDTH * 0.7f);
  int ptrWidth = (int)(NODE_WIDTH * 0.3f);

  // Couleurs néon
  Color borderColor = highlighted ? COLOR_NEON_GREEN : COLOR_NEON_BLUE;
  Color bgColor = COLOR_PANEL;

  // Zone data
  Rectangle dataRect = {x, y, dataWidth, NODE_HEIGHT};
  DrawRectangleRounded(dataRect, 0.1f, 4, bgColor);
  DrawRectRoundedLinesThick(dataRect, 0.1f, 4, 2, borderColor);

  // Zone pointeur
  Rectangle ptrRect = {x + dataWidth, y, ptrWidth, NODE_HEIGHT};
  DrawRectangleRounded(ptrRect, 0.1f, 4, COLOR_SECONDARY);
  DrawRectRoundedLinesThick(ptrRect, 0.1f, 4, 2, borderColor);

  // Valeur
  int fontSize = 14;
  int textW = MeasureText(data, fontSize);
  if (textW > dataWidth - 10) {
    fontSize = 10;
    textW = MeasureText(data, fontSize);
  }
  DrawText(data, x + (dataWidth - textW) / 2, y + (NODE_HEIGHT - fontSize) / 2,
           fontSize, COLOR_TEXT_WHITE);

  // Pointeur (point orange néon)
  DrawCircle(x + dataWidth + ptrWidth / 2, y + NODE_HEIGHT / 2, 5,
             COLOR_NEON_ORANGE);

  // Index au-dessus
  char idxStr[12];
  snprintf(idxStr, sizeof(idxStr), "[%d]", index);
  DrawText(idxStr, x + dataWidth / 2 - 10, y - 18, 12, COLOR_TEXT_DIM);
}

static void DrawNodeDouble(int x, int y, const char *data, bool highlighted,
                           int index) {
  int ptrWidth = (int)(NODE_WIDTH * 0.2f);
  int dataWidth = (int)(NODE_WIDTH * 0.6f);

  // Couleurs néon
  Color borderColor = highlighted ? COLOR_NEON_GREEN : COLOR_NEON_PURPLE;
  Color bgColor = COLOR_PANEL;

  // Zone prev
  Rectangle prevRect = {x, y, ptrWidth, NODE_HEIGHT};
  DrawRectangleRounded(prevRect, 0.1f, 4, COLOR_SECONDARY);
  DrawRectRoundedLinesThick(prevRect, 0.1f, 4, 2, borderColor);
  DrawCircle(x + ptrWidth / 2, y + NODE_HEIGHT / 2, 4, COLOR_NEON_BLUE);

  // Zone data
  Rectangle dataRect = {x + ptrWidth, y, dataWidth, NODE_HEIGHT};
  DrawRectangleRounded(dataRect, 0.1f, 4, bgColor);
  DrawRectRoundedLinesThick(dataRect, 0.1f, 4, 2, borderColor);

  // Zone next
  Rectangle nextRect = {x + ptrWidth + dataWidth, y, ptrWidth, NODE_HEIGHT};
  DrawRectangleRounded(nextRect, 0.1f, 4, COLOR_SECONDARY);
  DrawRectRoundedLinesThick(nextRect, 0.1f, 4, 2, borderColor);
  DrawCircle(x + ptrWidth + dataWidth + ptrWidth / 2, y + NODE_HEIGHT / 2, 4,
             COLOR_NEON_ORANGE);

  // Valeur
  int fontSize = 14;
  int textW = MeasureText(data, fontSize);
  if (textW > dataWidth - 10) {
    fontSize = 10;
    textW = MeasureText(data, fontSize);
  }
  DrawText(data, x + ptrWidth + (dataWidth - textW) / 2,
           y + (NODE_HEIGHT - fontSize) / 2, fontSize, COLOR_TEXT_WHITE);

  // Index au-dessus
  char idxStr[12];
  snprintf(idxStr, sizeof(idxStr), "[%d]", index);
  DrawText(idxStr, x + NODE_WIDTH / 2 - 10, y - 18, 12, COLOR_TEXT_DIM);
}

static void DrawInputNode(int x, int y, const char *text, bool isDouble,
                          bool active, int *outWidth) {
  // Couleurs - no green, use blue for active state
  Color borderColor = active ? COLOR_NEON_BLUE : COLOR_NEON_ORANGE;
  Color bgColor = active ? COLOR_SECONDARY : COLOR_PANEL;

  int totalWidth;
  int dataWidth;
  int centerX;

  if (isDouble) {
    // Double linked node: [prev] [data] [next]
    int ptrWidth = (int)(NODE_WIDTH * 0.2f);
    dataWidth = (int)(NODE_WIDTH * 0.6f);

    // Dynamically expand
    int textW = MeasureText(text, 14);
    if (textW > dataWidth - 20) {
      dataWidth = textW + 30;
    }
    totalWidth = ptrWidth * 2 + dataWidth;

    if (outWidth)
      *outWidth = totalWidth;

    // Label "NOUVEAU" above
    const char *label = "NOUVEAU";
    int labelW = MeasureText(label, 12);
    DrawText(label, x + (totalWidth - labelW) / 2, y - 20, 12, borderColor);

    // Zone prev pointer (left)
    Rectangle prevRect = {x, y, ptrWidth, NODE_HEIGHT};
    DrawRectangleRounded(prevRect, 0.1f, 4, COLOR_SECONDARY);
    DrawRectRoundedLinesThick(prevRect, 0.1f, 4, 2, borderColor);
    DrawCircle(x + ptrWidth / 2, y + NODE_HEIGHT / 2, 4, COLOR_NEON_PURPLE);

    // Zone data (centre)
    Rectangle dataRect = {x + ptrWidth, y, dataWidth, NODE_HEIGHT};
    DrawRectangleRounded(dataRect, 0.1f, 4, bgColor);
    DrawRectRoundedLinesThick(dataRect, 0.1f, 4, 2, borderColor);

    // Zone next pointer (right)
    Rectangle nextRect = {x + ptrWidth + dataWidth, y, ptrWidth, NODE_HEIGHT};
    DrawRectangleRounded(nextRect, 0.1f, 4, COLOR_SECONDARY);
    DrawRectRoundedLinesThick(nextRect, 0.1f, 4, 2, borderColor);
    DrawCircle(x + ptrWidth + dataWidth + ptrWidth / 2, y + NODE_HEIGHT / 2, 4,
               COLOR_NEON_ORANGE);

    centerX = x + ptrWidth + dataWidth / 2;
  } else {
    // Simple linked node: [data] [next]
    int ptrWidth = (int)(NODE_WIDTH * 0.3f);
    dataWidth = (int)(NODE_WIDTH * 0.7f);

    // Dynamically expand
    int textW = MeasureText(text, 14);
    if (textW > dataWidth - 20) {
      dataWidth = textW + 30;
    }
    totalWidth = dataWidth + ptrWidth;

    if (outWidth)
      *outWidth = totalWidth;

    // Label
    const char *label = "NOUVEAU";
    int labelW = MeasureText(label, 12);
    DrawText(label, x + (totalWidth - labelW) / 2, y - 20, 12, borderColor);

    // Zone data
    Rectangle dataRect = {x, y, dataWidth, NODE_HEIGHT};
    DrawRectangleRounded(dataRect, 0.1f, 4, bgColor);
    DrawRectRoundedLinesThick(dataRect, 0.1f, 4, 2, borderColor);

    // Zone pointeur
    Rectangle ptrRect = {x + dataWidth, y, ptrWidth, NODE_HEIGHT};
    DrawRectangleRounded(ptrRect, 0.1f, 4, COLOR_SECONDARY);
    DrawRectRoundedLinesThick(ptrRect, 0.1f, 4, 2, borderColor);
    DrawCircle(x + dataWidth + ptrWidth / 2, y + NODE_HEIGHT / 2, 5,
               COLOR_NEON_ORANGE);

    centerX = x + dataWidth / 2;
  }

  // Text in data zone
  int fontSize = 14;
  int displayW = MeasureText(text, fontSize);
  if (displayW > dataWidth - 10) {
    fontSize = 10;
    displayW = MeasureText(text, fontSize);
  }

  int textX = centerX - displayW / 2;
  int textY = y + (NODE_HEIGHT - fontSize) / 2;

  if (strlen(text) > 0) {
    DrawText(text, textX, textY, fontSize, COLOR_TEXT_WHITE);
  }

  // Cursor
  if (active) {
    int cursorX = (strlen(text) > 0) ? textX + displayW + 2 : centerX;
    if ((int)(GetTime() * 2) % 2 == 0) {
      DrawRectangle(cursorX, textY, 2, fontSize, COLOR_NEON_BLUE);
    }
  }

  // Hint
  if (strlen(text) == 0 && !active) {
    const char *hint = "Entrer valeur";
    int hintW = MeasureText(hint, 10);
    DrawText(hint, centerX - hintW / 2, textY, 10, COLOR_TEXT_DIM);
  }
}

static void DrawArrowRight(int x1, int y1, int x2, int y2, Color color) {
  DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 2, color);

  // Pointe de flèche
  float angle = atan2f((float)(y2 - y1), (float)(x2 - x1));
  float arrowSize = 8;
  Vector2 p1 = {x2 - arrowSize * cosf(angle - 0.5f),
                y2 - arrowSize * sinf(angle - 0.5f)};
  Vector2 p2 = {x2 - arrowSize * cosf(angle + 0.5f),
                y2 - arrowSize * sinf(angle + 0.5f)};
  DrawTriangle((Vector2){x2, y2}, p1, p2, color);
}

// ==================== MANUEL POPUP (Création liste vide) ====================

static const char *MANUAL_TYPE_NAMES[] = {"Int", "Float", "Char", "Str"};

static void DrawManualPopup(ListesScreen *screen) {
  // Overlay sombre
  DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 200});

  // Popup centré
  int popupW = 350, popupH = 220;
  int popupX = (WINDOW_WIDTH - popupW) / 2;
  int popupY = (WINDOW_HEIGHT - popupH) / 2;
  Rectangle popupRect = {popupX, popupY, popupW, popupH};

  DrawRectangleRounded(popupRect, 0.05f, 8, COLOR_PANEL);
  DrawRectRoundedLinesThick(popupRect, 0.05f, 8, 3, COLOR_NEON_GREEN);

  // Titre
  DrawText("SAISIE MANUELLE", popupX + 20, popupY + 15, 22, COLOR_NEON_GREEN);

  // Taille label + input
  DrawText("Taille:", popupX + 30, popupY + 60, 16, COLOR_TEXT_WHITE);

  // Input box pour taille
  screen->manual_size_input.bounds.x = popupX + 110;
  screen->manual_size_input.bounds.y = popupY + 55;
  screen->manual_size_input.bounds.width = 100;
  screen->manual_size_input.bounds.height = 30;
  DrawInputBox(&screen->manual_size_input);

  // Type label + dropdown
  DrawText("Type:", popupX + 30, popupY + 105, 16, COLOR_TEXT_WHITE);

  // Type dropdown (simple buttons)
  int typeX = popupX + 110;
  int typeY = popupY + 100;
  int typeW = 55, typeH = 28;
  for (int i = 0; i < 4; i++) {
    Rectangle typeBtn = {typeX + i * (typeW + 5), typeY, typeW, typeH};
    bool selected = (screen->manual_type_selected == i);
    bool hovered = CheckCollisionPointRec(GetMousePosition(), typeBtn);

    Color bgCol =
        selected ? COLOR_NEON_GREEN : (hovered ? COLOR_SECONDARY : COLOR_PANEL);
    Color textCol = selected ? COLOR_BG_DARK : COLOR_TEXT_WHITE;

    DrawRectangleRounded(typeBtn, 0.2f, 4, bgCol);
    DrawRectRoundedLinesThick(typeBtn, 0.2f, 4, 1, COLOR_NEON_GREEN);

    int tw = MeasureText(MANUAL_TYPE_NAMES[i], 14);
    DrawText(MANUAL_TYPE_NAMES[i], typeBtn.x + (typeW - tw) / 2, typeBtn.y + 6,
             14, textCol);
  }

  // Boutons OK / Annuler
  int btnW = 100, btnH = 35;
  int btnY = popupY + popupH - 55;

  // OK
  Rectangle okBtn = {popupX + popupW / 2 - btnW - 10, btnY, btnW, btnH};
  bool okHovered = CheckCollisionPointRec(GetMousePosition(), okBtn);
  DrawRectangleRounded(okBtn, 0.2f, 4,
                       okHovered ? COLOR_NEON_GREEN : (Color){40, 80, 40, 255});
  DrawRectRoundedLinesThick(okBtn, 0.2f, 4, 2, COLOR_NEON_GREEN);
  int okW = MeasureText("OK", 18);
  DrawText("OK", okBtn.x + (btnW - okW) / 2, okBtn.y + 8, 18, COLOR_TEXT_WHITE);

  // Annuler
  Rectangle cancelBtn = {popupX + popupW / 2 + 10, btnY, btnW, btnH};
  bool cancelHovered = CheckCollisionPointRec(GetMousePosition(), cancelBtn);
  DrawRectangleRounded(cancelBtn, 0.2f, 4,
                       cancelHovered ? COLOR_NEON_RED
                                     : (Color){80, 40, 40, 255});
  DrawRectRoundedLinesThick(cancelBtn, 0.2f, 4, 2, COLOR_NEON_RED);
  int cancelW = MeasureText("Annuler", 16);
  DrawText("Annuler", cancelBtn.x + (btnW - cancelW) / 2, cancelBtn.y + 9, 16,
           COLOR_TEXT_WHITE);
}

static void UpdateManualPopup(ListesScreen *screen) {
  // Update input box
  UpdateInputBox(&screen->manual_size_input);

  Vector2 mouse = GetMousePosition();

  // Popup dimensions
  int popupW = 350, popupH = 220;
  int popupX = (WINDOW_WIDTH - popupW) / 2;
  int popupY = (WINDOW_HEIGHT - popupH) / 2;

  // Type selection
  int typeX = popupX + 110;
  int typeY = popupY + 100;
  int typeW = 55, typeH = 28;
  for (int i = 0; i < 4; i++) {
    Rectangle typeBtn = {typeX + i * (typeW + 5), typeY, typeW, typeH};
    if (CheckCollisionPointRec(mouse, typeBtn) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      screen->manual_type_selected = i;
    }
  }

  // OK button
  int btnW = 100, btnH = 35;
  int btnY = popupY + popupH - 55;
  Rectangle okBtn = {popupX + popupW / 2 - btnW - 10, btnY, btnW, btnH};
  if (CheckCollisionPointRec(mouse, okBtn) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    // Parse size and create empty list
    int size = atoi(screen->manual_size_input.text);
    if (size > 0) {
      // Clear current list
      if (screen->type == LISTE_TYPE_SIMPLE) {
        ListeSimple_Vider(&screen->liste_simple);
      } else {
        ListeDouble_Vider(&screen->liste_double);
      }

      // Create empty nodes
      for (int i = 0; i < size; i++) {
        if (screen->type == LISTE_TYPE_SIMPLE) {
          ListeSimple_InsererFin(&screen->liste_simple, "");
        } else {
          ListeDouble_InsererFin(&screen->liste_double, "");
        }
      }

      // Reset scroll
      screen->scroll_y = 0;
      screen->target_scroll_y = 0;

      // Store type for validation when editing
      screen->random_data_type = screen->manual_type_selected;
    }
    screen->manual_popup_active = false;
  }

  // Cancel button
  Rectangle cancelBtn = {popupX + popupW / 2 + 10, btnY, btnW, btnH};
  if (CheckCollisionPointRec(mouse, cancelBtn) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    screen->manual_popup_active = false;
  }

  // ESC to close
  if (IsKeyPressed(KEY_ESCAPE)) {
    screen->manual_popup_active = false;
  }
}

// ==================== ÉCRAN LISTES ====================

void ListesInit(ListesScreen *screen) {
  screen->type = LISTE_TYPE_SIMPLE;
  ListeSimple_Init(&screen->liste_simple);
  ListeDouble_Init(&screen->liste_double);

  // Initialisation Manuel Popup
  screen->manual_popup_active = false;
  screen->manual_size_input = CreateInputBox(0, 0, 100, 30, 8);
  strncpy(screen->manual_size_input.text, "10", 8);
  screen->manual_type_selected = 0;

  // Navigation
  // Navigation
  // Back removed
  screen->btn_type_simple = CreateButton(WINDOW_WIDTH - 250, 15 + NAVBAR_HEIGHT,
                                         115, 30, "Simple", COLOR_NEON_BLUE);
  screen->btn_type_double = CreateButton(WINDOW_WIDTH - 130, 15 + NAVBAR_HEIGHT,
                                         115, 30, "Double", COLOR_NEON_BLUE);

  // ========== FRAME POSITIONS ==========
  // Frame 1: INSERTION (x=30, y=50, w=140, h=95)
  // ========== FRAME POSITIONS ==========
  // Frame 1: INSERTION (x=30, y=50, w=140, h=95)
  int f1_x = 30, f1_y = 15 + NAVBAR_HEIGHT;
  screen->btn_inserer_debut =
      CreateButton(f1_x + 10, f1_y + 22, 60, 26, "Debut", COLOR_NEON_GREEN);
  screen->btn_inserer_fin =
      CreateButton(f1_x + 75, f1_y + 22, 60, 26, "Fin", COLOR_NEON_GREEN);
  screen->btn_inserer_position =
      CreateButton(f1_x + 10, f1_y + 55, 60, 26, "Pos", COLOR_NEON_GREEN);
  screen->input_position_insert =
      CreateInputBox(f1_x + 75, f1_y + 55, 60, 26, 8);

  // Frame 2: SUPPRESSION (x=215, y=50, w=200, h=95)
  // Debut, Fin on left; Val stacked, Pos stacked (like RECHERCHE)
  int f2_x = 215, f2_y = 15 + NAVBAR_HEIGHT;
  screen->btn_supprimer_debut =
      CreateButton(f2_x + 10, f2_y + 22, 40, 26, "Deb", COLOR_NEON_RED);
  screen->btn_supprimer_fin =
      CreateButton(f2_x + 55, f2_y + 22, 35, 26, "Fin", COLOR_NEON_RED);
  // Val stacked: button on top, input below (larger)
  screen->btn_supprimer_valeur =
      CreateButton(f2_x + 95, f2_y + 18, 50, 24, "Val", COLOR_NEON_RED);
  screen->input_valeur_delete =
      CreateInputBox(f2_x + 95, f2_y + 44, 50, 24, 16);
  // Pos stacked: button on top, input below (larger)
  screen->btn_supprimer_position =
      CreateButton(f2_x + 150, f2_y + 18, 45, 24, "Pos", COLOR_NEON_RED);
  screen->input_position_delete =
      CreateInputBox(f2_x + 150, f2_y + 44, 45, 24, 8);

  // Frame 3: RECHERCHE (x=460, y=50, w=130, h=95)
  // Val stacked, Pos stacked
  int f3_x = 460, f3_y = 15 + NAVBAR_HEIGHT;
  screen->btn_rechercher_val =
      CreateButton(f3_x + 10, f3_y + 20, 55, 24, "Val", COLOR_NEON_BLUE);
  screen->btn_rechercher_pos =
      CreateButton(f3_x + 70, f3_y + 20, 55, 24, "Pos", COLOR_NEON_BLUE);
  screen->input_recherche_val =
      CreateInputBox(f3_x + 10, f3_y + 50, 55, 26, 16);
  screen->input_recherche_pos = CreateInputBox(f3_x + 70, f3_y + 50, 55, 26, 8);

  // Frame 4: TRI (x=635, y=50, w=175, h=95)
  // Bulle|Quick on top row, STOP centered at bottom
  int f4_x = 635, f4_y = 15 + NAVBAR_HEIGHT;
  screen->btn_tri_insertion =
      CreateButton(f4_x + 20, f4_y + 22, 65, 26, "Bulle", COLOR_NEON_PURPLE);
  screen->btn_tri_quicksort =
      CreateButton(f4_x + 95, f4_y + 22, 65, 26, "Quick", COLOR_NEON_PURPLE);
  // STOP centered below
  screen->btn_stop_tri =
      CreateButton(f4_x + 55, f4_y + 55, 70, 26, "STOP", COLOR_NEON_RED);

  // Frame 5: ALEATOIRE (x=855, y=50, w=170, h=95)
  // Row 1: Générer, Dropdown (type)
  // Row 2: Taille input (wider)
  int f5_x = 855, f5_y = 15 + NAVBAR_HEIGHT;
  screen->btn_generer_aleatoire =
      CreateButton(f5_x + 10, f5_y + 20, 75, 26, "Generer", COLOR_NEON_BLUE);
  // Dropdown will be drawn at f5_x + 90, f5_y + 20 (using random_data_type)
  // Taille input on second row - wider with placeholder for "Taille"
  screen->input_taille_aleatoire =
      CreateInputBox(f5_x + 10, f5_y + 52, 140, 28, 7);
  screen->random_data_type = 0;
  screen->type_dropdown_open = false;

  // Vider - far right edge, isolated from other frames
  screen->btn_vider = CreateButton(WINDOW_WIDTH - 85, 65 + NAVBAR_HEIGHT, 70,
                                   30, "Vider", COLOR_NEON_RED);

  // Bouton Manuel (à côté de Vider ou ailleurs, on le met à gauche de Aleatoire
  // pour l'instant ou près de l'input node) On le met en haut à droite, avant
  // Vider
  screen->btn_manual = CreateButton(WINDOW_WIDTH - 235, 60 + NAVBAR_HEIGHT, 140,
                                    40, "MANUEL", COLOR_NEON_GREEN);

  // Input node
  memset(screen->input_node_text, 0, sizeof(screen->input_node_text));
  screen->input_node_active = false;

  // Scroll
  screen->scroll_y = 0;
  screen->target_scroll_y = 0;

  // Highlight
  screen->highlighted_index = -1;
  screen->highlight_timer = 0;

  // Animation
  screen->anim_type = ANIM_NONE;
  screen->anim_progress = 0;
  screen->anim_target_index = -1;
  screen->anim_value = 0;

  // Tri
  screen->sort_running = false;
  screen->sort_i = 0;
  screen->sort_pass = 0;
  screen->sort_total = 0;
  screen->sort_swapped = false;
  screen->swap_idx1 = -1;
  screen->swap_idx2 = -1;
  screen->swap_progress = 0;
  screen->swap_animating = false;
  screen->sort_progress = 0;

  // QuickSort
  screen->quicksort_running = false;
  screen->qs_stack_top = 0;
  screen->qs_low = 0;
  screen->qs_high = 0;
  screen->qs_pivot_idx = 0;
  screen->qs_i = 0;
  screen->qs_j = 0;
  screen->qs_phase = 0;
  screen->qs_partitions_done = 0;
  screen->qs_total_partitions = 0;

  // Popup
  screen->popup_visible = false;
  memset(screen->popup_title, 0, sizeof(screen->popup_title));
  memset(screen->popup_message, 0, sizeof(screen->popup_message));
  screen->popup_timer = 0;

  // Status
  memset(screen->status_message, 0, sizeof(screen->status_message));
  screen->status_timer = 0;

  // Search Animation (Mario)
  screen->search_active = false;
  memset(screen->search_target_val, 0, sizeof(screen->search_target_val));
  screen->search_current_node_idx = 0;
  screen->search_jump_progress = 0.0f;
  screen->search_victory = false;
  screen->search_victory_timer = 0.0f;
  screen->search_char_pos = (Vector2){0, 0};
  screen->search_mode_is_pos = false;

  // Inline node editing
  screen->editing_node_index = -1;
  memset(screen->editing_node_text, 0, sizeof(screen->editing_node_text));
  screen->editing_node_active = false;

  // Camera
  screen->camera.target = (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
  screen->camera.offset = (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
  screen->camera.rotation = 0.0f;
  screen->camera.zoom = 1.0f;
  screen->target_zoom = 1.0f;
  screen->target_offset = screen->camera.offset;
}

void ListesFree(ListesScreen *screen) {
  ListeSimple_Free(&screen->liste_simple);
  ListeDouble_Free(&screen->liste_double);
}

AppScreen ListesUpdate(ListesScreen *screen) {
  float dt = GetFrameTime();

  // Scroll smooth
  screen->scroll_y = Lerp(screen->scroll_y, screen->target_scroll_y, 0.1f);

  // Timers
  if (screen->highlight_timer > 0)
    screen->highlight_timer -= dt;
  if (screen->popup_timer > 0) {
    screen->popup_timer -= dt;
    if (screen->popup_timer <= 0)
      screen->popup_visible = false;
  }
  if (screen->status_timer > 0)
    screen->status_timer -= dt;

  // Type switch - clear lists when switching
  if (UpdateButton(&screen->btn_type_simple)) {
    if (screen->type != LISTE_TYPE_SIMPLE) {
      // Clear both lists when switching
      ListeSimple_Vider(&screen->liste_simple);
      ListeDouble_Vider(&screen->liste_double);
      screen->input_node_text[0] = '\0';
      screen->scroll_y = 0;
      screen->target_scroll_y = 0;
    }
    screen->type = LISTE_TYPE_SIMPLE;
  }
  if (UpdateButton(&screen->btn_type_double)) {
    if (screen->type != LISTE_TYPE_DOUBLE) {
      // Clear both lists when switching
      ListeSimple_Vider(&screen->liste_simple);
      ListeDouble_Vider(&screen->liste_double);
      screen->input_node_text[0] = '\0';
      screen->scroll_y = 0;
      screen->target_scroll_y = 0;
    }
    screen->type = LISTE_TYPE_DOUBLE;
  }

  // Navigation Bar
  AppScreen next_screen = SCREEN_LISTES;
  if (UpdateNavigationBar(&next_screen))
    return next_screen;

  // Update inputs
  UpdateInputBox(&screen->input_position_insert);
  UpdateInputBox(&screen->input_valeur_delete);
  UpdateInputBox(&screen->input_position_delete);
  UpdateInputBox(&screen->input_recherche_val);
  UpdateInputBox(&screen->input_recherche_pos);
  UpdateInputBox(&screen->input_taille_aleatoire);

  // Input Node Click
  Rectangle vizRect = {30, 120 + NAVBAR_HEIGHT, WINDOW_WIDTH - 60,
                       WINDOW_HEIGHT - 145 - NAVBAR_HEIGHT};
  int inputNodeX = vizRect.x + (vizRect.width - NODE_WIDTH) / 2;
  int inputNodeY = vizRect.y + 25;
  Rectangle inputNodeRect = {inputNodeX, inputNodeY, NODE_WIDTH, NODE_HEIGHT};

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      CheckCollisionPointRec(GetMousePosition(), inputNodeRect)) {
    screen->input_node_active = !screen->input_node_active;
  }

  if (screen->input_node_active && !screen->manual_popup_active &&
      !screen->editing_node_active) {
    int key = GetCharPressed();
    while (key > 0) {
      if (key >= 32 && key <= 126 && strlen(screen->input_node_text) < 30) {
        int len = strlen(screen->input_node_text);
        screen->input_node_text[len] = (char)key;
        screen->input_node_text[len + 1] = '\0';
      }
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && strlen(screen->input_node_text) > 0) {
      screen->input_node_text[strlen(screen->input_node_text) - 1] = '\0';
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      screen->input_node_active = false;
    }
  }

  // Scroll with mouse wheel
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    screen->target_scroll_y -= wheel * 50;
    if (screen->target_scroll_y < 0)
      screen->target_scroll_y = 0;
  }

  // Insertion
  if (UpdateButton(&screen->btn_inserer_debut)) {
    if (strlen(screen->input_node_text) > 0) {
      if (screen->type == LISTE_TYPE_SIMPLE)
        ListeSimple_InsererDebut(&screen->liste_simple,
                                 screen->input_node_text);
      else
        ListeDouble_InsererDebut(&screen->liste_double,
                                 screen->input_node_text);
      memset(screen->input_node_text, 0, sizeof(screen->input_node_text));
    }
  }
  if (UpdateButton(&screen->btn_inserer_fin)) {
    if (strlen(screen->input_node_text) > 0) {
      if (screen->type == LISTE_TYPE_SIMPLE)
        ListeSimple_InsererFin(&screen->liste_simple, screen->input_node_text);
      else
        ListeDouble_InsererFin(&screen->liste_double, screen->input_node_text);
      memset(screen->input_node_text, 0, sizeof(screen->input_node_text));
    }
  }
  if (UpdateButton(&screen->btn_inserer_position)) {
    if (strlen(screen->input_node_text) > 0 &&
        strlen(screen->input_position_insert.text) > 0) {
      int pos = atoi(screen->input_position_insert.text);
      if (screen->type == LISTE_TYPE_SIMPLE)
        ListeSimple_InsererPosition(&screen->liste_simple,
                                    screen->input_node_text, pos);
      else
        ListeDouble_InsererPosition(&screen->liste_double,
                                    screen->input_node_text, pos);
      memset(screen->input_node_text, 0, sizeof(screen->input_node_text));
    }
  }

  // Suppression
  if (UpdateButton(&screen->btn_supprimer_debut)) {
    if (screen->type == LISTE_TYPE_SIMPLE)
      ListeSimple_SupprimerDebut(&screen->liste_simple);
    else
      ListeDouble_SupprimerDebut(&screen->liste_double);
  }
  if (UpdateButton(&screen->btn_supprimer_fin)) {
    if (screen->type == LISTE_TYPE_SIMPLE)
      ListeSimple_SupprimerFin(&screen->liste_simple);
    else
      ListeDouble_SupprimerFin(&screen->liste_double);
  }
  if (UpdateButton(&screen->btn_supprimer_valeur)) {
    if (strlen(screen->input_valeur_delete.text) > 0) {
      if (screen->type == LISTE_TYPE_SIMPLE)
        ListeSimple_SupprimerValeur(&screen->liste_simple,
                                    screen->input_valeur_delete.text);
      else
        ListeDouble_SupprimerValeur(&screen->liste_double,
                                    screen->input_valeur_delete.text);
    }
  }
  if (UpdateButton(&screen->btn_supprimer_position)) {
    if (strlen(screen->input_position_delete.text) > 0) {
      int pos = atoi(screen->input_position_delete.text);
      if (screen->type == LISTE_TYPE_SIMPLE)
        ListeSimple_SupprimerPosition(&screen->liste_simple, pos);
      else
        ListeDouble_SupprimerPosition(&screen->liste_double, pos);
    }
  }

  // Vider
  if (UpdateButton(&screen->btn_vider)) {
    if (screen->type == LISTE_TYPE_SIMPLE)
      ListeSimple_Vider(&screen->liste_simple);
    else
      ListeDouble_Vider(&screen->liste_double);
    screen->scroll_y = 0;
    screen->target_scroll_y = 0;
  }

  // Génération aléatoire
  if (UpdateButton(&screen->btn_generer_aleatoire)) {
    int count = atoi(screen->input_taille_aleatoire.text);
    if (count < 1)
      count = 10;
    // No limit on count - user can generate any size

    if (screen->type == LISTE_TYPE_SIMPLE)
      ListeSimple_Vider(&screen->liste_simple);
    else
      ListeDouble_Vider(&screen->liste_double);

    for (int i = 0; i < count; i++) {
      char val[32];
      if (screen->random_data_type == 0) {
        snprintf(val, sizeof(val), "%d", GetRandomValue(0, 999));
      } else if (screen->random_data_type == 1) {
        snprintf(val, sizeof(val), "%.2f",
                 (float)GetRandomValue(0, 10000) / 100.0f);
      } else if (screen->random_data_type == 2) {
        val[0] = 'A' + GetRandomValue(0, 25);
        val[1] = '\0';
      } else {
        int len = GetRandomValue(3, 6);
        for (int j = 0; j < len; j++)
          val[j] = 'a' + GetRandomValue(0, 25);
        val[len] = '\0';
      }
      if (screen->type == LISTE_TYPE_SIMPLE)
        ListeSimple_InsererFin(&screen->liste_simple, val);
      else
        ListeDouble_InsererFin(&screen->liste_double, val);
    }
  }

  // Dropdown type - positioned next to Générer button, opens upward
  Rectangle dropdownRect = {screen->btn_generer_aleatoire.bounds.x + 80,
                            screen->btn_generer_aleatoire.bounds.y, 70, 26};
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      CheckCollisionPointRec(GetMousePosition(), dropdownRect)) {
    screen->type_dropdown_open = !screen->type_dropdown_open;
  }
  if (screen->type_dropdown_open) {
    const char *types[] = {"Int", "Float", "Char", "Str"};
    for (int i = 0; i < 4; i++) {
      // Dropdown opens upward: options appear above the button (inside frame)
      Rectangle optRect = {dropdownRect.x, dropdownRect.y + 28 + i * 24, 70,
                           24};
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
          CheckCollisionPointRec(GetMousePosition(), optRect)) {
        screen->random_data_type = i;
        screen->type_dropdown_open = false;
      }
    }
  }

  // Recherche par valeur -> Mario Animation
  if ((screen->input_recherche_val.is_focused && IsKeyPressed(KEY_ENTER)) ||
      UpdateButton(&screen->btn_rechercher_val)) {
    if (strlen(screen->input_recherche_val.text) > 0) {
      strncpy(screen->search_target_val, screen->input_recherche_val.text, 31);
      screen->search_target_val[31] = '\0';
      screen->search_active = true;
      screen->search_current_node_idx = 0;
      screen->search_jump_progress = 0.0f;
      screen->search_victory = false;
      screen->search_victory_timer = 0.0f;
      screen->anim_type = ANIM_SEARCH;
      screen->input_recherche_val.is_focused = false;
      screen->search_mode_is_pos = false;
    }
  }

  // Update Camera
  screen->camera.zoom = Lerp(screen->camera.zoom, screen->target_zoom, 0.15f);
  screen->camera.offset.x =
      Lerp(screen->camera.offset.x, screen->target_offset.x, 0.15f);
  screen->camera.offset.y =
      Lerp(screen->camera.offset.y, screen->target_offset.y, 0.15f);

  // Mario Animation Logic
  if (screen->search_active) {
    // Auto-scroll to follow Mario
    if (!screen->search_victory) {
      int nodesPerRow = (screen->type == LISTE_TYPE_SIMPLE) ? 8 : 6;
      int currentRow = screen->search_current_node_idx / nodesPerRow;
      int rowHeight = NODE_HEIGHT + 60;
      float targetScroll = (float)(currentRow * rowHeight) - 250;
      if (targetScroll < 0)
        targetScroll = 0;
      screen->target_scroll_y = targetScroll;
    }

    if (screen->search_victory) {
      screen->search_victory_timer += dt;
      if (screen->search_victory_timer > 3.0f) {
        screen->search_active = false;
        screen->search_victory = false;
        screen->anim_type = ANIM_NONE;
        screen->highlighted_index = -1;
        screen->target_zoom = 1.0f;
        screen->target_offset =
            (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
        screen->camera.target =
            (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
      }
    } else {
      screen->search_jump_progress += dt * 2.0f;
      if (screen->search_jump_progress >= 1.0f) {
        screen->search_jump_progress = 0.0f;

        int count = (screen->type == LISTE_TYPE_SIMPLE)
                        ? screen->liste_simple.count
                        : screen->liste_double.count;
        char *currentVal = NULL;

        if (screen->search_current_node_idx < count) {
          if (screen->type == LISTE_TYPE_SIMPLE) {
            NodeSimple *c = screen->liste_simple.head;
            for (int k = 0; k < screen->search_current_node_idx && c; k++)
              c = c->next;
            if (c)
              currentVal = c->data;
          } else {
            NodeDouble *c = screen->liste_double.head;
            for (int k = 0; k < screen->search_current_node_idx && c; k++)
              c = c->next;
            if (c)
              currentVal = c->data;
          }

          if (currentVal &&
              strcmp(currentVal, screen->search_target_val) == 0) {
            screen->search_victory = true;
            screen->highlighted_index = screen->search_current_node_idx;
            snprintf(screen->popup_title, sizeof(screen->popup_title),
                     "CHAMPION !");
            snprintf(screen->popup_message, sizeof(screen->popup_message),
                     "J'ai attrape %s !", currentVal);
            screen->popup_visible = true;
            screen->popup_timer = 2.0f;
            screen->target_zoom = 2.0f;

            int idx = screen->search_current_node_idx;
            int nodesPerRow = (screen->type == LISTE_TYPE_SIMPLE) ? 8 : 6;
            int vizW = WINDOW_WIDTH - 60;
            int spacingX = (vizW - 50) / nodesPerRow;
            int startX = 60;
            int row = idx / nodesPerRow;
            int col = idx % nodesPerRow;
            int vx = startX + col * spacingX + 25;
            int vy = 285 + row * 110 - (int)screen->scroll_y;
            screen->camera.target = (Vector2){(float)vx, (float)vy};
            screen->target_offset =
                (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
          } else {
            screen->search_current_node_idx++;
          }
        } else {
          screen->search_active = false;
          screen->anim_type = ANIM_NONE;
          snprintf(screen->popup_title, sizeof(screen->popup_title),
                   "Non trouve");
          snprintf(screen->popup_message, sizeof(screen->popup_message),
                   "'%s' n'existe pas", screen->search_target_val);
          screen->popup_visible = true;
          screen->popup_timer = 2.0f;
        }
      }
    }
  }

  // Recherche par position
  if ((screen->input_recherche_pos.is_focused && IsKeyPressed(KEY_ENTER)) ||
      UpdateButton(&screen->btn_rechercher_pos)) {
    int count = (screen->type == LISTE_TYPE_SIMPLE)
                    ? screen->liste_simple.count
                    : screen->liste_double.count;
    int pos = atoi(screen->input_recherche_pos.text);

    if (pos >= 0 && pos < count) {
      char val_found[32] = "";
      if (screen->type == LISTE_TYPE_SIMPLE) {
        NodeSimple *current = screen->liste_simple.head;
        for (int i = 0; i < pos && current; i++)
          current = current->next;
        if (current)
          strncpy(val_found, current->data, 31);
      } else {
        NodeDouble *current = screen->liste_double.head;
        for (int i = 0; i < pos && current; i++)
          current = current->next;
        if (current)
          strncpy(val_found, current->data, 31);
      }
      strncpy(screen->search_target_val, val_found, 31);
      screen->search_target_val[31] = '\0';
      screen->search_active = true;
      screen->search_current_node_idx = 0;
      screen->search_jump_progress = 0.0f;
      screen->search_victory = false;
      screen->search_victory_timer = 0.0f;
      screen->anim_type = ANIM_SEARCH;
      screen->input_recherche_pos.is_focused = false;
      screen->search_mode_is_pos = true;
    } else {
      snprintf(screen->popup_title, sizeof(screen->popup_title),
               "Position invalide");
      snprintf(screen->popup_message, sizeof(screen->popup_message),
               "Doit etre entre 0 et %d", count - 1);
      screen->popup_visible = true;
      screen->popup_timer = 2.0f;
    }
  }

  // Sorting buttons
  int list_count = (screen->type == LISTE_TYPE_SIMPLE)
                       ? screen->liste_simple.count
                       : screen->liste_double.count;

  if (UpdateButton(&screen->btn_tri_insertion)) {
    if (list_count > 1 && list_count <= 50 && !screen->sort_running) {
      screen->sort_running = true;
      screen->sort_i = 0;
      screen->sort_pass = 0;
      screen->sort_total = list_count - 1;
      screen->sort_swapped = false;
      screen->swap_animating = false;
      screen->swap_idx1 = -1;
      screen->swap_idx2 = -1;
      screen->swap_progress = 0;
      screen->anim_type = ANIM_SORT;
    }
  }

  if (UpdateButton(&screen->btn_tri_quicksort)) {
    // QuickSort works on any list with 2+ elements
    if (list_count > 1 && !screen->quicksort_running && !screen->sort_running) {
      screen->quicksort_running = true;
      screen->qs_stack_top = 0;
      // Initialize for first partition directly (skip phase 0)
      screen->qs_low = 0;
      screen->qs_high = list_count - 1;
      screen->qs_pivot_idx = list_count - 1; // Last element as pivot
      screen->qs_i = 0;
      screen->qs_j = 0;
      screen->qs_phase = 1; // Start directly in partition phase
      screen->qs_partitions_done = 0;
      screen->qs_total_partitions = list_count;
      screen->swap_animating = false;
      screen->swap_idx1 = -1;
      screen->swap_idx2 = -1;
      screen->anim_type = ANIM_SORT;
      screen->sort_progress = 0;
    }
  }

  if (UpdateButton(&screen->btn_stop_tri)) {
    screen->sort_running = false;
    screen->quicksort_running = false;
    screen->swap_animating = false;
    screen->anim_type = ANIM_NONE;
    screen->sort_progress = 0;
  }

  // Bubble sort animation
  if (screen->sort_running) {
    int count = (screen->sort_running) ? ((screen->type == LISTE_TYPE_SIMPLE)
                                              ? screen->liste_simple.count
                                              : screen->liste_double.count)
                                       : 0;

    // Calculate progress for progress bar
    // Bubble sort worst case: n*(n-1)/2 comparisons
    // Current position: pass*(n-1) + i within current pass
    if (count > 1) {
      int totalComparisons = count * (count - 1) / 2;
      int currentComparisons = screen->sort_pass * (count - 1) + screen->sort_i;
      screen->sort_progress =
          (float)currentComparisons / (float)totalComparisons;
      if (screen->sort_progress > 1.0f)
        screen->sort_progress = 1.0f;
    }

    // Auto-scroll for sort
    int nodesPerRow = (screen->type == LISTE_TYPE_SIMPLE) ? 8 : 6;
    int currentRow = screen->sort_i / nodesPerRow;
    int rowHeight = NODE_HEIGHT + 60;
    float targetScroll = (float)(currentRow * rowHeight) - 250;
    if (targetScroll < 0)
      targetScroll = 0;
    screen->target_scroll_y = targetScroll;

    if (screen->swap_animating) {
      screen->swap_progress += dt * 3.0f;
      if (screen->swap_progress >= 1.0f) {
        screen->swap_progress = 1.0f;
        screen->swap_animating = false;

        // Perform actual swap
        char *data1 = NULL, *data2 = NULL;
        if (screen->type == LISTE_TYPE_SIMPLE) {
          NodeSimple *n1 = screen->liste_simple.head,
                     *n2 = screen->liste_simple.head;
          for (int i = 0; i < screen->swap_idx1 && n1; i++)
            n1 = n1->next;
          for (int i = 0; i < screen->swap_idx2 && n2; i++)
            n2 = n2->next;
          if (n1 && n2) {
            data1 = n1->data;
            data2 = n2->data;
          }
        } else {
          NodeDouble *n1 = screen->liste_double.head,
                     *n2 = screen->liste_double.head;
          for (int i = 0; i < screen->swap_idx1 && n1; i++)
            n1 = n1->next;
          for (int i = 0; i < screen->swap_idx2 && n2; i++)
            n2 = n2->next;
          if (n1 && n2) {
            data1 = n1->data;
            data2 = n2->data;
          }
        }
        if (data1 && data2) {
          char temp[32];
          strncpy(temp, data1, 31);
          strncpy(data1, data2, 31);
          strncpy(data2, temp, 31);
        }
        screen->sort_swapped = true;
        screen->sort_i++;
        screen->swap_idx1 = -1;
        screen->swap_idx2 = -1;
      }
    } else {
      if (screen->sort_i >= count - 1) {
        if (screen->sort_swapped) {
          screen->sort_i = 0;
          screen->sort_swapped = false;
          screen->sort_pass++;
        } else {
          screen->sort_running = false;
          screen->anim_type = ANIM_NONE;
        }
      } else {
        char *val1 = NULL, *val2 = NULL;
        if (screen->type == LISTE_TYPE_SIMPLE) {
          NodeSimple *c = screen->liste_simple.head;
          for (int j = 0; j < screen->sort_i && c; j++)
            c = c->next;
          if (c && c->next) {
            val1 = c->data;
            val2 = c->next->data;
          }
        } else {
          NodeDouble *c = screen->liste_double.head;
          for (int j = 0; j < screen->sort_i && c; j++)
            c = c->next;
          if (c && c->next) {
            val1 = c->data;
            val2 = c->next->data;
          }
        }

        bool swap = false;
        if (val1 && val2) {
          if (screen->random_data_type == 0)
            swap = (atoi(val1) > atoi(val2));
          else if (screen->random_data_type == 1)
            swap = (atof(val1) > atof(val2));
          else
            swap = (strcmp(val1, val2) > 0);
        }

        if (swap) {
          screen->swap_animating = true;
          screen->swap_progress = 0;
          screen->swap_idx1 = screen->sort_i;
          screen->swap_idx2 = screen->sort_i + 1;
        } else {
          screen->sort_i++;
        }
      }
    }
  }

  // QuickSort animation
  if (screen->quicksort_running) {
    int count = (screen->type == LISTE_TYPE_SIMPLE)
                    ? screen->liste_simple.count
                    : screen->liste_double.count;

    // Calculate progress
    if (screen->qs_total_partitions > 0) {
      screen->sort_progress = (float)screen->qs_partitions_done /
                              (float)screen->qs_total_partitions;
      if (screen->sort_progress > 1.0f)
        screen->sort_progress = 1.0f;
    } else {
      // Estimate total partitions as roughly log2(n) * n
      screen->qs_total_partitions = count > 1 ? count : 1;
    }

    if (screen->swap_animating) {
      // Animate swap
      screen->swap_progress += GetFrameTime() * 3.0f;
      if (screen->swap_progress >= 1.0f) {
        // Complete the swap
        if (screen->type == LISTE_TYPE_SIMPLE) {
          NodeSimple *n1 = screen->liste_simple.head;
          NodeSimple *n2 = screen->liste_simple.head;
          for (int j = 0; j < screen->swap_idx1 && n1; j++)
            n1 = n1->next;
          for (int j = 0; j < screen->swap_idx2 && n2; j++)
            n2 = n2->next;
          if (n1 && n2) {
            char temp[32];
            strncpy(temp, n1->data, 31);
            strncpy(n1->data, n2->data, 31);
            strncpy(n2->data, temp, 31);
          }
        } else {
          NodeDouble *n1 = screen->liste_double.head;
          NodeDouble *n2 = screen->liste_double.head;
          for (int j = 0; j < screen->swap_idx1 && n1; j++)
            n1 = n1->next;
          for (int j = 0; j < screen->swap_idx2 && n2; j++)
            n2 = n2->next;
          if (n1 && n2) {
            char temp[32];
            strncpy(temp, n1->data, 31);
            strncpy(n1->data, n2->data, 31);
            strncpy(n2->data, temp, 31);
          }
        }
        screen->swap_animating = false;
        screen->swap_idx1 = -1;
        screen->swap_idx2 = -1;
        screen->qs_i++;
      }
    } else {
      // QuickSort state machine
      if (screen->qs_phase == 1) {
        // Partition phase
        if (screen->qs_j < screen->qs_high) {
          // Get values at j and pivot
          char *val_j = NULL, *val_pivot = NULL;
          if (screen->type == LISTE_TYPE_SIMPLE) {
            NodeSimple *nj = screen->liste_simple.head;
            NodeSimple *np = screen->liste_simple.head;
            for (int k = 0; k < screen->qs_j && nj; k++)
              nj = nj->next;
            for (int k = 0; k < screen->qs_pivot_idx && np; k++)
              np = np->next;
            if (nj)
              val_j = nj->data;
            if (np)
              val_pivot = np->data;
          } else {
            NodeDouble *nj = screen->liste_double.head;
            NodeDouble *np = screen->liste_double.head;
            for (int k = 0; k < screen->qs_j && nj; k++)
              nj = nj->next;
            for (int k = 0; k < screen->qs_pivot_idx && np; k++)
              np = np->next;
            if (nj)
              val_j = nj->data;
            if (np)
              val_pivot = np->data;
          }

          bool less = false;
          if (val_j && val_pivot) {
            if (screen->random_data_type == 0)
              less = (atoi(val_j) < atoi(val_pivot));
            else if (screen->random_data_type == 1)
              less = (atof(val_j) < atof(val_pivot));
            else
              less = (strcmp(val_j, val_pivot) < 0);
          }

          if (less) {
            if (screen->qs_i != screen->qs_j) {
              // Need to swap
              screen->swap_animating = true;
              screen->swap_progress = 0;
              screen->swap_idx1 = screen->qs_i;
              screen->swap_idx2 = screen->qs_j;
              // qs_i and qs_j will be incremented after swap completes
            } else {
              screen->qs_i++;
              screen->qs_j++;
            }
          } else {
            screen->qs_j++;
          }
        } else {
          // Partition done - swap pivot with element at i
          if (screen->qs_i != screen->qs_high) {
            screen->swap_animating = true;
            screen->swap_progress = 0;
            screen->swap_idx1 = screen->qs_i;
            screen->swap_idx2 = screen->qs_high;
          }
          screen->qs_phase = 2;
        }
      } else if (screen->qs_phase == 2) {
        // Wait for final swap and push sub-arrays
        if (!screen->swap_animating) {
          int pivot = screen->qs_i;
          screen->qs_partitions_done++;

          // Push right sub-array
          if (pivot + 1 < screen->qs_high) {
            screen->qs_stack[screen->qs_stack_top++] = pivot + 1;
            screen->qs_stack[screen->qs_stack_top++] = screen->qs_high;
          }
          // Push left sub-array
          if (screen->qs_low < pivot - 1) {
            screen->qs_stack[screen->qs_stack_top++] = screen->qs_low;
            screen->qs_stack[screen->qs_stack_top++] = pivot - 1;
          }
          screen->qs_phase = 0;
        }
      } else if (screen->qs_phase == 0) {
        // Phase 0: pop next partition from stack or terminate
        if (screen->qs_stack_top == 0) {
          // Done - no more partitions
          screen->quicksort_running = false;
          screen->anim_type = ANIM_NONE;
          screen->sort_progress = 1.0f;
        } else {
          // Pop next partition
          screen->qs_high = screen->qs_stack[--screen->qs_stack_top];
          screen->qs_low = screen->qs_stack[--screen->qs_stack_top];

          if (screen->qs_low < screen->qs_high) {
            screen->qs_pivot_idx = screen->qs_high;
            screen->qs_i = screen->qs_low;
            screen->qs_j = screen->qs_low;
            screen->qs_phase = 1;
          }
          // If low >= high, stay in phase 0 to pop next
        }
      }
    }
  }

  // ==================== INLINE NODE EDITING ====================
  // 1. Handle Clicks (Start Editing)
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !screen->manual_popup_active &&
      !screen->popup_visible && !screen->search_active &&
      !screen->sort_running) {
    Vector2 mouseScreen = GetMousePosition();
    Rectangle vizRect = {30, 120 + NAVBAR_HEIGHT, WINDOW_WIDTH - 60,
                         WINDOW_HEIGHT - 145 - NAVBAR_HEIGHT};

    // Only check if inside visualization area
    if (CheckCollisionPointRec(mouseScreen, vizRect)) {
      Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, screen->camera);

      int listTop = vizRect.y + 25 + NODE_HEIGHT + 25; // derived from layout
      int nodesPerRow = (screen->type == LISTE_TYPE_SIMPLE) ? 8 : 6;
      int nodeSpacingX = (int)((vizRect.width - 50) / nodesPerRow);
      int rowHeight = NODE_HEIGHT + 60;
      int startX = vizRect.x + 30;
      int baseY = listTop + 90 - (int)screen->scroll_y;

      int count = (screen->type == LISTE_TYPE_SIMPLE)
                      ? screen->liste_simple.count
                      : screen->liste_double.count;
      int clickedIndex = -1;

      for (int i = 0; i < count; i++) {
        int col = i % nodesPerRow;
        int row = i / nodesPerRow;
        int x = startX + col * nodeSpacingX;
        int y = baseY + row * rowHeight;

        // Node rectangle (Left=x, Top=y-NODE_HEIGHT/2)
        Rectangle nodeRect = {(float)x, (float)(y - NODE_HEIGHT / 2),
                              (float)NODE_WIDTH, (float)NODE_HEIGHT};

        if (CheckCollisionPointRec(mouseWorld, nodeRect)) {
          clickedIndex = i;
          break;
        }
      }

      if (clickedIndex != -1) {
        screen->editing_node_index = clickedIndex;
        screen->editing_node_active = true;

        // Copy current data to buffer
        if (screen->type == LISTE_TYPE_SIMPLE) {
          NodeSimple *curr = screen->liste_simple.head;
          for (int k = 0; k < clickedIndex; k++)
            curr = curr->next;
          if (curr)
            snprintf(screen->editing_node_text, 64, "%s", curr->data);
        } else {
          NodeDouble *curr = screen->liste_double.head;
          for (int k = 0; k < clickedIndex; k++)
            curr = curr->next;
          if (curr)
            snprintf(screen->editing_node_text, 64, "%s", curr->data);
        }
      } else {
        // Clicked empty space -> stop editing
        screen->editing_node_active = false;
        screen->editing_node_index = -1;
      }
    }
  }

  // 2. Handle Edit Input
  if (screen->editing_node_active && screen->editing_node_index != -1) {
    // Input handling similar to InputBox
    int key = GetCharPressed();
    while (key > 0) {
      bool isValid = false;
      if (key >= 32 && key <= 126 && (strlen(screen->editing_node_text) < 30)) {
        if (screen->random_data_type == 0) { // Int
          if ((key >= '0' && key <= '9') ||
              (key == '-' && strlen(screen->editing_node_text) == 0))
            isValid = true;
        } else if (screen->random_data_type == 1) { // Float
          bool hasDot = (strchr(screen->editing_node_text, '.') != NULL);
          if ((key >= '0' && key <= '9') ||
              (key == '-' && strlen(screen->editing_node_text) == 0) ||
              (key == '.' && !hasDot))
            isValid = true;
        } else if (screen->random_data_type == 2) { // Char
          if (strlen(screen->editing_node_text) == 0)
            isValid = true;
        } else { // String
          isValid = true;
        }
      }

      if (isValid) {
        int len = strlen(screen->editing_node_text);
        screen->editing_node_text[len] = (char)key;
        screen->editing_node_text[len + 1] = '\0';
      }
      key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
      int len = strlen(screen->editing_node_text);
      if (len > 0)
        screen->editing_node_text[len - 1] = '\0';
    }

    // Confirm on ENTER
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
      // Save data back to node
      if (screen->type == LISTE_TYPE_SIMPLE) {
        NodeSimple *curr = screen->liste_simple.head;
        for (int k = 0; k < screen->editing_node_index; k++)
          curr = curr->next;
        if (curr) {
          memset(curr->data, 0, 32);
          strncpy(curr->data, screen->editing_node_text, 31);
        }
      } else {
        NodeDouble *curr = screen->liste_double.head;
        for (int k = 0; k < screen->editing_node_index; k++)
          curr = curr->next;
        if (curr) {
          memset(curr->data, 0, 32);
          strncpy(curr->data, screen->editing_node_text, 31);
        }
      }
      screen->editing_node_active = false;
      screen->editing_node_index = -1;
    }

    // Cancel on ESCAPE
    if (IsKeyPressed(KEY_ESCAPE)) {
      screen->editing_node_active = false;
      screen->editing_node_index = -1;
    }
  }

  // Handle manual popup
  if (screen->manual_popup_active) {
    UpdateManualPopup(screen);
  }

  // Manuel button click - opens popup
  if (UpdateButton(&screen->btn_manual)) {
    screen->manual_popup_active = true;
    strncpy(screen->manual_size_input.text, "", 8);
    screen->manual_size_input.cursor = 0;
    screen->manual_type_selected = 0;
    screen->manual_size_input.is_focused = true;

    // Clear other focus
    screen->input_node_active = false;
    screen->editing_node_active = false;
    screen->search_active = false;
  }

  return SCREEN_LISTES;
}

void ListesDraw(ListesScreen *screen) {
  ClearBackground(COLOR_BG_DARK);

  // Navigation buttons
  DrawNavigationBar(SCREEN_LISTES);
  DrawButton(&screen->btn_type_simple);
  DrawButton(&screen->btn_type_double);

  // Dynamic title - after navbar (at position 610 for 2cm gap after Graphes
  // button)
  const char *title = (screen->type == LISTE_TYPE_SIMPLE)
                          ? "LISTE SIMPLE CHAINEE"
                          : "LISTE DOUBLEMENT CHAINEE";
  Color titleColor = COLOR_NEON_BLUE;
  int titleX = 670;
  int titleY = (NAVBAR_HEIGHT - 18) / 2;
  DrawText(title, titleX, titleY, 18, titleColor);

  // Highlight current type
  if (screen->type == LISTE_TYPE_SIMPLE) {
    DrawRectangleLinesEx(screen->btn_type_simple.bounds, 2, COLOR_NEON_ORANGE);
  } else {
    DrawRectangleLinesEx(screen->btn_type_double.bounds, 2, COLOR_NEON_ORANGE);
  }

  // ============= CADRES COLORES AVEC HOVER =============
  // Borders are always lit with their neon color
  // Hover fills the inside with a subtle glow instead
  Vector2 mousePos = GetMousePosition();

  // Frame 1: INSERTION (Vert) - border always green
  Rectangle frameInsert = {30, 15 + NAVBAR_HEIGHT, 140, 95};
  bool hoverInsert = CheckCollisionPointRec(mousePos, frameInsert);
  if (hoverInsert) {
    DrawRectangleRounded(frameInsert, 0.1f, 8, (Color){80, 220, 120, 30});
  }
  DrawCustomFrame(frameInsert, "INSERTION", COLOR_NEON_GREEN);

  // Frame 2: SUPPRESSION (Rouge) - border always red
  Rectangle frameDel = {215, 15 + NAVBAR_HEIGHT, 200, 95};
  bool hoverDel = CheckCollisionPointRec(mousePos, frameDel);
  if (hoverDel) {
    DrawRectangleRounded(frameDel, 0.1f, 8, (Color){255, 80, 80, 30});
  }
  DrawCustomFrame(frameDel, "SUPPRESSION", COLOR_NEON_RED);

  // Frame 3: RECHERCHE (Bleu) - border always blue
  Rectangle frameSearch = {460, 15 + NAVBAR_HEIGHT, 130, 95};
  bool hoverSearch = CheckCollisionPointRec(mousePos, frameSearch);
  if (hoverSearch) {
    DrawRectangleRounded(frameSearch, 0.1f, 8, (Color){80, 180, 255, 30});
  }
  DrawCustomFrame(frameSearch, "RECHERCHE", COLOR_NEON_BLUE);

  // Frame 4: TRI (Violet) - border always purple
  Rectangle frameSort = {635, 15 + NAVBAR_HEIGHT, 175, 95};
  bool hoverSort = CheckCollisionPointRec(mousePos, frameSort);
  if (hoverSort) {
    DrawRectangleRounded(frameSort, 0.1f, 8, (Color){180, 130, 255, 30});
  }
  DrawCustomFrame(frameSort, "TRI", COLOR_NEON_PURPLE);

  // Frame 5: ALEATOIRE (Bleu) - border always blue
  Rectangle frameGen = {855, 15 + NAVBAR_HEIGHT, 170, 95};
  bool hoverGen = CheckCollisionPointRec(mousePos, frameGen);
  if (hoverGen) {
    DrawRectangleRounded(frameGen, 0.1f, 8, (Color){80, 180, 255, 30});
  }
  DrawCustomFrame(frameGen, "ALEATOIRE", COLOR_NEON_BLUE);

  // ============= BOUTONS DANS LES CADRES =============
  // Insertion
  DrawButton(&screen->btn_inserer_debut);
  DrawButton(&screen->btn_inserer_fin);
  DrawButton(&screen->btn_inserer_position);
  DrawInputBox(&screen->input_position_insert);

  // Suppression - all buttons and inputs on same lines
  DrawButton(&screen->btn_supprimer_debut);
  DrawButton(&screen->btn_supprimer_fin);
  DrawButton(&screen->btn_supprimer_valeur);
  DrawInputBox(&screen->input_valeur_delete);
  DrawButton(&screen->btn_supprimer_position);
  DrawInputBox(&screen->input_position_delete);

  // Recherche - standard buttons and inputs
  DrawButton(&screen->btn_rechercher_val);
  DrawInputBox(&screen->input_recherche_val);
  DrawButton(&screen->btn_rechercher_pos);
  DrawInputBox(&screen->input_recherche_pos);

  // Tri buttons and progress bar
  DrawButton(&screen->btn_tri_insertion);
  DrawButton(&screen->btn_tri_quicksort);
  DrawButton(&screen->btn_stop_tri);

  // Neon progress bar for sorting (in TRI frame bottom area)
  if (screen->sort_running || screen->quicksort_running) {
    Rectangle progressBg = {frameSort.x + 10, frameSort.y + 22,
                            frameSort.width - 20, 26};
    DrawRectangleRounded(progressBg, 0.3f, 8, COLOR_SECONDARY);
    DrawRectRoundedLinesThick(progressBg, 0.3f, 8, 1, COLOR_NEON_PURPLE);

    // Fill based on progress
    float progress = screen->sort_progress;
    if (progress > 0) {
      Rectangle progressFill = {progressBg.x + 2, progressBg.y + 2,
                                (progressBg.width - 4) * progress,
                                progressBg.height - 4};
      DrawRectangleRounded(progressFill, 0.3f, 8, COLOR_NEON_PURPLE);
    }

    // Percentage text
    char percentText[16];
    snprintf(percentText, sizeof(percentText), "%d%%", (int)(progress * 100));
    int txtW = MeasureText(percentText, 12);
    DrawText(percentText, progressBg.x + (progressBg.width - txtW) / 2,
             progressBg.y + 7, 12, COLOR_TEXT_WHITE);
  }

  // Aleatoire
  DrawButton(&screen->btn_generer_aleatoire);
  DrawInputBox(&screen->input_taille_aleatoire);
  // Draw "Taille:" hint if input is empty and not focused
  if (strlen(screen->input_taille_aleatoire.text) == 0 &&
      !screen->input_taille_aleatoire.is_focused) {
    DrawText("Taille...", screen->input_taille_aleatoire.bounds.x + 8,
             screen->input_taille_aleatoire.bounds.y + 8, 12, COLOR_TEXT_DIM);
  }

  // Dropdown - positioned next to Générer button
  const char *types[] = {"Int", "Float", "Char", "Str"};
  Rectangle dropdownRect = {screen->btn_generer_aleatoire.bounds.x + 80,
                            screen->btn_generer_aleatoire.bounds.y, 70, 26};
  DrawRectangleRounded(dropdownRect, 0.2f, 8, COLOR_PANEL);
  DrawRectRoundedLinesThick(dropdownRect, 0.2f, 8, 2, COLOR_BORDER);
  DrawText(types[screen->random_data_type], dropdownRect.x + 8,
           dropdownRect.y + 6, 14, COLOR_TEXT_MAIN);
  // Down arrow
  DrawText("v", dropdownRect.x + 55, dropdownRect.y + 5, 14, COLOR_TEXT_DIM);
  // NOTE: Dropdown options are drawn at the very end of this function to appear
  // on top

  DrawButton(&screen->btn_vider);
  // Custom Draw for Manual Button (Filled Green)
  Rectangle mbRect = screen->btn_manual.bounds;
  // Shadow/Glow
  if (screen->btn_manual.glow_alpha > 0.01f) {
    Color glow = COLOR_NEON_GREEN;
    glow.a = (unsigned char)(60 * screen->btn_manual.glow_alpha);
    DrawRectangleRounded((Rectangle){mbRect.x - 4, mbRect.y + 4,
                                     mbRect.width + 8, mbRect.height},
                         0.3f, 8, glow);
  }
  // Filled Background
  Color mbBg = screen->btn_manual.is_hovered ? COLOR_NEON_GREEN
                                             : (Color){60, 180, 90, 255};
  if (screen->btn_manual.is_pressed)
    mbBg = (Color){40, 140, 70, 255};
  DrawRectangleRounded(mbRect, 0.3f, 8, mbBg);

  // Border (matching or slightly lighter)
  DrawRectRoundedLinesThick(mbRect, 0.3f, 8, 2,
                            screen->btn_manual.is_hovered
                                ? (Color){150, 255, 180, 255}
                                : COLOR_NEON_GREEN);

  // Text (Dark on Green)
  int mbTxtW = MeasureText("MANUEL", FONT_SIZE_NORMAL);
  DrawText("MANUEL", mbRect.x + (mbRect.width - mbTxtW) / 2,
           mbRect.y + (mbRect.height - FONT_SIZE_NORMAL) / 2, FONT_SIZE_NORMAL,
           COLOR_BG_DARK);

  // Visualization area
  Rectangle vizRect = {30, 120 + NAVBAR_HEIGHT, WINDOW_WIDTH - 60,
                       WINDOW_HEIGHT - 145 - NAVBAR_HEIGHT};
  DrawPanel(vizRect, "Visualisation", COLOR_BORDER);

  // Input node
  int inputNodeWidth = NODE_WIDTH;
  int inputNodeX = vizRect.x + (vizRect.width - NODE_WIDTH) / 2;
  int inputNodeY = vizRect.y + 25;
  DrawInputNode(inputNodeX, inputNodeY, screen->input_node_text,
                screen->type == LISTE_TYPE_DOUBLE, screen->input_node_active,
                &inputNodeWidth);

  // List area
  int listTop = inputNodeY + NODE_HEIGHT + 25;
  int listBottom = vizRect.y + vizRect.height - 20;
  int listHeight = listBottom - listTop;

  int nodesPerRow = (screen->type == LISTE_TYPE_SIMPLE) ? 8 : 6;
  int nodeSpacingX = (int)((vizRect.width - 50) / nodesPerRow);
  int rowHeight = NODE_HEIGHT + 60;
  int startX = vizRect.x + 30;
  int baseY =
      listTop + 90 - (int)screen->scroll_y; // More margin for Mario visibility

  float swapOffset = 0;
  if (screen->swap_animating) {
    swapOffset = sinf(screen->swap_progress * 3.14159f) * 25;
  }

  // Clipping + Camera
  BeginScissorMode((int)vizRect.x + 5, listTop, (int)vizRect.width - 10,
                   listHeight);
  BeginMode2D(screen->camera);

  // Draw nodes
  if (screen->type == LISTE_TYPE_SIMPLE) {
    NodeSimple *current = screen->liste_simple.head;
    int idx = 0;
    while (current != NULL) {
      int col = idx % nodesPerRow;
      int row = idx / nodesPerRow;
      int x = startX + col * nodeSpacingX;
      int y = baseY + row * rowHeight;

      // Skip culling during zoom/search
      int nodeTop = y - NODE_HEIGHT / 2;
      int nodeBottom = y + NODE_HEIGHT / 2;
      bool zoomActive = (screen->camera.zoom > 1.05f) || screen->search_active;
      if (!zoomActive && (nodeBottom < listTop || nodeTop > listBottom)) {
        current = current->next;
        idx++;
        continue;
      }

      int yOffset = 0;
      if (screen->swap_animating) {
        if (idx == screen->swap_idx1)
          yOffset = -(int)swapOffset;
        else if (idx == screen->swap_idx2)
          yOffset = (int)swapOffset;
      }

      bool highlighted = (idx == screen->highlighted_index);
      bool swapping = screen->swap_animating &&
                      (idx == screen->swap_idx1 || idx == screen->swap_idx2);

      bool isEditing =
          (screen->editing_node_active && idx == screen->editing_node_index);

      // Hide node if Mario holds it
      if (screen->search_active && screen->search_victory &&
          idx == screen->search_current_node_idx) {
        // Skip drawing - Mario holds it
      } else {
        DrawNodeSimple(x, y - NODE_HEIGHT / 2 + yOffset,
                       isEditing ? screen->editing_node_text : current->data,
                       highlighted || swapping || isEditing, idx);

        if (isEditing) {
          // Draw editing cursor and border
          Rectangle nodeRect = {(float)x,
                                (float)(y - NODE_HEIGHT / 2 + yOffset),
                                (float)NODE_WIDTH, (float)NODE_HEIGHT};
          DrawRectangleLinesEx(nodeRect, 2, COLOR_NEON_ORANGE);

          // Simple cursor calculation (centered text assumption)
          int textW = MeasureText(screen->editing_node_text, 20);
          // Text is drawn centered in DrawNodeSimple, typically at x + WIDTH/2
          // - textW/2
          int cursorX = x + (NODE_WIDTH - textW) / 2 + textW + 2;
          int cursorY = y - NODE_HEIGHT / 2 + yOffset + (NODE_HEIGHT - 20) / 2;

          if ((int)(GetTime() * 2) % 2 == 0) {
            DrawRectangle(cursorX, cursorY, 2, 20, COLOR_NEON_ORANGE);
          }
        }
      }

      // Arrows
      if (current->next != NULL) {
        int nextCol = (idx + 1) % nodesPerRow;
        if (nextCol > 0) {
          DrawArrowRight(x + NODE_WIDTH, y + yOffset, x + nodeSpacingX - 15, y,
                         COLOR_NEON_ORANGE);
        }
      } else {
        DrawText("NULL", x + NODE_WIDTH + 5, y + yOffset - 8, 12,
                 COLOR_TEXT_DIM);
      }

      current = current->next;
      idx++;
    }

    if (screen->liste_simple.count == 0) {
      DrawText("Liste vide", vizRect.x + 50, listTop + 30, 16, COLOR_TEXT_DIM);
    }
  } else {
    // Double list - similar logic
    NodeDouble *current = screen->liste_double.head;
    int idx = 0;
    while (current != NULL) {
      int col = idx % nodesPerRow;
      int row = idx / nodesPerRow;
      int x = startX + col * nodeSpacingX;
      int y = baseY + row * rowHeight;

      int nodeTop = y - NODE_HEIGHT / 2;
      int nodeBottom = y + NODE_HEIGHT / 2;
      bool zoomActive = (screen->camera.zoom > 1.05f) || screen->search_active;
      if (!zoomActive && (nodeBottom < listTop || nodeTop > listBottom)) {
        current = current->next;
        idx++;
        continue;
      }

      int yOffset = 0;
      if (screen->swap_animating) {
        if (idx == screen->swap_idx1)
          yOffset = -(int)swapOffset;
        else if (idx == screen->swap_idx2)
          yOffset = (int)swapOffset;
      }

      bool highlighted = (idx == screen->highlighted_index);

      bool isEditing =
          (screen->editing_node_active && idx == screen->editing_node_index);

      if (screen->search_active && screen->search_victory &&
          idx == screen->search_current_node_idx) {
        // Skip
      } else {
        DrawNodeDouble(x, y - NODE_HEIGHT / 2 + yOffset,
                       isEditing ? screen->editing_node_text : current->data,
                       highlighted || isEditing, idx);

        if (isEditing) {
          // Draw editing cursor and border
          Rectangle nodeRect = {(float)x,
                                (float)(y - NODE_HEIGHT / 2 + yOffset),
                                (float)NODE_WIDTH, (float)NODE_HEIGHT};
          DrawRectangleLinesEx(nodeRect, 2, COLOR_NEON_ORANGE);

          // Simple cursor calculation (centered text assumption)
          int textW = MeasureText(screen->editing_node_text, 20);
          // Text is drawn centered in DrawNodeSimple, typically at x + WIDTH/2
          // - textW/2
          int cursorX = x + (NODE_WIDTH - textW) / 2 + textW + 2;
          int cursorY = y - NODE_HEIGHT / 2 + yOffset + (NODE_HEIGHT - 20) / 2;

          if ((int)(GetTime() * 2) % 2 == 0) {
            DrawRectangle(cursorX, cursorY, 2, 20, COLOR_NEON_ORANGE);
          }
        }
      }

      if (current->next != NULL) {
        int nextCol = (idx + 1) % nodesPerRow;
        if (nextCol > 0) {
          // Forward arrow (next) - upper line, orange
          DrawArrowRight(x + NODE_WIDTH, y + yOffset - 5, x + nodeSpacingX - 15,
                         y - 5, COLOR_NEON_ORANGE);
          // Backward arrow (prev) - lower line, purple
          DrawArrowRight(x + nodeSpacingX - 15, y + yOffset + 5, x + NODE_WIDTH,
                         y + 5, COLOR_NEON_PURPLE);
        }
      }

      current = current->next;
      idx++;
    }

    if (screen->liste_double.count == 0) {
      DrawText("Liste vide", vizRect.x + 50, listTop + 30, 16, COLOR_TEXT_DIM);
    }
  }

  // MARIO ANIMATION
  if (screen->search_active) {
    int targetIdx = screen->search_current_node_idx;
    int row = targetIdx / nodesPerRow;
    int col = targetIdx % nodesPerRow;
    int targetX = startX + col * nodeSpacingX + NODE_WIDTH / 2;
    int targetY = baseY + row * rowHeight + NODE_HEIGHT / 2;

    int prevX = targetX, prevY = targetY;
    bool isLineBreak = false;
    if (targetIdx > 0) {
      int pRow = (targetIdx - 1) / nodesPerRow;
      int pCol = (targetIdx - 1) % nodesPerRow;
      prevX = startX + pCol * nodeSpacingX + NODE_WIDTH / 2;
      prevY = baseY + pRow * rowHeight + NODE_HEIGHT / 2;
      if (pRow != row)
        isLineBreak = true;
    } else {
      prevX = targetX - 100;
    }

    float t = screen->search_jump_progress;
    Vector2 pos;

    if (isLineBreak) {
      int limitRight = startX + nodesPerRow * nodeSpacingX;
      int limitLeft = startX - NODE_WIDTH;
      float topY = (float)prevY - 30;
      if (t < 0.5f) {
        float subT = t * 2.0f;
        pos.x = Lerp((float)prevX, (float)limitRight, subT);
        pos.y = topY;
      } else {
        float subT = (t - 0.5f) * 2.0f;
        pos.x = Lerp((float)limitLeft, (float)targetX, subT);
        pos.y = (float)targetY - 30;
      }
    } else {
      pos.x = Lerp((float)prevX, (float)targetX, t);
      float baseLy = Lerp((float)prevY, (float)targetY, t) - 30;
      float jumpH = screen->search_victory ? 0 : 60.0f;
      float yOff = -jumpH * 4.0f * t * (1.0f - t);
      pos.y = baseLy + yOff;
    }

    // Draw Mario
    int mx = (int)pos.x;
    int my = (int)pos.y - 30;

    Color M_RED = (Color){230, 0, 0, 255};
    Color M_BLUE = (Color){0, 0, 230, 255};
    Color M_SKIN = (Color){255, 206, 170, 255};

    // Hat
    DrawRectangle(mx - 12, my - 14, 24, 6, M_RED);
    DrawRectangle(mx - 12, my - 14, 14, 8, M_RED);
    // Face
    DrawRectangle(mx - 10, my - 8, 20, 8, M_SKIN);
    DrawRectangle(mx + 2, my - 4, 8, 2, BLACK);
    // Body
    DrawRectangle(mx - 10, my, 20, 16, M_BLUE);

    if (screen->search_victory) {
      // Arms up
      DrawRectangle(mx - 16, my - 4, 6, 14, M_RED);
      DrawRectangle(mx + 10, my - 4, 6, 14, M_RED);

      // Draw lifted node
      int nodeLiftY = my - 45;
      DrawRectangleRounded((Rectangle){mx - NODE_WIDTH / 2,
                                       nodeLiftY - NODE_HEIGHT / 2, NODE_WIDTH,
                                       NODE_HEIGHT},
                           0.2f, 8, (Color){255, 200, 0, 255});
      DrawRectRoundedLinesThick((Rectangle){mx - NODE_WIDTH / 2,
                                            nodeLiftY - NODE_HEIGHT / 2,
                                            NODE_WIDTH, NODE_HEIGHT},
                                0.2f, 8, 2, BLACK);

      // Trophy text
      const char *displayText;
      static char posStr[16];
      if (screen->search_mode_is_pos) {
        displayText = screen->search_target_val;
      } else {
        snprintf(posStr, sizeof(posStr), "[%d]",
                 screen->search_current_node_idx);
        displayText = posStr;
      }
      int txtW = MeasureText(displayText, 20);
      DrawText(displayText, mx - txtW / 2, nodeLiftY - 10, 20, BLACK);
    } else {
      DrawRectangle(mx - 14, my + 2, 4, 12, M_RED);
      DrawRectangle(mx + 10, my + 2, 4, 12, M_RED);
    }

    // Buttons
    DrawRectangle(mx - 6, my + 4, 4, 4, GOLD);
    DrawRectangle(mx + 2, my + 4, 4, 4, GOLD);

    // Legs
    if (!screen->search_victory && (int)(t * 10) % 2 == 0) {
      DrawRectangle(mx - 10, my + 16, 8, 8, M_BLUE);
      DrawRectangle(mx + 2, my + 16, 8, 8, M_BLUE);
    } else {
      DrawRectangle(mx - 10, my + 16, 9, 8, M_BLUE);
      DrawRectangle(mx + 1, my + 16, 9, 8, M_BLUE);
    }

    if (screen->search_victory) {
      DrawText("IT'S ME!", mx - 20, my - 30, 10, GOLD);
    }
  }

  EndMode2D();
  EndScissorMode();

  // ============= DROPDOWN OPTIONS (drawn last to appear on top) =============
  if (screen->type_dropdown_open) {
    const char *typesOpts[] = {"Int", "Float", "Char", "Str"};
    Rectangle ddRect = {screen->btn_generer_aleatoire.bounds.x + 80,
                        screen->btn_generer_aleatoire.bounds.y, 70, 26};
    for (int i = 0; i < 4; i++) {
      Rectangle optRect = {ddRect.x, ddRect.y + 28 + i * 24, 70, 24};
      Color bg =
          (i == screen->random_data_type) ? COLOR_NEON_BLUE : COLOR_PANEL;
      DrawRectangleRounded(optRect, 0.2f, 8, bg);
      DrawRectRoundedLinesThick(optRect, 0.2f, 8, 1, COLOR_BORDER);
      DrawText(typesOpts[i], optRect.x + 8, optRect.y + 5, 14, COLOR_TEXT_MAIN);
    }
  }

  // Popup
  if (screen->popup_visible) {
    Rectangle popupRect = {WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 60, 300,
                           120};
    DrawRectangleRounded(popupRect, 0.1f, 8, COLOR_PANEL);
    DrawRectRoundedLinesThick(popupRect, 0.1f, 8, 2, COLOR_NEON_ORANGE);
    DrawText(screen->popup_title, popupRect.x + 20, popupRect.y + 15, 20,
             COLOR_NEON_ORANGE);
    DrawText(screen->popup_message, popupRect.x + 20, popupRect.y + 50, 14,
             COLOR_TEXT_MAIN);
  }

  // Manual input popup
  if (screen->manual_popup_active) {
    DrawManualPopup(screen);
  }
}

void ListesUnload(ListesScreen *screen) {
  ListeSimple_Free(&screen->liste_simple);
  ListeDouble_Free(&screen->liste_double);
}
