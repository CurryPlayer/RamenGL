# Task 03

## 3.0) Setup
Clonen Sie das aktuelle Rahmenprogramm aus GitLab.

**ACHTUNG: Die Updates im aktuellen Rahmenprogramm brechen
einige Annahmen aus task02. Clonen Sie das Rahmenprogramm deshalb
unbedingt neu, damit Sie aus task02 nichts kaputt machen.**

Nutzen Sie das Sourcefile `task03.cpp` um die Aufgaben zu implementiern.
Kompilieren Sie es zunächst und testen Sie, ob es sich starten lässt
und Sie ein Koordinatensystem sehen.

## 3.1) Nutzen der Farbattribute.
Alle drei Achsen des Koordinatensystem werden im Moment in rot
gerendert. Erweitern Sie das Programm, sodass der Vertexshader
das Farbattribut der Vertices erhält. Erweitern Sie den Shadercode
ebenfalls, damit diese Farbattribute an den Fragmentshader weitergereicht
werden.

## 3.2) Zylinder, Quader, Kugeln
Bauen Sie sich Funktionen, die Ihnen **mindestens** die folgenden
Geometrien erzeugen: Zylinder, Kugeln, Quader. 
Die Funktionen sollen ausserdem einen Parameter zur Bestimmung
der Farbe entgegennehmen. Beispiel:
```bash
std::vector<Vertex> CreateCylinder(const Vec3f& color);
```
**Achtung**: Vergessen Sie nicht, sinnvolle Normalenvektoren
zu erzeugen.

## 3.3) Rendern der Geometrie
Testen Sie Ihre geometrischen Körper, indem Sie diese rendern.
Das Hochladen der Daten auf die GPU und das Rendern erfolgt wie
schon in Taks 02. 

## 3.4) Debugging der Normalenvektoren
Wie testen Sie, ob Ihre Normalenvektoren korrekt sind und auch korrekt
im Shader sind? Nutzen Sie mindesten zwei Möglichkeiten.
Dokumentieren Sie Ihr vorgehen.

## 3.5) Beleuchtung
Im Moment ist die Szene noch sehr flach, da kein Beleuchtungsmodell
implementiert wurde. Erweitern Sie ihren Shadercode, sodass
die Szene durch eine Punktlichtquelle beleuchtet wird.
Recherchieren Sie hierzu das *Lambert cosine law*.
Gibt es bei der Transformation von Modellen hinsichtlich
der Normalenvektoren etwas zu beachten? Falls ja,
erleutern Sie was das Problem ist und leiten Sie
eine geeignete mathematische Lösung her.

## 3.6) Bewegung der Kamera
Erweitern Sie das Programm, sodass Sie die Kamera mit 
der Tastatur steuern können. Sie sollten in der Lage sein,
die Kamera entlang ihrer Achsen zu verschieben und zu rotierern.

## 3.7) Matrizen-Stack
Bauen Sie eine animierte Szene mithilfe eines Matrizenstapels und
den von Ihnen erstellten primitiven Körpern. Natürlich dürfen Sie
weitere geometrische Körper erstellen, falls Sie diese für
Ihre Szene benötigen (beispielsweise einen Kegel).  
Implementieren Sie hierzu am besten ein Klasse, welche die 
Methoden aus der Vorlesung zur Verfügung stellt.

Beispiel
```c
MatrixStack matrixStack{};
matrixStack.Push();
for ( int i = 0; i < numModels; i++ )
{
    matrixStack.rotate(RAMEN_WORLD_FORWARD, angle);
    matrixStack.translate(Vec3f{ 1.0f, 0.0f, -0.5f });
    glUniformMatrix4fv(0, 1, GL_FALSE, matrixStack.Last().Data());
    glDrawArrays(GL_TRIANGLES, 0, model.NumVertices());
}
matrixStack.Pop();
```
Nutzen Sie intern die Methoden aus der Mathematik-Bibliothek `rgl_math.h`.

Nutzen Sie alle drei Transformationsarten (Rotate, Scale, Translate) und
bauen Sie eine Animation, die den Matrixstack mindestens auf eine Tiefe 
(bzw. Höhe) von **drei** Matrizen ausbaut. Beispielsweise können Sie
die Bewegung eines Fingers simulieren.

Damit die Animation unabhängig von der Framerate läuft, stoppen Sie die
Zeit, die ein Frame benötigt. Hier können Sie folgenden Code nutzen:
```c
Uint64      ticksPerSecond = SDL_GetPerformanceFrequency();
Uint64      startCounter   = SDL_GetPerformanceCounter();
Uint64      endCounter     = SDL_GetPerformanceCounter();
while ( isRunning )
{
    double ticksPerFrame = (double)endCounter - (double)startCounter;
    double msPerFrame    = (ticksPerFrame / (double)ticksPerSecond) * 1000.0;
    startCounter         = SDL_GetPerformanceCounter();

    // Event loop, rendering, etc.

    endCounter = SDL_GetPerformanceCounter();
}
```
Sebstverstädnlich können Sie auch Funktionen aus der C++ Standardbibliothek
nutzen, wenn Ihnen das lieber ist.

## 3.8) Ship It.
Nutzen Sie das Script `shipit.sh` auf UNIX, bzw. `shipit.bat` auf Windows,
um einen Release-build Ihres Programms zu erstellen und es mit den assets
zu zippen.  
**HINWEIS**: Das Skript wird noch nachgeliefert! Achten Sie auf
Ankündigungen im Moodle-Kurs.
