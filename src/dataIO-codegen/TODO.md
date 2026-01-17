### TODO Type.H & Co

1. Erst alle Typen parsen.
1. Erst danach die Funktionen für alle Typen generieren.
1. Beim Parsen nicht nur Alternativen von eingebetteten Objekten erlauben (die aktuelle Definition von Variants), sondern auch Alternativen von Synonymen.
1. Beim Parsen Set aller geparsten Type-Objekte aufstellen
1. Diesen Set readonly an Type::genWrite() übergeben, damit die Synonyme aufgelöst werden können.
1. Beim Auflösen der Synonyme rekursiv Vereinigung aller Variants bilden, dabei ggf. Zyklus feststellen und dann entweder auf std::cerr Fehler ausgeben, oder Kommentar mit Hinweis auf Scheiterung beim gegebenen Typ generieren, oder sogar absichtlich eine fehlerhafte Zeile generieren, damit es schnell auffällt, und/oder ein assert(false) generieren etc.

#### Siehe Beispiel in pr-test-serwist/app/io-types.ts: `type ProblemC<A, B>`
