## Task 3.4

### Möglichkeit 1: Normalenvektoren als Linien rendern (visuelle Inspektion der Richtung)
Diese Methode zeichnet kleine Linien entlang der Normalenvektoren, um ihre Richtung und Länge visuell zu überprüfen. Wenn die Linien senkrecht zur Oberfläche stehen und gleich lang sind (normalisiert), sind die Normalen korrekt. Fehlerhafte Normalen würden schräg oder zu kurz/lang erscheinen.

#### Vorgehen:

- **Shader anpassen:** Erstelle einen separaten Shader (oder modifiziere task03.frag), der die Normalen als Linien rendert. Im Vertex-Shader (task03.vert) musst du die Normalen an den Fragment-Shader weitergeben. Im Fragment-Shader kannst du die Normalen skalieren und als Linien zeichnen.
  - Öffne task03.vert und stelle sicher, dass die Normalen (z.B. als out vec3 normal;) ausgegeben werden.
  - In task03.frag: Verwende die Normalen, um eine Farbe zu berechnen (z.B. Normalen als RGB: gl_FragColor = vec4(normal * 0.5 + 0.5, 1.0);). Für Linien kannst du einen Geometry-Shader hinzufügen, aber da dein Setup wahrscheinlich nur Vertex/Fragment hat, zeichne stattdessen die Normalen als separate Geometrie.

- **Code in task03.cpp anpassen:** Erstelle eine Hilfsfunktion, die aus den Vertices kleine Linien generiert (z.B. von jedem Vertex entlang der Normalen).
  - Füge nach dem Laden der Shader eine neue Funktion hinzu, z.B.:

```c++
std::vector<Vertex> CreateNormalLines(const std::vector<Vertex>& vertices, float lineLength = 0.1f) {
    std::vector<Vertex> lines;
    for (const auto& v : vertices) {
        // Startpunkt: Position des Vertex
        lines.push_back({v.position, v.normal, {1.0f, 1.0f, 0.0f}}); // Gelb für Start
        // Endpunkt: Position + Normal * Länge
        Vec3f endPos = v.position + v.normal * lineLength;
        lines.push_back({endPos, v.normal, {1.0f, 1.0f, 0.0f}}); // Gelb für Ende
    }
    return lines;
}
```
- !
  - Erstelle dann VAO/VBO für diese Linien, ähnlich wie bei den anderen Geometrien.
  - Im Render-Loop: Zeichne die Linien zusätzlich zu den Objekten mit glDrawArrays(GL_LINES, 0, lines.size());.

- **Testen:** Starte das Programm und schaue dir die Linien an. Für eine Kugel sollten die Normalen radial nach außen zeigen. Für einen Zylinder sollten sie tangential zur Mantelfläche stehen. Drehe die Kamera (später in 3.6 implementiert), um alle Seiten zu prüfen. Wenn Linien schräg oder ungleich sind, überprüfe deine Create*-Funktionen (z.B. Normalisierung mit Normalize() in rgl_math.h).
- **Dokumentation:** Notiere in einem Kommentar oder separaten Dokument: "Normalen-Linien-Test: Linien zeigen korrekt senkrecht zur Oberfläche für alle Geometrien. Beispiel: Bei der Kugel zeigen alle Linien radial nach außen, Länge ~0.1f."

#### Möglichkeit 2: Normalenvektoren als Farben darstellen (Shader-basierte Validierung)

Diese Methode nutzt den Fragment-Shader, um die Normalen direkt als RGB-Farben zu rendern. Normalisierte Normalen (Länge 1) ergeben Farben im Bereich [-1,1], die du auf [0,1] skalierst. Korrekte Normalen zeigen eine gleichmäßige Farbverteilung (z.B. Blau für Z-Achse). Fehlerhafte Normalen führen zu ungleichmäßigen oder falschen Farben.

#### Vorgehen:

- **Shader modifizieren:** Passe task03.frag an, um die Normalen als Farbe auszugeben, anstatt Beleuchtung zu berechnen.
  - In task03.vert: Stelle sicher, dass out vec3 fragNormal; gesetzt wird (z.B. fragNormal = normal;).
  - In task03.frag: Ersetze die Beleuchtung temporär durch:

```c++
in vec3 fragNormal;
void main() {
    // Normalen als RGB-Farbe darstellen (skaliert auf [0,1])
    vec3 color = (fragNormal + 1.0) * 0.5; // -1..1 -> 0..1
    gl_FragColor = vec4(color, 1.0);
}
```

- - Speichere die Original-Fragmente separat, um später zurückzuschalten.

- **Code in task03.cpp:** Keine großen Änderungen nötig – die Normalen werden bereits an den Shader übergeben. Stelle sicher, dass die Vertex-Attribute korrekt gebunden sind (siehe dein CreateGeometryBuffers-Lambda).
- **Testen:** Kompiliere und starte das Programm. Die Objekte sollten nun in Farben erscheinen, die den Normalen entsprechen (z.B. rote Flächen für X-Normalen, grüne für Y, blaue für Z). Bei einer Kugel siehst du einen Farbgradienten von Rot/Grün/Blau. Vergleiche mit erwarteten Werten: Normalen sollten normalisiert sein (Länge 1), also keine Über- oder Untersättigung. Wenn Farben falsch sind, prüfe die Normalenberechnung in CreateSphere/CreateCylinder (z.B. Normalize(p1)).
- **Dokumentation:** Notiere: "Normalen-Farb-Test: Normalen werden als RGB-Farben dargestellt. Beispiel: Kugel zeigt gleichmäßigen Gradienten (Blau oben, Rot rechts), was auf korrekte Normalisierung hinweist. Fehler: Ungleichmäßige Farben bei Zylinder-Mantel deuten auf falsche Tangential-Normalen hin."

## Task 3.5

**Vertex-Shader (task03.vert):**
- Normalen werden korrekt mit der inversen Transponierten der Model-View-Matrix transformiert, um Skalierung und Rotation zu berücksichtigen.
- Wenn ein Objekt z.B. nur auf der X-Achse in die Länge gezogen wird (skalieren), ändert sich die Neigung der Oberfläche. Würde die Normale mit der gleichen Matrix transformiert werden, würde sie in die gleiche Richtung gestreckt werden und stünde nicht mehr senkrecht (orthogonal) auf der Oberfläche.

**Fragment-Shader (task03.frag):**
- Implementiert Lambert'sches Cosinusgesetz für diffuse Beleuchtung mit Punktlichtquelle, plus Ambient-Licht.

Transformation von Normalen:
- Normalen müssen mit der inversen Transponierten der Model-Matrix transformiert werden, da Skalierung die Länge der Normalen ändert. Ohne dies würden skalierte Objekte falsch beleuchtet werden.
- Mathematisch: `N' = mat3(transpose(inverse(M))) * N`

Das Gesetz besagt, dass die Beleuchtungsintensität proportional zum Cosinus des Winkels zwischen der Normalen (N) und der Lichtrichtung (L) ist:
`diffuse = max(0, dot(N, L)) * lightColor * materialColor`

## Task 3.6

#### Tastenbelegung:
- **WASD**: Bewegung entlang der lokalen Achsen der Kamera
  - W: Vorwärts (entlang Forward-Vektor)
  - S: Rückwärts (entgegengesetzt zu Forward)
  - A: Links (entgegengesetzt zu Right)
  - D: Rechts (entlang Right-Vektor)
- **Q/E**: Hoch/Runter (entlang Up-Vektor)
  - Q: Hoch (entlang Up)
  - E: Runter (entgegengesetzt zu Up)
- **Pfeiltasten**: Rotation
  - UP: Pitch nach oben (positive Rotation um Right-Achse)
  - DOWN: Pitch nach unten (negative Rotation um Right-Achse)
  - LEFT: Yaw nach links (positive Rotation um Up-Achse)
  - RIGHT: Yaw nach rechts (negative Rotation um Up-Achse)
- **Page Up/Down**: Roll
  - Page Up: Roll im Uhrzeigersinn (positive Rotation um Forward-Achse)
  - Page Down: Roll gegen Uhrzeigersinn (negative Rotation um Forward-Achse)

#### Winkelvorzeichen und Verhalten:
- Bewegungen sind relativ zur aktuellen Orientierung der Kamera. Zum Beispiel bewegt W die Kamera in die Richtung, in die sie schaut, nicht unbedingt entlang der Welt-Z-Achse. Dies ermöglicht freie Navigation in der 3D-Szene, um alle Objekte aus verschiedenen Winkeln zu betrachten und die Beleuchtung zu validieren.


## Task 3.7

### Matrix-Stack-Animation implementiert
Eine animierte Szene mit einem Matrix-Stack wurde erstellt, die eine Hand mit Fingern simuliert. Die Animation nutzt alle drei Transformationsarten (Translate, Rotate, Scale) und baut den Stack auf mindestens drei Ebenen auf.

#### MatrixStack-Klasse:
- **Push()**: Speichert die aktuelle Matrix auf dem Stack.
- **Pop()**: Entfernt die oberste Matrix vom Stack.
- **Translate(Vec3f)**: Multipliziert die aktuelle Matrix mit einer Translationsmatrix.
- **Rotate(float angle, Vec3f axis)**: Multipliziert mit einer Rotationsmatrix um die gegebene Achse.
- **Scale(Vec3f)**: Multipliziert mit einer Skalierungsmatrix.
- **Last()**: Gibt eine Referenz auf die oberste Matrix zurück.

#### Animierte Szene: Hand mit Fingern
- **Handbasis**: Ein skalierter Zylinder als Handfläche.
- **Daumen**: Ein Kegel, der um die Z-Achse rotiert (sinusförmig animiert).
- **Zeigefinger, Mittelfinger, Ringfinger**: Zylinder, die um die X-Achse rotieren (verschiedene Frequenzen für natürliche Bewegung).
- Die Finger werden relativ zur Handbasis positioniert, rotiert und skaliert, wodurch der Stack auf drei Ebenen aufgebaut wird (Hand -> Fingerbasis -> Fingerrotation).

#### Zeitmessung für framerate-unabhängige Animation:
- Verwendet `SDL_GetPerformanceCounter()` und `SDL_GetPerformanceFrequency()` zur Messung der Zeit pro Frame.
- `animationTime` wird pro Frame um `msPerFrame / 1000.0` erhöht.
- Rotationen basieren auf `sinf(animationTime * frequency)`, um flüssige, periodische Bewegungen zu erzeugen.

#### Beispiel für Stack-Nutzung:
```cpp
MatrixStack matrixStack{};
matrixStack.Push(); // Ebene 1: Hand
matrixStack.Translate(Vec3f{0.0f, -1.0f, 0.0f});
matrixStack.Scale(Vec3f{0.5f, 1.0f, 0.5f});
// Render Hand
matrixStack.Push(); // Ebene 2: Finger
matrixStack.Translate(Vec3f{0.0f, 0.5f, 0.0f});
matrixStack.Rotate(sinf(animationTime * 2.5f) * 0.4f, Vec3f{1, 0, 0}); // Ebene 3: Rotation
matrixStack.Translate(Vec3f{0.0f, 0.5f, 0.0f});
// Render Finger
matrixStack.Pop(); // Zurück zu Ebene 2
matrixStack.Pop(); // Zurück zu Ebene 1
```

Die Animation läuft kontinuierlich und zeigt eine simulierte Handbewegung, die den Matrix-Stack effektiv demonstriert.
