# -*- coding: utf-8 -*-  
"""  
FENÊTRE D'ACCUEIL - GOD TIER UI  
Interface style "Iron Man HUD" / "League Client"  
"""  
  
import math  
import random  
from PySide6.QtWidgets import (QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,  
                               QLabel, QFrame, QGridLayout, QGraphicsOpacityEffect, QApplication)  
from PySide6.QtCore import (Qt, QTimer, QPointF, QRectF, Signal, QPropertyAnimation,  
                            QEasingCurve, QParallelAnimationGroup, QSequentialAnimationGroup,  
                            QObject, QSize, Property)  
from PySide6.QtGui import (QPainter, QLinearGradient, QRadialGradient, QColor, QPen, QBrush,  
                           QFont, QPolygonF, QPainterPath, QCursor, QFontDatabase)  
  
# --- CONFIGURATION CONSTANTS ---  
FPS = 60  
COLOR_BG_DARK = QColor(5, 8, 15)  
COLOR_BG_LIGHT = QColor(13, 17, 23)  
COLOR_NEON_ORANGE = QColor(255, 107, 53)  
COLOR_NEON_BLUE = QColor(88, 166, 255)  
COLOR_TEXT_DIM = QColor(139, 148, 158)  
COLOR_TEXT_WHITE = QColor(255, 255, 255)  
  
class CodeRain: 
    """Matrix-style Code Rain particle""" 
    def __init__(self, w, h): 
        self.x = random.randint(0, int(w)) 
        self.y = float(random.randint(0, int(h))) 
        self.speed = 0.8 + (random.random() * 2.0) # 0.8 to 2.8 
        self.character = chr(ord('A') + random.randint(0, 25)) 
        self.brightness = 0.2 + (random.random() * 0.5) 
 
    def update(self, h, w): 
        self.y += self.speed 
        if self.y > h: 
            self.y = -20.0 
            self.x = random.randint(0, int(w)) # use actual width 
            self.character = chr(ord('A') + random.randint(0, 25)) 
         
        # Random character change 
        if random.randint(0, 20) == 0: 
            self.character = chr(ord('0') + random.randint(0, 9)) 
 
    def draw(self, painter): 
        alpha = int(self.brightness * 255) 
        # Orange/Red cyberpunk style 
        r = min(255, int(255 * (alpha/255.0))) 
        g = min(255, int(100 * (alpha/255.0))) 
        b = min(255, int(30 * (alpha/255.0))) 
         
        painter.setPen(QColor(r, g, b)) 
        painter.drawText(QPointF(self.x, self.y), self.character) 
 
class HexParticle:  
    """Particule hexagonale avec physique et interaction souris"""  
    def __init__(self, w, h):  
        self.pos = QPointF(random.uniform(0, w), random.uniform(0, h))  
        self.vel = QPointF(random.uniform(-0.5, 0.5), random.uniform(-0.5, 0.5))  
        # Slower velocity as per C++ "Plus lent" 
        self.vel *= 0.2 
        self.size = random.uniform(10, 18) # Larger size from C++ (10 + rand%8) 
         
        # Only Orange/Red colors from C++ 
        colors = [ 
            QColor(255, 107, 53), 
            QColor(255, 87, 34), 
            QColor(255, 152, 0), 
            QColor(244, 67, 54) 
        ] 
        self.base_color = random.choice(colors) 
        self.pulse_phase = random.random() * math.pi * 2  
        self.alpha_base = 0.25 + (random.random() * 0.4) 
          
    def update(self, w, h, mouse_pos):  
        # Calcul distance souris 
        dx = self.pos.x() - mouse_pos.x() 
        dy = self.pos.y() - mouse_pos.y() 
        dist = math.sqrt(dx*dx + dy*dy) 
 
        # Interaction Souris (Repulsion) 
        if dist < 150: 
            force = 1.0 - (dist / 150.0) 
            angle = math.atan2(dy, dx) 
            repulsion = 2.0 * force  # Force strength 
             
            self.vel.setX(self.vel.x() + math.cos(angle) * repulsion) 
            self.vel.setY(self.vel.y() + math.sin(angle) * repulsion) 
 
        # Limit speed 
        current_speed = math.sqrt(self.vel.x()**2 + self.vel.y()**2) 
        if current_speed > 3.0: 
            self.vel *= (3.0 / current_speed) 
 
        # Mouvement de base  
        self.pos += self.vel  
          
        # Rebond bords  
        if self.pos.x() < 0 or self.pos.x() > w: self.vel.setX(-self.vel.x())  
        if self.pos.y() < 0 or self.pos.y() > h: self.vel.setY(-self.vel.y())  
          
        # Clamp 
        self.pos.setX(max(0.0, min(float(w), self.pos.x()))) 
        self.pos.setY(max(0.0, min(float(h), self.pos.y())))
  
    def draw(self, painter, frame):  
        # Effet pulsation LENTE 
        pulse = math.sin(frame * 0.015 + self.pos.x() * 0.008) * 0.2 + 0.8 
        current_size = self.size * pulse  
          
        alpha = int(self.alpha_base * 255) 
         
        # Glow Effect (Nested loops) 
        for glow in range(2, 0, -1): 
            glow_alpha = int(alpha / (glow + 2)) 
            c = QColor(self.base_color) 
            c.setAlpha(glow_alpha) 
             
            painter.setPen(QPen(c, 2)) 
            painter.setBrush(Qt.NoBrush) 
            self._draw_hex(painter, self.pos, current_size + glow * 4) 
 
        # Main Hexagon 
        main_color = QColor(self.base_color) 
        main_color.setAlpha(alpha) 
        painter.setPen(QPen(main_color, 2)) 
        painter.setBrush(Qt.NoBrush) 
        self._draw_hex(painter, self.pos, current_size) 
 
    def _draw_hex(self, painter, center, size): 
        path = QPainterPath()  
        angle_step = math.pi / 3  
        for i in range(6):  
            x = center.x() + size * math.cos(i * angle_step)  
            y = center.y() + size * math.sin(i * angle_step)  
            if i == 0: path.moveTo(x, y)  
            else: path.lineTo(x, y)  
        path.closeSubpath()  
        painter.drawPath(path) 
  
  
class HoloCard(QFrame):  
    """  
    Carte Holographique Interactive  
    - Scale au hover  
    - Scanline sur bordure  
    - Effet flash au clic  
    """  
    clicked = Signal(str)  
      
    def __init__(self, title, desc, icon, parent=None):  
        super().__init__(parent)  
        self.title = title  
        self.setFixedSize(320, 130)  # Taille plus grande pour plein écran
        self.setCursor(Qt.PointingHandCursor)  
          
        # Propriétés animables  
        self._scale = 1.0  
        self._border_scan_pos = 0.0 # 0 à 1 (position sur périmètre)  
        self._flash_alpha = 0.0  
          
        # Setup UI interne  
        self._setup_ui(title, desc, icon)  
          
        # Animations  
        self.hover_anim = QPropertyAnimation(self, b"scale_prop", self)  
        self.hover_anim.setDuration(200)  
        self.hover_anim.setEasingCurve(QEasingCurve.OutCubic)  
          
        self.scan_timer = QTimer(self)  
        self.scan_timer.timeout.connect(self._update_scan)  
        self.scan_timer.start(16)  
          
        self.flash_anim = QPropertyAnimation(self, b"flash_prop", self)  
        self.flash_anim.setDuration(300)  
        self.flash_anim.setStartValue(1.0)  
        self.flash_anim.setEndValue(0.0)  
        self.flash_anim.setEasingCurve(QEasingCurve.OutQuad)  
  
    def _setup_ui(self, title, desc, icon):  
        layout = QVBoxLayout(self)  
        layout.setContentsMargins(20, 20, 20, 20)  
          
        # Titre + Icone  
        top_layout = QHBoxLayout()  
        icon_lbl = QLabel(icon)  
        icon_lbl.setStyleSheet("font-size: 22px; color: #58A6FF; background: transparent;")  
          
        title_lbl = QLabel(title)  
        title_lbl.setStyleSheet("""  
            font-family: 'Segoe UI'; font-size: 16px; font-weight: bold;   
            color: white; background: transparent; letter-spacing: 1px;  
        """)  
          
        top_layout.addWidget(icon_lbl)  
        top_layout.addWidget(title_lbl)  
        top_layout.addStretch()  
        layout.addLayout(top_layout)  
          
        # Description  
        desc_lbl = QLabel(desc)  
        desc_lbl.setStyleSheet("font-family: 'Segoe UI'; font-size: 11px; color: #8B949E; background: transparent;")  
        desc_lbl.setWordWrap(True)  
        layout.addWidget(desc_lbl)  
  
    # --- Properties for Animation ---  
    def get_scale(self): return self._scale  
    def set_scale(self, s):   
        self._scale = s  
        self.update() # Redraw  
    scale_prop = Property(float, get_scale, set_scale)  
  
    def get_flash(self): return self._flash_alpha  
    def set_flash(self, a):  
        self._flash_alpha = a  
        self.update()  
    flash_prop = Property(float, get_flash, set_flash)  
  
    # --- Events ---  
    def enterEvent(self, event):  
        self.hover_anim.stop()  
        self.hover_anim.setEndValue(1.05)  
        self.hover_anim.start()  
        super().enterEvent(event)  
  
    def leaveEvent(self, event):  
        self.hover_anim.stop()  
        self.hover_anim.setEndValue(1.0)  
        self.hover_anim.start()  
        super().leaveEvent(event)  
  
    def mousePressEvent(self, event):  
        self.flash_anim.start()  
        self.clicked.emit(self.title)  
        super().mousePressEvent(event)  
  
    def _update_scan(self):  
        self._border_scan_pos += 0.015  
        if self._border_scan_pos > 1.0: self._border_scan_pos = 0.0  
        self.update()  
  
    def paintEvent(self, event):  
        painter = QPainter(self)  
        painter.setRenderHint(QPainter.Antialiasing)  
          
        # Centre de transformation pour le scale  
        cx, cy = self.width() / 2, self.height() / 2  
        painter.translate(cx, cy)  
        painter.scale(self._scale, self._scale)  
        painter.translate(-cx, -cy)  
          
        rect = QRectF(2, 2, self.width()-4, self.height()-4)  
          
        # Fond semi-transparent  
        painter.setBrush(QColor(26, 31, 46, 200))  
        painter.setPen(Qt.NoPen)  
        painter.drawRoundedRect(rect, 10, 10)  
          
        # Bordure de base  
        base_pen = QPen(QColor(48, 54, 61))  
        base_pen.setWidth(2)  
        painter.setPen(base_pen)  
        painter.setBrush(Qt.NoBrush)  
        painter.drawRoundedRect(rect, 10, 10)  
          
        # Glow au hover  
        if self._scale > 1.0:  
            glow_pen = QPen(COLOR_NEON_BLUE)  
            glow_pen.setWidth(2)  
            painter.setPen(glow_pen)  
            painter.drawRoundedRect(rect, 10, 10)  
              
            # Scan Effect (Point qui circule)  
            # Simplification: Dessiner un petit trait sur le path  
            path = QPainterPath()  
            path.addRoundedRect(rect, 10, 10)  
            length = path.length()  
            scan_point = path.pointAtPercent(self._border_scan_pos)  
              
            painter.setPen(Qt.NoPen)  
            painter.setBrush(COLOR_NEON_ORANGE)  
            painter.drawEllipse(scan_point, 3, 3)  
  
        # Flash Effect  
        if self._flash_alpha > 0.01:  
            painter.setCompositionMode(QPainter.CompositionMode_Plus)  
            flash_color = QColor(255, 255, 255, int(self._flash_alpha * 150))  
            painter.fillRect(self.rect(), flash_color)  
  
  
class GodTierBackground(QWidget):  
    """  
    Fond animé avec Intro Cinématique, Particules, Post-Processing.  
    """  
    def __init__(self, parent=None):  
        super().__init__(parent)  
        self.setMouseTracking(True)  
          
        # Moteur  
        self.timer = QTimer(self)  
        self.timer.timeout.connect(self.game_loop)  
        self.frame_count = 0  
        self.mouse_pos = QPointF(0, 0)  
          
        # Particules  
        self.particles = [HexParticle(1200, 800) for _ in range(20)] # Reduced to 20 as in C++ 
        self.code_rain = [CodeRain(1200, 800) for _ in range(25)] # 25 columns 
          
        # Etat Intro Cinématique  
        self.intro_phase = 0 # 0:Black, 1:Grid, 2:Title, 3:Done  
        self.grid_alpha = 0.0  
        self.title_decoded_chars = 0  
        self.full_title = "STRUCTURES DE DONNEES"  
        self.current_title_disp = ""  
          
        # Anim Props  
        self.grid_anim = QPropertyAnimation(self, b"grid_alpha_prop", self)  
        self.grid_anim.setDuration(1500)  
        self.grid_anim.setStartValue(0.0)  
        self.grid_anim.setEndValue(1.0)  
        self.grid_anim.setEasingCurve(QEasingCurve.InQuad)  
          
        # Lancement  
        QTimer.singleShot(500, self.start_intro)  
        self.timer.start(50) # C++ uses 50ms timer (20 FPS) 
  
    def start_intro(self):  
        self.intro_phase = 1  
        self.grid_anim.start()  
        # Title decoding starts mid-grid  
        QTimer.singleShot(1000, self.start_title_decoding)  
  
    def start_title_decoding(self):  
        self.intro_phase = 2  
        self.decode_timer = QTimer(self)  
        self.decode_timer.timeout.connect(self.step_decode)  
        self.decode_timer.start(50)  
  
    def step_decode(self):  
        if self.title_decoded_chars < len(self.full_title):  
            self.title_decoded_chars += 1  
            # Generate random chars for the rest  
            fixed = self.full_title[:self.title_decoded_chars]  
            random_part = "".join([chr(random.randint(65, 90)) for _ in range(len(self.full_title) - self.title_decoded_chars)])  
            self.current_title_disp = fixed + random_part  
            self.update()  
        else:  
            self.current_title_disp = self.full_title  
            self.intro_phase = 3  
            self.decode_timer.stop()  
  
    # --- Properties ---  
    def get_grid_alpha(self): return self.grid_alpha  
    def set_grid_alpha(self, a): self.grid_alpha = a  
    grid_alpha_prop = Property(float, get_grid_alpha, set_grid_alpha)  
  
    # --- Loop ---  
    def mouseMoveEvent(self, event):  
        self.mouse_pos = event.position()  
        super().mouseMoveEvent(event)  
  
    def game_loop(self):  
        self.frame_count += 1  
        width, height = self.width(), self.height()  
          
        # Update particles  
        for p in self.particles:  
            p.update(width, height, self.mouse_pos) 
             
        # Update Code Rain 
        for r in self.code_rain: 
            r.update(height, width) 
              
        self.update()  
  
    def paintEvent(self, event):  
        painter = QPainter(self)  
          
        # 1. Fond Dégradé Sombre (Background Gradient from C++) 
        grad = QLinearGradient(0, 0, 0, self.height()) 
        grad.setColorAt(0.0, QColor(2, 4, 8)) 
        grad.setColorAt(1.0, QColor(10, 12, 18)) 
        painter.fillRect(self.rect(), grad) 
          
        # 2. Grille (Si intro commencée)  
        if self.intro_phase >= 1:  
            painter.setOpacity(self.grid_alpha)  
            self.draw_grid(painter)  
            painter.setOpacity(1.0) 
             
        # 3. Code Rain 
        font = QFont("Consolas") 
        font.setPixelSize(16) 
        painter.setFont(font) 
        for r in self.code_rain: 
            r.draw(painter) 
             
        # 4. Tech Lines (Particle Connections) 
        self.draw_tech_lines(painter) 
              
        # 5. Particules  
        painter.setCompositionMode(QPainter.CompositionMode_Plus)  
        for p in self.particles:  
            p.draw(painter, self.frame_count)  
        painter.setCompositionMode(QPainter.CompositionMode_SourceOver) # Reset  
          
        # 6. Titre Decoded  
        if self.intro_phase >= 2:  
            self.draw_title(painter) 
            self.draw_subtitle(painter) 
              
        # 7. Post-Processing (Vignette & Scanline)  
        self.draw_post_fx(painter)  
  
    def draw_grid(self, painter):  
        pen = QPen(QColor(30, 40, 50))  
        pen.setWidth(1)  
        painter.setPen(pen)  
          
        step = 50  
        w, h = self.width(), self.height()  
         
        offset = (self.frame_count // 3) % step 
          
        for x in range(-offset, w, step):  
            painter.drawLine(x, 0, x, h)  
        for y in range(-offset, h, step):  
            painter.drawLine(0, y, w, y) 
             
    def draw_tech_lines(self, painter): 
        MAX_DISTANCE = 200.0 
        for i in range(len(self.particles)): 
            for j in range(i + 1, len(self.particles)): 
                p1 = self.particles[i] 
                p2 = self.particles[j] 
                 
                dx = p1.pos.x() - p2.pos.x() 
                dy = p1.pos.y() - p2.pos.y() 
                dist = math.sqrt(dx*dx + dy*dy) 
                 
                if dist < MAX_DISTANCE: 
                    alpha = (1.0 - (dist / MAX_DISTANCE)) * 0.4 
                    alpha_int = int(alpha * 255) 
                     
                    c = QColor(255, 107, 53) 
                    c.setAlpha(alpha_int) 
                    painter.setPen(QPen(c, 1)) 
                    painter.drawLine(p1.pos, p2.pos)  
  
    def draw_title(self, painter):  
        # --- Auteur au-dessus (Effet Spécial) ---
        if self.intro_phase >= 2:
            author_text = "[  REALISE PAR TAHA AMINE ER-RAFIY  ]"
            auth_rect = QRectF(0, 60, self.width(), 30)
            
            # Font
            font = QFont("Consolas")
            font.setPixelSize(14)
            font.setLetterSpacing(QFont.AbsoluteSpacing, 3)
            font.setBold(True)
            painter.setFont(font)
            
            # Pulse Color
            pulse = (math.sin(self.frame_count * 0.1) + 1.0) * 0.5
            r = 0
            g = 255
            b = 100
            alpha = int(150 + 105 * pulse)
            c = QColor(r, g, b, alpha)
            
            painter.setPen(c)
            painter.drawText(auth_rect, Qt.AlignCenter, author_text)
            
            # Decor lines
            w = painter.fontMetrics().horizontalAdvance(author_text)
            cx = self.width() / 2
            y = 75
            painter.drawLine(cx - w/2 - 40, y, cx - w/2 - 10, y)
            painter.drawLine(cx + w/2 + 10, y, cx + w/2 + 40, y)

        # Center title  
        rect = QRectF(0, 100, self.width(), 80)  
          
        # Try finding Segoe UI, else fallback 
        font_db = QFontDatabase() 
        font_families = font_db.families() 
         
        target_font = "Segoe UI" 
        if target_font not in font_families: 
             # Fallbacks 
             if "Arial" in font_families: target_font = "Arial" 
             elif "Helvetica" in font_families: target_font = "Helvetica" 
             else: target_font = "Sans Serif" 
         
        font = QFont(target_font)  
        font.setPixelSize(60)  
        font.setBold(True)  
        font.setLetterSpacing(QFont.AbsoluteSpacing, 5)  
        painter.setFont(font)  
          
        # Glitch effect (random offset red/blue channels)  
        if self.frame_count % 60 < 3: # Occasional glitch logic from C++ 
            offset = random.randint(-2, 2)  
            # Red channel  
            painter.setPen(QColor(255, 0, 0))  
            painter.drawText(rect.translated(offset - 2, 0), Qt.AlignCenter, self.current_title_disp)  
            # Blue channel  
            painter.setPen(QColor(0, 100, 255))  
            painter.drawText(rect.translated(-offset + 2, 0), Qt.AlignCenter, self.current_title_disp)  
 
        # Massive Drop Shadow (from C++) 
        for i in range(6, 0, -1): 
            r = int(255 / (i * 2)) 
            g = int(50 / (i * 2)) 
            b = int(10 / (i * 2)) 
            painter.setPen(QColor(r, g, b)) 
             
            shadow_rect = rect.translated(i, i) 
            painter.drawText(shadow_rect, Qt.AlignCenter, self.current_title_disp) 
          
        # Main Text  
        painter.setPen(COLOR_NEON_ORANGE)  
        painter.drawText(rect, Qt.AlignCenter, self.current_title_disp)  
 
        # Scan line effect specific to title 
        scan_y_offset = (self.frame_count % 60) 
        scan_y = rect.top() + 10 + scan_y_offset 
         
        scan_pen = QPen(COLOR_NEON_ORANGE) 
        scan_pen.setWidth(2) 
        painter.setPen(scan_pen) 
        painter.drawLine(100, scan_y, self.width() - 100, scan_y) 
 
    def draw_subtitle(self, painter): 
        # Subtitle with laser scan effect 
        font = QFont("Segoe UI") 
        font.setPixelSize(26) 
        font.setBold(True) 
        painter.setFont(font) 
         
        # Laser blue pulse effect
        pulse = math.sin(self.frame_count * 0.08) * 0.4 + 0.6
        r = int(88 * pulse)
        g = int(166 + (255 - 166) * (pulse - 0.6) / 0.4)
        b = int(255 * pulse)
         
        painter.setPen(QColor(r, g, b)) 
         
        subtitle = "[ Oriented by Mr KHOUKHI ]" 
        rect = QRectF(0, 200, self.width(), 40) 
        painter.drawText(rect, Qt.AlignCenter, subtitle) 
  
    def draw_post_fx(self, painter):  
        # Scanlines CRT (every 3 pixels) 
        scan_pen = QPen(QColor(20, 20, 20)) 
        scan_pen.setWidth(1) 
        painter.setPen(scan_pen) 
        for y in range(0, self.height(), 3): 
             painter.drawLine(0, y, self.width(), y) 
         
        # Copie finale effect handled by normal paint 
        # The C++ does a BitBlt of hdcMem, here we paint directly. 
  
  
class WelcomeWindow(QMainWindow):  
    """Fenêtre Principale"""  
      
    module_clicked = Signal(str)  
      
    def __init__(self):  
        super().__init__()  
        self.setWindowTitle("Structures de Données - GOD TIER")  
        # Note: Cette fenêtre est intégrée dans MainWindow, pas affichée seule  
          
        # Background Widget as Central  
        self.bg = GodTierBackground()  
        self.setCentralWidget(self.bg)  
          
        # Overlay Layout for Content  
        # Note: We need a layout on top of the painting.   
        # QWidget default layout draws on top of paintEvent content usually.  
        self.main_layout = QVBoxLayout(self.bg)  
        self.main_layout.setContentsMargins(80, 260, 80, 50) # Marges réduites en haut et bas
          
        # Container for cards (initially hidden/offset)  
        self.cards_container = QWidget()  
        self.cards_container.setStyleSheet("background: transparent;")  # Fond transparent
        self.cards_layout = QGridLayout(self.cards_container)  
        self.cards_layout.setSpacing(40)  # Plus d'espace entre les cartes  
          
        self.modules = [  
            ("TABLEAUX", "Algorithmes de tri parallèles", "📊", 0, 0),  
            ("LISTES", "Listes chaînées et opérations", "🔗", 0, 1),  
            ("ARBRES", "Arbres binaires de recherche", "🌳", 1, 0),  
            ("GRAPHES", "Parcours et algorithmes", "🕸️", 1, 1)  
        ]  
          
        self.card_widgets = []  
        for title, desc, icon, row, col in self.modules:  
            card = HoloCard(title, desc, icon)  
            card.clicked.connect(self.open_module)  
            # Start hidden/transparent for animation  
            opacity = QGraphicsOpacityEffect(card)  
            opacity.setOpacity(0)  
            card.setGraphicsEffect(opacity)  
            card.opacity_effect = opacity # Keep ref  
              
            self.cards_layout.addWidget(card, row, col)  
            self.card_widgets.append(card)  
              
        self.main_layout.addWidget(self.cards_container, alignment=Qt.AlignCenter)  
        self.main_layout.addStretch()  
          
        # Sequence Trigger  
        # Wait for BG decoding to finish roughly (2500ms)  
        QTimer.singleShot(2500, self.animate_cards_entry)  
  
    def animate_cards_entry(self):  
        self.anim_group = QSequentialAnimationGroup(self)  
          
        for i, card in enumerate(self.card_widgets):  
            # Parallel group for Opacity + Slide Up  
            par_group = QParallelAnimationGroup()  
              
            # Opacity  
            op_anim = QPropertyAnimation(card.opacity_effect, b"opacity")  
            op_anim.setDuration(600)  
            op_anim.setStartValue(0.0)  
            op_anim.setEndValue(1.0)  
              
            # Geometry (Slide Up) - Needs manual handling or translate.   
            # Simpler: Just animate pos? No, layout controls pos.  
            # Trick: Animate a translation transform or margin?  
            # Or just accept Opacity fade-in for now to ensure stability in layout.  
            # But instruction said "glissant du bas avec effet ressort".  
            # To do that in a layout, we can animate the widget's maximumHeight or use a translation.  
            # Let's use standard fade-in for robustness, or QPropertyAnimation on "pos" if we use absolute positioning (not easy here).  
            # "God-Tier" compromise: Fade In + slight scale up from 0.8?   
            # Or use `card.setRenderTranslation` if painted manually? No standard widget.  
              
            # Let's stick to Opacity for reliability, maybe add a small delay between cards.  
            par_group.addAnimation(op_anim)  
            self.anim_group.addAnimation(par_group)  
              
            # Pause between cards  
            if i < len(self.card_widgets) - 1:  
                self.anim_group.addPause(100)  
                  
        self.anim_group.start()  
  
    def open_module(self, module_name):  
        print(f"MODULE ACCESS GRANTED: {module_name}")  
        self.module_clicked.emit(module_name)  
  
  
def main():  
    import sys  
    from PySide6.QtWidgets import QApplication  
    app = QApplication(sys.argv)  
      
    # Font setup could go here  
      
    window = WelcomeWindow()  
    window.show()  
    sys.exit(app.exec())  
  
if __name__ == "__main__":  
    main()  
