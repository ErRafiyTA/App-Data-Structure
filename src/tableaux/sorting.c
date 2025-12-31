/**
 * SORTING.C - Implémentation des algorithmes de tri avec support multi-types
 */

#include "sorting.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sys/time.h>
#endif

// Noms des algorithmes
const char *SORT_NAMES[SORT_COUNT] = {"Bubble Sort", "Insertion Sort",
                                      "Shell Sort", "Quick Sort"};

// ==================== FONCTIONS UTILITAIRES ====================

static double GetHighResTime(void) {
#ifdef _WIN32
  LARGE_INTEGER freq, count;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&count);
  return (double)count.QuadPart / (double)freq.QuadPart;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
}

// ==================== FORMATAGE TEMPS ====================

void FormatTimeString(double seconds, char *buffer, int buf_size) {
  if (seconds < 0) {
    snprintf(buffer, buf_size, "-");
    return;
  }

  if (seconds >= 3600.0) {
    double hours = seconds / 3600.0;
    snprintf(buffer, buf_size, "%.2f h", hours);
  } else if (seconds >= 60.0) {
    int minutes = (int)(seconds / 60.0);
    double rem_secs = seconds - (minutes * 60.0);
    snprintf(buffer, buf_size, "%d min %.1f s", minutes, rem_secs);
  } else if (seconds >= 1.0) {
    snprintf(buffer, buf_size, "%.3f s", seconds);
  } else if (seconds >= 0.001) {
    double ms = seconds * 1000.0;
    snprintf(buffer, buf_size, "%.2f ms", ms);
  } else if (seconds >= 0.000001) {
    double us = seconds * 1000000.0;
    snprintf(buffer, buf_size, "%.1f microsec", us);
  } else {
    double ns = seconds * 1000000000.0;
    snprintf(buffer, buf_size, "%.2f ns", ns);
  }
}

// ==================== GÉNÉRATION DE DONNÉES TYPÉES ====================

long long *GenerateRandomInts(int size, long long min_val, long long max_val) {
  long long *data = (long long *)malloc(size * sizeof(long long));
  if (!data)
    return NULL;

  long long range = max_val - min_val + 1;
  for (int i = 0; i < size; i++) {
    // Générer un nombre aléatoire sur 64 bits
    long long r = ((long long)rand() << 32) | rand();
    data[i] = min_val + (r % range);
  }
  return data;
}

double *GenerateRandomFloats(int size, double min_val, double max_val) {
  double *data = (double *)malloc(size * sizeof(double));
  if (!data)
    return NULL;

  double range = max_val - min_val;
  for (int i = 0; i < size; i++) {
    data[i] = min_val + ((double)rand() / RAND_MAX) * range;
  }
  return data;
}

char *GenerateRandomChars(int size) {
  char *data = (char *)malloc(size * sizeof(char));
  if (!data)
    return NULL;

  for (int i = 0; i < size; i++) {
    data[i] = 'A' + (rand() % 26); // A-Z
  }
  return data;
}

char **GenerateRandomStrings(int size, int max_len) {
  char **data = (char **)malloc(size * sizeof(char *));
  if (!data)
    return NULL;

  for (int i = 0; i < size; i++) {
    int len = 3 + (rand() % (max_len - 2)); // 3 à max_len caracteres
    data[i] = (char *)malloc((len + 1) * sizeof(char));
    for (int j = 0; j < len; j++) {
      data[i][j] = 'a' + (rand() % 26);
    }
    data[i][len] = '\0';
  }
  return data;
}

GenericData *GenerateTypedData(int size, DataType type) {
  GenericData *gd = (GenericData *)malloc(sizeof(GenericData));
  if (!gd)
    return NULL;

  gd->type = type;
  gd->size = size;

  switch (type) {
  case DATA_TYPE_INT:
    // Plage millions à milliards
    gd->data.int_data = GenerateRandomInts(size, 1000000LL, 10000000000LL);
    break;
  case DATA_TYPE_FLOAT:
    gd->data.float_data = GenerateRandomFloats(size, 1.0, 1000000.0);
    break;
  case DATA_TYPE_CHAR:
    gd->data.char_data = GenerateRandomChars(size);
    break;
  case DATA_TYPE_STRING:
    gd->data.string_data = GenerateRandomStrings(size, 10);
    break;
  }

  return gd;
}

GenericData *CopyGenericData(const GenericData *src) {
  if (!src)
    return NULL;

  GenericData *copy = (GenericData *)malloc(sizeof(GenericData));
  if (!copy)
    return NULL;

  copy->type = src->type;
  copy->size = src->size;

  switch (src->type) {
  case DATA_TYPE_INT:
    copy->data.int_data = (long long *)malloc(src->size * sizeof(long long));
    memcpy(copy->data.int_data, src->data.int_data,
           src->size * sizeof(long long));
    break;
  case DATA_TYPE_FLOAT:
    copy->data.float_data = (double *)malloc(src->size * sizeof(double));
    memcpy(copy->data.float_data, src->data.float_data,
           src->size * sizeof(double));
    break;
  case DATA_TYPE_CHAR:
    copy->data.char_data = (char *)malloc(src->size * sizeof(char));
    memcpy(copy->data.char_data, src->data.char_data, src->size * sizeof(char));
    break;
  case DATA_TYPE_STRING:
    copy->data.string_data = (char **)malloc(src->size * sizeof(char *));
    for (int i = 0; i < src->size; i++) {
      copy->data.string_data[i] = strdup(src->data.string_data[i]);
    }
    break;
  }

  return copy;
}

void FreeGenericData(GenericData *data) {
  if (!data)
    return;

  switch (data->type) {
  case DATA_TYPE_INT:
    free(data->data.int_data);
    break;
  case DATA_TYPE_FLOAT:
    free(data->data.float_data);
    break;
  case DATA_TYPE_CHAR:
    free(data->data.char_data);
    break;
  case DATA_TYPE_STRING:
    for (int i = 0; i < data->size; i++) {
      free(data->data.string_data[i]);
    }
    free(data->data.string_data);
    break;
  }

  free(data);
}

// ==================== ANCIENNES FONCTIONS (COMPATIBILITÉ) ====================

int *GenerateRandomData(int size, int min_val, int max_val) {
  int *data = (int *)malloc(size * sizeof(int));
  if (!data)
    return NULL;

  int range = max_val - min_val + 1;
  for (int i = 0; i < size; i++) {
    data[i] = min_val + rand() % range;
  }
  return data;
}

int *CopyData(const int *src, int size) {
  if (!src || size <= 0)
    return NULL;
  int *copy = (int *)malloc(size * sizeof(int));
  if (!copy)
    return NULL;
  memcpy(copy, src, size * sizeof(int));
  return copy;
}

void FreeData(int *data) { free(data); }

// ==================== ALGORITHMES DE TRI - ENTIERS LONG ====================

void BubbleSortInt(long long *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    bool swapped = false;
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        long long temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = true;
      }
    }
    if (!swapped)
      break;
  }
}

void InsertionSortInt(long long *arr, int n) {
  for (int i = 1; i < n; i++) {
    long long key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void ShellSortInt(long long *arr, int n) {
  for (int gap = n / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < n; i++) {
      long long temp = arr[i];
      int j = i;
      while (j >= gap && arr[j - gap] > temp) {
        arr[j] = arr[j - gap];
        j -= gap;
      }
      arr[j] = temp;
    }
  }
}

static int PartitionInt(long long *arr, int low, int high) {
  long long pivot = arr[high];
  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (arr[j] <= pivot) {
      i++;
      long long temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
  long long temp = arr[i + 1];
  arr[i + 1] = arr[high];
  arr[high] = temp;
  return i + 1;
}

void QuickSortInt(long long *arr, int low, int high) {
  if (low < high) {
    int pi = PartitionInt(arr, low, high);
    QuickSortInt(arr, low, pi - 1);
    QuickSortInt(arr, pi + 1, high);
  }
}

// ==================== ALGORITHMES DE TRI - FLOAT ====================

void BubbleSortFloat(double *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    bool swapped = false;
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        double temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = true;
      }
    }
    if (!swapped)
      break;
  }
}

void InsertionSortFloat(double *arr, int n) {
  for (int i = 1; i < n; i++) {
    double key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void ShellSortFloat(double *arr, int n) {
  for (int gap = n / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < n; i++) {
      double temp = arr[i];
      int j = i;
      while (j >= gap && arr[j - gap] > temp) {
        arr[j] = arr[j - gap];
        j -= gap;
      }
      arr[j] = temp;
    }
  }
}

static int PartitionFloat(double *arr, int low, int high) {
  double pivot = arr[high];
  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (arr[j] <= pivot) {
      i++;
      double temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
  double temp = arr[i + 1];
  arr[i + 1] = arr[high];
  arr[high] = temp;
  return i + 1;
}

void QuickSortFloat(double *arr, int low, int high) {
  if (low < high) {
    int pi = PartitionFloat(arr, low, high);
    QuickSortFloat(arr, low, pi - 1);
    QuickSortFloat(arr, pi + 1, high);
  }
}

// ==================== ALGORITHMES DE TRI - CHAR ====================

void BubbleSortChar(char *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    bool swapped = false;
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        char temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = true;
      }
    }
    if (!swapped)
      break;
  }
}

void InsertionSortChar(char *arr, int n) {
  for (int i = 1; i < n; i++) {
    char key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void ShellSortChar(char *arr, int n) {
  for (int gap = n / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < n; i++) {
      char temp = arr[i];
      int j = i;
      while (j >= gap && arr[j - gap] > temp) {
        arr[j] = arr[j - gap];
        j -= gap;
      }
      arr[j] = temp;
    }
  }
}

static int PartitionChar(char *arr, int low, int high) {
  char pivot = arr[high];
  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (arr[j] <= pivot) {
      i++;
      char temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
  char temp = arr[i + 1];
  arr[i + 1] = arr[high];
  arr[high] = temp;
  return i + 1;
}

void QuickSortChar(char *arr, int low, int high) {
  if (low < high) {
    int pi = PartitionChar(arr, low, high);
    QuickSortChar(arr, low, pi - 1);
    QuickSortChar(arr, pi + 1, high);
  }
}

// ==================== ALGORITHMES DE TRI - STRING ====================

void BubbleSortString(char **arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    bool swapped = false;
    for (int j = 0; j < n - i - 1; j++) {
      if (strcmp(arr[j], arr[j + 1]) > 0) {
        char *temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = true;
      }
    }
    if (!swapped)
      break;
  }
}

void InsertionSortString(char **arr, int n) {
  for (int i = 1; i < n; i++) {
    char *key = arr[i];
    int j = i - 1;
    while (j >= 0 && strcmp(arr[j], key) > 0) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void ShellSortString(char **arr, int n) {
  for (int gap = n / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < n; i++) {
      char *temp = arr[i];
      int j = i;
      while (j >= gap && strcmp(arr[j - gap], temp) > 0) {
        arr[j] = arr[j - gap];
        j -= gap;
      }
      arr[j] = temp;
    }
  }
}

static int PartitionString(char **arr, int low, int high) {
  char *pivot = arr[high];
  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (strcmp(arr[j], pivot) <= 0) {
      i++;
      char *temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
  char *temp = arr[i + 1];
  arr[i + 1] = arr[high];
  arr[high] = temp;
  return i + 1;
}

void QuickSortString(char **arr, int low, int high) {
  if (low < high) {
    int pi = PartitionString(arr, low, high);
    QuickSortString(arr, low, pi - 1);
    QuickSortString(arr, pi + 1, high);
  }
}

// ==================== ANCIENNES FONCTIONS TRI (COMPATIBILITÉ)
// ====================

void BubbleSort(int *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    bool swapped = false;
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = true;
      }
    }
    if (!swapped)
      break;
  }
}

void InsertionSort(int *arr, int n) {
  for (int i = 1; i < n; i++) {
    int key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void ShellSort(int *arr, int n) {
  for (int gap = n / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < n; i++) {
      int temp = arr[i];
      int j = i;
      while (j >= gap && arr[j - gap] > temp) {
        arr[j] = arr[j - gap];
        j -= gap;
      }
      arr[j] = temp;
    }
  }
}

static int Partition(int *arr, int low, int high) {
  int pivot = arr[high];
  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (arr[j] <= pivot) {
      i++;
      int temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
  int temp = arr[i + 1];
  arr[i + 1] = arr[high];
  arr[high] = temp;
  return i + 1;
}

void QuickSort(int *arr, int low, int high) {
  if (low < high) {
    int pi = Partition(arr, low, high);
    QuickSort(arr, low, pi - 1);
    QuickSort(arr, pi + 1, high);
  }
}

// ==================== TRI GÉNÉRIQUE ====================

double TimedSortGeneric(SortAlgorithm algo, GenericData *data) {
  if (!data)
    return 0;

  double start = GetHighResTime();

  switch (data->type) {
  case DATA_TYPE_INT:
    switch (algo) {
    case SORT_BUBBLE:
      BubbleSortInt(data->data.int_data, data->size);
      break;
    case SORT_INSERTION:
      InsertionSortInt(data->data.int_data, data->size);
      break;
    case SORT_SHELL:
      ShellSortInt(data->data.int_data, data->size);
      break;
    case SORT_QUICK:
      QuickSortInt(data->data.int_data, 0, data->size - 1);
      break;
    default:
      break;
    }
    break;
  case DATA_TYPE_FLOAT:
    switch (algo) {
    case SORT_BUBBLE:
      BubbleSortFloat(data->data.float_data, data->size);
      break;
    case SORT_INSERTION:
      InsertionSortFloat(data->data.float_data, data->size);
      break;
    case SORT_SHELL:
      ShellSortFloat(data->data.float_data, data->size);
      break;
    case SORT_QUICK:
      QuickSortFloat(data->data.float_data, 0, data->size - 1);
      break;
    default:
      break;
    }
    break;
  case DATA_TYPE_CHAR:
    switch (algo) {
    case SORT_BUBBLE:
      BubbleSortChar(data->data.char_data, data->size);
      break;
    case SORT_INSERTION:
      InsertionSortChar(data->data.char_data, data->size);
      break;
    case SORT_SHELL:
      ShellSortChar(data->data.char_data, data->size);
      break;
    case SORT_QUICK:
      QuickSortChar(data->data.char_data, 0, data->size - 1);
      break;
    default:
      break;
    }
    break;
  case DATA_TYPE_STRING:
    switch (algo) {
    case SORT_BUBBLE:
      BubbleSortString(data->data.string_data, data->size);
      break;
    case SORT_INSERTION:
      InsertionSortString(data->data.string_data, data->size);
      break;
    case SORT_SHELL:
      ShellSortString(data->data.string_data, data->size);
      break;
    case SORT_QUICK:
      QuickSortString(data->data.string_data, 0, data->size - 1);
      break;
    default:
      break;
    }
    break;
  }

  double end = GetHighResTime();
  return end - start;
}

double TimedSort(SortAlgorithm algo, int *data_copy, int size) {
  clock_t start = clock();

  switch (algo) {
  case SORT_BUBBLE:
    BubbleSort(data_copy, size);
    break;
  case SORT_INSERTION:
    InsertionSort(data_copy, size);
    break;
  case SORT_SHELL:
    ShellSort(data_copy, size);
    break;
  case SORT_QUICK:
    QuickSort(data_copy, 0, size - 1);
    break;
  default:
    break;
  }

  clock_t end = clock();
  return (double)(end - start) / CLOCKS_PER_SEC;
}

// ==================== VÉRIFICATION TRI ====================

bool IsSortedGeneric(const GenericData *data) {
  if (!data || data->size <= 1)
    return true;

  switch (data->type) {
  case DATA_TYPE_INT:
    for (int i = 0; i < data->size - 1; i++) {
      if (data->data.int_data[i] > data->data.int_data[i + 1])
        return false;
    }
    break;
  case DATA_TYPE_FLOAT:
    for (int i = 0; i < data->size - 1; i++) {
      if (data->data.float_data[i] > data->data.float_data[i + 1])
        return false;
    }
    break;
  case DATA_TYPE_CHAR:
    for (int i = 0; i < data->size - 1; i++) {
      if (data->data.char_data[i] > data->data.char_data[i + 1])
        return false;
    }
    break;
  case DATA_TYPE_STRING:
    for (int i = 0; i < data->size - 1; i++) {
      if (strcmp(data->data.string_data[i], data->data.string_data[i + 1]) > 0)
        return false;
    }
    break;
  }
  return true;
}

bool IsSorted(const int *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    if (arr[i] > arr[i + 1])
      return false;
  }
  return true;
}

// ==================== TRI PARALLÈLE ====================

typedef struct {
  SortThread *st;
} ThreadArg;

#ifdef _WIN32
static DWORD WINAPI SortThreadFunc(LPVOID arg) {
  SortThread *st = (SortThread *)arg;
  st->running = true;
  st->completed = false;

  double start = GetHighResTime();
  TimedSortGeneric(st->algo, st->data);
  double end = GetHighResTime();

  st->elapsed_time = end - start;
  st->running = false;
  st->completed = true;

  return 0;
}
#else
static void *SortThreadFunc(void *arg) {
  SortThread *st = (SortThread *)arg;
  st->running = true;
  st->completed = false;

  double start = GetHighResTime();
  TimedSortGeneric(st->algo, st->data);
  double end = GetHighResTime();

  st->elapsed_time = end - start;
  st->running = false;
  st->completed = true;

  return NULL;
}
#endif

void StartSortThread(SortThread *st, SortAlgorithm algo, GenericData *data) {
  st->algo = algo;
  st->data = CopyGenericData(data); // Copie pour ce thread
  st->elapsed_time = 0;
  st->running = false;
  st->completed = false;
  st->should_stop = false;

#ifdef _WIN32
  st->thread = CreateThread(NULL, 0, SortThreadFunc, st, 0, NULL);
#else
  pthread_create(&st->thread, NULL, SortThreadFunc, st);
#endif
}

void WaitSortThread(SortThread *st) {
#ifdef _WIN32
  if (st->thread) {
    WaitForSingleObject(st->thread, INFINITE);
    CloseHandle(st->thread);
    st->thread = NULL;
  }
#else
  pthread_join(st->thread, NULL);
#endif
}

void StopSortThread(SortThread *st) {
  st->should_stop = true;
#ifdef _WIN32
  if (st->thread) {
    TerminateThread(st->thread, 0);
    CloseHandle(st->thread);
    st->thread = NULL;
  }
#else
  pthread_cancel(st->thread);
#endif
  st->running = false;
}

void StartAllSorts(SortManager *manager, GenericData *original) {
  manager->original = original;

  for (int i = 0; i < SORT_COUNT; i++) {
    manager->completed[i] = false;
    manager->running[i] = true;
    manager->times[i] = 0;
    StartSortThread(&manager->threads[i], (SortAlgorithm)i, original);
  }
}

bool AllSortsCompleted(SortManager *manager) {
  for (int i = 0; i < SORT_COUNT; i++) {
    if (!manager->threads[i].completed) {
      return false;
    }
    // Mettre à jour les temps
    manager->times[i] = manager->threads[i].elapsed_time;
    manager->completed[i] = manager->threads[i].completed;
    manager->running[i] = manager->threads[i].running;
    manager->sorted[i] = manager->threads[i].data;
  }
  return true;
}

void StopAllSorts(SortManager *manager) {
  for (int i = 0; i < SORT_COUNT; i++) {
    if (manager->running[i]) {
      StopSortThread(&manager->threads[i]);
    }
    manager->running[i] = false;
  }
}
