import sys
import random
from typing import Optional, Union, List, Tuple, Generator, Any
from PySide6.QtWidgets import *
from PySide6.QtCore import *
from PySide6.QtGui import *

# ==================== CONSTANTS & THEME ====================

# Colors (Sci-Fi Dark Theme)
COLOR_BG = QColor("#0D1117")          # Deep Black/Blue
COLOR_PANEL = QColor("#161B22")       # Darker Panel
COLOR_BORDER = QColor("#30363D")      # Borders
COLOR_ACCENT = QColor("#FF6B35")      # Orange Accent (Visualgo style)
COLOR_ACCENT_2 = QColor("#58A6FF")    # Blue Accent (Secondary)
COLOR_TEXT = QColor("#C9D1D9")        # Light Grey Text
COLOR_SUCCESS = QColor("#238636")     # Green
COLOR_DANGER = QColor("#cf222e")      # Red
COLOR_HIGHLIGHT = QColor("#D29922")   # Gold/Yellow for active elements

# Fonts
FONT_UI = "Segoe UI"
FONT_CODE = "Consolas"

# Dimensions
NODE_WIDTH = 120
NODE_HEIGHT = 50
DATA_RECT_RATIO = 0.7  # 70% for data, 30% for pointer
GRID_COLS = 7  # Nodes per row for grid view (max 7 to avoid horizontal scroll)
GRID_SPACING_X = NODE_WIDTH + 50
GRID_SPACING_Y = NODE_HEIGHT + 80

# Grid colors (Cyberpunk)
COLOR_BG_START = QColor("#05080f")
COLOR_BG_END = QColor("#0d1117")
COLOR_GRID = QColor("#141e32")

class ListGraphicsView(QGraphicsView):
    """
    Custom GraphicsView to handle Zoom and Cyberpunk Grid Background.
    """
    def __init__(self, scene, parent=None):
        super().__init__(scene, parent)
        self.setRenderHint(QPainter.Antialiasing)
        self.setTransformationAnchor(QGraphicsView.AnchorUnderMouse)
        self.setResizeAnchor(QGraphicsView.AnchorUnderMouse)
        self.setDragMode(QGraphicsView.NoDrag) # Disable Hand Drag
        # Remove border
        self.setStyleSheet("border: 0px;")

    def drawBackground(self, painter, rect):
        # 1. Gradient Background
        gradient = QLinearGradient(rect.topLeft(), rect.bottomRight())
        gradient.setColorAt(0, COLOR_BG_START)
        gradient.setColorAt(1, COLOR_BG_END)
        painter.fillRect(rect, gradient)

        # 2. Tech Grid
        grid_size = 50
        pen = QPen(COLOR_GRID)
        pen.setWidth(1)
        painter.setPen(pen)

        # Calculate grid lines based on scene rect
        left = int(rect.left()) - (int(rect.left()) % grid_size)
        top = int(rect.top()) - (int(rect.top()) % grid_size)

        # Draw vertical lines
        for x in range(left, int(rect.right()), grid_size):
            painter.drawLine(x, int(rect.top()), x, int(rect.bottom()))

        # Draw horizontal lines
        for y in range(top, int(rect.bottom()), grid_size):
            painter.drawLine(int(rect.left()), y, int(rect.right()), y)

# ==================== LOGIC CLASSES (BACKEND) ====================

class NodeSimple:
    """Nœud pour liste simplement chaînée"""
    def __init__(self, data):
        self.data = data
        self.next = None

class NodeDouble:
    """Nœud pour liste doublement chaînée"""
    def __init__(self, data):
        self.data = data
        self.prev = None
        self.next = None

class ListeSimple:
    """Liste simplement chaînée générique"""
    def __init__(self):
        self.head = None
        self.count = 0

    def insererDebut(self, value):
        new_node = NodeSimple(value)
        new_node.next = self.head
        self.head = new_node
        self.count += 1
        return new_node

    def insererFin(self, value):
        new_node = NodeSimple(value)
        if self.head is None:
            self.head = new_node
        else:
            temp = self.head
            while temp.next is not None:
                temp = temp.next
            temp.next = new_node
        self.count += 1
        return new_node

    def insererPosition(self, value, pos):
        if pos <= 0: return self.insererDebut(value)
        if pos >= self.count: return self.insererFin(value)
        
        new_node = NodeSimple(value)
        temp = self.head
        for _ in range(pos - 1):
            temp = temp.next
        
        new_node.next = temp.next
        temp.next = new_node
        self.count += 1
        return new_node

    def supprimerDebut(self):
        if self.head is None: return False
        self.head = self.head.next
        self.count -= 1
        return True

    def supprimerFin(self):
        if self.head is None: return False
        if self.head.next is None:
            self.head = None
            self.count -= 1
            return True
        
        temp = self.head
        while temp.next.next is not None:
            temp = temp.next
        temp.next = None
        self.count -= 1
        return True

    def modifier(self, index, new_value):
        if index < 0 or index >= self.count or self.head is None: return False
        temp = self.head
        for _ in range(index):
            temp = temp.next
        temp.data = new_value
        return True

    def supprimerPosition(self, pos):
        if pos < 0 or pos >= self.count or self.head is None: return False
        if pos == 0: return self.supprimerDebut()
        
        temp = self.head
        for _ in range(pos - 1):
            temp = temp.next
            
        temp.next = temp.next.next
        self.count -= 1
        return True

    def supprimerValeur(self, value):
        if self.head is None: return False
        if self.head.data == value: return self.supprimerDebut()
        
        temp = self.head
        while temp.next is not None:
            if temp.next.data == value:
                temp.next = temp.next.next
                self.count -= 1
                return True
            temp = temp.next
        return False

    def rechercher(self, value):
        temp = self.head
        pos = 0
        while temp is not None:
            if temp.data == value: return pos
            temp = temp.next
            pos += 1
        return -1

    def vider(self):
        self.head = None
        self.count = 0
    
    def toList(self):
        res = []
        curr = self.head
        while curr:
            res.append(curr.data)
            curr = curr.next
        return res

class ListeDouble:
    """Liste doublement chaînée générique"""
    def __init__(self):
        self.head = None
        self.tail = None
        self.count = 0

    def insererDebut(self, value):
        new_node = NodeDouble(value)
        new_node.prev = None
        new_node.next = self.head
        
        if self.head is not None:
            self.head.prev = new_node
        else:
            self.tail = new_node
            
        self.head = new_node
        self.count += 1
        return new_node

    def insererFin(self, value):
        new_node = NodeDouble(value)
        new_node.next = None
        new_node.prev = self.tail
        
        if self.tail is not None:
            self.tail.next = new_node
        else:
            self.head = new_node
            
        self.tail = new_node
        self.count += 1
        return new_node

    def insererPosition(self, value, pos):
        if pos <= 0: return self.insererDebut(value)
        if pos >= self.count: return self.insererFin(value)
        
        new_node = NodeDouble(value)
        temp = self.head
        for _ in range(pos):
            temp = temp.next
            
        new_node.next = temp
        new_node.prev = temp.prev
        temp.prev.next = new_node
        temp.prev = new_node
        self.count += 1
        return new_node

    def supprimerDebut(self):
        if self.head is None: return False
        self.head = self.head.next
        if self.head is not None:
            self.head.prev = None
        else:
            self.tail = None
        self.count -= 1
        return True

    def supprimerFin(self):
        if self.tail is None: return False
        self.tail = self.tail.prev
        if self.tail is not None:
            self.tail.next = None
        else:
            self.head = None
        self.count -= 1
        return True

    def modifier(self, index, new_value):
        if index < 0 or index >= self.count or self.head is None: return False
        temp = self.head
        for _ in range(index):
            temp = temp.next
        temp.data = new_value
        return True

    def supprimerPosition(self, pos):
        if pos < 0 or pos >= self.count or self.head is None: return False
        if pos == 0: return self.supprimerDebut()
        if pos == self.count - 1: return self.supprimerFin()
        
        temp = self.head
        for _ in range(pos):
            temp = temp.next
            
        temp.prev.next = temp.next
        temp.next.prev = temp.prev
        self.count -= 1
        return True

    def supprimerValeur(self, value):
        if self.head is None: return False
        
        temp = self.head
        while temp is not None:
            if temp.data == value:
                if temp == self.head:
                    return self.supprimerDebut()
                if temp == self.tail:
                    return self.supprimerFin()
                
                temp.prev.next = temp.next
                temp.next.prev = temp.prev
                self.count -= 1
                return True
            temp = temp.next
        return False

    def rechercher(self, value):
        temp = self.head
        pos = 0
        while temp is not None:
            if temp.data == value: return pos
            temp = temp.next
            pos += 1
        return -1

    def vider(self):
        self.head = None
        self.tail = None
        self.count = 0

    def toList(self):
        res = []
        curr = self.head
        while curr:
            res.append(curr.data)
            curr = curr.next
        return res

# ==================== VISUAL GRAPHICS ITEMS ====================

class VisualNode(QGraphicsObject):
    """
    Visual representation of a node.
    Simple: Data (Left) | Pointer (Right)
    Double: Pointer (Left) | Data (Middle) | Pointer (Right)
    """
    def __init__(self, value, is_double=False, visualizer=None):
        super().__init__()
        self.value = str(value)
        self.is_double = is_double
        self.value = str(value)
        self.is_double = is_double
        self.visualizer = visualizer
        self.width = NODE_WIDTH
        self.height = NODE_HEIGHT
        self.editor_proxy = None # For in-place editing
        self.bg_color = COLOR_PANEL
        self.border_color = COLOR_ACCENT
        self.highlighted = False
        self.override_color = None
        
        # Animations
        self.opacity_anim = QPropertyAnimation(self, b"opacity")
        self.opacity_anim.setDuration(500)
        self.pos_anim = QPropertyAnimation(self, b"pos")
        self.pos_anim.setDuration(500)
        self.pos_anim.setEasingCurve(QEasingCurve.InOutQuad)
        
        # Enable shadow
        self.setAcceptHoverEvents(True)
        self.setFlag(QGraphicsItem.ItemIsMovable, False) # Movable via code only

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            if self.editor_proxy:
                return # Already editing
            self.start_edit()
            event.accept()
        else:
            super().mousePressEvent(event)

    def start_edit(self):
        """Start in-place editing with a QLineEdit"""
        # Calculate Data Rect
        if not self.is_double:
            split_x = self.width * 0.7
            data_rect = QRectF(0, 0, split_x, self.height)
        else:
            w_ptr = self.width * 0.2
            w_data = self.width * 0.6
            data_rect = QRectF(w_ptr, 0, w_data, self.height)
            
        # Create Editor
        self.editor = QLineEdit(self.value)
        self.editor.setAlignment(Qt.AlignCenter)
        self.editor.setStyleSheet(f"background: #0D1117; color: white; border: 2px solid {COLOR_ACCENT.name()}; font-family: Consolas; font-weight: bold;")
        self.editor.selectAll()
        self.editor.editingFinished.connect(self.finish_edit)
        
        # Embed in Scene
        self.editor_proxy = QGraphicsProxyWidget(self)
        self.editor_proxy.setWidget(self.editor)
        self.editor_proxy.setGeometry(data_rect)
        self.editor.setFocus()

    def finish_edit(self):
        """Commit edit and close editor"""
        if not self.editor_proxy: return
        
        new_val = self.editor.text()
        
        # Commit to backend
        # Commit to backend
        if self.visualizer:
            if not self.visualizer.commit_edit(self, new_val):
                # Validation failed - Commit showed warning
                # Do not update visual value
                pass
            else:
                # Update Visual
                self.set_value(new_val)
        else:
            self.set_value(new_val)
        
        # Cleanup
        self.editor_proxy.setWidget(None) # Detach
        self.scene().removeItem(self.editor_proxy)
        self.editor_proxy = None
        self.editor = None
        
    def boundingRect(self):
        return QRectF(0, 0, self.width, self.height)
    
    def paint(self, painter, option, widget):
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Rectangles
        rect = self.boundingRect()
        
        # Draw Backgrounds
        if self.override_color:
            current_bg = self.override_color
        else:
            current_bg = COLOR_HIGHLIGHT if self.highlighted else self.bg_color
        
        pen = QPen(self.border_color)
        pen.setWidth(3)
        painter.setPen(pen)
        
        if not self.is_double:
            # Simple: [ Data 70% ] [ Ptr 30% ]
            split_x = self.width * 0.7
            data_rect = QRectF(0, 0, split_x, self.height)
            ptr_rect = QRectF(split_x, 0, self.width - split_x, self.height)
            
            painter.fillRect(data_rect, current_bg)
            painter.fillRect(ptr_rect, COLOR_ACCENT)
            
            painter.drawRect(data_rect)
            painter.drawRect(ptr_rect)
            
            # Text
            painter.setPen(COLOR_TEXT if not self.highlighted and not self.override_color else Qt.black)
            painter.setFont(QFont(FONT_CODE, 12, QFont.Bold))
            painter.drawText(data_rect, Qt.AlignCenter, self.value)
            
            painter.setPen(Qt.white)
            painter.drawText(ptr_rect, Qt.AlignCenter, "•")
            
        else:
            # Double: [ Prev 20% ] [ Data 60% ] [ Next 20% ]
            w_ptr = self.width * 0.2
            w_data = self.width * 0.6
            
            prev_rect = QRectF(0, 0, w_ptr, self.height)
            data_rect = QRectF(w_ptr, 0, w_data, self.height)
            next_rect = QRectF(w_ptr + w_data, 0, w_ptr, self.height)
            
            painter.fillRect(prev_rect, COLOR_ACCENT_2) # Prev is Blue-ish
            painter.fillRect(data_rect, current_bg)
            painter.fillRect(next_rect, COLOR_ACCENT)   # Next is Orange
            
            painter.drawRect(prev_rect)
            painter.drawRect(data_rect)
            painter.drawRect(next_rect)
            
            # Text
            painter.setPen(COLOR_TEXT if not self.highlighted and not self.override_color else Qt.black)
            painter.setFont(QFont(FONT_CODE, 12, QFont.Bold))
            painter.drawText(data_rect, Qt.AlignCenter, self.value)
            
            painter.setPen(Qt.white)
            painter.drawText(prev_rect, Qt.AlignCenter, "•")
            painter.drawText(next_rect, Qt.AlignCenter, "•")

    def set_highlight(self, active: bool):
        self.highlighted = active
        self.update()

    def set_color(self, color):
        self.override_color = color
        self.update()
    
    def set_value(self, val):
        self.value = str(val)
        self.update()

class VisualArrow(QGraphicsPathItem):
    """
    Arrow connecting two nodes.
    Supports wrapping lines (Grid Layout).
    """
    def __init__(self, start_node, end_node, is_double=False):
        super().__init__()
        self.start_node = start_node
        self.end_node = end_node
        self.is_double = is_double
        
        pen = QPen(COLOR_TEXT)
        pen.setWidth(3)
        self.setPen(pen)
        self.setZValue(-1) # Behind nodes
        self.update_position()

    def update_position(self):
        if not self.start_node or not self.end_node:
            return

        start_rect = self.start_node.sceneBoundingRect()
        end_rect = self.end_node.sceneBoundingRect()
        
        path = QPainterPath()
        
        # Check if wrapping (End Node is to the LEFT of Start Node AND Below)
        is_wrapping = (end_rect.center().x() < start_rect.center().x()) and (end_rect.center().y() > start_rect.center().y())
        
        if is_wrapping:
            # Draw Connector: Right -> Down -> Left -> Right
            # 1. Start from Right of StartNode
            s_x = start_rect.right()
            s_y = start_rect.center().y()
            
            # 2. Target Left of EndNode
            e_x = end_rect.left()
            e_y = end_rect.center().y()
            
            mid_y = (s_y + e_y) / 2
            
            path.moveTo(s_x, s_y)
            # Curve out to the right and down
            path.cubicTo(s_x + 50, s_y, s_x + 50, mid_y, (s_x + end_rect.left())/2 + 200, mid_y) 
            # This is complex to get perfect without overlapping. 
            # Simpler "Circuit" style:
            # Right -> Down (mid) -> Left (to match end x) -> Down (end y)
            
            # Revised simple wrap path:
            path = QPainterPath()
            path.moveTo(s_x, s_y)
            # Go Right
            path.lineTo(s_x + 30, s_y)
            # Go Down to mid vertical space
            path.lineTo(s_x + 30, mid_y)
            # Go Left to target X - 30
            path.lineTo(e_x - 30, mid_y)
            # Go Down to target Y
            path.lineTo(e_x - 30, e_y)
            # Go Right to target
            path.lineTo(e_x, e_y)
            
            self.draw_arrow_head(path, QPointF(e_x - 10, e_y), QPointF(e_x, e_y))
            
        else:
            # Standard Neighbor (Left -> Right)
            if not self.is_double:
                # Start from the "dot" in the pointer section (Right)
                start_x = start_rect.right() - (start_rect.width() * 0.15)
                start_y = start_rect.center().y()
                
                end_x = end_rect.left()
                end_y = end_rect.center().y()
                
                path.moveTo(start_x, start_y)
                
                # Curve
                ctrl1 = QPointF(start_x + 30, start_y)
                ctrl2 = QPointF(end_x - 30, end_y)
                path.cubicTo(ctrl1, ctrl2, QPointF(end_x, end_y))
                
                # Arrowhead >
                self.draw_arrow_head(path, ctrl2, QPointF(end_x, end_y))
                
            else:
                # Double: Two lines or Bidirectional
                s1_x = start_rect.right() - (start_rect.width() * 0.1)
                s1_y = start_rect.center().y() - 10
                
                e1_x = end_rect.left()
                e1_y = end_rect.center().y() - 10
                
                path.moveTo(s1_x, s1_y)
                path.lineTo(e1_x, e1_y)
                self.draw_arrow_head(path, QPointF(s1_x, s1_y), QPointF(e1_x, e1_y))
                
                s2_x = end_rect.left() + (end_rect.width() * 0.1)
                s2_y = end_rect.center().y() + 10
                
                e2_x = start_rect.right()
                e2_y = start_rect.center().y() + 10
                
                path.moveTo(s2_x, s2_y)
                path.lineTo(e2_x, e2_y)
                self.draw_arrow_head(path, QPointF(s2_x, s2_y), QPointF(e2_x, e2_y))
        
        self.setPath(path)

    def draw_arrow_head(self, path, p1, p2):
        arrow_size = 8
        line = QLineF(p1, p2)
        angle = line.angle()
        
        # Simple trig calculation approx
        import math
        angle_rad = math.radians(angle)
        p_arrow_1 = p2 - QPointF(math.cos(angle_rad - math.pi/6) * arrow_size, -math.sin(angle_rad - math.pi/6) * arrow_size)
        p_arrow_2 = p2 - QPointF(math.cos(angle_rad + math.pi/6) * arrow_size, -math.sin(angle_rad + math.pi/6) * arrow_size)

        path.moveTo(p2)
        path.lineTo(p_arrow_1)
        path.moveTo(p2)
        path.lineTo(p_arrow_2)

class VisualPointer(QGraphicsObject):
    """
    Floating label (HEAD, TAIL, TEMP) with an arrow pointing down to a node.
    """
    def __init__(self, text, color=COLOR_ACCENT_2):
        super().__init__()
        self.text = text
        self.color = color
        self.target_node = None 
        
        self.pos_anim = QPropertyAnimation(self, b"pos")
        self.pos_anim.setDuration(500)
        self.pos_anim.setEasingCurve(QEasingCurve.InOutQuad)
        self.setZValue(10) # On top
        
    def boundingRect(self):
        return QRectF(-25, -40, 50, 40)
        
    def paint(self, painter, option, widget):
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Label Box
        rect = QRectF(-25, -40, 50, 25)
        painter.setBrush(self.color)
        painter.setPen(Qt.NoPen)
        painter.drawRoundedRect(rect, 5, 5)
        
        # Text
        painter.setPen(Qt.black)
        painter.setFont(QFont(FONT_UI, 10, QFont.Bold))
        painter.drawText(rect, Qt.AlignCenter, self.text)
        
        # Arrow down
        painter.setPen(QPen(self.color, 2))
        painter.drawLine(0, -15, 0, 0)
        painter.drawLine(0, 0, -5, -8)
        painter.drawLine(0, 0, 5, -8)


# ==================== MAIN WINDOW & LAYOUT ====================

# ==================== GLOBAL STYLES & HELPERS ====================
# Shades of Orange (Gradient/Theme)
COLOR_MATTE_GREEN = "#FFA726"   # Insertion
COLOR_MATTE_RED = "#D84315"     # Suppression (Darkest)
COLOR_MATTE_BLUE = "#FFB74D"    # Recherche
COLOR_MATTE_PURPLE = "#F57C00"  # Tri
COLOR_MATTE_CYAN = "#FFE0B2"    # Aleatoire (Lightest)
COLOR_GREY_BORDER = "#5C6370"

# Frame style - Clean 2px border, dark semi-transparent background
def frame_style(color):
    r, g, b = int(color[1:3], 16), int(color[3:5], 16), int(color[5:7], 16)
    return f"""
        QFrame {{
            background-color: rgba(22, 27, 34, 0.8);
            border: 2px solid {color};
            border-radius: 8px;
        }}
        QFrame:hover {{
            border: 2px solid {color};
            background-color: rgba({r}, {g}, {b}, 30);
        }}
    """

# Button style - Dark base with neon border on hover (like Arbres/Graphes)
def btn_style(color):
    r, g, b = int(color[1:3], 16), int(color[3:5], 16), int(color[5:7], 16)
    return f"""
        QPushButton {{
            background-color: #161b22;
            color: #c9d1d9;
            border: 1px solid #30363d;
            border-radius: 4px;
            padding: 5px 10px;
            min-height: 24px;
            font-family: {FONT_UI};
            font-size: 11px;
            font-weight: bold;
        }}
        QPushButton:hover {{
            border: 2px solid {color};
            color: #ffffff;
            background-color: rgba({r}, {g}, {b}, 40);
        }}
        QPushButton:pressed {{
            background-color: #0d1117;
        }}
    """

# Input style - Dark base with colored border on focus (like Arbres)
def input_style(color):
    return f"""
        QLineEdit {{
            background-color: #0d1117;
            color: #ffffff;
            border: 1px solid #30363d;
            border-radius: 4px;
            padding: 5px;
            min-height: 24px;
            font-family: Consolas;
            font-size: 11px;
        }}
        QLineEdit:focus {{
            border: 2px solid {color};
        }}
    """

class ManualCreationDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Saisie Manuelle")
        self.resize(300, 150)
        self.setStyleSheet(f"background: {COLOR_BG.name()}; color: {COLOR_TEXT.name()};")
        
        layout = QVBoxLayout(self)
        
        # Form Layout
        form_layout = QFormLayout()
        
        # Size Input
        self.spin_size = QSpinBox()
        self.spin_size.setRange(1, 100)
        self.spin_size.setValue(5)
        self.spin_size.setStyleSheet(input_style(COLOR_ACCENT.name()))
        self.spin_size.setFocus()
        self.spin_size.selectAll()
        form_layout.addRow("Taille:", self.spin_size)
        
        # Type Input
        self.combo_type = QComboBox()
        self.combo_type.addItems(["Int", "Float", "Str", "Char"])
        self.combo_type.setStyleSheet(f"""
            QComboBox {{
                background: {COLOR_PANEL.name()};
                color: {COLOR_ACCENT.name()};
                border: 1px solid {COLOR_ACCENT.name()};
                padding: 4px;
            }}
             QComboBox QAbstractItemView {{
                background: {COLOR_PANEL.name()};
                color: {COLOR_TEXT.name()};
                selection-background-color: {COLOR_ACCENT.name()};
            }}
        """)
        form_layout.addRow("Type:", self.combo_type)
        
        layout.addLayout(form_layout)
        
        # Buttons
        btn_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        btn_box.accepted.connect(self.accept)
        btn_box.rejected.connect(self.reject)
        btn_box.accepted.connect(self.accept)
        btn_box.rejected.connect(self.reject)
        btn_box.setStyleSheet(f"QPushButton {{ {btn_style(COLOR_ACCENT.name())} }}")
        layout.addWidget(btn_box)
    
    def get_data(self):
        return self.spin_size.value(), self.combo_type.currentText()

class LinkedListVisualizer(QMainWindow):
    def __init__(self, list_type="Simple"):
        super().__init__()
        self.setWindowTitle(f"Linked List Visualizer - Sci-Fi")
        self.resize(1600, 900)
        self.setStyleSheet(f"background-color: {COLOR_BG.name()}; color: {COLOR_TEXT.name()};")
        
        # Central Widget is Stacked (Page 0: Selection, Page 1: Visualizer)
        self.stacked_widget = QStackedWidget()
        self.setCentralWidget(self.stacked_widget)
        
        # Setup Pages
        self.page_selection = QWidget()
        self.setup_selection_page()
        self.stacked_widget.addWidget(self.page_selection)
        
        self.page_visualizer = QWidget()
        self.layout_visualizer = QVBoxLayout(self.page_visualizer)
        self.layout_visualizer.setContentsMargins(0,0,0,0)
        self.stacked_widget.addWidget(self.page_visualizer)
        
        # Data
        self.list_type = "Simple"
        self.logic_list = ListeSimple()
        self.is_double = False
            
        self.visual_nodes = [] 
        self.visual_arrows = [] 
        self.visual_arrows = [] 
        self.pointers = {} 
        self.current_manual_type = "Str" # Default type for validation 
        
        # Animation State
        self.animation_generator = None
        self.timer = QTimer()
        self.timer.timeout.connect(self.next_step)
        self.timer_interval = 600
        
        self.setup_ui()
        self.setup_scene()
        self.init_pointers()
        
        # Start directly on Visualizer (Simple list by default)
        self.stacked_widget.setCurrentIndex(1)

    def closeEvent(self, event):
        # Just close, no reset needed
        super().closeEvent(event)

    def setup_selection_page(self):
        layout = QVBoxLayout(self.page_selection)
        layout.setAlignment(Qt.AlignCenter)
        
        lbl = QLabel("Veuillez choisir le type de liste chaînée :")
        lbl.setFont(QFont(FONT_UI, 24, QFont.Bold))
        lbl.setAlignment(Qt.AlignCenter)
        layout.addWidget(lbl)
        layout.addSpacing(50)
        
        btn_simple = QPushButton("Liste Simplement Chaînée")
        btn_simple.setFixedSize(400, 100)
        btn_simple.setFont(QFont(FONT_UI, 16, QFont.Bold))
        btn_simple.setStyleSheet(f"background-color: {COLOR_PANEL.name()}; border: 2px solid {COLOR_ACCENT.name()}; color: {COLOR_ACCENT.name()}; border-radius: 10px;")
        btn_simple.clicked.connect(lambda: self.select_list_type("Simple"))
        layout.addWidget(btn_simple, 0, Qt.AlignCenter)
        
        layout.addSpacing(20)
        
        btn_double = QPushButton("Liste Doublement Chaînée")
        btn_double.setFixedSize(400, 100)
        btn_double.setFont(QFont(FONT_UI, 16, QFont.Bold))
        btn_double.setStyleSheet(f"background-color: {COLOR_PANEL.name()}; border: 2px solid {COLOR_ACCENT_2.name()}; color: {COLOR_ACCENT_2.name()}; border-radius: 10px;")
        btn_double.clicked.connect(lambda: self.select_list_type("Double"))
        layout.addWidget(btn_double, 0, Qt.AlignCenter)
        
    def select_list_type(self, type_str):
        self.list_type = type_str
        self.reset_list()
        
        if type_str == "Simple":
            self.logic_list = ListeSimple()
            self.is_double = False
        else:
            self.logic_list = ListeDouble()
            self.is_double = True
            
        # Update Title
        if hasattr(self, 'title_label'):
            self.title_label.setText(f"LISTE CHAINEE {type_str.upper()}")
            
        # Switch View
        self.stacked_widget.setCurrentIndex(1)

    def go_back_menu(self):
        self.reset_list()
        self.close()  # Close window to return to main menu

    def setup_ui(self):
        # ==================== MATTE COLORS FOR FRAMES (easier on eyes) ====================
        # MOVED TO GLOBAL
        
        # ==================== TOP BAR: TITLE + TYPE BUTTONS ====================
        top_bar = QWidget()
        top_bar.setFixedHeight(45)
        top_bar.setStyleSheet("background: transparent;")
        top_layout = QHBoxLayout(top_bar)
        top_layout.setContentsMargins(20, 5, 20, 5)
        
        # Stretch before title to help center
        top_layout.addStretch()
        
        # Title (centered)
        self.title_label = QLabel(f"LISTE CHAINEE {self.list_type.upper()}")
        self.title_label.setAlignment(Qt.AlignCenter)
        self.title_label.setStyleSheet(f"""
            color: {COLOR_ACCENT.name()};
            font-family: {FONT_UI};
            font-size: 20px;
            font-weight: bold;
            letter-spacing: 3px;
        """)
        top_layout.addWidget(self.title_label)
        
        top_layout.addStretch()
        
        # Simple/Double Toggle Buttons
        btn_simple = QPushButton("Simple")
        btn_simple.setFixedSize(90, 30)
        btn_simple.setStyleSheet(btn_style(COLOR_MATTE_BLUE))
        btn_simple.clicked.connect(lambda: self.switch_list_type("Simple"))
        
        btn_double = QPushButton("Double")
        btn_double.setFixedSize(90, 30)
        btn_double.setStyleSheet(btn_style(COLOR_MATTE_BLUE))
        btn_double.clicked.connect(lambda: self.switch_list_type("Double"))
        
        top_layout.addWidget(btn_simple)
        top_layout.addWidget(btn_double)
        
        self.layout_visualizer.addWidget(top_bar)
        
        # ==================== CONTROL BAR: 5 FRAMES ====================
        control_bar = QWidget()
        control_bar.setFixedHeight(150)
        control_bar.setStyleSheet("background: rgba(13, 17, 23, 0.95);")
        control_layout = QHBoxLayout(control_bar)
        control_layout.setContentsMargins(15, 8, 15, 8)
        # control_layout.setSpacing(12) # Use stretch instead
        
        control_layout.addStretch()
        
        # ---------- FRAME 1: INSERTION (Green) ----------
        frame_insert_container = QVBoxLayout()
        frame_insert_container.setSpacing(5)
        
        lbl_insert = QLabel("INSERTION")
        lbl_insert.setStyleSheet(f"color: {COLOR_MATTE_GREEN}; font-size: 11px; font-weight: bold;")
        lbl_insert.setAlignment(Qt.AlignCenter)
        frame_insert_container.addWidget(lbl_insert)
        
        frame_insert = QFrame()
        frame_insert.setStyleSheet(frame_style(COLOR_MATTE_GREEN))
        insert_layout = QVBoxLayout(frame_insert)
        insert_layout.setContentsMargins(15, 18, 15, 18)
        insert_layout.setSpacing(8)
        
        insert_row1 = QHBoxLayout()
        insert_row1.setSpacing(8)
        btn_ins_debut = QPushButton("Debut")
        btn_ins_debut.setStyleSheet(btn_style(COLOR_MATTE_GREEN))
        btn_ins_debut.clicked.connect(lambda: self.start_animation(self.anim_insert_head))
        btn_ins_fin = QPushButton("Fin")
        btn_ins_fin.setStyleSheet(btn_style(COLOR_MATTE_GREEN))
        btn_ins_fin.clicked.connect(lambda: self.start_animation(self.anim_insert_tail))
        insert_row1.addWidget(btn_ins_debut)
        insert_row1.addWidget(btn_ins_fin)
        insert_layout.addLayout(insert_row1)
        
        insert_row2 = QHBoxLayout()
        insert_row2.setSpacing(8)
        btn_ins_pos = QPushButton("Pos")
        btn_ins_pos.setStyleSheet(btn_style(COLOR_MATTE_GREEN))
        btn_ins_pos.clicked.connect(lambda: self.start_animation(self.anim_insert_pos))
        self.input_pos_insert = QLineEdit("")
        self.input_pos_insert.setPlaceholderText("0")
        self.input_pos_insert.setFixedWidth(55)
        self.input_pos_insert.setStyleSheet(input_style(COLOR_MATTE_GREEN))
        insert_row2.addWidget(btn_ins_pos)
        insert_row2.addWidget(self.input_pos_insert)
        insert_layout.addLayout(insert_row2)
        
        frame_insert_container.addWidget(frame_insert)
        control_layout.addLayout(frame_insert_container)
        control_layout.addStretch()
        
        # ---------- FRAME 2: SUPPRESSION (Red) ----------
        frame_delete_container = QVBoxLayout()
        frame_delete_container.setSpacing(5)
        
        lbl_delete = QLabel("SUPPRESSION")
        lbl_delete.setStyleSheet(f"color: {COLOR_MATTE_RED}; font-size: 11px; font-weight: bold;")
        lbl_delete.setAlignment(Qt.AlignCenter)
        frame_delete_container.addWidget(lbl_delete)
        
        frame_delete = QFrame()
        frame_delete.setStyleSheet(frame_style(COLOR_MATTE_RED))
        delete_layout = QVBoxLayout(frame_delete)
        delete_layout.setContentsMargins(15, 18, 15, 18)
        delete_layout.setSpacing(8)
        
        delete_row1 = QHBoxLayout()
        delete_row1.setSpacing(8)
        btn_del_debut = QPushButton("Deb")
        btn_del_debut.setStyleSheet(btn_style(COLOR_MATTE_RED))
        btn_del_debut.clicked.connect(lambda: self.start_animation(self.anim_delete_head))
        btn_del_fin = QPushButton("Fin")
        btn_del_fin.setStyleSheet(btn_style(COLOR_MATTE_RED))
        btn_del_fin.clicked.connect(lambda: self.start_animation(self.anim_delete_tail))
        delete_row1.addWidget(btn_del_debut)
        delete_row1.addWidget(btn_del_fin)
        delete_layout.addLayout(delete_row1)
        
        delete_row2 = QHBoxLayout()
        delete_row2.setSpacing(8)
        
        # HBox Val
        hbox_del_val = QHBoxLayout()
        hbox_del_val.setSpacing(4)
        btn_del_val = QPushButton("Val")
        btn_del_val.setStyleSheet(btn_style(COLOR_MATTE_RED))
        btn_del_val.clicked.connect(lambda: self.start_animation(self.anim_delete_val))
        self.input_val_delete = QLineEdit("")
        self.input_val_delete.setFixedWidth(55)
        self.input_val_delete.setPlaceholderText("val")
        self.input_val_delete.setStyleSheet(input_style(COLOR_MATTE_RED))
        hbox_del_val.addWidget(btn_del_val)
        hbox_del_val.addWidget(self.input_val_delete)
        
        # HBox Pos
        hbox_del_pos = QHBoxLayout()
        hbox_del_pos.setSpacing(4)
        btn_del_pos = QPushButton("Pos")
        btn_del_pos.setStyleSheet(btn_style(COLOR_MATTE_RED))
        btn_del_pos.clicked.connect(lambda: self.start_animation(self.anim_delete_pos))
        self.input_pos_delete = QLineEdit("")
        self.input_pos_delete.setPlaceholderText("0")
        self.input_pos_delete.setFixedWidth(55)
        self.input_pos_delete.setStyleSheet(input_style(COLOR_MATTE_RED))
        hbox_del_pos.addWidget(btn_del_pos)
        hbox_del_pos.addWidget(self.input_pos_delete)
        
        delete_row2.addLayout(hbox_del_val)
        delete_row2.addLayout(hbox_del_pos)
        delete_layout.addLayout(delete_row2)
        
        frame_delete_container.addWidget(frame_delete)
        control_layout.addLayout(frame_delete_container)
        control_layout.addStretch()
        
        # ---------- FRAME 3: RECHERCHE (Blue) ----------
        frame_search_container = QVBoxLayout()
        frame_search_container.setSpacing(5)
        
        lbl_search = QLabel("RECHERCHE")
        lbl_search.setStyleSheet(f"color: {COLOR_MATTE_BLUE}; font-size: 11px; font-weight: bold;")
        lbl_search.setAlignment(Qt.AlignCenter)
        frame_search_container.addWidget(lbl_search)
        
        frame_search = QFrame()
        frame_search.setStyleSheet(frame_style(COLOR_MATTE_BLUE))
        search_layout = QVBoxLayout(frame_search)
        search_layout.setContentsMargins(15, 18, 15, 18)
        search_layout.setSpacing(8)
        
        search_main_row = QHBoxLayout()
        search_main_row.setSpacing(8)
        
        # Val Column
        vbox_search_val = QVBoxLayout()
        vbox_search_val.setSpacing(4)
        btn_search_val = QPushButton("Val")
        btn_search_val.setStyleSheet(btn_style(COLOR_MATTE_BLUE))
        btn_search_val.clicked.connect(lambda: self.start_animation(self.anim_search))
        self.input_search_val = QLineEdit("")
        self.input_search_val.setPlaceholderText("val")
        self.input_search_val.setFixedWidth(60)
        self.input_search_val.setStyleSheet(input_style(COLOR_MATTE_BLUE))
        vbox_search_val.addWidget(btn_search_val)
        vbox_search_val.addWidget(self.input_search_val)
        
        # Pos Column
        vbox_search_pos = QVBoxLayout()
        vbox_search_pos.setSpacing(4)
        btn_search_pos = QPushButton("Pos")
        btn_search_pos.setStyleSheet(btn_style(COLOR_MATTE_BLUE))
        btn_search_pos.clicked.connect(lambda: self.search_by_pos())
        self.input_search_pos = QLineEdit("")
        self.input_search_pos.setPlaceholderText("0")
        self.input_search_pos.setFixedWidth(55)
        self.input_search_pos.setStyleSheet(input_style(COLOR_MATTE_BLUE))
        vbox_search_pos.addWidget(btn_search_pos)
        vbox_search_pos.addWidget(self.input_search_pos)
        
        search_main_row.addLayout(vbox_search_val)
        search_main_row.addLayout(vbox_search_pos)
        search_layout.addLayout(search_main_row)
        
        frame_search_container.addWidget(frame_search)
        control_layout.addLayout(frame_search_container)
        control_layout.addStretch()
        
        # ---------- FRAME 4: TRI (Purple) ----------
        frame_sort_container = QVBoxLayout()
        frame_sort_container.setSpacing(5)
        
        lbl_sort = QLabel("TRI")
        lbl_sort.setStyleSheet(f"color: {COLOR_MATTE_PURPLE}; font-size: 11px; font-weight: bold;")
        lbl_sort.setAlignment(Qt.AlignCenter)
        frame_sort_container.addWidget(lbl_sort)
        
        frame_sort = QFrame()
        frame_sort.setStyleSheet(frame_style(COLOR_MATTE_PURPLE))
        sort_layout = QVBoxLayout(frame_sort)
        sort_layout.setContentsMargins(15, 18, 15, 18)
        sort_layout.setSpacing(8)
        
        sort_row1 = QHBoxLayout()
        sort_row1.setSpacing(8)
        btn_bubble = QPushButton("Bulle")
        btn_bubble.setStyleSheet(btn_style(COLOR_MATTE_PURPLE))
        btn_bubble.clicked.connect(lambda: self.start_animation(self.anim_bubble_sort))
        btn_quick = QPushButton("Quick")
        btn_quick.setStyleSheet(btn_style(COLOR_MATTE_PURPLE))
        btn_quick.clicked.connect(lambda: self.start_animation(self.anim_quicksort))
        sort_row1.addWidget(btn_bubble)
        sort_row1.addWidget(btn_quick)
        sort_layout.addLayout(sort_row1)
        
        sort_row2 = QHBoxLayout()
        btn_stop = QPushButton("STOP")
        btn_stop.setStyleSheet(btn_style(COLOR_MATTE_RED))
        btn_stop.clicked.connect(self.stop_animation)
        sort_row2.addStretch()
        sort_row2.addWidget(btn_stop)
        sort_row2.addStretch()
        sort_layout.addLayout(sort_row2)
        
        frame_sort_container.addWidget(frame_sort)
        control_layout.addLayout(frame_sort_container)
        control_layout.addStretch()
        
        # ---------- FRAME 5: ALEATOIRE (Cyan) ----------
        frame_random_container = QVBoxLayout()
        frame_random_container.setSpacing(5)
        
        lbl_random = QLabel("ALEATOIRE")
        lbl_random.setStyleSheet(f"color: {COLOR_MATTE_CYAN}; font-size: 11px; font-weight: bold;")
        lbl_random.setAlignment(Qt.AlignCenter)
        frame_random_container.addWidget(lbl_random)
        
        frame_random = QFrame()
        frame_random.setStyleSheet(frame_style(COLOR_MATTE_CYAN))
        random_layout = QVBoxLayout(frame_random)
        random_layout.setContentsMargins(15, 18, 15, 18)
        random_layout.setSpacing(8)
        
        random_row1 = QHBoxLayout()
        random_row1.setSpacing(8)
        btn_gen = QPushButton("Generer")
        btn_gen.setStyleSheet(btn_style(COLOR_MATTE_CYAN))
        btn_gen.clicked.connect(self.generate_random_list)
        
        self.combo_type = QComboBox()
        self.combo_type.addItems(["Int", "Float", "Char", "Str"])
        self.combo_type.setFixedWidth(60)
        self.combo_type.setStyleSheet(f"""
            QComboBox {{
                background-color: #161b22;
                color: #c9d1d9;
                border: 1px solid #30363d;
                border-radius: 4px;
                padding: 5px;
                font-size: 11px;
            }}
            QComboBox:hover {{
                border: 2px solid {COLOR_MATTE_CYAN};
            }}
            QComboBox::drop-down {{ border: none; width: 15px; }}
            QComboBox QAbstractItemView {{
                background: {COLOR_PANEL.name()};
                color: {COLOR_TEXT.name()};
                selection-background-color: {COLOR_MATTE_CYAN};
            }}
        """)
        random_row1.addWidget(btn_gen)
        random_row1.addWidget(self.combo_type)
        random_layout.addLayout(random_row1)
        
        random_row2 = QHBoxLayout()
        random_row2.setSpacing(8)
        lbl_taille = QLabel("Taille:")
        lbl_taille.setStyleSheet(f"color: #8B949E; font-size: 10px; border: none;")
        self.input_gen_size = QLineEdit("")
        self.input_gen_size.setPlaceholderText("20")
        self.input_gen_size.setFixedWidth(60)
        self.input_gen_size.setStyleSheet(input_style(COLOR_MATTE_CYAN))
        random_row2.addWidget(lbl_taille)
        random_row2.addWidget(self.input_gen_size)
        random_row2.addStretch()
        random_layout.addLayout(random_row2)
        
        frame_random_container.addWidget(frame_random)
        control_layout.addLayout(frame_random_container)
        control_layout.addStretch()
        
        btn_vider = QPushButton("Vider")
        btn_vider.setFixedSize(70, 35)
        btn_vider.setStyleSheet(btn_style(COLOR_MATTE_RED))
        btn_vider.clicked.connect(self.reset_list)
        control_layout.addWidget(btn_vider)
        control_layout.addStretch()
        
        self.layout_visualizer.addWidget(control_bar)
        
        # ==================== VALUE INPUT (Noeud Nouveau) ====================
        input_bar = QWidget()
        input_bar.setFixedHeight(70)
        input_bar.setStyleSheet("background: transparent;")
        input_layout = QHBoxLayout(input_bar)
        input_layout.setContentsMargins(20, 5, 20, 5)
        
        input_layout.addStretch()
        
        lbl_nouveau = QLabel("NOUVEAU")
        lbl_nouveau.setStyleSheet(f"color: {COLOR_ACCENT.name()}; font-size: 10px; font-weight: bold;")
        input_layout.addWidget(lbl_nouveau)
        
        # Visual Node Input Container
        self.input_node_container = QFrame()
        self.input_node_container.setFixedHeight(50)
        self.input_node_container.setStyleSheet("background: transparent; border: none;")
        self.input_node_layout = QHBoxLayout(self.input_node_container)
        self.input_node_layout.setContentsMargins(0, 0, 0, 0)
        self.input_node_layout.setSpacing(0)
        
        # Create node-style input (will be updated based on list type)
        self._create_visual_input_node()
        
        input_layout.addWidget(self.input_node_container)
        
        # Manual Button (Right of Nouveau) - GREEN for visibility
        btn_manual = QPushButton("Manuel")
        btn_manual.setCursor(Qt.PointingHandCursor)
        btn_manual.setFixedSize(80, 45) # Match input height
        btn_manual.setStyleSheet("""
            QPushButton {
                background-color: #238636;
                color: white;
                border: none;
                border-radius: 6px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #2ea043;
            }
        """)
        btn_manual.setFont(QFont(FONT_UI, 11, QFont.Bold))
        btn_manual.clicked.connect(self.open_manual_dialog)
        input_layout.addWidget(btn_manual)
        
        input_layout.addStretch()
        
        # Speed Slider (on the right side)
        lbl_speed = QLabel("Vitesse:")
        lbl_speed.setStyleSheet(f"color: {COLOR_TEXT.name()}; font-size: 9px;")
        input_layout.addWidget(lbl_speed)
        
        self.slider_speed = QSlider(Qt.Horizontal)
        self.slider_speed.setRange(50, 2000)
        self.slider_speed.setValue(600)
        self.slider_speed.setInvertedAppearance(True)
        self.slider_speed.setFixedWidth(80)
        self.slider_speed.setStyleSheet(f"""
            QSlider::groove:horizontal {{
                background: rgba(48, 54, 61, 0.5);
                height: 6px;
                border-radius: 3px;
            }}
            QSlider::handle:horizontal {{
                background: {COLOR_ACCENT_2.name()};
                width: 14px;
                margin: -4px 0;
                border-radius: 7px;
            }}
        """)
        self.slider_speed.valueChanged.connect(lambda v: self.timer.setInterval(v))
        input_layout.addWidget(self.slider_speed)
        
        # Retour button
        btn_back = QPushButton("Retour")
        btn_back.setStyleSheet(f"""
            QPushButton {{
                background: transparent;
                color: rgba(200, 200, 220, 0.7);
                border: 1px solid rgba(88, 166, 255, 0.3);
                border-radius: 4px;
                padding: 8px 15px;
                font-size: 10px;
                font-weight: bold;
            }}
            QPushButton:hover {{
                background: rgba(88, 166, 255, 0.1);
                color: white;
            }}
        """)
        btn_back.clicked.connect(self.go_back_menu)
        input_layout.addWidget(btn_back)
        
        self.layout_visualizer.addWidget(input_bar)
        
        # ==================== GRAPHICS VIEW ====================
        # ==================== GRAPHICS VIEW ====================
        self.scene = QGraphicsScene()
        # Fix the scene rect to prevent expansion from off-screen animation items
        self.scene.setSceneRect(0, 0, 1300, 5000)
        
        self.view = ListGraphicsView(self.scene)
        
        # Map Mode configuration (Scrollbars & Alignment)
        self.view.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        self.view.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.view.setAlignment(Qt.AlignLeft | Qt.AlignTop)
        
        self.layout_visualizer.addWidget(self.view)
    
    def _create_visual_input_node(self):
        """Create a visual input that looks like a node (Simple or Double)"""
        # Clear existing widgets
        while self.input_node_layout.count():
            item = self.input_node_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
        
        # Colors
        border_color = COLOR_ACCENT.name() if not self.is_double else COLOR_ACCENT_2.name()
        ptr_color = "#FF6B35"  # Orange for next pointer
        prev_color = "#58A6FF"  # Blue for prev pointer
        
        if self.is_double:
            # Double node: [prev] [data] [next]
            # Prev pointer zone
            prev_zone = QFrame()
            prev_zone.setFixedSize(30, 45)
            prev_zone.setStyleSheet(f"""
                background: rgba(88, 166, 255, 0.3);
                border: 2px solid {prev_color};
                border-radius: 4px 0 0 4px;
                border-right: none;
            """)
            self.input_node_layout.addWidget(prev_zone)
            
            # Data zone (input)
            self.input_value = QLineEdit("")
            self.input_value.setFixedSize(100, 45)
            self.input_value.setAlignment(Qt.AlignCenter)
            self.input_value.setPlaceholderText("Valeur")
            self.input_value.setStyleSheet(f"""
                QLineEdit {{
                    background: rgba(22, 27, 34, 0.95);
                    color: {COLOR_TEXT.name()};
                    border-top: 2px solid {border_color};
                    border-bottom: 2px solid {border_color};
                    border-left: none;
                    border-right: none;
                    border-radius: 0;
                    padding: 8px;
                    font-family: {FONT_CODE};
                    font-size: 13px;
                    font-weight: bold;
                }}
            """)
            self.input_node_layout.addWidget(self.input_value)
            
            # Next pointer zone
            next_zone = QFrame()
            next_zone.setFixedSize(30, 45)
            next_zone.setStyleSheet(f"""
                background: rgba(255, 107, 53, 0.3);
                border: 2px solid {ptr_color};
                border-radius: 0 4px 4px 0;
                border-left: none;
            """)
            self.input_node_layout.addWidget(next_zone)
            
            self.input_node_container.setFixedWidth(165)
        else:
            # Simple node: [data] [next]
            # Data zone (input)
            self.input_value = QLineEdit("")
            self.input_value.setFixedSize(100, 45)
            self.input_value.setAlignment(Qt.AlignCenter)
            self.input_value.setPlaceholderText("Valeur")
            self.input_value.setStyleSheet(f"""
                QLineEdit {{
                    background: rgba(22, 27, 34, 0.95);
                    color: {COLOR_TEXT.name()};
                    border: 2px solid {border_color};
                    border-radius: 4px 0 0 4px;
                    border-right: none;
                    padding: 8px;
                    font-family: {FONT_CODE};
                    font-size: 13px;
                    font-weight: bold;
                }}
            """)
            self.input_node_layout.addWidget(self.input_value)
            
            # Next pointer zone
            next_zone = QFrame()
            next_zone.setFixedSize(35, 45)
            next_zone.setStyleSheet(f"""
                background: rgba(255, 107, 53, 0.3);
                border: 2px solid {ptr_color};
                border-radius: 0 4px 4px 0;
                border-left: none;
            """)
            self.input_node_layout.addWidget(next_zone)
            
            self.input_node_container.setFixedWidth(140)
        
        # Create hidden input_idx (used by animations)
        self.input_idx = QLineEdit("0")
        self.input_idx.setVisible(False)
    
    def switch_list_type(self, type_str):
        """Switch between Simple and Double list types without going back to selection"""
        if type_str == self.list_type:
            return
        self.list_type = type_str
        self.reset_list()
        
        if type_str == "Simple":
            self.logic_list = ListeSimple()
            self.is_double = False
        else:
            self.logic_list = ListeDouble()
            self.is_double = True
        
        # Recreate the visual input node for the new type
        self._create_visual_input_node()
        
        # Update title color based on type
        color = COLOR_ACCENT.name() if type_str == "Simple" else COLOR_ACCENT_2.name()
        self.title_label.setText(f"LISTE CHAINEE {type_str.upper()}")
        self.title_label.setStyleSheet(f"""
            color: {color};
            font-family: {FONT_UI};
            font-size: 20px;
            font-weight: bold;
            letter-spacing: 3px;
        """)
    
    def search_by_pos(self):
        """Highlight node at specific position"""
        try:
            pos = int(self.input_search_pos.text())
            if 0 <= pos < len(self.visual_nodes):
                # Reset all highlights
                for node in self.visual_nodes:
                    node.set_highlight(False)
                    node.set_color(None)
                # Highlight target
                self.visual_nodes[pos].set_highlight(True)
                self.visual_nodes[pos].set_color(COLOR_SUCCESS)
                # Auto-reset after 2 seconds
                QTimer.singleShot(2000, lambda: self.visual_nodes[pos].set_color(None) if pos < len(self.visual_nodes) else None)
        except:
            pass
    
    def stop_animation(self):
        """Stop current animation"""
        self.timer.stop()
        self.animation_generator = None
        # Reset all node colors
        for node in self.visual_nodes:
            node.set_highlight(False)
            node.set_color(None)
        # Hide pointers
        for ptr in self.pointers.values():
            ptr.setVisible(False)
    
    def anim_bubble_sort(self):
        """Bubble sort animation"""
        n = len(self.visual_nodes)
        for i in range(n):
            swapped = False
            for j in range(0, n - i - 1):
                # Highlight comparing nodes
                self.visual_nodes[j].set_color(COLOR_HIGHLIGHT)
                self.visual_nodes[j + 1].set_color(COLOR_HIGHLIGHT)
                self.focus_on_node(j)  # Camera follow
                yield
                
                # Get values for comparison
                try:
                    val_j = float(self.visual_nodes[j].value)
                    val_j1 = float(self.visual_nodes[j + 1].value)
                    should_swap = val_j > val_j1
                except:
                    should_swap = str(self.visual_nodes[j].value) > str(self.visual_nodes[j + 1].value)
                
                if should_swap:
                    # Swap colors to red during swap
                    self.visual_nodes[j].set_color(COLOR_DANGER)
                    self.visual_nodes[j + 1].set_color(COLOR_DANGER)
                    yield
                    
                    # Swap values
                    temp = self.visual_nodes[j].value
                    self.visual_nodes[j].set_value(self.visual_nodes[j + 1].value)
                    self.visual_nodes[j + 1].set_value(temp)
                    swapped = True
                    yield
                
                # Reset colors
                self.visual_nodes[j].set_color(None)
                self.visual_nodes[j + 1].set_color(None)
            
            # Mark sorted element
            if n - i - 1 < len(self.visual_nodes):
                self.visual_nodes[n - i - 1].set_color(COLOR_SUCCESS)
            
            if not swapped:
                break
        
        # Final - mark all as sorted
        for node in self.visual_nodes:
            node.set_color(COLOR_SUCCESS)
        yield
        
        # Reset after brief pause
        for node in self.visual_nodes:
            node.set_color(None)
        yield


    def wheelEvent(self, event):
        # Scroll only - no zoom
        # Let the default scroll behavior handle it
        delta = event.angleDelta().y()
        self.view.verticalScrollBar().setValue(
            self.view.verticalScrollBar().value() - delta
        )

    def setup_scene(self):
        # Scene is now empty initially - nodes will be added when generating list
        pass

    def init_pointers(self):
        self.pointers['HEAD'] = VisualPointer("HEAD")
        self.scene.addItem(self.pointers['HEAD'])
        self.pointers['HEAD'].setPos(-50, 0)
        self.pointers['HEAD'].setVisible(False)
        
        self.pointers['TAIL'] = VisualPointer("TAIL")
        self.scene.addItem(self.pointers['TAIL'])
        self.pointers['TAIL'].setPos(-50, 0)
        self.pointers['TAIL'].setVisible(False)
        
        self.pointers['TEMP'] = VisualPointer("TEMP")
        self.scene.addItem(self.pointers['TEMP'])
        self.pointers['TEMP'].setVisible(False)
        
        self.pointers['PTR1'] = VisualPointer("P1", COLOR_HIGHLIGHT)
        self.scene.addItem(self.pointers['PTR1'])
        self.pointers['PTR1'].setVisible(False)
        
        self.pointers['PTR2'] = VisualPointer("P2", COLOR_HIGHLIGHT)
        self.scene.addItem(self.pointers['PTR2'])
        self.pointers['PTR2'].setVisible(False)

    # --- LOGIC & ANIMATION ---

    def log(self, message):
        pass # Stubbed out as Trace dock is removed

    def update_code(self, lines, active_line_index=-1):
        pass # Stubbed out as Code dock is removed
    
    def reset_list(self):
        if self.animation_generator: return
        self.logic_list.vider()
        for node in self.visual_nodes:
            self.scene.removeItem(node)
        self.visual_nodes.clear()
        self.update_arrows()
        for p in self.pointers.values(): p.setVisible(False)

    def generate_random_list(self):
        if self.animation_generator: return
        self.reset_list()
        
        try:
            size = int(self.input_gen_size.text())
        except:
            size = 10
            
        dtype = self.combo_type.currentText()
        
        # Optimize generation: Don't animate creation, just dump items
        for i in range(size):
            val = 0
            if dtype == "Int":
                val = random.randint(1000, 100000)
            elif dtype == "Float":
                val = round(random.uniform(1000.0, 100000.0), 2)
            elif dtype == "Str":
                val = "".join(random.choices("ABCDEFGHIJKLMNOPQRSTUVWXYZ", k=6))
            else: # Char
                val = chr(random.randint(65, 90))
                
            self.logic_list.insererFin(val)

            vnode = VisualNode(val, self.is_double, self)
            self.visual_nodes.append(vnode)
            self.scene.addItem(vnode)
        
        # Force initial position calculation immediately without animation
        self.rearrange_nodes(instant=True)
        
        # Force pointers update
        self.move_pointer_visual('HEAD', 0)
        self.move_pointer_visual('TAIL', len(self.visual_nodes) - 1)



    def edit_node_value(self, visual_node):
        """Handle click on node to edit value"""
        if self.animation_generator: return # No editing during animation
        
        # Find index
        try:
            index = self.visual_nodes.index(visual_node)
        except ValueError:
            return
            
        old_val = visual_node.value
        text, ok = QInputDialog.getText(self, "Modifier Valeur", "Nouvelle valeur:", text=old_val)
        
        if ok:
            # Update backend
            self.logic_list.modifier(index, text)
            # Update visual
            visual_node.set_value(text)

    def commit_edit(self, visual_node, new_val):
        """Update backend data from visual node edit"""
        # Validate Input
        if not self.validate_input(new_val, self.current_manual_type):
            QMessageBox.warning(self, "Erreur", f"Valeur invalide pour le type {self.current_manual_type}")
            # Reset visual value to old value? Or just ignore update.
            # VisualNode already updated local display, we should force it back?
            # VisualNode calls set_value AFTER this returns. 
            # Wait, VisualNode implementation:
            # if self.visualizer: self.visualizer.commit_edit(self, new_val)
            # self.set_value(new_val) (Always happens).
            # I must raise usage of validation in VisualNode or checking return.
            # But VisualNode is generic.
            # Correct approach: commit_edit raises exception or returns False?
            # Correct approach: commit_edit raises exception or returns False?
            return False
            
        try:
            index = self.visual_nodes.index(visual_node)
            self.logic_list.modifier(index, new_val)
            return True
        except ValueError:
            return False

    def validate_input(self, value, type_str):
        if not value: return True # Allow empty? Or strict? "nodes vides" means initially empty.
        
        try:
            if type_str == "Int":
                int(value)
            elif type_str == "Float":
                float(value)
            elif type_str == "Char":
                if len(value) != 1: return False
            # Str is always valid
        except ValueError:
            return False
        return True

    def open_manual_dialog(self):
        if self.animation_generator: return
        dialog = ManualCreationDialog(self)
        if dialog.exec():
            size, dtype = dialog.get_data()
            self.current_manual_type = dtype
            self.generate_empty_list_manual(size)

    def generate_empty_list_manual(self, size):
        self.reset_list()
        placeholder = ""
        for i in range(size):
            self.logic_list.insererFin(placeholder)
            vnode = VisualNode(placeholder, self.is_double, self)
            self.visual_nodes.append(vnode)
            self.scene.addItem(vnode)
        
        self.rearrange_nodes(instant=True)
        self.move_pointer_visual('HEAD', 0)
        self.move_pointer_visual('TAIL', len(self.visual_nodes) - 1)

    def start_animation(self, generator_func):
        if self.animation_generator: return
        self.animation_generator = generator_func()
        self.timer.start(self.timer_interval)

    def next_step(self):
        if not self.animation_generator:
            self.timer.stop()
            return
        try:
            next(self.animation_generator)
        except StopIteration:
            self.animation_generator = None
            self.timer.stop()
            self.rearrange_nodes()

    def get_node_pos(self, index):
        # GRID LAYOUT CALCULATION - LEFT ALIGNED
        col = index % GRID_COLS
        row = index // GRID_COLS
        
        # Left aligned with margin equal to right margin (Center in 1300px scene)
        # Grid width = 6*170 + 120 = 1140. Scene = 1300. Msg = (1300-1140)/2 = 80.
        start_x = 80  # Centered margin
        start_y = 150  # Top margin increased further for HEAD pointer / aesthetics
        
        x = start_x + col * GRID_SPACING_X
        y = start_y + row * GRID_SPACING_Y
        
        return QPointF(x, y)

    def rearrange_nodes(self, instant=False):
        # Only rearrange visual nodes that are currently in the list
        for i, node in enumerate(self.visual_nodes):
            target = self.get_node_pos(i)
            # Use direct move if list is huge or requested instant
            if instant or len(self.visual_nodes) > 100:
                node.setPos(target)
            elif node.pos() != target:
                node.pos_anim.setStartValue(node.pos())
                node.pos_anim.setEndValue(target)
                node.pos_anim.start()
        
        if instant:
            self.update_arrows()
        else:
            QTimer.singleShot(100, self.update_arrows)
            
        if self.visual_nodes:
            self.move_pointer_visual('HEAD', 0)
        else:
            self.pointers['HEAD'].setVisible(False)
        
        # Reset view to origin (left-top aligned) but keep some margin
        if self.visual_nodes and not self.animation_generator:
            # Only if not animating specific focused events
            pass

    def update_arrows(self):
        for arrow in self.visual_arrows: self.scene.removeItem(arrow)
        self.visual_arrows.clear()
        
        # Don't draw thousands of arrows if zoomed out massively? 
        # For now, draw all.
        for i in range(len(self.visual_nodes) - 1):
            arrow = VisualArrow(self.visual_nodes[i], self.visual_nodes[i+1], self.is_double)
            self.scene.addItem(arrow)
            self.visual_arrows.append(arrow)

    def move_pointer_visual(self, name, index):
        ptr = self.pointers.get(name)
        if not ptr: return
        if 0 <= index < len(self.visual_nodes):
            node = self.visual_nodes[index]
            target_pos = node.pos()
            final_pos = target_pos + QPointF(NODE_WIDTH/2, -50)
            ptr.pos_anim.setStartValue(ptr.pos())
            ptr.pos_anim.setEndValue(final_pos)
            ptr.pos_anim.start()
            ptr.setVisible(True)
            # Auto-scroll to follow the node
            self.focus_on_node(index)
        else:
            ptr.setVisible(False)
    
    def focus_on_node(self, index):
        """Scroll the view to center on the node at given index"""
        if 0 <= index < len(self.visual_nodes):
            node = self.visual_nodes[index]
            # Get scene position and ensure it's visible
            self.view.ensureVisible(node, 100, 100)
            # Optionally center on the node for better visibility
            # self.view.centerOn(node)

    # --- ANIMATORS ---
    
    def anim_insert_head(self):
        val = self.input_value.text()
        val = self.input_value.text()
        vnode = VisualNode(val, self.is_double, self)
        vnode.setPos(-700, -150)
        self.scene.addItem(vnode)
        yield
        
        self.logic_list.insererDebut(val)
        self.visual_nodes.insert(0, vnode)
        self.move_pointer_visual('HEAD', 0)
        self.rearrange_nodes()
        yield

    def anim_insert_tail(self):
        val = self.input_value.text()
        val = self.input_value.text()
        vnode = VisualNode(val, self.is_double, self)
        vnode.setPos(0, 150)
        self.scene.addItem(vnode)
        yield
        self.logic_list.insererFin(val)
        self.visual_nodes.append(vnode)
        self.rearrange_nodes()
        yield

    def anim_insert_pos(self):
        try:
            pos = int(self.input_pos_insert.text()) # Fix: Use specific input
        except: return
        if pos <= 0: yield from self.anim_insert_head(); return
        if pos >= len(self.visual_nodes): yield from self.anim_insert_tail(); return
        
        val = self.input_value.text()
        val = self.input_value.text()
        vnode = VisualNode(val, self.is_double, self)
        vnode.setPos(0, -200)
        self.scene.addItem(vnode)
        yield
        
        # Traverse
        self.move_pointer_visual('TEMP', 0)
        for i in range(pos-1):
            self.move_pointer_visual('TEMP', i)
            yield
        
        self.logic_list.insererPosition(val, pos)
        self.visual_nodes.insert(pos, vnode)
        self.pointers['TEMP'].setVisible(False)
        self.rearrange_nodes()
        yield

    def anim_delete_head(self):
        if not self.visual_nodes: return
        node = self.visual_nodes.pop(0)
        self.logic_list.supprimerDebut()
        self.scene.removeItem(node)
        self.rearrange_nodes()
        yield

    def anim_delete_tail(self):
        if not self.visual_nodes: return
        node = self.visual_nodes.pop()
        self.logic_list.supprimerFin()
        self.scene.removeItem(node)
        self.rearrange_nodes()
        yield
        
    def anim_delete_pos(self):
        try:
            pos = int(self.input_pos_delete.text()) # Fix: Use specific input
        except: return
        if pos < 0 or pos >= len(self.visual_nodes): return
        
        self.move_pointer_visual('TEMP', 0)
        for i in range(pos):
            self.move_pointer_visual('TEMP', i)
            yield
            
        node = self.visual_nodes.pop(pos)
        self.logic_list.supprimerPosition(pos)
        self.scene.removeItem(node)
        self.pointers['TEMP'].setVisible(False)
        self.rearrange_nodes()
        yield
        
    def anim_delete_val(self):
        val = self.input_val_delete.text() # Fix: Use specific input
        self.move_pointer_visual('TEMP', 0)
        idx = 0
        found = False
        while idx < len(self.visual_nodes):
            self.move_pointer_visual('TEMP', idx)
            if self.visual_nodes[idx].value == val:
                found = True
                break
            idx += 1
            yield
            
        if found:
            node = self.visual_nodes.pop(idx)
            self.logic_list.supprimerValeur(val) # Logic matches visual index
            self.scene.removeItem(node)
        else:
            pass
            
        self.pointers['TEMP'].setVisible(False)
        self.rearrange_nodes()
        yield

    def anim_search(self):
        val = self.input_search_val.text() # Fix: Use specific input, not main 'Nouveau' input
        self.move_pointer_visual('TEMP', 0)
        idx = 0
        while idx < len(self.visual_nodes):
            self.move_pointer_visual('TEMP', idx)
            self.visual_nodes[idx].set_highlight(True)
            self.focus_on_node(idx)  # Camera follow
            yield
            # Robust Comparison
            node_val = self.visual_nodes[idx].value.strip()
            search_val = val.strip()
            
            match = False
            if node_val == search_val:
                match = True
            else:
                # Try numeric comparison (handles "20" == "20.0")
                try:
                    if float(node_val) == float(search_val):
                        match = True
                except ValueError:
                    pass
            
            if match:
                self.visual_nodes[idx].set_color(COLOR_SUCCESS)
                yield
                self.visual_nodes[idx].set_color(None)
                break
            self.visual_nodes[idx].set_highlight(False)
            idx += 1
        self.pointers['TEMP'].setVisible(False)
        yield
        
    def anim_reverse(self):
        # Visual Swap
        left = 0
        right = len(self.visual_nodes) - 1
        self.pointers['PTR1'].setVisible(True)
        self.pointers['PTR2'].setVisible(True)
        while left < right:
            self.move_pointer_visual('PTR1', left)
            self.move_pointer_visual('PTR2', right)
            yield
            # Swap values
            v1 = self.visual_nodes[left].value
            v2 = self.visual_nodes[right].value
            self.visual_nodes[left].set_value(v2)
            self.visual_nodes[right].set_value(v1)
            left += 1
            right -= 1
            yield
        self.pointers['PTR1'].setVisible(False)
        self.pointers['PTR2'].setVisible(False)
        yield

    def anim_quicksort(self):
        # QuickSort Visual Implementation
        # We will operate on the visual_nodes array directly and swap values
        
        def partition(low, high):
            pivot_node = self.visual_nodes[high]
            pivot_val = float(pivot_node.value) if pivot_node.value.replace('.','',1).isdigit() else pivot_node.value
            
            pivot_node.set_color(COLOR_ACCENT) # Pivot is Orange
            
            i = low - 1
            
            for j in range(low, high):
                node_j = self.visual_nodes[j]
                val_j = float(node_j.value) if node_j.value.replace('.','',1).isdigit() else node_j.value
                
                # Highlight comparison
                self.move_pointer_visual('PTR1', j)
                yield
                
                compare = (val_j < pivot_val) if isinstance(pivot_val, (int, float)) else (str(val_j) < str(pivot_val))
                
                if compare:
                    i += 1
                    # Swap visual_nodes[i] and visual_nodes[j] values
                    if i != j:
                        node_i = self.visual_nodes[i]
                        
                        node_i.set_color(COLOR_DANGER)
                        node_j.set_color(COLOR_DANGER)
                        yield
                        
                        temp_val = node_i.value
                        node_i.set_value(node_j.value)
                        node_j.set_value(temp_val)
                        yield
                        
                        node_i.set_color(None)
                        node_j.set_color(None)
            
            # Swap pivot to correct position
            if i + 1 != high:
                node_i_next = self.visual_nodes[i+1]
                pivot_node.set_color(COLOR_DANGER)
                node_i_next.set_color(COLOR_DANGER)
                yield
                
                temp_val = node_i_next.value
                node_i_next.set_value(pivot_node.value)
                pivot_node.set_value(temp_val)
                yield
                
                node_i_next.set_color(None)
                
            pivot_node.set_color(None)
            return i + 1

        # Iterative QuickSort (simulated recursive generator)
        # Using a stack to avoid recursion depth issues in generator
        stack = [(0, len(self.visual_nodes) - 1)]
        
        while stack:
            low, high = stack.pop()
            if low < high:
                # Need to yield from partition generator
                # But python generators don't return values easily in 3.9 for assignment
                # So we inline partition logic or wrap it carefully.
                # Let's wrap logic:
                
                # PARTITION LOGIC INLINE
                pivot_node = self.visual_nodes[high]
                try:
                    pivot_val = float(pivot_node.value)
                except:
                    pivot_val = pivot_node.value
                
                pivot_node.set_color(COLOR_ACCENT)
                i = low - 1
                
                for j in range(low, high):
                    self.move_pointer_visual('PTR1', j)
                    # yield # Fast sort? Uncomment for slower
                    
                    node_j = self.visual_nodes[j]
                    try:
                        val_j = float(node_j.value)
                    except:
                        val_j = node_j.value
                    
                    # Safe Compare
                    try:
                        if val_j < pivot_val:
                            i += 1
                            if i != j:
                                node_i = self.visual_nodes[i]
                                temp = node_i.value
                                node_i.set_value(node_j.value)
                                node_j.set_value(temp)
                                yield
                    except: pass
                
                # Swap pivot
                node_pivot_final = self.visual_nodes[i+1]
                temp = node_pivot_final.value
                node_pivot_final.set_value(pivot_node.value)
                pivot_node.set_value(temp)
                pivot_node.set_color(None)
                yield
                
                pi = i + 1
                # END PARTITION
                
                stack.append((low, pi - 1))
                stack.append((pi + 1, high))
        
        self.pointers['PTR1'].setVisible(False)
        
        # SYNC FIX: Rebuild logic list to match visual order
        self.logic_list.vider()
        if self.list_type == "Simple":
            for node in self.visual_nodes:
                self.logic_list.insererFin(node.value) # Note: value is str
        else:
            for node in self.visual_nodes:
                self.logic_list.insererFin(node.value)
                
        yield


# ==================== FONCTIONS PUBLIQUES ====================

def InitModuleListes():
    return True

def ShowListesInterface(parent=None):
    # Workflow: Return the unified visualizer window
    # The Selection Screen is now Page 0 of the window itself.
    window = LinkedListVisualizer()
    window.showMaximized()
    return window

def CleanupModuleListes():
    pass

if __name__ == "__main__":
    app = QApplication(sys.argv)
    # Test workflow
    win = ShowListesInterface()
    sys.exit(app.exec())
