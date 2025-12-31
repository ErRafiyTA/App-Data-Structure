/**
 * SORTING.H - Algorithmes de tri
 */

#ifndef SORTING_H
#define SORTING_H

#include <stdbool.h>

// Note: Ne pas inclure windows.h ici pour éviter les conflits avec Raylib
// Les handles de threads sont gérés avec void* et castés dans sorting.c

// ==================== TYPES DE DONNÉES ====================

typedef enum {
  DATA_TYPE_INT,
  DATA_TYPE_FLOAT,
  DATA_TYPE_CHAR,
  DATA_TYPE_STRING
} DataType;

// ==================== ALGORITHMES DE TRI ====================

typedef enum {
  SORT_BUBBLE,
  SORT_INSERTION,
  SORT_SHELL,
  SORT_QUICK,
  SORT_COUNT
} SortAlgorithm;

// ==================== DONNÉES GÉNÉRIQUES ====================

// Structure pour stocker des données de différents types
typedef struct {
  DataType type;
  int size;
  union {
    long long *int_data; // Entiers (millions/milliards)
    double *float_data;  // Réels
    char *char_data;     // Caractères
    char **string_data;  // Chaînes
  } data;
} GenericData;

// ==================== STRUCTURE POUR TRI PARALLÈLE ====================

typedef struct {
  SortAlgorithm algo;
  GenericData *data; // Copie des données à trier
  double elapsed_time;
  bool running;
  bool completed;
  bool should_stop;
  void *thread; // Handle de thread (HANDLE sur Windows, pthread_t sur Linux)
} SortThread;

// Structure pour gérer tous les tris
typedef struct {
  GenericData *original;
  GenericData *sorted[SORT_COUNT];
  double times[SORT_COUNT];
  bool completed[SORT_COUNT];
  bool running[SORT_COUNT];
  SortThread threads[SORT_COUNT];
} SortManager;

// Noms des algorithmes
extern const char *SORT_NAMES[SORT_COUNT];

// ==================== GÉNÉRATION DE DONNÉES ====================

// Génère des données aléatoires selon le type
GenericData *GenerateTypedData(int size, DataType type);

// Génère des entiers dans une plage (millions/milliards)
long long *GenerateRandomInts(int size, long long min_val, long long max_val);

// Génère des réels aléatoires
double *GenerateRandomFloats(int size, double min_val, double max_val);

// Génère des caractères aléatoires
char *GenerateRandomChars(int size);

// Génère des chaînes aléatoires
char **GenerateRandomStrings(int size, int max_len);

// Copie des données génériques
GenericData *CopyGenericData(const GenericData *src);

// Libère la mémoire
void FreeGenericData(GenericData *data);

// Ancienne fonction pour compatibilité
int *GenerateRandomData(int size, int min_val, int max_val);
int *CopyData(const int *src, int size);
void FreeData(int *data);

// ==================== ALGORITHMES DE TRI (GÉNÉRIQUES) ====================

// Tri à bulles
void BubbleSortInt(long long *arr, int n);
void BubbleSortFloat(double *arr, int n);
void BubbleSortChar(char *arr, int n);
void BubbleSortString(char **arr, int n);

// Tri par insertion
void InsertionSortInt(long long *arr, int n);
void InsertionSortFloat(double *arr, int n);
void InsertionSortChar(char *arr, int n);
void InsertionSortString(char **arr, int n);

// Tri Shell
void ShellSortInt(long long *arr, int n);
void ShellSortFloat(double *arr, int n);
void ShellSortChar(char *arr, int n);
void ShellSortString(char **arr, int n);

// Tri rapide
void QuickSortInt(long long *arr, int low, int high);
void QuickSortFloat(double *arr, int low, int high);
void QuickSortChar(char *arr, int low, int high);
void QuickSortString(char **arr, int low, int high);

// Anciennes fonctions pour compatibilité
void BubbleSort(int *arr, int n);
void InsertionSort(int *arr, int n);
void ShellSort(int *arr, int n);
void QuickSort(int *arr, int low, int high);

// ==================== TRI PARALLÈLE ====================

// Lance un tri dans un thread séparé
void StartSortThread(SortThread *st, SortAlgorithm algo, GenericData *data);

// Attend la fin d'un thread de tri
void WaitSortThread(SortThread *st);

// Arrête un thread de tri
void StopSortThread(SortThread *st);

// Lance tous les tris en parallèle
void StartAllSorts(SortManager *manager, GenericData *original);

// Vérifie si tous les tris sont terminés
bool AllSortsCompleted(SortManager *manager);

// Arrête tous les tris
void StopAllSorts(SortManager *manager);

// ==================== MESURE DE PERFORMANCE ====================

// Exécute un tri et mesure le temps
double TimedSortGeneric(SortAlgorithm algo, GenericData *data);

// Ancienne fonction pour compatibilité
double TimedSort(SortAlgorithm algo, int *data_copy, int size);

// Vérifie si des données sont triées
bool IsSortedGeneric(const GenericData *data);
bool IsSorted(const int *arr, int n);

// ==================== FORMATAGE TEMPS ====================

// Formate le temps en chaîne lisible (ns, µs, ms, s, min, h)
void FormatTimeString(double seconds, char *buffer, int buf_size);

#endif // SORTING_H
