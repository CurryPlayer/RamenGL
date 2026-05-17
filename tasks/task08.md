# Task 08 - Phong-Blinn Beleuchtungsmodell und Normalmapping.

## 8.0 Phong-Blinn Beleuchtungsmodell

Erweitern Sie Ihren Renderer um das **Phong-Blinn Beleuchtungsmodell**.
Bauen Sie dazu auf dem Code aus Task 07 auf.
Eine detaillierte Abhandlung des Beleuchtungsmodells finden Sie in
*Computergrafik, Band 1 - Nischwitz et al.* in **Kapitel 12.1.3**.

Der Boden soll nach wie vor von der Beleuchtung ausgenommen und
ausschliesslich durch die ihm zugewiesene Textur gerendert werden.

Fügen Sie dem Programm ein weiteres UI-Panel hinzu über
das Sie **emissive**, **ambiente**, **diffuse** und **spekulare**
Material-Farbeigenschaften einstellen können. Verwenden Sie hierzu
`ImGui::ColorPicker3()`. Des weiteren fügen Sie einen Regler hinzu,
der Ihnen das Einstellen des **shininess**-Faktors für die
spekulare Beleuchtung erlaubt.

Erlauben Sie weiterhin die Möglichkeit, dass die Oberfläche Ihres
Objekts die Umgebungstexture der Cubemap refklektiert. Erstellen
Sie hierzu ebenfalls eine UI-Komponente, die das Regeln der
**reflectivness** ermöglicht.

Führen Sie die Beleuchtungsberechnungen sowohl **pro Vertex** als
auch **pro Fragment** durch. Implementieren Sie ein UI-Element,
sodass Sie zwischen den beiden Methoden umschalten können.
Erläutern Sie, wie sich die Berechnung pro Fragment
im Gegensatz zur Lichtberechnung pro Vertex unterscheidet und wie
sich die beiden Verfahren nennen.
Welche visuellen Unterschiede gibt es und wann sind diese besonders
auffällig? Was ist gerade bei der Berechnung pro Fragment
im Fragmentshader zu beachten?

## 8.1 Normalmapping (TBA)
