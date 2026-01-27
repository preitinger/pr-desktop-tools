# TODO

Map erstellen: std::map<Type, TypeDecl> (aufpassen wegen move keine Referenzen sicher! - oder evtl doch mit unique_ptr arbeiten...)

Dann je eine TypeDecl mit lokalem Namen für alle inneren zusammengesetzten Typen in allen Typdefinitionen (vor allem auch Typen von Attributen) erstellen und in obige Map einfügen.

Dann je Eintrag in der Map entweder je eine lokale write- und Lese-Funktion oder eine globale write- und Lese-Funktion für lokale respektive globale Typen erstellen auf Basis der umgeformten Definitionen, die nur noch Literale, AttributeListen oder Referenzen sind.
Aber für die Erstellung des schreib-, Lese-Funktionspaares muss dann jeder Typ umgekehrt umgeformt werden ähnlich wie ValidUnion, damit die Tags bestimmt werden können.

## Konzept noch mal sauber

Zuerst muss das Chaos behoben werden: der Type Variant, soll nicht Werte, sondern shared pointer als Alternativen haben. Durchdenken ob das wirklich gut ist

1. Typdeklarationen im Input stur parsen und unverändert in einem Container sammeln
1. Feststellen, wie viele Unterstriche am Anfang von Typen im Input vorkommen; als lokales Präfix dann einen Unterstrich mehr nehmen, z.B. wenn vorkommt: `type _X = ...`, wird das lokale Präfix: `'__'`
1. Alle Typausdrücke in allen Typdeklarationen ggf. durch Referenzen ersetzen, so dass nur Referenzen übrig bleiben. Dabei neue lokale Typdeklarationen erstellen mit sicher eindeutigem Namen durch Voranstellen eines sonst nie vorkommenden Präfixes. Durch je eine Map in jede Richtung Duplikate vermeiden.
1. Für jede Typdeklaration (globale wie lokale) je eine der folgenden Funktionen generieren:
    - falls Typdeklaration Templateargumente enthält:
        - `genWrite${name}`
        - `genRead${name}`
    - sonst:
        - `write${name}`
        - `read${name}`