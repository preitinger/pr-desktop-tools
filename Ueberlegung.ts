// nur fuer die Ueberlegung:
type _TemplateArg = {

}
type _ValidType = {
    type: 'Literal'
    val: string
} | {
    type: 'Reference'
    name: string;
    templateArgs: _TemplateArg[]
}
type _Attribute = {
    name: string;
    type: _ValidType;
}
type _AttributeList = _Attribute[]


type I32 = number;
type Utf = string;
type DataOut = 'DataOut'
type DataIn = 'DataIn'
type Writer<T> = (dout: DataOut, x: T) => void
type Reader<T> = (din: DataIn) => T

const writeI32: Writer<I32> = (dout, x) => {
    // dout.i32(x)
}
const readI32: Reader<I32> = (din) => Math.random();

function writeTag(dout: DataOut, tag: number) {
    // dout.u8(tag)
}

function readTag(din: DataIn) {
    return Math.random();
}

type Fies<T, S> = {
    type: 'a';
    t: T;
} | {
    type: 'b';
    t: T;
    s: S;
}

type Fies2<T> = Fies<Fies3<T, I32, T>, T>
type Fies3<T, S, R> = {
    type: '3a';
    t: T
} | {
    type: '3b';
    s: S
} | {
    type: '3c';
    r: R
}

type Fies4 = {
    fies: Fies<I32, Utf>
    fies2: Fies2<Utf>
    fies3: Fies3<Fies2<I32>, Utf, I32>
    fiesUnion: Fies2<I32> | Fies3<Utf, I32, Utf>
}

const fies4: Fies4 = {
    fies: {
        type: 'b',
        t: 1,
        s: ''
    },
    fies2: {
        type: 'b',
        t: {
            type: '3b',
            s: 32
        },
        s: 'x'
    },
    fies3: {
        type: '3a',
        t: { // Fies2<I32> = Fies<Fies3<I32, I32, I32>, I32>
            type: 'b',
            t: {
                type: '3a',
                t: 1
            },
            s: 32,
        },
    },
    fiesUnion: {
        type: '3c',
        r: 'r'
    }
}

// temporaer:

type _Fies2_0<T> = Fies3<T, I32, T>
type _Fies2<T> = Fies<_Fies2_0<T>, T>

/**
 *  nur fuer mich zur Ueberpruefung:
 */
function convertFies2<T>(fies2: Fies2<T>): _Fies2<T> {
    return fies2;
}

/**
 *  nur fuer mich zur Ueberpruefung:
 */
function convertFies2_rev<T>(fies2: _Fies2<T>): Fies2<T> {
    return fies2;
}



const fiesValidated = {
    literals: [],
    attributeLists: [
        [
            {
                name: 'type',
                type: {
                    type: 'Literal',
                    val: 'a'
                }
            },
            {
                name: 't',
                type: {
                    type: 'Reference',
                    name: 'T',
                    templateArgs: [],
                }
            }],
        [
            {
                name: 'type',
                type: {
                    type: 'Literal',
                    val: 'b'
                }
            },
            {
                name: 't',
                type: {
                    type: 'Reference',
                    name: 'T',
                    templateArgs: [],
                }
            },
            {
                name: 's',
                type: {
                    type: 'Reference',
                    name: 'S',
                    templateArgs: [],
                }
            }
        ]
    ]
}

// Fies faktorisieren, nichts zu tun, aber deklarieren
type _Fies<T, S> = {
    type: 'a';
    t: T;
} | {
    type: 'b';
    t: T;
    s: S;
}

function genWriteFies<T, S>(writeT: Writer<T>, writeS: Writer<S>) {
    return function (dout: DataOut, x: Fies<T, S>) {
        switch (x.type) {
            case 'a':
                writeTag(dout, 0);
                writeT(dout, x.t);
                break;
            case 'b':
                writeTag(dout, 1);
                writeT(dout, x.t);
                writeS(dout, x.s);
                break;
            default:
                console.log('never');
        }
    }
}

const fies2Validated = {
    literals: [],
    attributeLists: [ // TODO ersetzen: T/_Fies2_0<T>, S/T
        [
            {
                name: 'type',
                type: {
                    type: 'Literal',
                    val: 'a'
                }
            },
            {
                name: 't',
                type: {
                    // type: 'Reference',
                    // name: 'T',
                    // templateArgs: [],

                    type: 'Reference',
                    name: '_Fies2_0',
                    templateArgs: ['T']
                }
            }],
        [
            {
                name: 'type',
                type: {
                    type: 'Literal',
                    val: 'b'
                }
            },
            {
                name: 't',
                type: {
                    // type: 'Reference',
                    // name: 'T',
                    // templateArgs: [],

                    type: 'Reference',
                    name: '_Fies2_0',
                    templateArgs: ['T']
                }
            },
            {
                name: 's',
                type: {
                    type: 'Reference',
                    // name: 'S',
                    // templateArgs: [],
                    name: 'T',
                    templateArgs: [],
                }
            }
        ]
    ]
}

function genWrite_Fies2_0<T>(writeT: Writer<T>): Writer<_Fies2_0<T>> {
    type __X = _Fies2_0<T>;
    /*
validate(__X) = 
validate(_Fies2_0<T>) = 
validate(Fies3<T, I32, T>) =
validate({
    type: '3a';
    t: T
} | {
    type: '3b';
    s: I32
} | {
    type: '3c';
    r: T
})
    */
    const write__X: Writer<__X> = (dout, x) => {
        switch (x.type) {
            case '3a':
                writeTag(dout, 0);
                writeT(dout, x.t);
                break;
            case '3b':
                writeTag(dout, 1);
                writeI32(dout, x.s);
                break;
            case '3c':
                writeTag(dout, 2);
                writeT(dout, x.r);
                break;
        }
    }
    return write__X;
}

// _Fies2 als faktorisierten Fies2 deklarieren:
type _Fies2<T> = _Fies<_Fies3<T, I32, T>, T>

function genWriteFies2<T>(writeT: Writer<T>): Writer<Fies2<T>> {
    type __X = _Fies2<T>
    /*
validate(__X) =
validate(_Fies2<T>) =
validate(Fies<_Fies2_0<T>, T>) =
validate({
    type: 'a'
    t: _Fies2_0<T>
} | {
    type: 'b'
    t: _Fies2_0<T>
    s: T
})
    */
    type __0 = _Fies2_0<T>
    /*
... =
validate({
    type: 'a'
    t: __0
} | {
    type: 'b'
    t: __0
    s: T
})
    */
    const write__0: Writer<__0> = genWrite_Fies2_0<T>(writeT);
    const write__X: Writer<__X> = (dout, x) => {
        switch (x.type) {
            case 'a':
                writeTag(dout, 0);
                write__0(dout, x.t);
                break;
            case 'b':
                writeTag(dout, 1);
                write__0(dout, x.t);
                writeT(dout, x.s);
                break;
        }
    }

    return write__X;
}

function genRead_Fies2_0<T>(readT: Reader<T>): Reader<_Fies2_0<T>> {
    type __X = _Fies2_0<T>
    const read__X: Reader<__X> = (din) => {
        const tag = readTag(din);
        switch (tag) {
            case 0:
                return {
                    type: '3a',
                    t: readT(din)
                }
            case 1:
                return {
                    type: '3b',
                    s: readI32(din)
                }
            default:
                throw new Error('invalid tag');
        }
    }

    return read__X;
}

function genReadFies2<T>(readT: Reader<T>): Reader<Fies2<T>> {
    type __X = _Fies2<T>
    /*
validate(__X) =
validate(_Fies2<T>) =
validate(Fies<_Fies2_0<T>, T>) =
// Nach Templateparametereinsetzung dann wieder faktorisieren:
validate(Fies<__0, T>) =
validate(__1) =
*/
    type __0 = _Fies2_0<T>
    type __1 = Fies<__0, T>

    /*
    ... =
    validate(Fies<__0, T>) =
    validate({
        type: 'a';
        t: __0;
    } | {
        type: 'b';
        t: __0;
        s: T;
    })
    */
    const read__0: Reader<__0> = genRead_Fies2_0<T>(readT);
    const read__X: Reader<__X> = (din) => {
        const tag = readTag(din);
        switch (tag) {
            case 0:
                return {
                    type: 'a',
                    t: read__0(din),
                }
            case 1:
                return {
                    type: 'b',
                    t: read__0(din),
                    s: readT(din)
                }
            default:
                throw new Error('Invalid tag');
        }
    }
    return read__X;
}

function genWriteFies3<T, S, R>(writeT: Writer<T>, writeS: Writer<S>, writeR: Writer<R>): Writer<Fies3<T, S, R>> {
    type __X = Fies3<T, S, R>
    /*
    validate(__X) =
    validate(Fies3<T, S, R>) =
    validate({
        type: '3a';
        t: T
    } | {
        type: '3b';
        s: S
    } | {
        type: '3c';
        r: R
    })
    // Keine weitere Faktorisierung moeglich, ENDE
    */
    const write__X: Writer<__X> = (dout, x) => {
        switch (x.type) {
            case '3a':
                writeTag(dout, 0);
                writeT(dout, x.t);
                break;
            case '3b':
                writeTag(dout, 1);
                writeS(dout, x.s);
                break;
            case '3c':
                writeTag(dout, 2);
                writeR(dout, x.r);
                break;
        }
    }

    return write__X;
}

function genReadFies3<T, S, R>(readT: Reader<T>, readS: Reader<S>, readR: Reader<R>): Reader<Fies3<T, S, R>> {
    type __X = Fies3<T, S, R>
    /*
    validate(__X) =
    validate(Fies3<T, S, R>) =
    validate({
        type: '3a';
        t: T
    } | {
        type: '3b';
        s: S
    } | {
        type: '3c';
        r: R
    })
    // Keine weitere Faktorisierung moeglich, ENDE
    */
    const read__X: Reader<__X> = (din) => {
        const tag = readTag(din);
        switch (tag) {
            case 0:
                return {
                    type: '3a',
                    t: readT(din)
                }
            case 1:
                return {
                    type: '3b',
                    s: readS(din)
                }
            case 2:
                return {
                    type: '3c',
                    r: readR(din)
                }
            default:
                throw new Error('Invalid tag');
        }
    }

    return read__X;
}

type _Fies4_0 = Fies<I32, Utf>
type _Fies4_1 = Fies2<Utf>
type _Fies4_2 = Fies2<I32>
type _Fies4_3 = Fies3<_Fies4_2, Utf, I32>
type _Fies4_4 = Fies3<Utf, I32, Utf>
type _Fies4_5 = _Fies4_2 | _Fies4_4
type _Fies4 = {
    fies: _Fies4_0
    fies2: _Fies4_1
    fies3: _Fies4_3
    fiesUnion: _Fies4_5
}

/**
 * nur fuer mich zur Ueberpruefung:
 */
function convertFies4(fies4: Fies4): _Fies4 {
    return fies4;
}

/**
 * nur fuer mich zur Ueberpruefung:
 */
function convertFies4_rev(fies4: _Fies4): Fies4 {
    return fies4;
}

const write_Fies4: Writer<_Fies4> = (dout, x) => {
    /* Faktorisieren: _Fies4_0, ..., _Fies4_5, _Fies4
    validate(_Fies4) =
    validate({
        fies: _Fies4_0
        fies2: _Fies4_1
        fies3: _Fies4_3
        fiesUnion: _Fies4_5
    })
    // Fertig, da keine Referenz oder Union mehr enthalten
    */

    // nur eine AttributeList, also kein Tag!
    write_Fies4_0(dout, x.fies);
    write_Fies4_1(dout, x.fies2);
    write_Fies4_3(dout, x.fies3);
    write_Fies4_5(dout, x.fiesUnion);
}

export const writeFies4 = write_Fies4;

const write_Fies4_0: Writer<_Fies4_0> = (dout, x) => {
    /* Faktorisieren
}