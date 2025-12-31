# -*- coding: utf-8 -*-
from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton, 
                               QStackedWidget, QFrame, QGridLayout, QLineEdit, QComboBox, 
                               QTextEdit, QMessageBox, QDialog, QScrollArea, QSizePolicy,
                               QMainWindow, QSpinBox)
from PySide6.QtCore import Qt, QThread, Signal, Slot, QTimer, QElapsedTimer, QRect
from PySide6.QtGui import QFont, QColor, QPalette, QPainter, QBrush, QTextCursor, QTextCharFormat, QCursor

import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np
import random
import time

# ==================== SORT MANAGER ====================

class SortManager:
    def __init__(self):
        self.data = []

    def generate_data(self, size, data_type):
        if data_type == "int":
            self.data = [random.randint(1_000_000, 10_000_000_000) for _ in range(size)]
        elif data_type == "float":
            self.data = [random.uniform(1.0, 1000.0) for _ in range(size)]
        elif data_type == "char":
            self.data = [chr(random.randint(65, 90)) for _ in range(size)]
        else:
            self.data = [str(random.randint(1, 1000)) for _ in range(size)]
        return self.data

    def get_data(self):
        return self.data

    def run_sort(self, algo_name, data):
        # Create a copy to sort
        arr = list(data)
        start_time = time.perf_counter()
        
        if algo_name == "Bubble Sort":
            self.bubble_sort(arr)
        elif algo_name == "Insertion Sort":
            self.insertion_sort(arr)
        elif algo_name == "Shell Sort":
            self.shell_sort(arr)
        elif algo_name == "Quick Sort":
            self.quick_sort(arr, 0, len(arr) - 1)
        else:
            arr.sort()
            
        end_time = time.perf_counter()
        return (end_time - start_time), arr

    def bubble_sort(self, arr):
        n = len(arr)
        for i in range(n):
            for j in range(0, n - i - 1):
                if arr[j] > arr[j + 1]:
                    arr[j], arr[j + 1] = arr[j + 1], arr[j]

    def insertion_sort(self, arr):
        for i in range(1, len(arr)):
            key = arr[i]
            j = i - 1
            while j >= 0 and key < arr[j]:
                arr[j + 1] = arr[j]
                j -= 1
            arr[j + 1] = key

    def shell_sort(self, arr):
        n = len(arr)
        gap = n // 2
        while gap > 0:
            for i in range(gap, n):
                temp = arr[i]
                j = i
                while j >= gap and arr[j - gap] > temp:
                    arr[j] = arr[j - gap]
                    j -= gap
                arr[j] = temp
            gap //= 2

    def quick_sort(self, arr, low, high):
        if low < high:
            pi = self.partition(arr, low, high)
            self.quick_sort(arr, low, pi - 1)
            self.quick_sort(arr, pi + 1, high)

    def partition(self, arr, low, high):
        pivot = arr[high]
        i = low - 1
        for j in range(low, high):
            if arr[j] <= pivot:
                i += 1
                arr[i], arr[j] = arr[j], arr[i]
        arr[i + 1], arr[high] = arr[high], arr[i + 1]
        return i + 1

def fmt_time(seconds):
    """Formate le temps comme dans main.cpp"""
    if seconds < 0: return ""
    if seconds >= 3600.0:
        hours = seconds / 3600.0
        return f"{hours:.2f} h"
    elif seconds >= 60.0:
        minutes = int(seconds / 60.0)
        rem_secs = seconds - (minutes * 60.0)
        return f"{minutes} min {rem_secs:.1f} s"
    elif seconds >= 1.0:
        return f"{seconds:.3f} s"
    elif seconds >= 0.001:
        ms = seconds * 1000.0
        return f"{ms:.2f} ms"
    elif seconds >= 0.000001:
        us = seconds * 1000000.0
        return f"{us:.2f} µs"
    else:
        ns = seconds * 1000000000.0
        return f"{ns:.2f} ns"

# ==================== ALGO CODES & GENERATORS ====================

BUBBLE_SORT_C = """void bubbleSort(int arr[], int n) {
    int i, j;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                swap(&arr[j], &arr[j+1]);
            }
        }
    }
}"""
BUBBLE_SORT_PY = """def bubble_sort(arr):
    n = len(arr)
    for i in range(n):
        for j in range(0, n-i-1):
            if arr[j] > arr[j+1]:
                arr[j], arr[j+1] = arr[j+1], arr[j]
"""
INSERTION_SORT_C = """void insertionSort(int arr[], int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}"""
INSERTION_SORT_PY = """def insertion_sort(arr):
    for i in range(1, len(arr)):
        key = arr[i]
        j = i - 1
        while j >= 0 and key < arr[j]:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = key
"""
SHELL_SORT_C = """void shellSort(int arr[], int n) {
    for (int gap = n/2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i += 1) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
                arr[j] = arr[j - gap];
            arr[j] = temp;
        }
    }
}"""
SHELL_SORT_PY = """def shell_sort(arr):
    n = len(arr)
    gap = n // 2
    while gap > 0:
        for i in range(gap, n):
            temp = arr[i]
            j = i
            while j >= gap and arr[j - gap] > temp:
                arr[j] = arr[j - gap]
                j -= gap
            arr[j] = temp
        gap //= 2
"""
QUICK_SORT_C = """void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}"""
QUICK_SORT_PY = """def quick_sort(arr, low, high):
    if low < high:
        pi = partition(arr, low, high)
        quick_sort(arr, low, pi - 1)
        quick_sort(arr, pi + 1, high)
"""

CODE_MAP = {
    "Bubble Sort": {"c": BUBBLE_SORT_C, "py": BUBBLE_SORT_PY},
    "Insertion Sort": {"c": INSERTION_SORT_C, "py": INSERTION_SORT_PY},
    "Shell Sort": {"c": SHELL_SORT_C, "py": SHELL_SORT_PY},
    "Quick Sort": {"c": QUICK_SORT_C, "py": QUICK_SORT_PY}
}

COMPLEXITY_MAP = {
    "Bubble Sort": "O(n²)",
    "Insertion Sort": "O(n²)",
    "Shell Sort": "O(n log n)",
    "Quick Sort": "O(n log n)"
}

ALGO_DESC = {
    "Bubble Sort": "Repeatedly steps through the list, compares adjacent elements and swaps them if they are in the wrong order.",
    "Insertion Sort": "Builds the final sorted array one item at a time. It's efficient for small or nearly-sorted datasets.",
    "Shell Sort": "An optimization of insertion sort that allows the exchange of items that are far apart.",
    "Quick Sort": "A divide-and-conquer algorithm that works by selecting a 'pivot' and partitioning the other elements into two sub-arrays."
}

def bubble_sort_gen(arr):
    n = len(arr)
    yield arr, {}, 0, 0
    yield arr, {}, 1, 1
    for i in range(n):
        yield arr, {}, 2, 2
        for j in range(0, n-i-1):
            yield arr, {j: 2, j+1: 2}, 3, 3 # Compare (Blue)
            if arr[j] > arr[j+1]:
                yield arr, {j: 1, j+1: 1}, 4, 4 # Swap (Red)
                arr[j], arr[j+1] = arr[j+1], arr[j]
                yield arr, {j: 1, j+1: 1}, 5, 5

def insertion_sort_gen(arr):
    n = len(arr)
    yield arr, {}, 0, 0
    for i in range(1, n):
        yield arr, {i: 1}, 1, 2 # Key (Red)
        key = arr[i]
        yield arr, {i: 1}, 2, 3
        j = i - 1
        yield arr, {i: 1, j: 2}, 3, 4 # Key Red, Compare Blue
        while j >= 0 and key < arr[j]:
            yield arr, {j: 2, j+1: 1}, 4, 5
            arr[j + 1] = arr[j]
            yield arr, {j: 2, j+1: 1}, 5, 6
            j -= 1
            yield arr, {j: 2, j+1: 1} if j>=0 else {j+1: 1}, 6, 7
        arr[j + 1] = key
        yield arr, {j+1: 1}, 7, 8

def shell_sort_gen(arr):
    n = len(arr)
    yield arr, {}, 0, 0
    gap = n // 2
    yield arr, {}, 1, 2
    while gap > 0:
        yield arr, {}, 1, 3
        for i in range(gap, n):
            yield arr, {i: 1}, 2, 4
            temp = arr[i]
            yield arr, {i: 1}, 2, 5
            j = i
            yield arr, {i: 1, j: 2}, 3, 6
            while j >= gap and arr[j - gap] > temp:
                yield arr, {j: 1, j-gap: 2}, 4, 7
                arr[j] = arr[j - gap]
                yield arr, {j: 1, j-gap: 2}, 5, 8
                j -= gap
                yield arr, {j: 1}, 4, 9
            arr[j] = temp
            yield arr, {j: 1}, 6, 10
        gap //= 2
        yield arr, {}, 1, 11

def quick_sort_gen(arr):
    yield arr, {}, 0, 0
    size = len(arr)
    stack = [0] * size
    top = -1
    top += 1
    stack[top] = 0
    top += 1
    stack[top] = size - 1
    while top >= 0:
        high = stack[top]
        top -= 1
        low = stack[top]
        top -= 1
        yield arr, {low: 2, high: 2}, 1, 1
        i = (low - 1)
        pivot = arr[high]
        for j in range(low, high):
             if arr[j] <= pivot:
                 i += 1
                 arr[i], arr[j] = arr[j], arr[i]
                 yield arr, {i: 1, j: 2, high: 1}, 2, 2 
        arr[i + 1], arr[high] = arr[high], arr[i + 1]
        yield arr, {i+1: 1, high: 2}, 2, 2 
        pi = i + 1
        if pi - 1 > low:
            top += 1
            stack[top] = low
            top += 1
            stack[top] = pi - 1
            yield arr, {}, 3, 3 
        if pi + 1 < high:
            top += 1
            stack[top] = pi + 1
            top += 1
            stack[top] = high
            yield arr, {}, 4, 4

GEN_MAP = {
    "Bubble Sort": bubble_sort_gen,
    "Insertion Sort": insertion_sort_gen,
    "Shell Sort": shell_sort_gen,
    "Quick Sort": quick_sort_gen
}

# ==================== LAUNCHER WIDGET ====================

class TableauLauncher(QWidget):
    mode_selected = Signal(str)

    def __init__(self):
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setAlignment(Qt.AlignCenter)
        layout.setSpacing(40)
        
        title = QLabel("MODULE TABLEAUX")
        title.setStyleSheet("font-size: 28px; font-weight: bold; color: white;")
        title.setAlignment(Qt.AlignCenter)
        layout.addWidget(title)
        
        cards_layout = QHBoxLayout()
        cards_layout.setSpacing(40)
        cards_layout.setAlignment(Qt.AlignCenter)
        
        # Card 1: Visualization
        self.viz_card = self.create_card("Visualisation Algorithmique", 
                                       "Voir le tri étape par étape avec le code source.", 
                                       "#58A6FF")
        self.viz_card.mousePressEvent = lambda e: self.mode_selected.emit("viz")
        cards_layout.addWidget(self.viz_card)
        
        # Card 2: Performance
        self.perf_card = self.create_card("Performance Grandes Tailles", 
                                        "Benchmark et graphiques pour grands volumes de données.", 
                                        "#FF6B35")
        self.perf_card.mousePressEvent = lambda e: self.mode_selected.emit("perf")
        cards_layout.addWidget(self.perf_card)
        
        layout.addLayout(cards_layout)

    def create_card(self, title, desc, color):
        frame = QFrame()
        frame.setFixedSize(300, 300)
        frame.setCursor(QCursor(Qt.PointingHandCursor))
        frame.setStyleSheet(f"""
            QFrame {{
                background-color: #161B22;
                border: 2px solid #30363D;
                border-radius: 15px;
            }}
            QFrame:hover {{
                border: 2px solid {color};
                background-color: #1C2128;
            }}
        """)
        l = QVBoxLayout(frame)
        l.setAlignment(Qt.AlignCenter)
        
        lbl_t = QLabel(title)
        lbl_t.setStyleSheet(f"font-size: 22px; font-weight: bold; color: {color}; background: transparent; border: none;")
        lbl_t.setWordWrap(True)
        lbl_t.setAlignment(Qt.AlignCenter)
        l.addWidget(lbl_t)
        
        lbl_d = QLabel(desc)
        lbl_d.setStyleSheet("font-size: 14px; color: #8B949E; background: transparent; border: none;")
        lbl_d.setWordWrap(True)
        lbl_d.setAlignment(Qt.AlignCenter)
        l.addWidget(lbl_d)
        
        return frame

# ==================== VIZ MODULE WIDGET ====================

class NeonBarWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.data = []
        self.color_map = {} # {index: color_code} 1=Red, 2=Blue
        self.default_color = QColor("#aaaaaa")
        self.red_color = QColor("#FF0055") # Key
        self.blue_color = QColor("#5555ff") # Compare
        
    def set_data(self, data, color_map={}):
        self.data = data
        self.color_map = color_map
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        w = self.width()
        h = self.height()
        painter.fillRect(0, 0, w, h, QColor("#0D1117"))
        
        if not self.data: return
        
        n = len(self.data)
        bar_w = w / n
        max_val = max(self.data) if self.data else 1
        
        # Margins
        top_margin = 30
        bottom_margin = 10
        available_h = h - top_margin - bottom_margin
        
        # Font for numbers
        font_size = max(8, min(12, int(bar_w / 2.5)))
        font = QFont("Segoe UI", font_size, QFont.Bold)
        painter.setFont(font)
        
        for i, val in enumerate(self.data):
            bar_h = (val / max_val) * available_h
            x = i * bar_w
            y = h - bar_h - bottom_margin
            
            color = self.default_color
            if i in self.color_map:
                code = self.color_map[i]
                if code == 1: color = self.red_color
                elif code == 2: color = self.blue_color
                
            painter.setBrush(QBrush(color))
            painter.setPen(Qt.NoPen)
            # Draw bar
            painter.drawRect(QRect(int(x+1), int(y), int(bar_w-2), int(bar_h)))
            
            # Draw value above bar
            if bar_w > 15:
                painter.setPen(QColor("white"))
                # Position text slightly above the bar
                text_rect = QRect(int(x), int(y - 20), int(bar_w), 20)
                painter.drawText(text_rect, Qt.AlignCenter, str(val))

class VizWidget(QWidget):
    back_signal = Signal()

    def __init__(self):
        super().__init__()
        layout = QHBoxLayout(self)
        
        # Left: Controls + Viz
        left_layout = QVBoxLayout()
        
        # Header
        header = QHBoxLayout()
        btn_back = QPushButton("← Retour")
        btn_back.clicked.connect(self.back_signal.emit)
        btn_back.setStyleSheet("background-color: #30363D; color: white; border: none; padding: 5px 10px;")
        header.addWidget(btn_back)
        header.addStretch()
        left_layout.addLayout(header)
        
        # Title
        self.lbl_title = QLabel("Insertion Sort")
        self.lbl_title.setStyleSheet("font-size: 24px; font-weight: bold; color: white; margin-top: 10px;")
        left_layout.addWidget(self.lbl_title)
        
        self.viz_display = NeonBarWidget()
        left_layout.addWidget(self.viz_display)
        
        # Stats Label
        self.lbl_stats = QLabel("Complexité: - | Temps: 0s")
        self.lbl_stats.setStyleSheet("font-size: 14px; font-weight: bold; color: #58A6FF;")
        left_layout.addWidget(self.lbl_stats)
        
        # Controls
        ctrl = QHBoxLayout()
        ctrl.addWidget(QLabel("Taille:"))
        self.spin_size = QSpinBox()
        self.spin_size.setRange(2, 50)
        self.spin_size.setValue(15)
        self.spin_size.setStyleSheet("background-color: #161B22; color: white;")
        ctrl.addWidget(self.spin_size)
        
        lbl_max = QLabel("(Max: 50)")
        lbl_max.setStyleSheet("color: #8B949E; font-size: 10px;")
        ctrl.addWidget(lbl_max)
        
        self.btn_input = QPushButton("Saisir Données")
        self.btn_input.clicked.connect(self.open_input)
        self.btn_input.setStyleSheet("background-color: #238636; color: white;")
        ctrl.addWidget(self.btn_input)
        
        self.combo_algo = QComboBox()
        self.combo_algo.addItems(["Bubble Sort", "Insertion Sort", "Shell Sort", "Quick Sort"])
        self.combo_algo.setStyleSheet("background-color: #161B22; color: white;")
        self.combo_algo.currentIndexChanged.connect(self.update_code)
        ctrl.addWidget(self.combo_algo)
        
        self.btn_start = QPushButton("Démarrer")
        self.btn_start.clicked.connect(self.start_viz)
        self.btn_start.setStyleSheet("background-color: #1f6feb; color: white;")
        self.btn_start.setEnabled(False)
        ctrl.addWidget(self.btn_start)
        left_layout.addLayout(ctrl)
        
        layout.addLayout(left_layout, 2)
        
        # Right: Code
        right_layout = QVBoxLayout()
        
        # Language Toggles
        lang_layout = QHBoxLayout()
        self.btn_py = QPushButton("Python")
        self.btn_py.setCheckable(True)
        self.btn_py.setChecked(True)
        self.btn_py.clicked.connect(lambda: self.set_lang("py"))
        self.btn_py.setStyleSheet("""
            QPushButton { background-color: #30363D; color: white; border: none; padding: 5px; }
            QPushButton:checked { background-color: #238636; }
        """)
        
        self.btn_c = QPushButton("C")
        self.btn_c.setCheckable(True)
        self.btn_c.clicked.connect(lambda: self.set_lang("c"))
        self.btn_c.setStyleSheet("""
            QPushButton { background-color: #30363D; color: white; border: none; padding: 5px; }
            QPushButton:checked { background-color: #238636; }
        """)
        
        lang_layout.addWidget(self.btn_py)
        lang_layout.addWidget(self.btn_c)
        right_layout.addLayout(lang_layout)
        
        self.code_view = QTextEdit()
        self.code_view.setReadOnly(True)
        self.code_view.setStyleSheet("background-color: #161B22; color: #C9D1D9; font-family: Consolas; font-size: 12px;")
        right_layout.addWidget(self.code_view)
        
        layout.addLayout(right_layout, 1)
        
        self.current_lang = "py"
        self.timer = QTimer()
        self.timer.timeout.connect(self.step)
        self.update_code()

    def set_lang(self, lang):
        self.current_lang = lang
        self.btn_py.setChecked(lang == "py")
        self.btn_c.setChecked(lang == "c")
        self.update_code()

    def open_input(self):
        size = self.spin_size.value()
        d = ManualInputWindow(size, "int", self)
        if d.exec():
            self.data = d.result_data
            self.viz_display.set_data(self.data)
            self.btn_start.setEnabled(True)

    def update_code(self):
        algo = self.combo_algo.currentText()
        self.code_view.setText(CODE_MAP[algo][self.current_lang])
        self.lbl_title.setText(algo)
        self.lbl_stats.setText(f"Complexité: {COMPLEXITY_MAP[algo]} | Temps: 0s")

    def start_viz(self):
        if not hasattr(self, 'data'): return
        algo = self.combo_algo.currentText()
        self.gen = GEN_MAP[algo](list(self.data))
        self.start_time = time.time()
        self.timer.start(100)

    def step(self):
        try:
            arr, color_map, line_c, line_py = next(self.gen)
            self.viz_display.set_data(arr, color_map)
            
            line = line_py if self.current_lang == "py" else line_c
            self.highlight_line(line)
            
            elapsed = time.time() - self.start_time
            algo = self.combo_algo.currentText()
            self.lbl_stats.setText(f"Complexité: {COMPLEXITY_MAP[algo]} | Temps: {elapsed:.2f}s")
            
        except StopIteration:
            self.timer.stop()

    def highlight_line(self, line):
        cursor = self.code_view.textCursor()
        cursor.movePosition(QTextCursor.Start)
        for _ in range(line):
            cursor.movePosition(QTextCursor.Down)
        cursor.select(QTextCursor.LineUnderCursor)
        fmt = QTextCharFormat()
        fmt.setBackground(QColor("#238636"))
        fmt.setForeground(QColor("white"))
        
        self.update_code() # Reset format
        
        cursor = self.code_view.textCursor()
        cursor.movePosition(QTextCursor.Start)
        for _ in range(line):
            cursor.movePosition(QTextCursor.Down)
        cursor.select(QTextCursor.LineUnderCursor)
        cursor.setCharFormat(fmt)

# ==================== PERFORMANCE MODULE (ORIGINAL RESTORED) ====================

class SortWorker(QThread):
    """Thread pour exécuter un algorithme de tri sans geler l'UI"""
    finished_signal = Signal(str, float, list)  # Algo Name, Duration, Sorted Data

    def __init__(self, manager, algo_name, data):
        super().__init__()
        self.manager = manager
        self.algo_name = algo_name
        self.data = list(data)  # Copie des données

    def run(self):
        duration, sorted_data = self.manager.run_sort(self.algo_name, self.data)
        self.finished_signal.emit(self.algo_name, duration, sorted_data)


class ManualInputWindow(QDialog):
    """Fenêtre de saisie manuelle - Version avec génération progressive"""
    def __init__(self, target_size, data_type, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Saisie Manuelle")
        self.resize(500, 600)
        self.target_size = target_size
        self.data_type = data_type
        self.current_index = 0
        self.result_data = []
        self.input_fields = []
        self.field_frames = []
        
        self.setStyleSheet("""
            QDialog { background-color: #0D1117; }
            QLabel { color: #C9D1D9; }
            QScrollArea { background-color: #0D1117; border: none; }
        """)
        
        self.init_ui()
        
    def init_ui(self):
        layout = QVBoxLayout(self)
        layout.setSpacing(15)
        layout.setContentsMargins(20, 20, 20, 20)
        
        # Header avec compteur
        self.lbl_info = QLabel(f"Saisie : 0 / {self.target_size} éléments")
        self.lbl_info.setStyleSheet("font-size: 20px; font-weight: bold; color: #FF6B35;")
        self.lbl_info.setAlignment(Qt.AlignCenter)
        layout.addWidget(self.lbl_info)
        
        # Type de donnée
        type_label = QLabel(f"Type: {self.data_type}")
        type_label.setStyleSheet("font-size: 14px; color: #8B949E;")
        type_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(type_label)
        
        # Zone scrollable
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setStyleSheet("""
            QScrollArea {
                background-color: #0D1117;
                border: 1px solid #30363D;
                border-radius: 8px;
            }
        """)
        
        self.scroll_content = QWidget()
        self.scroll_layout = QVBoxLayout(self.scroll_content)
        self.scroll_layout.setSpacing(10)
        self.scroll_layout.setContentsMargins(10, 10, 10, 10)
        
        self.scroll_area.setWidget(self.scroll_content)
        layout.addWidget(self.scroll_area)
        
        # Bouton de validation (caché au début)
        self.btn_validate = QPushButton("✓ Générer le Tableau")
        self.btn_validate.setStyleSheet("""
            QPushButton {
                background-color: #238636;
                color: white;
                border: none;
                padding: 12px;
                border-radius: 8px;
                font-weight: bold;
                font-size: 16px;
            }
            QPushButton:hover { background-color: #2EA043; }
            QPushButton:disabled {
                background-color: #30363D;
                color: #8B949E;
            }
        """)
        self.btn_validate.clicked.connect(self.validate_and_finish)
        self.btn_validate.setEnabled(False)
        layout.addWidget(self.btn_validate)
        
        # Hint
        lbl_hint = QLabel("⏎ ENTRÉE pour ajouter une nouvelle case")
        lbl_hint.setStyleSheet("color: #8B949E; font-style: italic; font-size: 12px;")
        lbl_hint.setAlignment(Qt.AlignCenter)
        layout.addWidget(lbl_hint)
        
        # Créer la PREMIÈRE case seulement
        self.add_input_field()
    
    def add_input_field(self):
        """Ajoute un nouveau champ de saisie"""
        if len(self.input_fields) >= self.target_size:
            return
        
        index = len(self.input_fields)
        
        # Conteneur pour un champ
        field_frame = QFrame()
        field_frame.setStyleSheet("""
            QFrame {
                background-color: #161B22;
                border: 1px solid #30363D;
                border-radius: 6px;
                padding: 8px;
            }
        """)
        field_layout = QHBoxLayout(field_frame)
        field_layout.setContentsMargins(8, 8, 8, 8)
        
        # Label numéro
        lbl_num = QLabel(f"#{index + 1}")
        lbl_num.setStyleSheet("font-weight: bold; color: #58A6FF; font-size: 14px;")
        lbl_num.setFixedWidth(40)
        field_layout.addWidget(lbl_num)
        
        # Input field
        inp = QLineEdit()
        inp.setPlaceholderText(f"Valeur {index + 1} ({self.data_type})")
        inp.setStyleSheet("""
            QLineEdit { 
                font-size: 16px; 
                padding: 10px; 
                border: 2px solid #58A6FF; 
                border-radius: 6px;
                background-color: #0D1117;
                color: #C9D1D9;
            }
            QLineEdit:focus {
                border: 2px solid #FF6B35;
            }
        """)
        
        # Connecter le signal returnPressed
        inp.returnPressed.connect(lambda: self.on_enter_pressed(inp))
        field_layout.addWidget(inp)
        
        # Ajouter au layout principal
        self.scroll_layout.addWidget(field_frame)
        self.input_fields.append(inp)
        self.field_frames.append(field_frame)
        
        # FOCUS AUTOMATIQUE sur la nouvelle case
        inp.setFocus()
        
        # Update compteur
        self.lbl_info.setText(f"Saisie : {len(self.input_fields)} / {self.target_size} éléments")
        
        # SCROLL AUTOMATIQUE vers la nouvelle case
        QTimer.singleShot(10, lambda: self.ensure_field_visible(field_frame))
        
        # Activer le bouton quand on atteint la taille
        if len(self.input_fields) == self.target_size:
            self.btn_validate.setEnabled(True)
            self.btn_validate.setStyleSheet("""
                QPushButton {
                    background-color: #238636;
                    color: white;
                    border: none;
                    padding: 12px;
                    border-radius: 8px;
                    font-weight: bold;
                    font-size: 16px;
                }
                QPushButton:hover { background-color: #2EA043; }
            """)
    
    def ensure_field_visible(self, field_frame):
        """Assure qu'un champ est visible dans la zone de défilement"""
        # Calculer la position relative du champ
        scrollbar = self.scroll_area.verticalScrollBar()
        
        # Obtenir la position du champ dans le conteneur
        pos = field_frame.mapTo(self.scroll_content, field_frame.rect().topLeft())
        
        # Ajuster la position de défilement
        scrollbar.setValue(pos.y() - 50)  # -50 pour un peu d'espace au-dessus
    
    def on_enter_pressed(self, inp):
        """Appelé quand l'utilisateur appuie sur ENTRÉE"""
        val_text = inp.text().strip()
        
        # Vérifier si la valeur est valide
        if val_text:
            try:
                if self.data_type == "int":
                    val = int(val_text)
                elif self.data_type == "float":
                    val = float(val_text)
                elif self.data_type == "char":
                    val = val_text[0]
                else:
                    val = val_text
                
                # Ajouter à la liste
                self.result_data.append(val)
                
                # Changer le style pour indiquer que c'est valide
                inp.setStyleSheet("""
                    QLineEdit { 
                        font-size: 16px; 
                        padding: 10px; 
                        border: 2px solid #3FB950; 
                        border-radius: 6px;
                        background-color: #0D1117;
                        color: #C9D1D9;
                    }
                """)
                
                # Désactiver la case précédente
                inp.setReadOnly(True)
                
                # Ajouter une nouvelle case si pas encore atteint la taille max
                if len(self.input_fields) < self.target_size:
                    QTimer.singleShot(10, self.add_input_field)
                else:
                    # Si on a atteint la taille, focus sur bouton
                    self.btn_validate.setFocus()
                    
            except ValueError:
                inp.setStyleSheet("""
                    QLineEdit { 
                        font-size: 16px; 
                        padding: 10px; 
                        border: 2px solid #F44336; 
                        border-radius: 6px;
                        background-color: #0D1117;
                        color: #C9D1D9;
                    }
                """)
                # Message d'erreur
                QMessageBox.warning(self, "Erreur", 
                                  f"Valeur invalide pour le type {self.data_type}!")
                return
        else:
            # Champ vide, on ne fait rien
            inp.setStyleSheet("""
                QLineEdit { 
                    font-size: 16px; 
                    padding: 10px; 
                    border: 2px solid #F44336; 
                    border-radius: 6px;
                    background-color: #0D1117;
                    color: #C9D1D9;
                }
            """)
            QMessageBox.warning(self, "Erreur", "Veuillez saisir une valeur!")
            return
    
    def validate_and_finish(self):
        """Valide toutes les valeurs et ferme la fenêtre"""
        # Vérifier qu'on a toutes les valeurs
        if len(self.result_data) == self.target_size:
            self.accept()
        else:
            # Trouver la première case vide
            for i, inp in enumerate(self.input_fields):
                if not inp.text().strip():
                    inp.setFocus()
                    QMessageBox.warning(self, "Erreur", 
                                      f"Case #{i+1} est vide!")
                    break


class GraphWindow(QMainWindow):
    """Fenêtre pour afficher le graphique de performance"""
    def __init__(self, history, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Analyse de Performance")
        self.resize(1100, 750)
        
        # Style sombre
        self.setStyleSheet("""
            QMainWindow {
                background-color: #0D1117;
            }
        """)
        
        # Conteneur principal
        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)
        layout.setContentsMargins(10, 10, 10, 10)
        
        # Canvas matplotlib
        self.figure = Figure(figsize=(10, 7), dpi=100, facecolor='#0D1117')
        self.canvas = FigureCanvas(self.figure)
        layout.addWidget(self.canvas)
        
        self.plot_curves(history)

    def plot_curves(self, history):
        """Trace les courbes de performance avec courbes exponentielles et effet glow"""
        import math
        
        # Nettoyer la figure
        self.figure.clear()
        
        # Créer le subplot
        ax = self.figure.add_subplot(111)
        ax.set_facecolor('#0D1117')
        
        # Définir les couleurs (comme en C)
        algo_colors = {
            "Bubble Sort": '#FF6B35',     # Orange (COLOR_NEON_ORANGE)
            "Insertion Sort": '#58A6FF',  # Blue (COLOR_NEON_BLUE)
            "Shell Sort": '#FFC107',      # Yellow
            "Quick Sort": '#3FB950'       # Green (COLOR_NEON_GREEN)
        }
        
        algos = list(algo_colors.keys())
        
        # Trouver le temps max
        max_time = 0.001
        for entry in history:
            for algo in algos:
                if algo in entry and entry[algo] > max_time:
                    max_time = entry[algo]
        max_time *= 1.15  # 15% de marge
        
        if not history:
            ax.text(0.5, 0.5, 'Lancez un tri pour voir les courbes',
                   transform=ax.transAxes, ha='center', va='center',
                   color='#8B949E', fontsize=14)
            self.canvas.draw()
            return
        
        # Paramètres du graphique (comme en C)
        graph_height = 1.0  # Hauteur normalisée
        level_spacing = graph_height / (len(algos) + 1)
        
        # Dernier résultat de l'historique pour afficher les courbes
        last_entry = history[-1]
        
        # Dessiner chaque courbe exponentielle depuis l'origine
        for i, algo in enumerate(algos):
            color = algo_colors[algo]
            target_y = (i + 1) * level_spacing  # Niveau vertical pour cet algo
            
            if algo not in last_entry:
                continue
            
            current_time = last_entry[algo]
            if current_time <= 0:
                continue
            
            # Calculer le point final
            end_x = current_time / max_time  # Normalisé [0, 1]
            end_y = target_y
            
            # Générer la courbe exponentielle
            segments = 50
            k = 2.5  # Paramètre de courbure
            exp_k = math.exp(k)
            
            x_points = [0]
            y_points = [0]
            
            for s in range(1, segments + 1):
                t = s / segments
                # Progression exponentielle: monte rapidement puis ralentit
                exp_progress = (math.exp(k * t) - 1.0) / (exp_k - 1.0)
                
                x = t * end_x
                y = exp_progress * end_y
                
                x_points.append(x)
                y_points.append(y)
            
            # Convertir en temps réel pour l'axe X
            x_time = [x * max_time * 1000 for x in x_points]  # En millisecondes
            
            # Effet GLOW - plusieurs lignes avec alpha décroissant
            # Glow externe (large, très transparent)
            ax.plot(x_time, y_points, 
                   color=color, linewidth=8, alpha=0.15)
            # Glow moyen
            ax.plot(x_time, y_points, 
                   color=color, linewidth=5, alpha=0.3)
            # Ligne principale
            ax.plot(x_time, y_points, 
                   color=color, linewidth=2.5, alpha=1.0,
                   label=f"{algo}: {current_time*1000:.1f} ms")
            
            # Point final avec glow
            final_x = x_time[-1]
            final_y = y_points[-1]
            
            # Glow du point
            ax.scatter([final_x], [final_y], s=200, color=color, alpha=0.3, zorder=5)
            ax.scatter([final_x], [final_y], s=100, color=color, alpha=0.5, zorder=6)
            # Point central blanc
            ax.scatter([final_x], [final_y], s=40, color='white', zorder=7)
        
        # Point d'origine avec glow
        ax.scatter([0], [0], s=60, color='white', zorder=10)
        
        # Axes et labels
        ax.set_xlabel('Temps (ms)', color='#58A6FF', fontsize=12, fontweight='bold')
        ax.set_ylabel('Progression', color='#58A6FF', fontsize=12, fontweight='bold')
        ax.set_title('Courbes de Tri', color='#C9D1D9', fontsize=16, fontweight='bold', pad=15)
        
        # Limites
        ax.set_xlim(0, max_time * 1000 * 1.05)
        ax.set_ylim(0, graph_height * 1.1)
        
        # Grille subtile
        ax.grid(True, linestyle='--', alpha=0.2, color='#30363D')
        
        # Masquer les ticks Y (c'est juste une progression visuelle)
        ax.set_yticks([])
        
        # Style des ticks X
        ax.tick_params(axis='x', colors='#C9D1D9', labelsize=10)
        
        # Légende avec fond sombre
        legend = ax.legend(loc='upper right', fontsize=11, framealpha=0.95)
        legend.get_frame().set_facecolor('#161B22')
        legend.get_frame().set_edgecolor('#30363D')
        for text in legend.get_texts():
            text.set_color('#C9D1D9')
        
        # Cadre
        for spine in ax.spines.values():
            spine.set_color('#30363D')
            spine.set_linewidth(1)
        
        # Ajuster la mise en page
        self.figure.tight_layout()
        
        # Dessiner
        self.canvas.draw()


class PerformanceWidget(QWidget):
    """Encapsulation de l'ancienne TableauWindow dans un widget pour le Stack"""
    back_signal = Signal()

    def __init__(self):
        super().__init__()
        # On n'appelle pas de setWindowTitle car c'est un widget maintenant
        
        # Style repris de TableauWindow
        self.setStyleSheet("""
            QWidget { background-color: #0D1117; color: #C9D1D9; }
            QLabel { color: #C9D1D9; font-family: 'Segoe UI'; }
            QLineEdit, QComboBox { 
                background-color: #161B22; 
                color: #C9D1D9; 
                border: 1px solid #30363D; 
                padding: 5px; 
                border-radius: 4px;
            }
            QTextEdit { 
                background-color: #0D1117; 
                color: #C9D1D9; 
                border: 1px solid #30363D; 
                font-family: 'Consolas';
            }
            QPushButton {
                background-color: #238636;
                color: white;
                border: none;
                padding: 8px 16px;
                border-radius: 6px;
                font-weight: bold;
            }
            QPushButton:hover { background-color: #2EA043; }
            QPushButton:disabled { background-color: #30363D; color: #8B949E; }
        """)

        self.manager = SortManager()
        self.workers = []
        self.history = []
        self.current_run_results = {}
        self.completed_count = 0
        self.target_display_algo = None
        
        # Timer smooth
        self.algo_elapsed_timers = {}
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_all_timers)
        self.timer.setInterval(50)
        
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout(self)
        layout.setSpacing(15)
        layout.setContentsMargins(20, 20, 20, 20)

        # === HEADER ===
        header_layout = QHBoxLayout()
        
        # Bouton Retour ajouté pour le module
        self.btn_back = QPushButton("← Retour")
        self.btn_back.clicked.connect(self.back_signal.emit)
        self.btn_back.setStyleSheet("background-color: #30363D; padding: 8px;")
        header_layout.addWidget(self.btn_back)
        
        title = QLabel("ALGORITHMES DE TRI PARALLÈLES")
        title.setStyleSheet("font-size: 24px; font-weight: bold; color: #FF6B35;")
        header_layout.addWidget(title)
        
        header_layout.addStretch()
        
        layout.addLayout(header_layout)

        # === CONTROLS ===
        controls_frame = QFrame()
        controls_frame.setStyleSheet("background-color: #161B22; border-radius: 8px; border: 1px solid #30363D;")
        controls_layout = QGridLayout(controls_frame)
        
        # Ligne 1: Config
        controls_layout.addWidget(QLabel("Taille:"), 0, 0)
        self.size_input = QLineEdit("1000")
        self.size_input.setFixedWidth(100)
        controls_layout.addWidget(self.size_input, 0, 1)
        
        controls_layout.addWidget(QLabel("Type:"), 0, 2)
        self.type_combo = QComboBox()
        self.type_combo.addItems(["Entiers", "Réels", "Caractères", "Chaînes"])
        controls_layout.addWidget(self.type_combo, 0, 3)
        
        # Boutons Mode
        self.btn_random = QPushButton("Aléatoire")
        self.btn_random.clicked.connect(self.generate_random)
        self.btn_random.setStyleSheet("background-color: #FF6B35;")
        controls_layout.addWidget(self.btn_random, 0, 4)
        
        self.btn_manual = QPushButton("Manuel")
        self.btn_manual.clicked.connect(self.open_manual_input)
        self.btn_manual.setStyleSheet("background-color: #FF6B35;")
        controls_layout.addWidget(self.btn_manual, 0, 5)
        
        controls_layout.setColumnStretch(6, 1)

        # Ligne 2: Algorithmes
        lbl_algo = QLabel("Algorithmes:")
        lbl_algo.setStyleSheet("font-weight: bold; margin-top: 10px;")
        controls_layout.addWidget(lbl_algo, 1, 0)
        
        btn_layout = QHBoxLayout()
        self.sort_buttons = {}
        algos = ["Bubble Sort", "Insertion Sort", "Shell Sort", "Quick Sort"]
        colors = ['#FF5252', '#448AFF', '#FFC107', '#69F0AE']
        
        for i, algo in enumerate(algos):
            btn = QPushButton(algo)
            btn.setStyleSheet(f"background-color: {colors[i]}; color: black;")
            btn.clicked.connect(lambda checked, a=algo: self.start_sorts(a))
            btn.setEnabled(False)
            btn_layout.addWidget(btn)
            self.sort_buttons[algo] = btn
            
        controls_layout.addLayout(btn_layout, 1, 1, 1, 6)
        
        # Ligne 3: Actions Globales
        action_layout = QHBoxLayout()
        self.btn_stop = QPushButton("Arrêter le Tri")
        self.btn_stop.setStyleSheet("background-color: #F44336;")
        self.btn_stop.setEnabled(False)
        self.btn_stop.clicked.connect(self.stop_sorts)
        action_layout.addWidget(self.btn_stop)
        
        self.btn_graph = QPushButton("📊 Graphique")
        self.btn_graph.setStyleSheet("background-color: #A371F7;")
        self.btn_graph.setEnabled(False)
        self.btn_graph.clicked.connect(self.show_graph)
        action_layout.addWidget(self.btn_graph)
        
        controls_layout.addLayout(action_layout, 2, 0, 1, 7)
        
        layout.addWidget(controls_frame)

        # === VISUALIZATION ===
        viz_layout = QHBoxLayout()
        viz_layout.addWidget(self.create_viz_panel("Données Initiales (Avant)", "text_before"))
        viz_layout.addWidget(self.create_viz_panel("Données Triées (Après)", "text_after"))
        layout.addLayout(viz_layout)

        # === STATUS ===
        status_frame = QFrame()
        status_frame.setStyleSheet("background-color: #161B22; border-radius: 8px; border: 1px solid #30363D;")
        self.status_layout = QGridLayout(status_frame)
        
        self.status_labels = {}
        self.timer_labels = {}
        
        algos = ["Bubble Sort", "Insertion Sort", "Shell Sort", "Quick Sort"]
        
        for i, algo in enumerate(algos):
            lbl_name = QLabel(f"{algo}:")
            lbl_name.setStyleSheet("font-weight: bold; font-size: 14px; color: #58A6FF;")
            
            lbl_timer = QLabel("0 s")
            lbl_timer.setStyleSheet("color: #E3B341; font-size: 18px; font-weight: bold;")
            
            lbl_status = QLabel("En attente")
            lbl_status.setStyleSheet("color: #8B949E; font-size: 12px;")
            
            self.status_layout.addWidget(lbl_name, 0, i)
            self.status_layout.addWidget(lbl_timer, 1, i)
            self.status_layout.addWidget(lbl_status, 2, i)
            
            self.status_labels[algo] = lbl_status
            self.timer_labels[algo] = lbl_timer
            
        layout.addWidget(status_frame)

    def create_viz_panel(self, title, attr_name):
        panel = QWidget()
        l = QVBoxLayout(panel)
        l.setContentsMargins(0, 0, 0, 0)
        lbl = QLabel(title)
        lbl.setStyleSheet("font-size: 14px; font-weight: bold; color: #58A6FF;")
        l.addWidget(lbl)
        text_edit = QTextEdit()
        text_edit.setReadOnly(True)
        text_edit.setMinimumHeight(300)
        setattr(self, attr_name, text_edit)
        l.addWidget(text_edit)
        return panel

    def get_current_type(self):
        type_map = {"Entiers": "int", "Réels": "float", "Caractères": "char", "Chaînes": "string"}
        return type_map[self.type_combo.currentText()]

    def generate_random(self):
        try:
            size = int(self.size_input.text())
            if size <= 0: raise ValueError
        except ValueError:
            QMessageBox.warning(self, "Erreur", "Taille invalide.")
            return

        data_type = self.get_current_type()
        
        if data_type == "int":
            # Génération entre 1 million et 10 milliards
            data = [random.randint(1_000_000, 10_000_000_000) for _ in range(size)]
            self.manager.data = data
        else:
            data = self.manager.generate_data(size, data_type)
            
        self.display_data(self.text_before, data)
        self.text_after.clear()
        self.enable_sort_buttons(True)
        self.reset_status()

    def open_manual_input(self):
        try:
            size = int(self.size_input.text())
            if size <= 0: raise ValueError
        except ValueError:
            QMessageBox.warning(self, "Erreur", "Veuillez définir une taille d'abord.")
            return
            
        dialog = ManualInputWindow(size, self.get_current_type(), self)
        if dialog.exec():
            self.manager.data = dialog.result_data
            self.display_data(self.text_before, self.manager.data)
            self.text_after.clear()
            self.enable_sort_buttons(True)
            self.reset_status()

    def display_data(self, text_widget, data):
        """Affiche TOUTES les données sans tronquer"""
        if not data:
            text_widget.setText("Aucune donnée")
            return
            
        chunks = []
        row = []
        for i, item in enumerate(data):
            row.append(str(item))
            if (i + 1) % 10 == 0:
                chunks.append("\t".join(row))
                row = []
        if row:
            chunks.append("\t".join(row))
            
        final_text = "\n".join(chunks)
        text_widget.setText(final_text)
        
        # Scroll vers le haut
        text_widget.verticalScrollBar().setValue(0)

    def enable_sort_buttons(self, enable):
        for btn in self.sort_buttons.values():
            btn.setEnabled(enable)
        self.btn_graph.setEnabled(enable)

    def reset_status(self):
        for lbl in self.status_labels.values():
            lbl.setText("En attente")
            lbl.setStyleSheet("color: #8B949E; font-size: 12px;")
        for lbl in self.timer_labels.values():
            lbl.setText("0 s")
            lbl.setStyleSheet("color: #8B949E; font-size: 18px; font-weight: bold;")
        
        # Réinitialiser les timers
        self.algo_elapsed_timers.clear()

    def start_sorts(self, target_algo):
        """Lance TOUS les tris, mais affiche le résultat de target_algo"""
        self.target_display_algo = target_algo
        self.enable_sort_buttons(False)
        self.btn_stop.setEnabled(True)
        self.text_after.clear()
        
        self.current_run_results = {}
        self.completed_count = 0
        self.workers = []
        
        algos = ["Bubble Sort", "Insertion Sort", "Shell Sort", "Quick Sort"]
        data = self.manager.get_data()
        
        # Démarrer le timer global
        self.timer.start()
        
        # Initialiser les timers pour chaque algo
        self.algo_elapsed_timers.clear()
        
        for algo in algos:
            worker = SortWorker(self.manager, algo, data)
            worker.finished_signal.connect(self.on_sort_finished)
            self.workers.append(worker)
            
            # Créer et démarrer un QElapsedTimer pour cet algo
            elapsed_timer = QElapsedTimer()
            elapsed_timer.start()
            self.algo_elapsed_timers[algo] = elapsed_timer
            
            # Réinitialiser l'affichage
            self.status_labels[algo].setText("En cours...")
            self.status_labels[algo].setStyleSheet("color: #E3B341; font-size: 12px;")
            self.timer_labels[algo].setText("0 s")
            self.timer_labels[algo].setStyleSheet("color: #E3B341; font-size: 18px; font-weight: bold;")
            
            worker.start()

    def update_all_timers(self):
        """Met à jour TOUS les timers individuels des algos en cours"""
        for algo, elapsed_timer in list(self.algo_elapsed_timers.items()):
            if algo in self.timer_labels:
                elapsed_ms = elapsed_timer.elapsed()
                elapsed_sec = elapsed_ms / 1000.0
                time_str = fmt_time(elapsed_sec)
                self.timer_labels[algo].setText(time_str)

    def stop_sorts(self):
        for w in self.workers:
            if w.isRunning():
                w.terminate()
        self.timer.stop()
        self.enable_sort_buttons(True)
        self.btn_stop.setEnabled(False)
        
        # Arrêter tous les timers
        self.algo_elapsed_timers.clear()
        
        QMessageBox.information(self, "Arrêt", "Tri arrêté.")

    @Slot(str, float, list)
    def on_sort_finished(self, algo_name, duration, sorted_data):
        self.current_run_results[algo_name] = duration
        self.completed_count += 1
        
        # Arrêter le timer individuel de cet algo
        if algo_name in self.algo_elapsed_timers:
            del self.algo_elapsed_timers[algo_name]
        
        # Update Status avec temps final
        time_str = fmt_time(duration)
        self.status_labels[algo_name].setText(f"✓ Terminé")
        self.status_labels[algo_name].setStyleSheet("color: #3FB950; font-size: 12px;")
        
        # Afficher le temps FINAL
        self.timer_labels[algo_name].setText(time_str)
        self.timer_labels[algo_name].setStyleSheet("color: #3FB950; font-size: 18px; font-weight: bold;")
        
        # Afficher le résultat si c'est l'algo cible
        if algo_name == self.target_display_algo:
            self.display_data(self.text_after, sorted_data)
        
        if self.completed_count == 4:
            self.all_sorts_finished()

    def all_sorts_finished(self):
        self.timer.stop()
        self.history.append(self.current_run_results)
        self.enable_sort_buttons(True)
        self.btn_stop.setEnabled(False)
        self.btn_graph.setEnabled(True)
        
        # Nettoyer les timers
        self.algo_elapsed_timers.clear()

    def show_graph(self):
        if not self.history:
            QMessageBox.information(self, "Info", "Aucun historique de tri disponible.")
            return
        
        # Créer et afficher la fenêtre graphique
        self.graph_window = GraphWindow(self.history, self)
        self.graph_window.show()


# ==================== MAIN TABLEAU WINDOW CONTAINER ====================

class TableauWindow(QWidget):
    """
    Fenêtre principale du module Tableaux.
    Contient un StackedWidget pour naviguer entre :
    0. Launcher (Choix du mode)
    1. VizWidget (Visualisation pas à pas)
    2. PerformanceWidget (Benchmark - encapsule l'ancien code)
    """
    def __init__(self):
        super().__init__()
        self.setStyleSheet("background-color: #0D1117; color: #C9D1D9;")
        
        self.stack = QStackedWidget()
        
        # 0. Launcher
        self.launcher = TableauLauncher()
        self.launcher.mode_selected.connect(self.switch_mode)
        self.stack.addWidget(self.launcher)
        
        # 1. Viz
        self.viz_widget = VizWidget()
        self.viz_widget.back_signal.connect(self.go_home)
        self.stack.addWidget(self.viz_widget)
        
        # 2. Performance (encapsulated)
        self.perf_widget = PerformanceWidget()
        self.perf_widget.back_signal.connect(self.go_home)
        self.stack.addWidget(self.perf_widget)
        
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.stack)

    def switch_mode(self, mode):
        if mode == "viz":
            self.stack.setCurrentIndex(1)
        elif mode == "perf":
            self.stack.setCurrentIndex(2)

    def go_home(self):
        self.stack.setCurrentIndex(0)
