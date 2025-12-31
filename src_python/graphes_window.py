# -*- coding: utf-8 -*-
"""
MODULE GRAPHES - Interface PySide6
Visualisation et manipulation de graphes
"""

import random
import collections
import math
from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
                               QSpinBox, QGraphicsView, QGraphicsScene, QGraphicsEllipseItem,
                               QGraphicsLineItem, QGraphicsSimpleTextItem, QGroupBox,
                               QRadioButton, QFormLayout, QSlider, QCheckBox, QInputDialog,
                               QMessageBox, QGraphicsPathItem, QGraphicsItem)
from PySide6.QtCore import Qt, QPointF, QTimer, QRectF
from PySide6.QtGui import (QPen, QBrush, QColor, QFont, QPainter, QLinearGradient,
                           QPainterPath, QPolygonF, QPainterPathStroker)


# ==================== CONSTANTES ====================

NODE_RADIUS = 25
COLOR_BG_START = QColor("#05080f")
COLOR_BG_END = QColor("#0d1117")
COLOR_GRID = QColor("#141e32")
COLOR_NODE_FILL = QColor(26, 31, 46, 200)
COLOR_NODE_BORDER = QColor("#FF6B35")
COLOR_TEXT = QColor("#FFFFFF")
COLOR_EDGE = QColor("#58A6FF")
COLOR_VISITING = QColor("#FF6B35")
COLOR_VISITED = QColor("#238636")
COLOR_PATH = QColor("#F0E68C")

GLOBAL_STYLESHEET = """
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
QSpinBox, QLineEdit {
    background-color: #0d1117;
    border: 1px solid #30363d;
    color: #ffffff;
    padding: 4px;
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
QCheckBox {
    color: #c9d1d9;
}
QCheckBox::indicator:checked {
    background-color: #FF6B35;
    border: 1px solid #FF6B35;
}
"""


# ==================== STRUCTURES DE DONNÉES ====================

class GraphNode:
    """Nœud de graphe"""
    def __init__(self, node_id, x=0, y=0):
        self.id = node_id
        self.x = x
        self.y = y
        self.neighbors = {}  # {neighbor_id: weight}
    
    def add_neighbor(self, neighbor_id, weight=1):
        self.neighbors[neighbor_id] = weight
    
    def remove_neighbor(self, neighbor_id):
        if neighbor_id in self.neighbors:
            del self.neighbors[neighbor_id]


class Graph:
    """Structure de graphe"""
    def __init__(self, directed=False):
        self.nodes = {}  # {node_id: GraphNode}
        self.directed = directed
    
    def add_node(self, node_id, x=0, y=0):
        if node_id not in self.nodes:
            self.nodes[node_id] = GraphNode(node_id, x, y)
        return self.nodes[node_id]
    
    def remove_node(self, node_id):
        if node_id in self.nodes:
            # Supprimer toutes les arêtes vers ce nœud
            for node in self.nodes.values():
                node.remove_neighbor(node_id)
            del self.nodes[node_id]
    
    def add_edge(self, from_id, to_id, weight=1):
        if from_id in self.nodes and to_id in self.nodes:
            self.nodes[from_id].add_neighbor(to_id, weight)
            if not self.directed:
                self.nodes[to_id].add_neighbor(from_id, weight)
    
    def remove_edge(self, from_id, to_id):
        if from_id in self.nodes:
            self.nodes[from_id].remove_neighbor(to_id)
        if not self.directed and to_id in self.nodes:
            self.nodes[to_id].remove_neighbor(from_id)
    
    def get_edges(self):
        """Retourne la liste des arêtes"""
        edges = []
        seen = set()
        for node_id, node in self.nodes.items():
            for neighbor_id, weight in node.neighbors.items():
                if self.directed:
                    edges.append((node_id, neighbor_id, weight))
                else:
                    edge_key = tuple(sorted([node_id, neighbor_id]))
                    if edge_key not in seen:
                        seen.add(edge_key)
                        edges.append((node_id, neighbor_id, weight))
        return edges

    def has_negative_weights(self):
        """Check if graph contains any negative weights"""
        for node in self.nodes.values():
            for weight in node.neighbors.values():
                if weight < 0:
                    return True
        return False
    
    def bfs(self, start_id):
        """Parcours en largeur (BFS)"""
        if start_id not in self.nodes:
            return
        visited = set()
        queue = collections.deque([start_id])
        while queue:
            node_id = queue.popleft()
            if node_id not in visited:
                visited.add(node_id)
                yield node_id
                for neighbor_id in self.nodes[node_id].neighbors:
                    if neighbor_id not in visited:
                        queue.append(neighbor_id)
    
    def dfs(self, start_id):
        """Parcours en profondeur (DFS)"""
        if start_id not in self.nodes:
            return
        visited = set()
        stack = [start_id]
        while stack:
            node_id = stack.pop()
            if node_id not in visited:
                visited.add(node_id)
                yield node_id
                for neighbor_id in reversed(list(self.nodes[node_id].neighbors.keys())):
                    if neighbor_id not in visited:
                        stack.append(neighbor_id)
    
    def dijkstra(self, start_id, end_id):
        """Algorithme de Dijkstra - retourne le chemin le plus court"""
        import heapq
        
        # Check for negative weights
        if self.has_negative_weights():
            return None, -1 # Signal error code for UI to handle
            
        if start_id not in self.nodes or end_id not in self.nodes:
            return None, float('inf')
        
        distances = {node_id: float('inf') for node_id in self.nodes}
        distances[start_id] = 0
        previous = {node_id: None for node_id in self.nodes}
        pq = [(0, start_id)]
        
        while pq:
            current_dist, current_id = heapq.heappop(pq)
            
            if current_id == end_id:
                break
            
            if current_dist > distances[current_id]:
                continue
            
            for neighbor_id, weight in self.nodes[current_id].neighbors.items():
                distance = current_dist + weight
                if distance < distances[neighbor_id]:
                    distances[neighbor_id] = distance
                    previous[neighbor_id] = current_id
                    heapq.heappush(pq, (distance, neighbor_id))
        
        # Reconstruire le chemin
        path = []
        current = end_id
        while current is not None:
            path.append(current)
            current = previous[current]
        path.reverse()
        
        if path[0] != start_id:
            return None, float('inf')
        
        return path, distances[end_id]
    
    def find_all_paths(self, start_id, end_id, max_paths=50):
        """Find all paths between two nodes using DFS backtracking"""
        if start_id not in self.nodes or end_id not in self.nodes:
            return []
        
        all_paths = []
        
        def dfs(current, path, cost, visited):
            if len(all_paths) >= max_paths:
                return
            
            if current == end_id:
                all_paths.append((list(path), cost))
                return
            
            for neighbor_id, weight in self.nodes[current].neighbors.items():
                if neighbor_id not in visited:
                    visited.add(neighbor_id)
                    path.append(neighbor_id)
                    dfs(neighbor_id, path, cost + weight, visited)
                    path.pop()
                    visited.discard(neighbor_id)
        
        visited = {start_id}
        dfs(start_id, [start_id], 0, visited)
        
        # Sort by cost
        all_paths.sort(key=lambda x: x[1])
        return all_paths
    
    def bellman_ford(self, start_id, end_id):
        """Bellman-Ford algorithm - handles negative weights"""
        if start_id not in self.nodes or end_id not in self.nodes:
            return None, float('inf')
        
        # Initialize distances
        distances = {node_id: float('inf') for node_id in self.nodes}
        distances[start_id] = 0
        previous = {node_id: None for node_id in self.nodes}
        
        # Get all edges
        edges = []
        for node_id, node in self.nodes.items():
            for neighbor_id, weight in node.neighbors.items():
                edges.append((node_id, neighbor_id, weight))
        
        # Relax edges V-1 times
        for _ in range(len(self.nodes) - 1):
            for from_id, to_id, weight in edges:
                if distances[from_id] != float('inf') and distances[from_id] + weight < distances[to_id]:
                    distances[to_id] = distances[from_id] + weight
                    previous[to_id] = from_id
        
        # Check for negative cycles
        for from_id, to_id, weight in edges:
            if distances[from_id] != float('inf') and distances[from_id] + weight < distances[to_id]:
                return None, float('inf')  # Negative cycle detected
        
        # Reconstruct path
        if distances[end_id] == float('inf'):
            return None, float('inf')
        
        path = []
        current = end_id
        while current is not None:
            path.append(current)
            current = previous[current]
        path.reverse()
        
        return path, distances[end_id]
    
    def floyd_warshall(self):
        """Floyd-Warshall algorithm - all pairs shortest paths"""
        node_list = list(self.nodes.keys())
        n = len(node_list)
        node_to_idx = {node: i for i, node in enumerate(node_list)}
        
        # Initialize distance matrix
        dist = [[float('inf')] * n for _ in range(n)]
        next_node = [[None] * n for _ in range(n)]
        
        # Set diagonal to 0
        for i in range(n):
            dist[i][i] = 0
        
        # Fill initial distances from edges
        for node_id, node in self.nodes.items():
            i = node_to_idx[node_id]
            for neighbor_id, weight in node.neighbors.items():
                j = node_to_idx[neighbor_id]
                dist[i][j] = weight
                next_node[i][j] = neighbor_id
        
        # Floyd-Warshall main loop
        for k in range(n):
            for i in range(n):
                for j in range(n):
                    if dist[i][k] + dist[k][j] < dist[i][j]:
                        dist[i][j] = dist[i][k] + dist[k][j]
                        next_node[i][j] = next_node[i][k]
        
        return dist, next_node, node_list, node_to_idx


# ==================== ÉLÉMENTS GRAPHIQUES ====================

class GraphNodeItem(QGraphicsEllipseItem):
    """Élément graphique pour un nœud"""
    def __init__(self, node_id, x, y, radius=NODE_RADIUS):
        super().__init__(-radius, -radius, radius * 2, radius * 2)
        self.node_id = node_id
        self.setPos(x, y)
        
        # Style
        self.default_pen = QPen(COLOR_NODE_BORDER, 2)
        self.hover_pen = QPen(QColor("#FFFFFF"), 3)
        self.setPen(self.default_pen)
        self.setBrush(QBrush(COLOR_NODE_FILL))
        
        # Texte
        self.text_item = QGraphicsSimpleTextItem(str(node_id), self)
        self.text_item.setBrush(COLOR_TEXT)
        self.text_item.setFont(QFont("Consolas", 10, QFont.Bold))
        text_rect = self.text_item.boundingRect()
        self.text_item.setPos(-text_rect.width() / 2, -text_rect.height() / 2)
        
        # Interactivité
        self.setFlag(QGraphicsEllipseItem.ItemIsMovable)
        self.setFlag(QGraphicsEllipseItem.ItemSendsGeometryChanges)
        self.setAcceptHoverEvents(True)
        self.setCursor(Qt.PointingHandCursor)
        
        self.edges = []  # Liste des arêtes connectées
        self.node_clicked = None  # Callback for click handling
        self.block_drag = False  # Block dragging when in special modes
    
    def mousePressEvent(self, event):
        if self.node_clicked:
            self.node_clicked(self.node_id)
        if not self.block_drag:
            super().mousePressEvent(event)
        else:
            event.accept()  # Consume the event to prevent any drag
    
    def hoverEnterEvent(self, event):
        self.setPen(self.hover_pen)
        self.setBrush(QBrush(QColor(40, 50, 70, 220)))
        super().hoverEnterEvent(event)
    
    def hoverLeaveEvent(self, event):
        self.setPen(self.default_pen)
        self.setBrush(QBrush(COLOR_NODE_FILL))
        super().hoverLeaveEvent(event)
    
    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionHasChanged:
            # Mettre à jour les arêtes connectées
            for edge in self.edges:
                edge.update_position()
        return super().itemChange(change, value)
    
    def set_highlight(self, color):
        self.setPen(QPen(color, 3))
        self.setBrush(QBrush(color.darker(200)))
    
    def reset_style(self):
        self.setPen(self.default_pen)
        self.setBrush(QBrush(COLOR_NODE_FILL))


class GraphEdgeItem(QGraphicsPathItem):
    """Élément graphique pour une arête"""
    def __init__(self, from_node, to_node, weight=1, directed=False):
        super().__init__()
        self.from_node = from_node
        self.to_node = to_node
        self.weight = weight
        self.directed = directed
        
        self.default_pen = QPen(COLOR_EDGE, 2)
        self.setPen(self.default_pen)
        
        # Texte du poids
        self.weight_text = QGraphicsSimpleTextItem(str(weight), self)
        self.weight_text.setBrush(QColor("#8B949E"))
        self.weight_text.setFont(QFont("Consolas", 9))
        
        # Ajouter aux listes des nœuds
        from_node.edges.append(self)
        to_node.edges.append(self)
        
        # Interactivité
        self.setAcceptHoverEvents(True)
        self.edge_clicked = None  # Callback
        
        self.update_position()
        
    def shape(self):
        """Zone de clic élargie pour faciliter la sélection"""
        path = QPainterPath()
        path.addPath(self.path())
        stroker = QPainterPathStroker()
        stroker.setWidth(10) # 10px click tolerence
        return stroker.createStroke(path)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton and self.edge_clicked:
            self.edge_clicked(self)
            event.accept()
        else:
            super().mousePressEvent(event)
    
    def hoverEnterEvent(self, event):
        self.setPen(QPen(QColor("#FFFFFF"), 3))
        super().hoverEnterEvent(event)

    def hoverLeaveEvent(self, event):
        self.setPen(QPen(COLOR_EDGE, 2))
        super().hoverLeaveEvent(event)
    
    def update_position(self):
        from_pos = self.from_node.pos()
        to_pos = self.to_node.pos()
        
        path = QPainterPath()
        path.moveTo(from_pos)
        path.lineTo(to_pos)
        
        # Ajouter une flèche si dirigé
        if self.directed:
            angle = math.atan2(to_pos.y() - from_pos.y(), to_pos.x() - from_pos.x())
            arrow_size = 10
            # Position de la flèche (avant le nœud)
            arrow_pos = QPointF(
                to_pos.x() - NODE_RADIUS * math.cos(angle),
                to_pos.y() - NODE_RADIUS * math.sin(angle)
            )
            # Points de la flèche
            p1 = QPointF(
                arrow_pos.x() - arrow_size * math.cos(angle - math.pi / 6),
                arrow_pos.y() - arrow_size * math.sin(angle - math.pi / 6)
            )
            p2 = QPointF(
                arrow_pos.x() - arrow_size * math.cos(angle + math.pi / 6),
                arrow_pos.y() - arrow_size * math.sin(angle + math.pi / 6)
            )
            path.moveTo(arrow_pos)
            path.lineTo(p1)
            path.moveTo(arrow_pos)
            path.lineTo(p2)
        
        self.setPath(path)
        
        # Position du texte du poids
        mid_x = (from_pos.x() + to_pos.x()) / 2
        mid_y = (from_pos.y() + to_pos.y()) / 2
        self.weight_text.setPos(mid_x + 5, mid_y - 15)
    
    def set_highlight(self, color):
        self.setPen(QPen(color, 3))
    
    def reset_style(self):
        self.setPen(self.default_pen)


# ==================== VUE GRAPHIQUE ====================

class GraphGraphicsView(QGraphicsView):
    """Vue graphique pour le graphe avec fond stylisé"""
    def __init__(self, scene):
        super().__init__(scene)
        self.setRenderHint(QPainter.Antialiasing)
        self.setRenderHint(QPainter.SmoothPixmapTransform)
        self.setViewportUpdateMode(QGraphicsView.FullViewportUpdate)
        self.setDragMode(QGraphicsView.ScrollHandDrag)
        self.setTransformationAnchor(QGraphicsView.AnchorUnderMouse)
        self.setResizeAnchor(QGraphicsView.AnchorUnderMouse)
        self.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.setBackgroundBrush(QBrush(COLOR_BG_END))
        self._zoom = 1
    
    def drawBackground(self, painter, rect):
        # Dégradé de fond
        gradient = QLinearGradient(rect.topLeft(), rect.bottomRight())
        gradient.setColorAt(0, COLOR_BG_START)
        gradient.setColorAt(1, COLOR_BG_END)
        painter.fillRect(rect, gradient)
        
        # Grille
        pen = QPen(COLOR_GRID, 0.5)
        painter.setPen(pen)
        grid_size = 50
        left = int(rect.left()) - (int(rect.left()) % grid_size)
        top = int(rect.top()) - (int(rect.top()) % grid_size)
        
        for x in range(int(left), int(rect.right()), grid_size):
            painter.drawLine(x, int(rect.top()), x, int(rect.bottom()))
        for y in range(int(top), int(rect.bottom()), grid_size):
            painter.drawLine(int(rect.left()), y, int(rect.right()), y)
    
    def wheelEvent(self, event):
        factor = 1.15
        if event.angleDelta().y() > 0:
            self.scale(factor, factor)
            self._zoom *= factor
        else:
            self.scale(1 / factor, 1 / factor)
            self._zoom /= factor


# ==================== FENÊTRE PRINCIPALE ====================

class GraphesWindow(QWidget):
    """Widget principal du module Graphes"""
    def __init__(self):
        super().__init__()
        self.setStyleSheet(GLOBAL_STYLESHEET)
        
        self.graph = Graph(directed=False)
        self.node_items = {}
        self.edge_items = []
        
        # Animation
        self.animation_timer = QTimer()
        self.animation_timer.timeout.connect(self.play_step)
        self.current_generator = None
        self.visited_nodes = []
        
        # Tool modes
        self.is_delete_mode = False
        self.is_modify_mode = False
        self.edge_mode_from = None  # For edge creation
        
        self._init_ui()
    
    def _init_ui(self):
        main_layout = QHBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)
        
        # Panneau de contrôle à gauche
        self.control_panel = QWidget()
        self.control_panel.setFixedWidth(280)
        self.control_panel.setStyleSheet("background-color: #161b22;")
        control_layout = QVBoxLayout(self.control_panel)
        main_layout.addWidget(self.control_panel)
        
        # Vue graphique à droite
        self.scene = QGraphicsScene()
        self.scene.setSceneRect(-1000, -1000, 2000, 2000)
        self.view = GraphGraphicsView(self.scene)
        main_layout.addWidget(self.view, 1)
        
        # Floating Tools Panel (Right side)
        self.tools_panel = QWidget(self.view)
        self.tools_panel.setFixedWidth(200)
        self.tools_panel.setStyleSheet("""
            background-color: rgba(22, 27, 34, 0.95); 
            border: 1px solid #30363d; 
            border-radius: 8px;
        """)
        tools_layout = QVBoxLayout(self.tools_panel)
        tools_layout.setContentsMargins(10, 10, 10, 10)
        tools_layout.setSpacing(8)
        
        # Operations in floating panel
        ops_group = QGroupBox("OPÉRATIONS")
        ops_layout = QVBoxLayout()
        
        self.btn_delete_mode = QPushButton("Mode Suppression")
        self.btn_delete_mode.setCheckable(True)
        self.btn_delete_mode.setStyleSheet("QPushButton:checked { background-color: #cf222e; color: white; }")
        self.btn_delete_mode.clicked.connect(self._toggle_delete_mode)
        
        self.btn_edge_mode = QPushButton("Mode Arête")
        self.btn_edge_mode.setCheckable(True)
        self.btn_edge_mode.setStyleSheet("QPushButton:checked { background-color: #238636; color: white; }")
        self.btn_edge_mode.clicked.connect(self._toggle_edge_mode)
        
        self.btn_modify_mode = QPushButton("Mode Modifier")
        self.btn_modify_mode.setCheckable(True)
        self.btn_modify_mode.setStyleSheet("QPushButton:checked { background-color: #1f6feb; color: white; }")
        self.btn_modify_mode.clicked.connect(self._toggle_modify_mode)
        
        ops_layout.addWidget(self.btn_delete_mode)
        ops_layout.addWidget(self.btn_edge_mode)
        ops_layout.addWidget(self.btn_modify_mode)
        ops_group.setLayout(ops_layout)
        tools_layout.addWidget(ops_group)
        tools_layout.addStretch()
        self.tools_panel.adjustSize()
        
        # === LEFT PANEL CONTROLS ===
        
        # 1. Type de graphe
        type_group = QGroupBox("TYPE DE GRAPHE")
        type_layout = QVBoxLayout()
        self.rb_undirected = QRadioButton("Non-orienté")
        self.rb_directed = QRadioButton("Orienté")
        self.rb_undirected.setChecked(True)
        self.rb_undirected.toggled.connect(self._update_graph_type)
        type_layout.addWidget(self.rb_undirected)
        type_layout.addWidget(self.rb_directed)
        type_group.setLayout(type_layout)
        control_layout.addWidget(type_group)
        

        
        # 3. Édition
        edit_group = QGroupBox("ÉDITION")
        edit_layout = QVBoxLayout()
        self.btn_add_node = QPushButton("+ AJOUTER NŒUD")
        self.btn_add_node.clicked.connect(self._add_node)
        self.btn_add_edge = QPushButton("+ AJOUTER ARÊTE")
        self.btn_add_edge.clicked.connect(self._add_edge)
        self.btn_clear = QPushButton("EFFACER TOUT")
        self.btn_clear.clicked.connect(self._clear_graph)
        edit_layout.addWidget(self.btn_add_node)
        edit_layout.addWidget(self.btn_add_edge)
        edit_layout.addWidget(self.btn_clear)
        edit_group.setLayout(edit_layout)
        control_layout.addWidget(edit_group)
        
        # 4. Algorithmes
        algo_group = QGroupBox("ALGORITHMES")
        algo_layout = QVBoxLayout()
        self.btn_dijkstra = QPushButton("DIJKSTRA")
        self.btn_dijkstra.clicked.connect(self._run_dijkstra)
        self.btn_bellman = QPushButton("BELLMAN-FORD")
        self.btn_bellman.clicked.connect(self._run_bellman)
        self.btn_floyd = QPushButton("FLOYD-WARSHALL")
        self.btn_floyd.clicked.connect(self._run_floyd)
        
        algo_layout.addWidget(self.btn_dijkstra)
        algo_layout.addWidget(self.btn_bellman)
        algo_layout.addWidget(self.btn_floyd)
        algo_group.setLayout(algo_layout)
        control_layout.addWidget(algo_group)
        
        control_layout.addStretch()
        
        # Log
        self.lbl_log = QLabel("LOG: En attente...")
        self.lbl_log.setStyleSheet("font-family: 'Consolas'; font-weight: bold; color: #58A6FF;")
        self.lbl_log.setWordWrap(True)
        control_layout.addWidget(self.lbl_log)
        
        control_layout.addStretch()
        
        # Log
        self.lbl_log = QLabel("LOG: En attente...")
        self.lbl_log.setStyleSheet("font-family: 'Consolas'; font-weight: bold; color: #58A6FF;")
        self.lbl_log.setWordWrap(True)
        control_layout.addWidget(self.lbl_log)
    
    def resizeEvent(self, event):
        # Position floating panel
        if hasattr(self, 'tools_panel') and hasattr(self, 'view'):
            padding = 20
            x = self.view.width() - self.tools_panel.width() - padding
            y = padding
            self.tools_panel.move(x, y)
        super().resizeEvent(event)
    
    def _toggle_delete_mode(self):
        self.is_delete_mode = self.btn_delete_mode.isChecked()
        if self.is_delete_mode:
            self.btn_delete_mode.setText("Supprimer: ON")
            self.btn_edge_mode.setChecked(False)
            self.btn_modify_mode.setChecked(False)
            self.is_modify_mode = False
            self.edge_mode_from = None
            self.setCursor(Qt.CrossCursor)
            self._update_node_draggable(False)
        else:
            self.btn_delete_mode.setText("Mode Suppression")
            self.setCursor(Qt.ArrowCursor)
            self._update_node_draggable(True)
    
    def _toggle_edge_mode(self):
        if self.btn_edge_mode.isChecked():
            self.btn_edge_mode.setText("Arête: ON")
            self.btn_delete_mode.setChecked(False)
            self.btn_modify_mode.setChecked(False)
            self.is_delete_mode = False
            self.is_modify_mode = False
            self.edge_mode_from = None
            self.lbl_log.setText("LOG: Cliquez sur le nœud source")
            self._update_node_draggable(False)
        else:
            self.btn_edge_mode.setText("Mode Arête")
            self.edge_mode_from = None
            self._update_node_draggable(True)
    
    def _toggle_modify_mode(self):
        self.is_modify_mode = self.btn_modify_mode.isChecked()
        if self.is_modify_mode:
            self.btn_modify_mode.setText("Modifier: ON")
            self.btn_delete_mode.setChecked(False)
            self.btn_edge_mode.setChecked(False)
            self.is_delete_mode = False
            self.edge_mode_from = None
            self.lbl_log.setText("LOG: Cliquez sur un nœud pour le renommer")
            self._update_node_draggable(False)
        else:
            self.btn_modify_mode.setText("Mode Modifier")
            self._update_node_draggable(True)
    
    def _update_node_draggable(self, draggable):
        """Enable or disable dragging for all nodes"""
        for item in self.node_items.values():
            item.setFlag(QGraphicsEllipseItem.ItemIsMovable, draggable)
            item.block_drag = not draggable
    
    def _handle_node_click(self, node_id):
        """Handle click on a node based on current mode"""
        # Delete mode
        if self.is_delete_mode:
            self._delete_node(node_id)
            return
        
        # Modify mode
        if self.is_modify_mode:
            self._modify_node(node_id)
            return
        
        # Edge creation mode
        if self.btn_edge_mode.isChecked():
            if self.edge_mode_from is None:
                self.edge_mode_from = node_id
                self.lbl_log.setText(f"LOG: Source: {node_id}. Cliquez destination.")
                # Highlight source node
                if node_id in self.node_items:
                    self.node_items[node_id].set_highlight(QColor("#238636"))
            else:
                if self.edge_mode_from == node_id:
                    self.lbl_log.setText("LOG: Même nœud - annulé")
                    self._reset_styles()
                    self.edge_mode_from = None
                    return
                
                # Ask for weight
                # CHANGED: Allow negative inputs (-1000 to 1000)
                weight, ok = QInputDialog.getInt(self, "Poids", f"Poids de l'arête {self.edge_mode_from} → {node_id}:", 1, -1000, 1000)
                if ok:
                    self.graph.add_edge(self.edge_mode_from, node_id, weight)
                    self._redraw_graph()
                    self.lbl_log.setText(f"LOG: Arête {self.edge_mode_from} → {node_id} (poids {weight})")
                else:
                    self.lbl_log.setText("LOG: Arête annulée")
                
                self.edge_mode_from = None
    
                self.edge_mode_from = None
    
    def _handle_edge_click(self, edge_item):
        """Handle click on an edge"""
        # Delete mode
        if self.is_delete_mode:
            self.graph.remove_edge(edge_item.from_node.node_id, edge_item.to_node.node_id)
            self._redraw_graph()
            self.lbl_log.setText(f"LOG: Arête supprimée")
            return
            
        # Modify mode (or default if we want to allow modification by default)
        if self.is_modify_mode:
            current_weight = edge_item.weight
            new_weight, ok = QInputDialog.getInt(self, "Modifier Poids", 
                                               f"Nouveau poids ({edge_item.from_node.node_id}-{edge_item.to_node.node_id}):",
                                               current_weight, -1000, 1000)
            if ok:
                # Update logic graph
                self.graph.add_edge(edge_item.from_node.node_id, edge_item.to_node.node_id, new_weight)
                self._redraw_graph() # Simplest way to refresh visual
                self.lbl_log.setText(f"LOG: Poids modifié en {new_weight}")
            return
            
    def _delete_node(self, node_id):
        """Delete a node and its connected edges"""
        self.graph.remove_node(node_id)
        self._redraw_graph()
        self.lbl_log.setText(f"LOG: Nœud '{node_id}' supprimé")
    
    def _modify_node(self, node_id):
        """Modify a node's label"""
        new_label, ok = QInputDialog.getText(self, "Modifier Nœud", f"Nouveau nom pour '{node_id}':")
        if not ok or not new_label:
            return
        
        if new_label == node_id:
            return  # No change
        
        if new_label in self.graph.nodes:
            QMessageBox.warning(self, "Erreur", f"Le nœud '{new_label}' existe déjà")
            return
        
        # Get current node data
        old_node = self.graph.nodes[node_id]
        x, y = old_node.x, old_node.y
        neighbors = dict(old_node.neighbors)
        
        # For undirected, also get incoming edges
        incoming = {}
        for nid, node in self.graph.nodes.items():
            if node_id in node.neighbors and nid != node_id:
                incoming[nid] = node.neighbors[node_id]
        
        # Remove old node
        self.graph.remove_node(node_id)
        
        # Add new node
        self.graph.add_node(new_label, x, y)
        
        # Restore outgoing edges
        for neighbor_id, weight in neighbors.items():
            if neighbor_id in self.graph.nodes:
                self.graph.add_edge(new_label, neighbor_id, weight)
        
        # Restore incoming edges (for directed graphs)
        for from_id, weight in incoming.items():
            if from_id in self.graph.nodes:
                self.graph.nodes[from_id].add_neighbor(new_label, weight)
        
        self._redraw_graph()
        self.lbl_log.setText(f"LOG: '{node_id}' renommé en '{new_label}'")
    
    def _update_graph_type(self):
        self.graph.directed = self.rb_directed.isChecked()
        self._redraw_graph()
    
    def _generate_sample_graph(self):
        """Génère un graphe d'exemple"""
        self._clear_graph()
        
        # Créer des nœuds en cercle
        center_x, center_y = 0, 0
        radius = 200
        num_nodes = 6
        
        for i in range(num_nodes):
            angle = 2 * math.pi * i / num_nodes - math.pi / 2
            x = center_x + radius * math.cos(angle)
            y = center_y + radius * math.sin(angle)
            self.graph.add_node(chr(65 + i), x, y)  # A, B, C, ...
        
        # Ajouter quelques arêtes
        edges = [('A', 'B', 4), ('A', 'C', 2), ('B', 'C', 1), ('B', 'D', 5),
                 ('C', 'D', 8), ('C', 'E', 10), ('D', 'E', 2), ('D', 'F', 6), ('E', 'F', 3)]
        for from_id, to_id, weight in edges:
            if from_id in self.graph.nodes and to_id in self.graph.nodes:
                self.graph.add_edge(from_id, to_id, weight)
        
        self._redraw_graph()
    

    
    def _sync_node_positions(self):
        """Sync visual node positions back to graph data"""
        for node_id, item in self.node_items.items():
            if node_id in self.graph.nodes:
                self.graph.nodes[node_id].x = item.pos().x()
                self.graph.nodes[node_id].y = item.pos().y()
    
    def _redraw_graph(self):
        """Redessine le graphe"""
        # Sync positions before clearing
        self._sync_node_positions()
        
        # Visual Update for Dijkstra Button
        has_neg = self.graph.has_negative_weights()
        if hasattr(self, 'btn_dijkstra'):
            self.btn_dijkstra.setEnabled(not has_neg)
            if has_neg:
                self.btn_dijkstra.setToolTip("Désactivé: Poids négatifs détectés")
            else:
                self.btn_dijkstra.setToolTip("")
        
        self.scene.clear()
        self.node_items.clear()
        self.edge_items.clear()
        
        # Créer les nœuds graphiques
        for node_id, node in self.graph.nodes.items():
            item = GraphNodeItem(node_id, node.x, node.y)
            item.node_clicked = lambda nid=node_id: self._handle_node_click(nid)
            self.scene.addItem(item)
            self.node_items[node_id] = item
        
        # Créer les arêtes graphiques
        for from_id, to_id, weight in self.graph.get_edges():
            if from_id in self.node_items and to_id in self.node_items:
                edge = GraphEdgeItem(
                    self.node_items[from_id],
                    self.node_items[to_id],
                    weight,
                    self.graph.directed
                )
                edge.edge_clicked = self._handle_edge_click
                self.scene.addItem(edge)
                self.edge_items.append(edge)
        
        # Mettre les arêtes derrière les nœuds
        for edge in self.edge_items:
            edge.setZValue(-1)
    
    def _add_node(self):
        """Ajoute un nouveau nœud avec label personnalisé"""
        # Ask for custom label
        label, ok = QInputDialog.getText(self, "Nouveau Nœud", "Nom du nœud:")
        if not ok or not label:
            return
        
        # Check if label already exists
        if label in self.graph.nodes:
            QMessageBox.warning(self, "Erreur", f"Le nœud '{label}' existe déjà")
            return
        
        # Position au centre (user can drag)
        x = random.randint(-100, 100)
        y = random.randint(-100, 100)
        
        self.graph.add_node(label, x, y)
        item = GraphNodeItem(label, x, y)
        item.node_clicked = lambda nid=label: self._handle_node_click(nid)
        self.scene.addItem(item)
        self.node_items[label] = item
        self.lbl_log.setText(f"LOG: Nœud '{label}' ajouté")
    
    def _add_edge(self):
        """Ajoute une nouvelle arête"""
        node_ids = list(self.graph.nodes.keys())
        if len(node_ids) < 2:
            QMessageBox.warning(self, "Erreur", "Il faut au moins 2 nœuds")
            return
        
        from_id, ok1 = QInputDialog.getItem(self, "Arête", "Nœud source:", node_ids, 0, False)
        if not ok1:
            return
        
        to_id, ok2 = QInputDialog.getItem(self, "Arête", "Nœud destination:", node_ids, 0, False)
        if not ok2:
            return
        
        weight, ok3 = QInputDialog.getInt(self, "Arête", "Poids:", 1, -1000, 1000)
        if not ok3:
            return
        
        if from_id == to_id:
            QMessageBox.warning(self, "Erreur", "Les nœuds doivent être différents")
            return
        
        self.graph.add_edge(from_id, to_id, weight)
        edge = GraphEdgeItem(
            self.node_items[from_id],
            self.node_items[to_id],
            weight,
            self.graph.directed
        )
        edge.setZValue(-1)
        edge.edge_clicked = self._handle_edge_click
        self.scene.addItem(edge)
        self.edge_items.append(edge)
        self.lbl_log.setText(f"LOG: Arête {from_id}-{to_id} (poids {weight}) ajoutée")
    
    def _clear_graph(self):
        """Efface le graphe"""
        self.scene.clear()
        self.graph = Graph(self.graph.directed)
        self.node_items.clear()
        self.edge_items.clear()
        self.lbl_log.setText("LOG: Graphe effacé")
    
    def _reset_styles(self):
        """Réinitialise les styles de tous les éléments"""
        for item in self.node_items.values():
            item.reset_style()
        for edge in self.edge_items:
            edge.reset_style()
    
    def _run_bfs(self):
        """Lance l'animation BFS"""
        if not self.graph.nodes:
            return
        self._reset_styles()
        node_ids = list(self.graph.nodes.keys())
        start_id, ok = QInputDialog.getItem(self, "BFS", "Nœud de départ:", node_ids, 0, False)
        if ok:
            self.current_generator = self.graph.bfs(start_id)
            self.visited_nodes = []
            self.animation_timer.start(self.slider_speed.value())
            self.lbl_log.setText(f"LOG: BFS depuis {start_id}...")
    
    def _run_dfs(self):
        """Lance l'animation DFS"""
        if not self.graph.nodes:
            return
        self._reset_styles()
        node_ids = list(self.graph.nodes.keys())
        start_id, ok = QInputDialog.getItem(self, "DFS", "Nœud de départ:", node_ids, 0, False)
        if ok:
            self.current_generator = self.graph.dfs(start_id)
            self.visited_nodes = []
            self.animation_timer.start(self.slider_speed.value())
            self.lbl_log.setText(f"LOG: DFS depuis {start_id}...")
    
    def _run_dijkstra(self):
        """Lance l'algorithme de Dijkstra et affiche tous les chemins"""
        # Check immediately
        if self.graph.has_negative_weights():
            QMessageBox.warning(self, "Attention", "Le graphe contient des poids négatifs.\nDijkstra ne peut pas être utilisé.\nUtilisez Bellman-Ford.")
            return

        if not self.graph.nodes:
            return
        self._reset_styles()
        node_ids = list(self.graph.nodes.keys())
        
        start_id, ok1 = QInputDialog.getItem(self, "Dijkstra", "Nœud de départ:", node_ids, 0, False)
        if not ok1:
            return
        
        end_id, ok2 = QInputDialog.getItem(self, "Dijkstra", "Nœud d'arrivée:", node_ids, 0, False)
        if not ok2:
            return
        
        # Find all paths
        all_paths = self.graph.find_all_paths(start_id, end_id)
        
        if not all_paths:
            self.lbl_log.setText(f"LOG: Aucun chemin de {start_id} à {end_id}")
            QMessageBox.information(self, "Dijkstra", f"Aucun chemin trouvé de {start_id} à {end_id}")
            return
        
        # Highlight shortest path (first in sorted list)
        shortest_path, shortest_cost = all_paths[0]
        for i, node_id in enumerate(shortest_path):
            if node_id in self.node_items:
                self.node_items[node_id].set_highlight(COLOR_PATH)
            if i > 0:
                prev_id = shortest_path[i - 1]
                for edge in self.edge_items:
                    if (edge.from_node.node_id == prev_id and edge.to_node.node_id == node_id) or \
                       (edge.to_node.node_id == prev_id and edge.from_node.node_id == node_id):
                        edge.set_highlight(COLOR_PATH)
        
        # Build result message
        result_lines = [f"TOUS LES CHEMINS ({len(all_paths)} trouvés):\n"]
        for i, (path, cost) in enumerate(all_paths[:10]):  # Show max 10
            path_str = " → ".join(str(p) for p in path)
            prefix = "★ " if i == 0 else "  "
            result_lines.append(f"{prefix}{path_str} (coût: {cost})")
        
        if len(all_paths) > 10:
            result_lines.append(f"\n... et {len(all_paths) - 10} autres chemins")
        
        # Show popup
        QMessageBox.information(self, f"Dijkstra: {start_id} → {end_id}", "\n".join(result_lines))
        
        path_str = " → ".join(str(p) for p in shortest_path)
        self.lbl_log.setText(f"LOG: Plus court: {path_str} (coût: {shortest_cost})")
    
    def _run_bellman(self):
        """Lance l'algorithme de Bellman-Ford et affiche tous les chemins"""
        if not self.graph.nodes:
            return
        self._reset_styles()
        node_ids = list(self.graph.nodes.keys())
        
        start_id, ok1 = QInputDialog.getItem(self, "Bellman-Ford", "Nœud de départ:", node_ids, 0, False)
        if not ok1:
            return
        
        end_id, ok2 = QInputDialog.getItem(self, "Bellman-Ford", "Nœud d'arrivée:", node_ids, 0, False)
        if not ok2:
            return
        
        # Check for negative cycles using bellman_ford
        bf_path, bf_distance = self.graph.bellman_ford(start_id, end_id)
        if bf_path is None and bf_distance == float('inf'):
            self.lbl_log.setText(f"LOG: Cycle négatif détecté ou aucun chemin")
            QMessageBox.warning(self, "Bellman-Ford", "Cycle négatif détecté ou aucun chemin trouvé")
            return
        
        # Find all paths
        all_paths = self.graph.find_all_paths(start_id, end_id)
        
        if not all_paths:
            self.lbl_log.setText(f"LOG: Aucun chemin de {start_id} à {end_id}")
            QMessageBox.information(self, "Bellman-Ford", f"Aucun chemin trouvé de {start_id} à {end_id}")
            return
        
        # Highlight shortest path (first in sorted list)
        shortest_path, shortest_cost = all_paths[0]
        for i, node_id in enumerate(shortest_path):
            if node_id in self.node_items:
                self.node_items[node_id].set_highlight(COLOR_PATH)
            if i > 0:
                prev_id = shortest_path[i - 1]
                for edge in self.edge_items:
                    if (edge.from_node.node_id == prev_id and edge.to_node.node_id == node_id) or \
                       (edge.to_node.node_id == prev_id and edge.from_node.node_id == node_id):
                        edge.set_highlight(COLOR_PATH)
        
        # Build result message
        result_lines = [f"TOUS LES CHEMINS ({len(all_paths)} trouvés):\n"]
        for i, (path, cost) in enumerate(all_paths[:10]):  # Show max 10
            path_str = " → ".join(str(p) for p in path)
            prefix = "★ " if i == 0 else "  "
            result_lines.append(f"{prefix}{path_str} (coût: {cost})")
        
        if len(all_paths) > 10:
            result_lines.append(f"\n... et {len(all_paths) - 10} autres chemins")
        
        # Show popup
        QMessageBox.information(self, f"Bellman-Ford: {start_id} → {end_id}", "\n".join(result_lines))
        
        path_str = " → ".join(str(p) for p in shortest_path)
        self.lbl_log.setText(f"LOG: Plus court: {path_str} (coût: {shortest_cost})")
    
    def _run_floyd(self):
        """Lance l'algorithme de Floyd-Warshall"""
        if not self.graph.nodes:
            return
        self._reset_styles()
        
        dist, next_node, node_list, node_to_idx = self.graph.floyd_warshall()
        
        # Build result matrix
        result_lines = ["MATRICE DES DISTANCES (Floyd-Warshall):\n"]
        result_lines.append("     " + "  ".join(f"{n:>5}" for n in node_list))
        
        for i, node in enumerate(node_list):
            row = f"{node:>5}"
            for j in range(len(node_list)):
                d = dist[i][j]
                if d == float('inf'):
                    row += "    ∞"
                else:
                    row += f"{d:>5}"
            result_lines.append(row)
        
        QMessageBox.information(self, "Floyd-Warshall", "\n".join(result_lines))
        self.lbl_log.setText("LOG: Floyd-Warshall - Matrice des distances calculée")
    
    def play_step(self):
        """Joue une étape de l'animation"""
        if self.current_generator is None:
            return
        
        try:
            node_id = next(self.current_generator)
            if node_id in self.node_items:
                # Mettre le nœud précédent en "visité"
                if self.visited_nodes:
                    prev_id = self.visited_nodes[-1]
                    if prev_id in self.node_items:
                        self.node_items[prev_id].set_highlight(COLOR_VISITED)
                
                # Mettre le nœud actuel en "en cours"
                self.node_items[node_id].set_highlight(COLOR_VISITING)
                self.visited_nodes.append(node_id)
                
                path_str = " → ".join(self.visited_nodes)
                self.lbl_log.setText(f"LOG: Parcours: {path_str}")
        except StopIteration:
            self._stop_animation()
            # Mettre le dernier nœud en vert
            if self.visited_nodes:
                last_id = self.visited_nodes[-1]
                if last_id in self.node_items:
                    self.node_items[last_id].set_highlight(COLOR_VISITED)
            self.lbl_log.setText(f"LOG: Parcours terminé! ({len(self.visited_nodes)} nœuds)")
    
    def _stop_animation(self):
        """Arrête l'animation"""
        self.animation_timer.stop()
        self.current_generator = None
