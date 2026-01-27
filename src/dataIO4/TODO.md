# TODO

------------------
Klasse Substitution
* Liste von Paaren von je einer Variable und einem Term, d.h. `TypeExp`

------------------
Methode `Sp<const TypeExp> TypeExp::substitute(const Substitution& s) const`

------------------
`TsAttributeList::TsAttributeList(...)`
* im Konstruktor prüfen, ob gültiges Attribut mit Namen `type` enthalten ist.
    * Falls ja
        * `Sp<const TsAttribute> TsAttributeList::validType` setzen
    * Falls nein
        * `Sp<const TsAttribute> TsAttributeList>>validType` leer lassen, auch wenn Name zwar `type`, aber Typ kein Literal ist.

------------------
`virtual Sp<const TypeExp> TypeExp::factorize(DeclCol& declCol, const Str& prefixForNew) const = 0`

------------------
`virtual Sp<const TypeExp> TsLiteral::factorize(DeclCol& declCol, const Str& prefixForNew) override`

------------------
`virtual Sp<const TypeExp> TsReference::factorize(DeclCol& declCol, const Str& prefixForNew) override`
* Je templateArg
    * Suchen in declCol
        * Falls gefunden
            * templateArg durch gefundene Variable ersetzen
        * Falls nicht gefunden

------------------
`virtual Sp<const TypeExp> TsAttributeList::factorize(DeclCol& declCol, const Str& prefixForNew) override`

------------------
`virtual Sp<const TypeExp> TsUnion::factorize(DeclCol& declCol, const Str& prefixForNew) override`

------------------
`virtual Sp<const TypeExp> TsArray::factorize(DeclCol& declCol, const Str& prefixForNew) override`
* Basistyp faktorisieren
* Prüfen, ob durch Faktorisierung geändert
    * Ja
        * neues Array mit neuem Basistyp zurückgeben
    * Nein
        * `return shared_from_this();`


------------------
`class ValidateException`

------------------
Interface `class ValidType`

------------------
`class SimpleValidType : public ValidType`
* `exp: TypeExp`
    * beinhaltet wahlweise:
        * ein einzelnes Literal
        * eine unauflösbare Referenz, für die je eine benutzerdefinierte Schreib- und Lese-Funktion (ggf. generisch) vorausgesetzt wird.

------------------
`class ValidAttribute`
* name
* `type: Sp<const ValidType> type;

------------------
`class ValidAttributeList : public ValidType`
* `Vec<Sp<const ValidAttribute>> attributes;`

------------------
`class ValidUnion : public ValidType`
* literals
* attributeLists
* Methoden
    * `void addLiteral(Sp<TSLiteral> literal)`
        * Ignorieren, falls schon vorhanden
    * `void addAttributeList(Sp<TSAttributeList> attributeList)`
        * Prüfen, ob type vorhanden und Typ TSLiteral hat und Wert des Literals noch nicht in `this->attributeLists` vorhanden ist.
        * Falls ja:
            * in `attributeLists` hinzufügen
        * Falls nein:
            * `ValidateException`

------------------
`virtual ValidType TypeExp::validate(DeclCol& declCol) const = 0`

------------------
`TSLiteral::validate override`
* `SimpleValidType` zurueckgeben

------------------
`ValidType TSUnion::validate(DeclCol& declCol) const override`
* throw `ValidateException`, z.B. 
    * Array in Union
    * Name in Union not defined: 'Bla'
    * Wrong number of template args for 'Bla': 'n', but requires 'm'
* add decl for each substituted reference in `declCol`
* Ablauf:
    * Lokale Variable `auto validUnion = ms<ValidUnion>();`
    * Für jeden Eintrag `alternative` in `alternatives`:
        * `alternative->validateAsUnionChild(declCol, *validUnion);`
    * `return validUnion;`

------------------
`ValidType TSReference::validate(DeclCol& declCol) const override`
* Suche eigenen Namen in `declCol`
    * Falls gefunden: `decl: Decl`
        * Vergleiche Anzahl der Templateparameter
            * Falls verschieden
                * `ValidateException`: Wrong number of template args for 'Bla': 'n', but requires 'm'
            * Falls gleich
                * Ersetze in gefundenem `decl.exp` jeden formalen Templateparameter in `decl.templateArgs` durch den entsprechenden `TypeExp` in `this->templateArgs` und erhalte `substituted`
                * Rekursiv: `return substituted->validate(declCol);`
    * Falls nicht gefunden
        * gebe `ValidReference` zurück
* Überprüfen ob in `.tailr1`-Datei in Kommentar zu dieser Funktion `Eliminated tail recursion` steht.

------------------
`virtual void TypeExp::validateAsUnionChild(DeclCol& declCol, ValidType& validType) const = 0`

------------------
`void TSReference::validateAsUnionChild(DeclCol& declCol, ValidType& validType) const override`
* Suche eigenen Namen in `declCol`
    * Falls gefunden: `decl: Decl`
        * Vergleiche Anzahl der Templateparameter
            * Falls verschieden
                * `ValidateException`: Wrong number of template args for 'Bla': 'n', but requires 'm'
            * Falls gleich
                * Ersetze in gefundenem `decl.exp` jeden formalen Templateparameter in `decl.templateArgs` durch den entsprechenden `TypeExp` in `this->templateArgs` und erhalte `substituted`
            * Rekursiv: `substituted->validate(declCol, validType);`
    * Falls nicht gefunden
        * `throw ValidateException("Name in Union not defined: 'Bla'");`
* Überprüfen ob in `.tailr1`-Datei in Kommentar zu dieser Funktion `Eliminated tail recursion` steht.
