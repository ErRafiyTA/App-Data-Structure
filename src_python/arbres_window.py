import random
import collections
from PySide6.QtWidgets import (QMainWindow, QGraphicsView, QGraphicsScene,
                               QGraphicsEllipseItem, QGraphicsLineItem, QGraphicsItem,
                               QWidget, QVBoxLayout, QPushButton, QLabel,
                               QSpinBox, QComboBox, QInputDialog, QDockWidget, QFormLayout,
                               QGraphicsSimpleTextItem, QMessageBox, QGroupBox, QRadioButton,
                               QSlider, QHBoxLayout, QDialog, QLineEdit)
from PySide6.QtCore import Qt, QPointF, QRectF, QTimer, QPoint
from PySide6.QtGui import QPen, QBrush, QColor, QFont, QPainter, QTransform, QLinearGradient, QCursor


# === BARRE DE TITRE PERSONNALISÉE ===
class SimpleTitleBar(QWidget):
    """Barre de titre sombre avec boutons stylisés"""
    
    def __init__(self, parent, title=""):
        super().__init__(parent)
        self.parent_window = parent
        self.setFixedHeight(40)
        self.setStyleSheet("background-color: #0D1117;")
        
        self.dragging = False
        self.drag_position = QPoint()
        
        layout = QHBoxLayout(self)
        layout.setContentsMargins(15, 0, 10, 0)
        layout.setSpacing(0)
        
        self.title_label = QLabel(title)
        self.title_label.setStyleSheet("""
            color: #FF6B35;
            font-family: 'Segoe UI';
            font-size: 12px;
            font-weight: bold;
            letter-spacing: 2px;
        """)
        layout.addWidget(self.title_label)
        layout.addStretch()
        
        btn_style = """
            QPushButton { background-color: transparent; color: #8B949E; border: none;
                font-family: 'Segoe UI'; font-size: 16px; font-weight: bold; padding: 8px 15px; }
            QPushButton:hover { color: #FFFFFF; background-color: #21262D; }
        """
        
        self.btn_minimize = QPushButton("─")
        self.btn_minimize.setStyleSheet(btn_style)
        self.btn_minimize.setFixedSize(45, 40)
        self.btn_minimize.setCursor(QCursor(Qt.PointingHandCursor))
        self.btn_minimize.clicked.connect(parent.showMinimized)
        layout.addWidget(self.btn_minimize)
        
        self.btn_maximize = QPushButton("□")
        self.btn_maximize.setStyleSheet(btn_style)
        self.btn_maximize.setFixedSize(45, 40)
        self.btn_maximize.setCursor(QCursor(Qt.PointingHandCursor))
        self.btn_maximize.clicked.connect(self.toggle_maximize)
        layout.addWidget(self.btn_maximize)
        
        close_style = """
            QPushButton { background-color: transparent; color: #8B949E; border: none;
                font-family: 'Segoe UI'; font-size: 16px; font-weight: bold; padding: 8px 15px; }
            QPushButton:hover { color: #FFFFFF; background-color: #cf222e; }
        """
        self.btn_close = QPushButton("✕")
        self.btn_close.setStyleSheet(close_style)
        self.btn_close.setFixedSize(45, 40)
        self.btn_close.setCursor(QCursor(Qt.PointingHandCursor))
        self.btn_close.clicked.connect(parent.close)
        layout.addWidget(self.btn_close)
    
    def toggle_maximize(self):
        if self.parent_window.isMaximized():
            self.parent_window.showNormal()
            self.btn_maximize.setText("□")
        else:
            self.parent_window.showMaximized()
            self.btn_maximize.setText("❐")
    
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.dragging = True
            self.drag_position = event.globalPosition().toPoint() - self.parent_window.frameGeometry().topLeft()
            event.accept()
    
    def mouseMoveEvent(self, event):
        if self.dragging:
            self.parent_window.move(event.globalPosition().toPoint() - self.drag_position)
            event.accept()
    
    def mouseReleaseEvent(self, event):
        self.dragging = False
    
    def mouseDoubleClickEvent(self, event):
        self.toggle_maximize()


# Configuration Constants
NODE_RADIUS = 25
NODE_DIAMETER = NODE_RADIUS * 2
H_SPACING = 40
V_SPACING = 120 

# Cyberpunk Palette
COLOR_BG_START = QColor("#05080f")
COLOR_BG_END = QColor("#0d1117")
COLOR_GRID = QColor("#141e32")
COLOR_NODE_FILL = QColor(26, 31, 46, 200)
COLOR_NODE_BORDER = QColor("#FF6B35")
COLOR_TEXT = QColor("#FFFFFF")
COLOR_EDGE = QColor("#58A6FF")
COLOR_VISITING = QColor("#FF6B35")
COLOR_VISITED = QColor("#58A6FF")
COLOR_HOVER = QColor("#2a3b55") 

# Stylesheet
GLOBAL_STYLESHEET = """
QMainWindow {
    background-color: #0d1117;
}
QDockWidget {
    background-color: #0d1117;
    color: #FF6B35;
    border: 1px solid #141e32;
}
QDockWidget::title {
    background: #161b22;
    padding-left: 5px;
    color: #FF6B35;
}
QWidget {
    background-color: #0d1117;
    color: #c9d1d9;
    font-family: "Segoe UI";
}
QGroupBox {
    border: 1px solid #141e32;
    margin-top: 20px;
    font-weight: bold;
    color: #FF6B35;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 3px 0 3px;
}
QPushButton {
    background-color: #161b22;
    border: 1px solid #30363d;
    color: #c9d1d9;
    padding: 5px 15px;
    border-radius: 4px;
}
QPushButton:hover {
    border: 1px solid #FF6B35;
    color: #ffffff;
    background-color: #1f2428;
}
QPushButton:pressed {
    background-color: #0d1117;
}
QLabel {
    color: #8B949E;
}
QLineEdit, QSpinBox {
    background-color: #0d1117;
    border: 1px solid #30363d;
    color: #ffffff;
    padding: 4px;
    font-family: "Consolas";
}
QSlider::groove:horizontal {
    border: 1px solid #30363d;
    height: 8px;
    background: #0d1117;
    margin: 2px 0;
    border-radius: 4px;
}
QSlider::handle:horizontal {
    background: #FF6B35;
    border: 1px solid #FF6B35;
    width: 18px;
    height: 18px;
    margin: -7px 0;
    border-radius: 9px;
}
QRadioButton {
    color: #c9d1d9;
}
QRadioButton::indicator:checked {
    background-color: #FF6B35;
    border: 1px solid #FF6B35;
    border-radius: 6px;
}
QDialog {
    background-color: #0d1117;
}
"""

class Node:
    """
    Backend representation of a tree node.
    """
    def __init__(self, value):
        self.value = str(value)
        self.children = []
        
        # Layout attributes
        self.x = 0.0
        self.y = 0.0
        self.subtree_width = 0.0
        
        # Visual reference (set during drawing)
        self.visual_item = None

    def add_child(self, child_node):
        # Ensure child_node is not None (or handle it if intended)
        if child_node:
            self.children.append(child_node)
        else:
            # If logic requires None for spacing, append it.
            # But most visual logic filters None.
            # Binary logic expects explicit [left, right] which can be None.
            pass

    def set_children(self, children_list):
        self.children = children_list
        
    def __repr__(self):
        return f"Node({self.value})"

class LayoutEngine:
    """
    Static class to handle the calculation of X, Y coordinates for the tree.
    """
    @staticmethod
    def calculate_layout(root):
        if not root:
            return
        LayoutEngine._calculate_subtree_width(root)
        LayoutEngine._assign_coordinates(root, 0, 0)

    @staticmethod
    def _calculate_subtree_width(node):
        if not node.children:
            node.subtree_width = NODE_DIAMETER
            return node.subtree_width

        total_width = 0
        for i, child in enumerate(node.children):
            if child is None:
                # Reserve space for ghost node
                total_width += NODE_DIAMETER
            else:
                total_width += LayoutEngine._calculate_subtree_width(child)
            
            if i < len(node.children) - 1:
                total_width += H_SPACING
        
        node.subtree_width = max(NODE_DIAMETER, total_width)
        return node.subtree_width

    @staticmethod
    def _assign_coordinates(node, x_start, depth):
        node.y = depth * V_SPACING

        if not node.children:
            node.x = x_start + node.subtree_width / 2
        else:
            current_x = x_start
            
            first_child_center = None
            last_child_center = None
            
            for i, child in enumerate(node.children):
                width = 0
                center_of_child = 0
                
                if child is None:
                    width = NODE_DIAMETER
                    center_of_child = current_x + width / 2
                else:
                    LayoutEngine._assign_coordinates(child, current_x, depth + 1)
                    width = child.subtree_width
                    center_of_child = child.x
                
                if i == 0:
                    first_child_center = center_of_child
                if i == len(node.children) - 1:
                    last_child_center = center_of_child
                
                current_x += width + H_SPACING
            
            # Center parent over the span of its children (including ghosts)
            if first_child_center is not None and last_child_center is not None:
                node.x = (first_child_center + last_child_center) / 2
            else:
                # Should not happen given logic, but fallback
                node.x = x_start + node.subtree_width / 2

# --- Graphics Classes ---

class VisualNode(QGraphicsEllipseItem):
    """
    Graphical representation of a Node.
    """
    def __init__(self, node, visualizer_window):
        super().__init__(-NODE_RADIUS, -NODE_RADIUS, NODE_DIAMETER, NODE_DIAMETER)
        self.node = node
        self.node.visual_item = self # Link back
        self.visualizer_window = visualizer_window
        
        self.current_fill = COLOR_NODE_FILL
        self.current_border = COLOR_NODE_BORDER
        
        self.setBrush(QBrush(self.current_fill)) 
        pen = QPen(self.current_border)
        pen.setWidth(2)
        self.setPen(pen)
        self.setZValue(1)
        
        self.text_item = QGraphicsSimpleTextItem(self.node.value, self)
        font = QFont("Consolas", 10, QFont.Bold)
        self.text_item.setFont(font)
        self.text_item.setBrush(QBrush(COLOR_TEXT))
        
        text_rect = self.text_item.boundingRect()
        self.text_item.setPos(-text_rect.width() / 2, -text_rect.height() / 2)
        
        self.setAcceptHoverEvents(True)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.visualizer_window.handle_node_click(self.node)
            event.accept()
        else:
            super().mousePressEvent(event)

    def hoverEnterEvent(self, event):
        if self.visualizer_window.animation_timer.isActive():
            return
        self.setBrush(QBrush(COLOR_HOVER))
        super().hoverEnterEvent(event)

    def hoverLeaveEvent(self, event):
        if self.visualizer_window.animation_timer.isActive():
            return
        self.setBrush(QBrush(self.current_fill))
        super().hoverLeaveEvent(event)

    def paint(self, painter, option, widget=None):
        painter.setRenderHint(QPainter.Antialiasing)
        super().paint(painter, option, widget)

    def set_color_visiting(self):
        self.current_fill = COLOR_VISITING
        self.setBrush(QBrush(self.current_fill))

    def set_color_visited(self):
        self.current_fill = COLOR_VISITED
        self.setBrush(QBrush(self.current_fill))

    def reset_color(self):
        self.current_fill = COLOR_NODE_FILL
        self.setBrush(QBrush(self.current_fill))

class VisualEdge(QGraphicsLineItem):
    def __init__(self, x1, y1, x2, y2):
        super().__init__(x1, y1, x2, y2)
        pen = QPen(COLOR_EDGE)
        pen.setWidth(2)
        self.setPen(pen)
        self.setZValue(0)

class TreeGraphicsView(QGraphicsView):
    """
    Custom GraphicsView to handle Zoom, Smart Panning and Cyberpunk Background.
    """
    def __init__(self, scene, visualizer_window=None, parent=None):
        super().__init__(scene, parent)
        self.visualizer_window = visualizer_window
        self.setRenderHint(QPainter.Antialiasing)
        self.setTransformationAnchor(QGraphicsView.AnchorUnderMouse)
        self.setResizeAnchor(QGraphicsView.AnchorUnderMouse)
        self.setDragMode(QGraphicsView.NoDrag) 
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

    def wheelEvent(self, event):
        zoom_in_factor = 1.15
        zoom_out_factor = 1 / zoom_in_factor

        if event.angleDelta().y() > 0:
            zoom_factor = zoom_in_factor
        else:
            zoom_factor = zoom_out_factor

        self.scale(zoom_factor, zoom_factor)



    def mouseReleaseEvent(self, event):
        super().mouseReleaseEvent(event)
        self.setDragMode(QGraphicsView.NoDrag)
    
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            item = self.itemAt(event.pos())
            if isinstance(item, VisualNode) or (item and item.parentItem() and isinstance(item.parentItem(), VisualNode)):
                self.setDragMode(QGraphicsView.NoDrag)
            else:
                self.setDragMode(QGraphicsView.ScrollHandDrag)
        super().mousePressEvent(event)

# --- Dialog for Binary Choice ---

class AddBinaryNodeDialog(QDialog):
    def __init__(self, parent=None, left_enabled=True, right_enabled=True):
        super().__init__(parent)
        self.setWindowTitle("Ajouter Nœud")
        self.choice = None
        self.value = None
        self.setMinimumWidth(300)
        
        layout = QVBoxLayout()
        
        lbl = QLabel("Valeur et Position :")
        layout.addWidget(lbl)
        
        # Use QLineEdit for generic input
        self.input_val = QLineEdit()
        self.input_val.setPlaceholderText("Ex: A, 42, Bob...")
        layout.addWidget(self.input_val)
        
        btn_layout = QHBoxLayout()
        self.btn_left = QPushButton("Gauche")
        self.btn_left.setEnabled(left_enabled)
        self.btn_left.clicked.connect(lambda: self.set_choice("left"))
        
        self.btn_right = QPushButton("Droite")
        self.btn_right.setEnabled(right_enabled)
        self.btn_right.clicked.connect(lambda: self.set_choice("right"))
        
        btn_layout.addWidget(self.btn_left)
        btn_layout.addWidget(self.btn_right)
        layout.addLayout(btn_layout)
        
        self.setLayout(layout)

    def set_choice(self, direction):
        if not self.input_val.text():
            QMessageBox.warning(self, "Erreur", "Veuillez entrer une valeur.")
            return
        self.choice = direction
        self.value = self.input_val.text()
        self.accept()

# --- Main Window ---

class ConfigArbresWindow(QWidget):
    """Fenêtre Arbres intégrée dans la fenêtre principale"""
    def __init__(self):
        super().__init__()
        
        # Appliquer le style
        self.setStyleSheet(GLOBAL_STYLESHEET)
        
        # Layout principal
        main_layout = QHBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)

        self.root = None
        self.is_binary = True 
        
        # Animation State
        self.animation_timer = QTimer()
        self.animation_timer.timeout.connect(self.play_step)
        self.current_generator = None
        self.visited_trace = []
        self.last_visited_node = None 
        
        
        # New Feature Flags
        self.is_delete_mode = False
        self.is_modify_mode = False # NEW
        self.search_target = None

        self._init_ui(main_layout)
        self.reset_tree()

    def _init_ui(self, main_layout):
        # Panneau de contrôle à gauche (CONFIGURATION)
        self.control_panel = QWidget()
        self.control_panel.setFixedWidth(260)
        self.control_panel.setStyleSheet("background-color: #161b22; border-right: 1px solid #30363d;")
        control_layout = QVBoxLayout(self.control_panel)
        main_layout.addWidget(self.control_panel)
        
        # Vue principale pour l'arbre (au centre) -> Prend TOUT l'espace
        self.scene = QGraphicsScene()
        self.view = TreeGraphicsView(self.scene, visualizer_window=self)
        main_layout.addWidget(self.view, 1)

        # Panneau d'outils à droite (ACTIONS) -> FLOTTANT
        self.tools_panel = QWidget(self.view) # Parent is view
        self.tools_panel.setFixedWidth(240)
        # Card Style
        self.tools_panel.setStyleSheet("""
            background-color: rgba(22, 27, 34, 0.95); 
            border: 1px solid #30363d; 
            border-radius: 8px;
        """)
        # We don't add it to layout! It floats.
        
        tools_layout = QVBoxLayout(self.tools_panel)
        tools_layout.setContentsMargins(10, 10, 10, 10)
        tools_layout.setSpacing(10)

        # === LEFT PANEL (SETUP) ===

        # 1. Tree Type
        type_group = QGroupBox("STRUCTURE")
        type_layout = QVBoxLayout()
        self.rb_binary = QRadioButton("Arbre Binaire")
        self.rb_nary = QRadioButton("Arbre N-aire")
        self.rb_binary.setChecked(True)
        self.rb_binary.toggled.connect(self._update_tree_type)
        
        type_layout.addWidget(self.rb_binary)
        type_layout.addWidget(self.rb_nary)
        type_group.setLayout(type_layout)
        control_layout.addWidget(type_group)

        # 2. Random Gen
        gen_group = QGroupBox("GÉNÉRATION")
        gen_layout = QFormLayout()
        
        self.input_size = QLineEdit()
        self.input_size.setPlaceholderText("10")
        
        self.input_depth = QLineEdit()
        self.input_depth.setPlaceholderText("3")
        
        self.input_max_children = QLineEdit()
        self.input_max_children.setPlaceholderText("3")
        self.input_max_children.setEnabled(False) 

        self.btn_generate = QPushButton("GÉNÉRER")
        self.btn_generate.clicked.connect(self.generate_random_tree)
        self.btn_generate.setStyleSheet("background-color: #238636; color: white; font-weight: bold;")

        gen_layout.addRow("Taille :", self.input_size)
        gen_layout.addRow("Profondeur :", self.input_depth)
        gen_layout.addRow("Branchement :", self.input_max_children)
        
        # New Dropdown for Data Type
        self.combo_type = QComboBox()
        self.combo_type.addItems(["Int", "Float", "Char", "Str"])
        self.combo_type.setStyleSheet(f"""
            QComboBox {{
                background-color: #0d1117;
                color: #c9d1d9;
                border: 1px solid #30363d;
                padding: 4px;
            }}
            QComboBox QAbstractItemView {{
                background-color: #161b22;
                color: #c9d1d9;
                selection-background-color: #FF6B35;
            }}
        """)
        gen_layout.addRow("Type Données :", self.combo_type)
        
        gen_layout.addRow(self.btn_generate)
        gen_group.setLayout(gen_layout)
        control_layout.addWidget(gen_group)

        # 3. Manual Reset
        manual_group = QGroupBox("ÉDITION")
        manual_layout = QVBoxLayout()
        lbl_info = QLabel("Cliquer nœud pour ajouter.")
        lbl_info.setWordWrap(True)
        self.btn_reset = QPushButton("TOUT EFFACER")
        self.btn_reset.clicked.connect(self.reset_tree)
        self.btn_reset.setStyleSheet("color: #cf222e;")
        
        manual_layout.addWidget(lbl_info)
        manual_layout.addWidget(self.btn_reset)
        manual_group.setLayout(manual_layout)
        control_layout.addWidget(manual_group)

        # Helper to hide info label if needed
        self.lbl_manual_info = lbl_info
        
        # 4. Traversal Animations (Moved back to Left)
        anim_group = QGroupBox("ALGORITHMES")
        anim_layout = QVBoxLayout()
        
        self.btn_bfs = QPushButton("Largeur (BFS)")
        self.btn_bfs.clicked.connect(lambda: self.start_animation(self.algo_bfs))
        
        self.btn_pre = QPushButton("Pré-ordre (DFS)")
        self.btn_pre.clicked.connect(lambda: self.start_animation(self.algo_dfs_preorder))
        
        self.btn_in = QPushButton("In-ordre (DFS)")
        self.btn_in.clicked.connect(lambda: self.start_animation(self.algo_dfs_inorder))
        
        self.btn_post = QPushButton("Post-ordre (DFS)")
        self.btn_post.clicked.connect(lambda: self.start_animation(self.algo_dfs_postorder))
        
        # Speed Slider
        speed_layout = QHBoxLayout()
        self.slider_speed = QSlider(Qt.Horizontal)
        self.slider_speed.setRange(100, 2000)
        self.slider_speed.setValue(800)
        self.slider_speed.setInvertedAppearance(True)
        speed_layout.addWidget(QLabel("Speed"))
        speed_layout.addWidget(self.slider_speed)

        self.btn_stop = QPushButton("STOP")
        self.btn_stop.clicked.connect(self.stop_animation)
        self.btn_stop.setStyleSheet("border: 1px solid #cf222e; color: #cf222e;")

        anim_layout.addWidget(self.btn_bfs)
        anim_layout.addWidget(self.btn_pre)
        anim_layout.addWidget(self.btn_in)
        anim_layout.addWidget(self.btn_post)
        anim_layout.addLayout(speed_layout)
        anim_layout.addWidget(self.btn_stop)
        
        self.lbl_trace = QLabel("LOG: Système prêt.")
        self.lbl_trace.setWordWrap(True)
        self.lbl_trace.setStyleSheet("color: #8B949E; font-size: 10px; margin-top: 5px;")
        anim_layout.addWidget(self.lbl_trace)
        
        anim_group.setLayout(anim_layout)
        control_layout.addWidget(anim_group)
        
        control_layout.addStretch()

        # === RIGHT PANEL (TOOLS) ===
        
        # 4. Search Tool
        search_group = QGroupBox("RECHERCHE")
        search_layout = QHBoxLayout()
        self.input_search = QLineEdit()
        self.input_search.setPlaceholderText("Val...")
        self.btn_search = QPushButton("GO")
        self.btn_search.setFixedWidth(40)
        self.btn_search.clicked.connect(self.start_search)
        search_layout.addWidget(self.input_search)
        search_layout.addWidget(self.btn_search)
        search_group.setLayout(search_layout)
        tools_layout.addWidget(search_group)

        # 5. Operations Group (Delete / Convert / Modify)
        ops_group = QGroupBox("OPÉRATIONS")
        ops_layout = QVBoxLayout()
        
        self.btn_modify_mode = QPushButton("Modifier Valeur")
        self.btn_modify_mode.setCheckable(True)
        self.btn_modify_mode.setStyleSheet("QPushButton:checked { background-color: #D29922; color: white; }")
        self.btn_modify_mode.clicked.connect(self.toggle_modify_mode)
        
        self.btn_delete_mode = QPushButton("Mode Suppression")
        self.btn_delete_mode.setCheckable(True)
        self.btn_delete_mode.setStyleSheet("QPushButton:checked { background-color: #cf222e; color: white; }")
        self.btn_delete_mode.clicked.connect(self.toggle_delete_mode)
        
        self.btn_convert = QPushButton("Convertir -> Binaire")
        self.btn_convert.clicked.connect(self.convert_nary_to_binary)
        self.btn_convert.setEnabled(False) 

        self.btn_reorder = QPushButton("ORDONNER (BST)")
        self.btn_reorder.clicked.connect(self.reorder_tree_as_bst)
        self.btn_reorder.setStyleSheet("color: #D29922; border: 1px solid #D29922;")
        
        # Add buttons to layout
        ops_layout.addWidget(self.btn_modify_mode)
        ops_layout.addWidget(self.btn_delete_mode)
        ops_layout.addWidget(self.btn_convert)
        ops_layout.addWidget(self.btn_reorder)

        ops_group.setLayout(ops_layout)
        tools_layout.addWidget(ops_group)
        
        # Make tools panel fit content tight vertically
        tools_layout.addStretch() 
        
        # Initial sizing
        self.tools_panel.adjustSize()

    def resizeEvent(self, event):
        # Handle Floating Panel Positioning
        if hasattr(self, 'tools_panel') and hasattr(self, 'view'):
            padding = 20
            # Position: Top-Right of view
            # View coords in main window? View is widget in main_layout
            # But tools_panel is child of view. So coords relative to view.
            
            x = self.view.width() - self.tools_panel.width() - padding
            y = padding
            
            self.tools_panel.move(x, y)
            
        super().resizeEvent(event)

    def _update_tree_type(self):
        self.is_binary = self.rb_binary.isChecked()
        self.input_max_children.setEnabled(not self.is_binary)
        self.btn_in.setEnabled(self.is_binary)
        self.btn_convert.setEnabled(not self.is_binary)
        
        # Clear tree on switch (like Raylib)
        self.reset_tree()

    def reset_tree(self):
        self.stop_animation()
        self.root = Node("RACINE")  # Start with editable root
        self.root_needs_value = True  # Flag to show prompt
        if hasattr(self, 'lbl_manual_info'):
            self.lbl_manual_info.setVisible(True) # Show help text on reset
        self.redraw_scene()

    def generate_random_tree(self):
        try:
            self.stop_animation()
            
            # Parsing Inputs
            try:
                max_depth = int(self.input_depth.text()) if self.input_depth.text() else 3
            except: max_depth = 3
                
            try:
                max_nodes = int(self.input_size.text()) if self.input_size.text() else 10
            except: max_nodes = 10
            
            if self.is_binary:
                max_children = 2
            else:
                try:
                    max_children = int(self.input_max_children.text()) if self.input_max_children.text() else 3
                except: max_children = 3
            
            # Candidate Slot Algorithm for Exact Count
            # 1. Create Root
            dtype = "Int"
            if hasattr(self, 'combo_type'):
                dtype = self.combo_type.currentText()
                
            self.root = Node(self.generate_value(dtype))
            self.root_needs_value = False # FIX: Hide label after generation
            self.nodes_created = 1
            
            # 2. Candidate List: (node, depth)
            
            # 3. Fill until max_nodes reached
            # Maintain list of all available parent slots
            msg_queue = [] # List of (parent_node, current_depth)
            msg_queue.append((self.root, 1))
            
            # Optimization: If max_children is large, we might pick the same parent many times.
            # To ensure layout doesn't look like a stick, we prefer parents with fewer children?
            # Or just purely random (Queue method).
            
            failed_attempts = 0
            
            while self.nodes_created < max_nodes:
                if not msg_queue: break
                
                # Pick a random candidate parent
                idx = random.randrange(len(msg_queue))
                parent, p_depth = msg_queue[idx]
                
                # Check constraints
                if len(parent.children) >= max_children:
                    msg_queue.pop(idx)
                    continue
                
                if p_depth >= max_depth:
                    msg_queue.pop(idx)
                    continue
                    
                # Add child
                val = self.generate_value(dtype)
                child = Node(val)
                parent.add_child(child)
                self.nodes_created += 1
                
                # Add child to candidates for future growth
                msg_queue.append((child, p_depth + 1))
                
            self.redraw_scene()
            
        except Exception as e:
            QMessageBox.critical(self, "Erreur Génération", f"Erreur: {str(e)}")
            import traceback
            traceback.print_exc()

    def generate_value(self, dtype):
        if dtype == "Int":
            return str(random.randint(1, 99))
        elif dtype == "Float":
            return str(round(random.uniform(1.0, 99.0), 1))
        elif dtype == "Char":
            return chr(random.randint(65, 90))
        else: # Str
            return "".join(random.choices("ABCDEFGHIJKLMNOPQRSTUVWXYZ", k=3))
    
    def reorder_tree_as_bst(self):
        try:
            if not self.root: return
            
            # 1. Collect all values
            values = []
            stack = [self.root]
            while stack:
                n = stack.pop()
                if n.value: # Ignore empty nodes if any
                    values.append(n.value)
                for c in n.children:
                    if c: stack.append(c)
            
            # 2. Sort values using robust comparison (Handles Mixed Types without crash)
            def smart_key(v):
                # Returns tuple (type_priority, value)
                # Numbers (0) < Strings (1)
                try: 
                    return (0, float(v))
                except: 
                    return (1, str(v))
                
            values.sort(key=smart_key)
            
            # 3. Rebuild BST
            self.stop_animation()
            self.is_binary = True
            self.rb_binary.setChecked(True)
            self.input_max_children.setEnabled(False)
            
            self.root_needs_value = False 
            
            def build_bst(sorted_vals):
                if not sorted_vals: return None
                mid = len(sorted_vals) // 2
                node = Node(sorted_vals[mid])
                
                left = build_bst(sorted_vals[:mid])
                right = build_bst(sorted_vals[mid+1:])
                
                if left or right:
                    node.set_children([left, right])
                    
                return node
                
            self.root = build_bst(values)
            self.redraw_scene()
            QMessageBox.information(self, "Succès", "L'arbre a été réordonné en BST (Arbre Binaire de Recherche) équilibré.")
            
        except Exception as e:
            QMessageBox.critical(self, "Erreur Tri", f"Erreur lors du tri: {str(e)}")
            import traceback
            traceback.print_exc()

    def handle_node_click(self, node):
        if self.animation_timer.isActive():
            return 
        
        # INITIAL ROOT VALUE - when clicking RACINE for first time
        if getattr(self, 'root_needs_value', False) and node == self.root:
            text, ok = QInputDialog.getText(self, "Valeur Racine", "Entrez la valeur de la racine:")
            if ok and text:
                node.value = text
                self.root_needs_value = False
                self.redraw_scene()
            return
            
        # DELETE MODE
        if self.is_delete_mode:
            self.delete_node_subtree(node)
            return

        # MODIFY MODE
        if self.is_modify_mode:
            text, ok = QInputDialog.getText(self, "Modifier Valeur", "Nouvelle valeur:", text=node.value)
            if ok and text:
                node.value = text
                self.redraw_scene()
            return
            
        # ADD MODE (Default)
        if self.is_binary:
            has_left = len(node.children) > 0 and node.children[0] is not None
            has_right = len(node.children) > 1 and node.children[1] is not None
            
            if has_left and has_right:
                QMessageBox.warning(self, "Structure Saturée", "Nœud binaire complet.")
                return
                
            dlg = AddBinaryNodeDialog(self, left_enabled=not has_left, right_enabled=not has_right)
            if dlg.exec():
                new_node = Node(dlg.value)
                while len(node.children) < 2:
                    node.children.append(None)
                if dlg.choice == "left":
                    node.children[0] = new_node
                elif dlg.choice == "right":
                    node.children[1] = new_node
                self.redraw_scene()
        else:
            text, ok = QInputDialog.getText(self, "Injection de Donnée", "Valeur du nouveau fils:")
            if ok and text:
                new_node = Node(text)
                node.add_child(new_node)
                self.redraw_scene()

    def toggle_modify_mode(self):
        self.is_modify_mode = self.btn_modify_mode.isChecked()
        if self.is_modify_mode:
            self.btn_modify_mode.setText("Modifier: ON")
            # Turn off delete Logic
            if self.is_delete_mode:
                self.btn_delete_mode.setChecked(False)
                self.toggle_delete_mode()
            self.setCursor(Qt.IBeamCursor)
        else:
            self.btn_modify_mode.setText("Modifier Valeur")
            self.setCursor(Qt.ArrowCursor)

    def toggle_delete_mode(self):
        self.is_delete_mode = self.btn_delete_mode.isChecked()
        if self.is_delete_mode:
            self.btn_delete_mode.setText("Supprimer: ON")
            if self.is_modify_mode:
                self.btn_modify_mode.setChecked(False)
                self.toggle_modify_mode()
            self.setCursor(Qt.CrossCursor)
        else:
            self.btn_delete_mode.setText("Mode Suppression")
            self.setCursor(Qt.ArrowCursor)

    def delete_node_subtree(self, target_node):
        if target_node == self.root:
            self.reset_tree()
            self.toggle_delete_mode() # Auto-off
            self.btn_delete_mode.setChecked(False)
            return

        parent = self._find_parent(self.root, target_node)
        if parent:
            if self.is_binary:
                # Replace with None to keep structure
                for i, child in enumerate(parent.children):
                    if child == target_node:
                        parent.children[i] = None
                        break
                # Trim trailing Nones? Not strictly necessary but clean
                while parent.children and parent.children[-1] is None:
                    parent.children.pop()
            else:
                # N-ary: Remove from list
                if target_node in parent.children:
                    parent.children.remove(target_node)
            
            self.redraw_scene()

    def _find_parent(self, current, target):
        if not current: return None
        for child in current.children:
            if child == target:
                return current
            if child:
                res = self._find_parent(child, target)
                if res: return res
        return None

    def start_search(self):
        val = self.input_search.text()
        if not val: return
        self.start_animation(lambda root: self.algo_search_anim(root, val))

    def convert_nary_to_binary(self):
        if self.is_binary or not self.root:
            return
            
        # LCRS Algorithm
        # 1. Create purely new structure
        new_root = self._convert_node_lcrs(self.root)
        
        self.root = new_root
        
        # Switch UI
        self.rb_binary.blockSignals(True)
        self.rb_binary.setChecked(True)
        self.rb_binary.blockSignals(False)
        
        self.is_binary = True
        self.input_max_children.setEnabled(False)
        self.btn_in.setEnabled(True)
        self.btn_convert.setEnabled(False)
        
        self.redraw_scene()
        QMessageBox.information(self, "Conversion", "Conversion terminée (LCRS).")

    def _convert_node_lcrs(self, nary_node):
        """Recursively convert a node and its children to LCRS format."""
        if not nary_node: return None
        
        # Create the binary equivalent of current node
        bin_node = Node(nary_node.value)
        
        if not nary_node.children:
            return bin_node
            
        # Process children to form the Left Child -> Right Sibling chain
        
        # 1. First child becomes Left Child of bin_node
        first_child_nary = nary_node.children[0]
        left_child_bin = self._convert_node_lcrs(first_child_nary)
        
        # Ensure children array has space for Left [0]
        bin_node.children.append(left_child_bin) 
        
        # 2. Subsequent children become Right Siblings of the previous child
        current_sibling_bin = left_child_bin
        
        for i in range(1, len(nary_node.children)):
            next_child_nary = nary_node.children[i]
            next_sibling_bin = self._convert_node_lcrs(next_child_nary)
            
            # Ensure current sibling has space for Right [1]
            # Since it's a new binary node, children is empty or has left.
            # We need to set children[1].
            while len(current_sibling_bin.children) < 2:
                current_sibling_bin.children.append(None)
            
            current_sibling_bin.children[1] = next_sibling_bin
            current_sibling_bin = next_sibling_bin
            
        return bin_node

    def _convert_siblings(self, nary_children, binary_parent):
        # Deprecated / Replaced by _convert_node_lcrs recursion strategy
        pass

    def redraw_scene(self):
        self.scene.clear()
        if not self.root:
            # Draw clickable RACINE node with arrow
            from PySide6.QtWidgets import QGraphicsTextItem, QGraphicsEllipseItem, QGraphicsPolygonItem
            from PySide6.QtGui import QPolygonF
        if not self.root:
            return

        LayoutEngine.calculate_layout(self.root)
        self._draw_recursive(self.root)
        
        # If root needs value, add arrow and prompt
        if getattr(self, 'root_needs_value', False) and self.root:
            from PySide6.QtWidgets import QGraphicsTextItem, QGraphicsPolygonItem
            from PySide6.QtGui import QPolygonF
            
            root_x = self.root.x
            root_y = self.root.y
            
            # Arrow pointing down to node
            arrow = QPolygonF([
                QPointF(root_x, root_y - NODE_RADIUS - 15),  # Point (bottom)
                QPointF(root_x - 10, root_y - NODE_RADIUS - 30),  # Left
                QPointF(root_x + 10, root_y - NODE_RADIUS - 30)   # Right
            ])
            arrow_item = QGraphicsPolygonItem(arrow)
            arrow_item.setBrush(QBrush(COLOR_EDGE))
            arrow_item.setPen(QPen(COLOR_EDGE))
            self.scene.addItem(arrow_item)
            
            # Prompt text above arrow
            prompt = QGraphicsTextItem("Tapez pour commencer")
            prompt.setDefaultTextColor(COLOR_EDGE)
            font = QFont("Segoe UI", 11)
            prompt.setFont(font)
            prompt.setPos(root_x - prompt.boundingRect().width() / 2, root_y - NODE_RADIUS - 60)
            self.scene.addItem(prompt)
        
        self.view.setSceneRect(self.scene.itemsBoundingRect())
        r = self.scene.sceneRect()
        self.scene.setSceneRect(r.adjusted(-50, -50, 50, 50))
    
    def create_root_node(self):
        """Called when user wants to create root on empty tree"""
        if self.root:
            return
        text, ok = QInputDialog.getText(self, "Créer Racine", "Valeur de la racine:")
        if ok and text:
            self.root = Node(text)
            self.redraw_scene()

    def _draw_recursive(self, node):
        if node is None:
            return

        v_node = VisualNode(node, self)
        v_node.setPos(node.x, node.y)
        self.scene.addItem(v_node)

        for child in node.children:
            if child is None:
                continue
                
            edge = VisualEdge(node.x, node.y, child.x, child.y)
            self.scene.addItem(edge)
            self._draw_recursive(child)

    # --- Animation Logic ---

    def start_animation(self, algo_func):
        if not self.root:
            return
        
        self.stop_animation() 
        self.current_generator = algo_func(self.root)
        self.visited_trace = []
        self.lbl_trace.setText("LOG: Traitement en cours...")
        self.last_visited_node = None
        
        self.animation_timer.start(self.slider_speed.value())

    def stop_animation(self):
        self.animation_timer.stop()
        self.current_generator = None
        self.last_visited_node = None
        if self.root:
            self._reset_visuals_recursive(self.root)
        self.lbl_trace.setText("LOG: Système prêt.")

    def _reset_visuals_recursive(self, node):
        if node is None:
            return
        if node.visual_item:
            node.visual_item.reset_color()
        for child in node.children:
            self._reset_visuals_recursive(child)

    def play_step(self):
        self.animation_timer.setInterval(self.slider_speed.value())
        
        try:
            event = next(self.current_generator)
            node, state = event
            
            if state == 'visiting':
                if node.visual_item:
                    node.visual_item.set_color_visiting()
                self.visited_trace.append(node.value)
                self.lbl_trace.setText("LOG: " + " -> ".join(self.visited_trace))
            elif state == 'finished':
                if node.visual_item:
                    node.visual_item.set_color_visited()
                    
        except StopIteration:
            self.animation_timer.stop()
            trace_str = " -> ".join(self.visited_trace)
            QMessageBox.information(self, "Analyse Terminée", f"Séquence :\n{trace_str}")

    # --- Algorithms (Generators) ---
    
    def algo_bfs(self, root):
        queue = collections.deque([root])
        visited = set()
        visited.add(root)
        
        while queue:
            node = queue.popleft()
            yield (node, 'visiting')
            
            for child in node.children:
                if child and child not in visited:
                    visited.add(child)
                    queue.append(child)
            
            yield (node, 'finished')

    def algo_dfs_preorder(self, node):
        yield (node, 'visiting')
        for child in node.children:
            if child:
                yield from self.algo_dfs_preorder(child)
        yield (node, 'finished')

    def algo_dfs_postorder(self, node):
        for child in node.children:
            if child:
                yield from self.algo_dfs_postorder(child)
        
        yield (node, 'visiting') 
        yield (node, 'finished')

    def algo_dfs_inorder(self, node):
        if not node.children:
            yield (node, 'visiting')
            yield (node, 'finished')
            return

        left = node.children[0] if len(node.children) > 0 else None
        right = node.children[1] if len(node.children) > 1 else None
        
        if left:
            yield from self.algo_dfs_inorder(left)
        
        yield (node, 'visiting')
        
        if right:
            yield from self.algo_dfs_inorder(right)
            
        yield (node, 'finished')

    def algo_search_anim(self, root, target_val):
        """Animation de recherche"""
        queue = collections.deque([root])
        
        while queue:
            node = queue.popleft()
            yield (node, 'visiting')
            
            if node.value == target_val:
                # Found! highlight and stop
                yield (node, 'finished')
                QMessageBox.information(self, "Succès", f"Valeur '{target_val}' trouvée !")
                return
            
            # Add children to search
            for child in node.children:
                if child:
                    queue.append(child)
                    
            yield (node, 'finished')
            
        QMessageBox.warning(self, "Échec", f"Valeur '{target_val}' non trouvée.")


if __name__ == "__main__":
    import sys
    from PySide6.QtWidgets import QApplication
    app = QApplication(sys.argv)
    window = ConfigArbresWindow()
    window.show()
    sys.exit(app.exec())
