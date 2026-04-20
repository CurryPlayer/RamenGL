**Zusammenfassung der verwendeten OpenGL Funktionen (DSA - Direct State Access)**

Um ein 3D-Modell in modernem OpenGL (ab v4.5) auf der Grafikkarte zu rendern, müssen die Modelldaten (Vertices) in den Speicher der GPU geladen. Hierfür werden Buffer (VBOs) und Vertex Array Objects (VAOs) genutzt.

* **`glCreateBuffers`**: Erstellt ein oder mehrere Buffer-Objekte und initialisiert ihren internen Zustand sofort. Dies ist der moderne (DSA) Weg, Puffer zu erstellen.
* **`glNamedBufferSubData`**: Schreibt oder aktualisiert Daten direkt in einem spezifischen Buffer, ohne dass dieser vorher mit `glBindBuffer` an den globalen Zustand gebunden werden muss. (Setzt voraus, dass der Speicher vorher allokiert wurde).
* **`glCreateVertexArrays`**: Erstellt und initialisiert ein Vertex Array Object (VAO). Das VAO speichert alle Konfigurationen darüber, wie unsere Vertex-Daten gelesen werden sollen.
* **`glVertexArrayVertexBuffer`**: Sagt dem VAO, welcher Buffer (VBO) an welchem "Binding-Punkt" verwendet werden soll. Hier wird auch festgelegt, wie groß der Abstand (Stride) von einem Vertex zum nächsten im Speicher ist.
* **`glVertexArrayAttribFormat`**: Beschreibt das Format eines einzelnen Attributs (z. B. Position). Hier legen wir fest, dass die Position aus 3 `GL_FLOAT` Werten besteht und wo innerhalb eines einzelnen Vertex-Datenblocks (Offset) sie anfängt.
* **`glVertexArrayAttribBinding`**: Verknüpft einen Attribut-Index (z. B. `layout(location = 0)` im Shader) mit einem Buffer-Binding-Punkt (den wir zuvor mit `glVertexArrayVertexBuffer` definiert haben).
* **`glEnableVertexArrayAttrib`**: Schaltet das entsprechende Vertex-Attribut im VAO frei, sodass der Shader darauf zugreifen kann.
* **`glBindVertexArray`**: Bindet unser fertig konfiguriertes VAO an den aktuellen OpenGL-Zustand. Dies ist nötig, kurz bevor wir den Draw-Call absetzen.
* **`glDrawArrays`**: Weist die GPU an, die Daten aus dem aktuell gebundenen VAO zu lesen und daraus Geometrie (z.B. `GL_TRIANGLES`) zu zeichnen.

**Der Unterschied zwischen `glCreateBuffers` und `glGenBuffers`**

* **`glGenBuffers` (Alt / Legacy):** Reserviert lediglich einen *Namen* (eine ID) für einen Buffer. Das tatsächliche Buffer-Objekt in OpenGL wird noch **nicht** erstellt. Erst wenn man `glBindBuffer` aufruft, generiert OpenGL das Objekt im Hintergrund. Dies erzwingt die Manipulation über den globalen Zustandsautomaten (State Machine).
* **`glCreateBuffers` (Neu / DSA):** Generiert nicht nur die ID, sondern erstellt und initialisiert das Buffer-Objekt **sofort**. Dadurch kann man Funktionen wie `glNamedBufferData` oder `glNamedBufferSubData` aufrufen, um Daten hochzuladen, *ohne* den Buffer binden zu müssen. Das macht den Code sicherer und verhindert Seiteneffekte durch unbeabsichtigte Bindungen.
