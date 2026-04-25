## 4.2

### 1. Reservierung des Namens
```cpp
GLuint textureHandle;
glCreateTextures(GL_TEXTURE_2D, 1, &textureHandle);
```
* **Was es tut:** Dieser Befehl erzeugt ein neues Texturobjekt im Speicher der GPU.
* **Details:** `textureHandle` ist am Ende einfach eine ID (eine Zahl), mit der du diese spezifische Textur später jederzeit wieder aufrufen kannst. Das `GL_TEXTURE_2D` sagt OpenGL, dass es sich um ein flaches 2D-Bild handelt.

---

### 2. Konfiguration (Sampling-Parameter)
Die nächsten vier Zeilen legen fest, wie die Grafikkarte reagiert, wenn die Textur "verformt" oder "unpassend" angezeigt wird.

```cpp
glTextureParameteri(textureHandle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTextureParameteri(textureHandle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
```
* **Wrap-Modus:** Was passiert, wenn Texturkoordinaten außerhalb von $[0.0, 1.0]$ liegen?
* `GL_CLAMP_TO_EDGE` sagt: "Wiederhole das Bild nicht, sondern ziehe den letzten Pixel am Rand einfach weiter." ($S$ ist die horizontale, $T$ die vertikale Achse).

```cpp
glTextureParameteri(textureHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTextureParameteri(textureHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
```
* **Filter-Modus:** Wie wird die Textur gezeichnet, wenn sie kleiner (`MIN`) oder größer (`MAG`) als das Original skaliert wird?
* `GL_NEAREST` bedeutet "Pixel-Art-Look": Es wird immer der am nächsten liegende Pixel gewählt, ohne zu verschwimmen. (Die Alternative wäre `GL_LINEAR` für eine weiche Filterung).



---

### 3. Speicherplatz reservieren
```cpp
glTextureStorage2D(textureHandle, 1, GL_RGBA8, image.GetWidth(), image.GetHeight());
```
* **Was es tut:** Dies ist die "Platzreservierung". Die GPU reserviert einen festen Speicherblock für ein Bild dieser Größe.
* **Wichtig:** `GL_RGBA8` definiert das interne Format (8 Bit pro Kanal: Rot, Grün, Blau, Alpha). Die `1` gibt an, wie viele Mipmap-Level (verkleinerte Versionen des Bildes) erstellt werden sollen – hier nur das Original.

---

### 4. Daten übertragen
```cpp
glTextureSubImage2D(textureHandle, 0, 0, 0, image.GetWidth(), image.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
```
* **Was es tut:** Jetzt erst werden die tatsächlichen Bilddaten vom RAM deines Computers in den reservierten Speicher der Grafikkarte kopiert.
* **Parameter:** Hier gibst du an, welches Format die Quelldaten haben (`GL_RGBA` und `GL_UNSIGNED_BYTE`) und wo der Zeiger auf die Pixeldaten im RAM liegt (`image.Data()`).

---

## 4.4

Der Hauptunterschied zwischen glBindTexture und glBindTextureUnit liegt in der API-Version (Modern vs. Legacy), der Art der Zustandsänderung und der Benutzerfreundlichkeit.
Computer Graphics Stack Exchange
Computer Graphics Stack Exchange
+1
glBindTextureUnit ist eine modernere Funktion (ab OpenGL 4.5), die Teil des sogenannten Direct State Access (DSA) ist.
Hier ist die detaillierte Gegenüberstellung:
1. glBindTexture(GL_TEXTURE_2D, textureHandle);
   Funktionsweise: Bindet eine Textur an den aktuell aktiven Textur-Target (GL_TEXTURE_2D) der aktuell aktiven Textureinheit.
   Ablauf: Erfordert meist zwei Schritte:
   glActiveTexture(GL_TEXTURE0 + unit); (Bestimmt die Einheit)
   glBindTexture(GL_TEXTURE_2D, textureHandle); (Bestimmt die Textur)
   Zustand: Ändert den globalen OpenGL-Zustand (welche Einheit aktiv ist).
   Einsatz: Standard in älteren OpenGL-Versionen (vor 4.5) oder OpenGL ES.
2. glBindTextureUnit(0, textureHandle);
   Funktionsweise: Bindet die Textur (textureHandle) direkt an die angegebene Textureinheit (hier 0), ohne dass diese vorher mit glActiveTexture aktiviert werden muss.
   Ablauf: Nur ein einziger Aufruf notwendig.
   Zustand: Ändert nicht den globalen Zustand der aktiven Textur (kein glActiveTexture nötig).
   Einsatz: Modernes OpenGL (4.5+), sauberer und effizienter, da weniger Zustandsänderungen (State Changes) nötig sind.
   Vorteil: Die Art der Textur (1D, 2D, 3D, CubeMap) wird automatisch vom Texturobjekt erkannt, man muss nicht explizit GL_TEXTURE_2D angeben.
3. Falls das Bild auf dem Kopf ist, können die UV-Koordinaten vertauscht werden. Der Startpunkt ist häufig unten links (0,0), Texturen beginnen aber häufig oben rechts (1,1).
   Die UVs müssen geändert werden:
   - bottom-left (0,0) → top-right (1,1),
   - bottom-right (1,0) → top-left (0,1),
   - top-right (1,1) → bottom-left (0,0),
   - top-left (0,1) → bottom-right (1,0).

## 4.5

Bilddownload: https://svs.gsfc.nasa.gov/vis/a000000/a002900/a002915/bluemarble-1024.png
