Hi Jens,

ich spiele wieder rum mit meine gpx tracks. Da ich dich gerne anschreibe, dachte ich, ich teile es mal mit dir :-) Vielleicht bestaetigt es oder widerspricht es, was du erwartest.

Ich will die kommende Steigung auf mein (alten) etrex 10 anzeigen. Dafuer misshandle ich die Funktion "next waypoint" und benenne die Waypoints so:"nummer-steigung-zeit", zB "01-06-12:35". Zeit ist die Sollankunftzeit, ergibt sich aus eine 15kmh Sollschnitt. Der GPS zeigt noch an, wieviele kilometer bis zum waypoint, und die aktuelle Uhrzeit. Also sehe ich zB:
|----------------|
| 01-06-12:35    |
| 12:15 # 5.2 km |
|----------------|
Ich muss die waypoints vorberechnen und will also die kommende Steigung ("06" im Beispiel) zwischen die waypoints (alle 10 km etwa) berechnen. Ich kann das nicht per hand machen, das sind viel zuviele Teilstrecken. Also brauch ich ein super-smart Algorithmus.

Als Test nehme ich mir eine Fahrt, die ich letztes Wochenende gefahren bin. Ich habe drei websites die Hm berechnen lassen: 
- gpx.studio (daher kommt auch mein Test track "elevation.gpx"), 
- komoot, 
- outdooractive (oa). 
Ich lade die gpx hoch ("elevation.gpx"), lass die Hm berechnen von komoot oder oa, und lade den track wieder als gpx runter  (elevation-komoot bzw elevation-oa).

Mein Algorithmus ist der triviale (keine Filterung, nichts, einfach addieren, siehe elevation.py). Hier das, was rauskommt, und im Vergleich, dass was die jeweiligen webseiten berechnen ("expected"):
```
loading test/elevation.gpx
computed elevation=1285.6
expected elevation=1257.0

loading test/elevation-komoot.gpx
computed elevation=1282.1
expected elevation=1390.0

loading test/elevation-oa.gpx
computed elevation=1807.9
expected elevation=1099.0
```

Man sieht: drei verschiedene Hoehenmodelle:
```
test/elevation.gpx:    <trkpt lat="48.98175" lon="10.911422">
test/elevation.gpx-        <ele>410.4</ele>

test/elevation-komoot.gpx:      <trkpt lat="48.981750" lon="10.911422">
test/elevation-komoot.gpx-        <ele>410.463828</ele>

test/elevation-oa.gpx:      <trkpt lat="48.98175" lon="10.911422">
test/elevation-oa.gpx-        <ele>411.59359</ele>
```

Von Hoehenprofilvergleich hat 
- gpx.studio den groeberen HModell,
- oa den feinsten  HModell (bei weitem),
- komoot liegt dazwischen, aber eher wie gpx.studio.
(Der Hprofil von oa sieht am unruhigsten, viele kleine Zickzack, zum Beispiel).

Ich glaub ich bleib mit dieser Kombi (gpx.studio, keine Filterung). Interessanterweise sehe ich, dass wenn jemand mir ein gpxtrack mit Hoehenangaben sendet, ich kann nicht einfach daraus die Hm berechnen. Ich muss seine Hoehenangaben mit den von gpx.studio ersetzen.

So. Das wars, hat Spass gemacht :-)

LG
Julien 
