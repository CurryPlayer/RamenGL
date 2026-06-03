1. Schatten-Berechnung (Pass 1 & 2)
* Pass 1 (Shadow Map): Die Szene wird aus der Sicht des Lichts gerendert. Ich nutze dafür die neue OrthographicProjection, um eine gerichtete Lichtquelle (wie die Sonne) zu simulieren. Das Ergebnis wird in einer 2048x2048 Depth-Texture gespeichert.
* Pass 2 (Scene Rendering): Sowohl der Boden als auch das Modell berechnen nun für jedes Fragment die Position im "Light Space". Durch einen Vergleich der Fragment-Tiefe mit dem Wert in der Shadow Map wird entschieden, ob das Fragment im Schatten liegt.

2. Behebung von Artefakten (wie in der Aufgabe gefordert)
* Shadow Acne: Im Fragment-Shader berechne ich einen dynamischen bias basierend auf dem Winkel zwischen Normalenvektor und Lichtrichtung. Dies verhindert die typischen Streifenmuster auf Oberflächen.
* Peter Panning: Im ersten Pass nutze ich glCullFace(GL_FRONT). Das rendert nur die Rückseiten der Objekte in die Shadow Map, was Lücken zwischen Objekt und Schatten minimiert.
* Over-sampling: Fragmente außerhalb des Lichtkegels werden explizit als "nicht im Schatten" markiert, um schwarze Ränder am Horizont zu vermeiden.

3. Visuelle Erweiterungen & UI
* Ground Texture: Ich habe eine Textur für die Plane hinzugefügt (linux-quake). Da die Plane-Vertices keine UV-Koordinaten haben, nutze ich Planar Mapping im Shader (XZ-Koordinaten werden als UVs verwendet).
* Modell-Integration: Das Modell nutzt weiterhin Environment Mapping (Reflektionen), reagiert aber nun zusätzlich auf den Schattenwurf.
* ImGui: Steuern der Lichtposition, Schatten-Aktivierung und Boden-Textur über das UI-Panel.