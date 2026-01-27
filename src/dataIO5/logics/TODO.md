# TODO

Nette Idee mit den shared pointern, aber katastrophal für die Performance.

1. Überflüssige Vorab-Substitutionen durch intelligente "Nur-Variablen-wenn-sie-drankommen-Strategie" ersetzen
1. Substitution::apply sollte const& statt shared_ptr erhalten und leeren shared_ptr zurückgeben, wenn nichts geändert wurde.
1. Pool für alle Variablennamen, danach nur noch mit ID (Index in einen vector) arbeiten. "Bloß keine Stringvergleiche!" ;-)
1. Für Terme einen vector, z.B.
    ```
    struct Term {
        uint8_t type;    // Variable, Konstante, Funktion
        uint32_t symbol; // ID für den Namen
        uint32_t first_child; // Index im Vector
        uint32_t next_sibling; 
    };
    ```
1. Unifikation nicht als map, sondern als vector. Man kann eigentlich ein großes Array nehmen, in dem alle möglichen Variablen Platz haben und eine Belegung für eine Variable an die dem Index des Variablennamens im Namenspool zugehörige Stelle legen. Am Besten für alle möglichen Variablen einen zusammengehörigen Integerbereich am besten von 0 an reservieren und Funktionsnamen dahinter, oder wenn Funktionsnamen eher begrenzt sind umgekehrt.
1. Bei der Unifikation selber kein rekursiver Abstieg, sondern einen Vector als Stack benutzen in den zu vergleichende Termpaare gelegt werden.