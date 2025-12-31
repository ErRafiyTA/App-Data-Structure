# -*- coding: utf-8 -*-
"""
POINT D'ENTRÉE PYSIDE6
Application avec performance GPU et barre de titre personnalisée
"""

import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QStackedWidget, QWidget, 
                               QVBoxLayout, QLabel, QPushButton, QHBoxLayout, QMessageBox, QSizePolicy)
from PySide6.QtGui import QFont, QAction, QPalette, QColor, QCursor, QPainter
from PySide6.QtCore import Qt, QPoint, QRect

from gui_pyqt.welcome_window import WelcomeWindow
from gui_pyqt.tableau_window import TableauWindow
from gui_pyqt.listes_window import ShowListesInterface
from gui_pyqt.arbres_window import ConfigArbresWindow
from gui_pyqt.graphes_window import GraphesWindow


class CustomTitleBar(QWidget):
    """Barre de titre personnalisée avec boutons stylisés"""
    
    def __init__(self, parent):
        super().__init__(parent)
        self.parent_window = parent
        self.setFixedHeight(40)
        self.setStyleSheet("background-color: #0D1117;")
        
        # Variables pour le déplacement de fenêtre
        self.dragging = False
        self.drag_position = QPoint()
        
        layout = QHBoxLayout(self)
        layout.setContentsMargins(15, 0, 10, 0)
        layout.setSpacing(0)
        
        # Titre
        self.title_label = QLabel("STRUCTURES DE DONNÉES - PREMIUM EDITION")
        self.title_label.setStyleSheet("""
            color: #FF6B35;
            font-family: 'Segoe UI';
            font-size: 12px;
            font-weight: bold;
            letter-spacing: 2px;
        """)
        layout.addWidget(self.title_label)
        
        layout.addStretch()
        
        # Boutons de contrôle
        button_style = """
            QPushButton {
                background-color: transparent;
                color: #8B949E;
                border: none;
                font-family: 'Segoe UI';
                font-size: 16px;
                font-weight: bold;
                padding: 8px 15px;
            }
            QPushButton:hover {
                color: #FFFFFF;
                background-color: #21262D;
            }
        """
        
        # Bouton Minimiser
        self.btn_minimize = QPushButton("─")
        self.btn_minimize.setStyleSheet(button_style)
        self.btn_minimize.setFixedSize(45, 40)
        self.btn_minimize.setCursor(QCursor(Qt.PointingHandCursor))
        self.btn_minimize.clicked.connect(parent.showMinimized)
        layout.addWidget(self.btn_minimize)
        
        # Bouton Maximiser/Restaurer
        self.btn_maximize = QPushButton("□")
        self.btn_maximize.setStyleSheet(button_style)
        self.btn_maximize.setFixedSize(45, 40)
        self.btn_maximize.setCursor(QCursor(Qt.PointingHandCursor))
        self.btn_maximize.clicked.connect(self.toggle_maximize)
        layout.addWidget(self.btn_maximize)
        
        # Bouton Fermer (rouge au hover)
        close_style = """
            QPushButton {
                background-color: transparent;
                color: #8B949E;
                border: none;
                font-family: 'Segoe UI';
                font-size: 16px;
                font-weight: bold;
                padding: 8px 15px;
            }
            QPushButton:hover {
                color: #FFFFFF;
                background-color: #cf222e;
            }
        """
        self.btn_close = QPushButton("✕")
        self.btn_close.setStyleSheet(close_style)
        self.btn_close.setFixedSize(45, 40)
        self.btn_close.setCursor(QCursor(Qt.PointingHandCursor))
        self.btn_close.clicked.connect(parent.close)
        layout.addWidget(self.btn_close)
    
    def toggle_maximize(self):
        """Bascule entre maximisé et restauré"""
        if self.parent_window.isMaximized():
            self.parent_window.showNormal()
            self.btn_maximize.setText("□")
        else:
            self.parent_window.showMaximized()
            self.btn_maximize.setText("❐")
    
    def mousePressEvent(self, event):
        """Début du déplacement de la fenêtre"""
        if event.button() == Qt.LeftButton:
            self.dragging = True
            self.drag_position = event.globalPosition().toPoint() - self.parent_window.frameGeometry().topLeft()
            event.accept()
    
    def mouseMoveEvent(self, event):
        """Déplacement de la fenêtre"""
        if self.dragging:
            self.parent_window.move(event.globalPosition().toPoint() - self.drag_position)
            event.accept()
    
    def mouseReleaseEvent(self, event):
        """Fin du déplacement"""
        self.dragging = False
    
    def mouseDoubleClickEvent(self, event):
        """Double-clic pour maximiser/restaurer"""
        self.toggle_maximize()


class RotatedButton(QPushButton):
    """Bouton avec texte vertical"""
    def __init__(self, text, parent=None, is_danger=False):
        super().__init__(text, parent)
        self.setFixedWidth(45)
        self.setFixedHeight(120)
        self.setCheckable(True)
        self.setCursor(Qt.PointingHandCursor)
        self.is_danger = is_danger
        
    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Colors
        bg_color = Qt.transparent
        text_color = QColor("#8B949E")
        border_color = Qt.transparent
        
        if self.isChecked():
            bg_color = QColor("rgba(255, 107, 53, 0.1)") if not self.is_danger else QColor("rgba(207, 34, 46, 0.1)")
            text_color = QColor("#FFFFFF")
            border_color = QColor("#FF6B35") if not self.is_danger else QColor("#cf222e")
        elif self.underMouse():
            bg_color = QColor("#21262D") if not self.is_danger else QColor("rgba(207, 34, 46, 0.1)")
            text_color = QColor("#FFFFFF")
            
        if self.is_danger:
            if not self.isChecked() and not self.underMouse():
                text_color = QColor("#cf222e")
        
        # Draw Background
        painter.fillRect(self.rect(), bg_color)
        
        # Draw Active Border (Left side -> Logical Top of unrotated?)
        # Let's put border on the LEFT of the vertical strip (so x=0 to 3)
        if self.isChecked():
            painter.fillRect(0, 0, 3, self.height(), border_color)
            
        # Draw Text Rotated
        painter.save()
        painter.translate(self.width() / 2, self.height() / 2)
        painter.rotate(-90)
        
        font = self.font()
        font.setFamily("Segoe UI")
        font.setBold(True)
        font.setPointSize(12)
        painter.setFont(font)
        painter.setPen(text_color)
        
        # Centered text in local rotated system
        # Rect centered at 0,0
        # Text dimensions
        rect = QRect(-self.height() // 2, -self.width() // 2, self.height(), self.width())
        painter.drawText(rect, Qt.AlignCenter, self.text())
        
        painter.restore()

class NavBar(QWidget):
    """Barre de navigation latérale (Verticale)"""
    def __init__(self, parent_window):
        super().__init__(parent_window)
        self.parent_window = parent_window
        self.setFixedWidth(48)
        self.setStyleSheet("background-color: #0D1117; border-right: 1px solid #30363D;")
        
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 10, 0, 10)
        layout.setSpacing(5)
        
        self.buttons = {}
        
        # Modules
        self.add_nav_button("ACCUEIL", "welcome")
        self.add_nav_button("TABLEAUX", "tableaux")
        self.add_nav_button("LISTES", "listes")
        self.add_nav_button("ARBRES", "arbres")
        self.add_nav_button("GRAPHES", "graphes")
        
        layout.addStretch()
        
        # Footer / Quit
        self.add_nav_button("QUITTER", "quit", is_danger=True)
        
    def add_nav_button(self, text, page_name, is_danger=False):
        btn = RotatedButton(text, is_danger=is_danger)
        
        if page_name == "quit":
            btn.clicked.connect(self.parent_window.close)
        else:
            btn.clicked.connect(lambda: self.parent_window.showWindow(page_name))
            
        self.layout().addWidget(btn)
        self.buttons[page_name] = btn

    def set_active(self, page_name):
        """Met à jour l'état actif des boutons"""
        for name, btn in self.buttons.items():
            if name == page_name:
                btn.setChecked(True)
            else:
                btn.setChecked(False)
            btn.update() # Force repaint


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        
        # Fenêtre sans bordure
        self.setWindowFlags(Qt.FramelessWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground, False)
        
        # Appliquer un style sombre
        self.setupDarkTheme()
        
        # Créer le widget principal
        main_widget = QWidget()
        main_widget.setStyleSheet("background-color: #0D1117;")
        self.setCentralWidget(main_widget)
        
        main_layout = QVBoxLayout(main_widget)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)
        
        # Custom Title Bar
        self.title_bar = CustomTitleBar(self)
        main_layout.addWidget(self.title_bar)
        
        # Content Layout (Horizontal: NavBar + StackedWidget)
        content_widget = QWidget()
        content_layout = QHBoxLayout(content_widget)
        content_layout.setContentsMargins(0, 0, 0, 0)
        content_layout.setSpacing(0)
        main_layout.addWidget(content_widget)
        
        # NavBar
        self.navbar = NavBar(self)
        content_layout.addWidget(self.navbar)
        
        # Stacked Widget
        self.stacked_widget = QStackedWidget()
        content_layout.addWidget(self.stacked_widget)
        
        # Créer les différentes fenêtres
        self.createWindows()
        
        # Afficher la fenêtre d'accueil par défaut
        self.showWindow("welcome")
    
    def setupDarkTheme(self):
        """Configure le thème sombre"""
        palette = QPalette()
        palette.setColor(QPalette.Window, QColor(13, 17, 23))
        palette.setColor(QPalette.WindowText, QColor(201, 209, 217))
        palette.setColor(QPalette.Base, QColor(30, 35, 45))
        palette.setColor(QPalette.AlternateBase, QColor(13, 17, 23))
        palette.setColor(QPalette.ToolTipBase, QColor(13, 17, 23))
        palette.setColor(QPalette.ToolTipText, QColor(201, 209, 217))
        palette.setColor(QPalette.Text, QColor(201, 209, 217))
        palette.setColor(QPalette.Button, QColor(13, 17, 23))
        palette.setColor(QPalette.ButtonText, QColor(201, 209, 217))
        palette.setColor(QPalette.BrightText, Qt.red)
        palette.setColor(QPalette.Highlight, QColor(255, 107, 53))
        palette.setColor(QPalette.HighlightedText, Qt.black)
        self.setPalette(palette)
    
    def createWindows(self):
        """Crée les différentes fenêtres de l'application"""
        # Fenêtre d'accueil
        self.welcome_window = WelcomeWindow()
        self.welcome_window.module_clicked.connect(self.on_module_clicked)
        self.stacked_widget.addWidget(self.welcome_window)
        
        # Fenêtre Tableaux
        self.tableaux_window = TableauWindow()
        self.stacked_widget.addWidget(self.tableaux_window)
        
        # Fenêtre Arbres (dans la même fenêtre principale)
        self.arbres_window = ConfigArbresWindow()
        self.stacked_widget.addWidget(self.arbres_window)
        
        # Fenêtre Graphes (dans la même fenêtre principale)
        self.graphes_window = GraphesWindow()
        self.stacked_widget.addWidget(self.graphes_window)

    def on_module_clicked(self, module_name):
        """Gère le clic sur un module depuis l'accueil"""
        if module_name == "TABLEAUX":
            self.showWindow("tableaux")
        elif module_name == "LISTES":
            self.showWindow("listes")
        elif module_name == "ARBRES":
            self.showWindow("arbres")
        elif module_name == "GRAPHES":
            self.showWindow("graphes")
    
    def showWindow(self, window_name):
        """Affiche une fenêtre spécifique"""
        self.navbar.set_active(window_name)
        
        if window_name == "welcome":
            self.stacked_widget.setCurrentWidget(self.welcome_window)
            self.title_bar.title_label.setText("STRUCTURES DE DONNÉES - ACCUEIL")
            
        elif window_name == "tableaux":
            self.stacked_widget.setCurrentWidget(self.tableaux_window)
            self.title_bar.title_label.setText("STRUCTURES DE DONNÉES - TABLEAUX")
            
        elif window_name == "listes":
            # Lazy loading if not created yet (though createWindows creates references?)
            # Actually createWindows didn't create LISTES in previous code, it initialized self.listes_window = None.
            # So I need to create it here and add it to stack if not exists.
            if not hasattr(self, 'listes_page'):
                 self.listes_page = ShowListesInterface() # This returns the LinkedListVisualizer MainWindow
                 # QMainWindow inside QStackedWidget needs flag?
                 # It behaves like a widget usually.
                 self.listes_page.setWindowFlags(Qt.Widget) # Ensure it's treated as widget
                 self.stacked_widget.addWidget(self.listes_page)
            
            self.stacked_widget.setCurrentWidget(self.listes_page)
            self.title_bar.title_label.setText("STRUCTURES DE DONNÉES - LISTES CHAÎNÉES")
            
        elif window_name == "arbres":
            self.stacked_widget.setCurrentWidget(self.arbres_window)
            self.title_bar.title_label.setText("STRUCTURES DE DONNÉES - ARBRES")
            
        elif window_name == "graphes":
            self.stacked_widget.setCurrentWidget(self.graphes_window)
            self.title_bar.title_label.setText("STRUCTURES DE DONNÉES - GRAPHES")
    
    def showModuleDev(self, module_name):
        """Affiche un message pour les modules en développement"""
        QMessageBox.information(self, "Info", 
                              f"Le module {module_name} est en cours de développement.")
    
    def showAbout(self):
        """Affiche la boîte de dialogue À propos"""
        QMessageBox.about(self, "À propos",
                         "Application de Structures de Données - Premium Edition\n\n"
                         "Version 2.0 avec accélération GPU\n\n"
                         "Modules implémentés :\n"
                         "• Tableaux dynamiques\n"
                         "• Listes chaînées (simple/double)\n"
                         "• Arbres binaires\n\n"
                         "© 2024 - Tous droits réservés")


def main():
    """Lancer l'application PySide6"""
    print("=" * 60)
    print("STRUCTURES DE DONNÉES - PREMIUM EDITION")
    print("Interface PySide6 avec accélération GPU")
    print("=" * 60)
    
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Style moderne
    
    # Fenêtre principale
    window = MainWindow()
    window.showMaximized()
    
    sys.exit(app.exec())


if __name__ == "__main__":
    main()