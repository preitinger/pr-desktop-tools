import { readU32, readU8, readUtf, writeU32, writeU8, writeUtf, type DataIn, type DataOut, type Mixed, type MixedA, type MixedB, type MixedInst, type MixedInst2, type MySimpleSynonym, type MySynonym, type OneLiteral, type OneSynonym, type Reader, type Simple, type Simple2, type U8, type Writer } from "./bin-io-types";

type Bla = 'x'[];

const bla: Bla = ['x', 'x'];
type Blubb<T> = T;

const blubb: Blubb<Bla> = ['x']

interface MyInt<T> {
    t: T;
}

export function genWriteMixedA_Variant<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: MixedA<T>) {

        writeU8(dout, x.a);
    }
}

export function genReadMixedA_Variant<T>(readT: Reader<T>) {
    return function (din: DataIn): MixedA<T> {

        return {
            type: 'a',
            a: readU8(din),
        };
    }
}

export function genWriteMixedA<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: MixedA<T>) {
        genWriteMixedA_Variant(writeT)(dout, x);
    }
}

export function genReadMixedA<T>(readT: Reader<T>) {
    return function (din: DataIn): MixedA<T> {
        return genReadMixedA_Variant(readT)(din);
    }
}


// genRndObj:
const rndMixedA = [
    {
        type:         /* RandomAttributeTypeValGen nyi */,
        a:         /* RandomAttributeTypeValGen nyi */,
    },
]

export function genWriteMixedB_Variant<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: MixedB<T>) {
        const write0 = genWriteMixedA(writeT);

        switch (x.type) {
            case 'b':
                writeU8(dout, x.b);
                write0(dout, x.mixedA);
                break;
            case 'c':
                break;
        }
    }
}

export function genReadMixedB_Variant<T>(readT: Reader<T>) {
    return function (din: DataIn, tag: U8): MixedB<T> {
        const read0 = genReadMixedA(readT);

        switch (tag) {
            case 0:
                return {
                    type: 'b',
                    b: readU8(din),
                    mixedA: read0(din),
                };
            case 1:
                return {
                    type: 'c',
                };
            default: throw new Error('Invalid tag for MixedB: ' + tag);
        }
    }
}

export function genWriteMixedB<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: MixedB<T>) {
        switch (x.type) {
            case 'b':
                dout.u8(0);
                break;
            case 'c':
                dout.u8(1);
                break;
        }
        genWriteMixedB_Variant(writeT)(dout, x);
    }
}

export function genReadMixedB<T>(readT: Reader<T>) {
    return function (din: DataIn): MixedB<T> {
        const tag = din.u8();
        return genReadMixedB_Variant(readT)(din, tag);
    }
}


// genRndObj:
const rndMixedB = [
    {
        type:         /* RandomAttributeTypeValGen nyi */,
        b:         /* RandomAttributeTypeValGen nyi */,
        mixedA:         /* RandomAttributeTypeValGen nyi */,
    },
    {
        type:         /* RandomAttributeTypeValGen nyi */,
    },
]

export function genWriteMixed_Variant<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: Mixed<T>) {
        const writeMixedA_Variant = genWriteMixedA_Variant(writeT);
        const writeMixedB_Variant = genWriteMixedB_Variant(writeT);

        switch (x) {
            case 'a':
                // do nothing, here; Only in the main write function, the tag is written for this case.
                break;
            case 'b':
                // do nothing, here; Only in the main write function, the tag is written for this case.
                break;
            default:
                switch (x.type) {
                    case 'a':
                        writeMixedA_Variant(dout, x);
                        break;
                    case 'b':
                        writeMixedB_Variant(dout, x);
                        break;
                    case 'c':
                        writeMixedB_Variant(dout, x);
                        break;
                }
        } // switch
    }
}

export function genReadMixed_Variant<T>(readT: Reader<T>) {
    return function (din: DataIn, tag: U8): Mixed<T> {
        const readMixedA_Variant = genReadMixedA_Variant(readT);
        const readMixedB_Variant = genReadMixedB_Variant(readT);

        switch (tag) {
            case 0:
                return 'a';
            case 1:
                return 'b';
            case 2:
                return readMixedA_Variant(din);
            case 3:
                return readMixedB_Variant(din, 0);
            case 4:
                return readMixedB_Variant(din, 1);
            default: throw new Error('Invalid tag for Mixed: ' + tag);
        }
    }
}

export function genWriteMixed<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: Mixed<T>) {
        switch (x) {
            case 'a':
                dout.u8(0);
                break;
            case 'b':
                dout.u8(1);
                break;
            default:
                switch (x.type) {
                    case 'a':
                        dout.u8(2);
                        break;
                    case 'b':
                        dout.u8(3);
                        break;
                    case 'c':
                        dout.u8(4);
                        break;
                }
        } // switch
        genWriteMixed_Variant(writeT)(dout, x);
    }
}

export function genReadMixed<T>(readT: Reader<T>) {
    return function (din: DataIn): Mixed<T> {
        const tag = din.u8();
        return genReadMixed_Variant(readT)(din, tag);
    }
}


// genRndObj:
const rndMixed = [
]

export function writeMixedInst_Variant(dout: DataOut, x: MixedInst) {
    const writeMixed_Variant = genWriteMixed_Variant(writeUtf);

    switch (x) {
        case 'a':
            // do nothing, here; Only in the main write function, the tag is written for this case.
            break;
        case 'b':
            // do nothing, here; Only in the main write function, the tag is written for this case.
            break;
        default:
            switch (x.type) {
                case 'a':
                    writeMixed_Variant(dout, x);
                    break;
                case 'b':
                    writeMixed_Variant(dout, x);
                    break;
                case 'c':
                    writeMixed_Variant(dout, x);
                    break;
            }
    } // switch
}

export function readMixedInst_Variant(din: DataIn, tag: U8): MixedInst {
    const readMixed_Variant = genReadMixed_Variant(readUtf);

    switch (tag) {
        case 0:
            return 'a';
        case 1:
            return 'b';
        case 2:
            return readMixed_Variant(din, 0);
        case 3:
            return readMixed_Variant(din, 1);
        case 4:
            return readMixed_Variant(din, 2);
        default: throw new Error('Invalid tag for MixedInst: ' + tag);
    }
}

export function writeMixedInst(dout: DataOut, x: MixedInst) {
    switch (x) {
        case 'a':
            dout.u8(0);
            break;
        case 'b':
            dout.u8(1);
            break;
        default:
            switch (x.type) {
                case 'a':
                    dout.u8(2);
                    break;
                case 'b':
                    dout.u8(3);
                    break;
                case 'c':
                    dout.u8(4);
                    break;
            }
    } // switch
    writeMixedInst_Variant(dout, x);
}

export function readMixedInst(din: DataIn): MixedInst {
    const tag = din.u8();
    return readMixedInst_Variant(din, tag);
}


// genRndObj:
const rndMixedInst = [
]

export function writeMixedInst2_Variant(dout: DataOut, x: MixedInst2) {
    const write0 = genWriteMixed(writeUtf);
    const writeMixed_Variant = genWriteMixed_Variant(write0);

    switch (x) {
        case 'a':
            // do nothing, here; Only in the main write function, the tag is written for this case.
            break;
        case 'b':
            // do nothing, here; Only in the main write function, the tag is written for this case.
            break;
        default:
            switch (x.type) {
                case 'a':
                    writeMixed_Variant(dout, x);
                    break;
                case 'b':
                    writeMixed_Variant(dout, x);
                    break;
                case 'c':
                    writeMixed_Variant(dout, x);
                    break;
            }
    } // switch
}

export function readMixedInst2_Variant(din: DataIn, tag: U8): MixedInst2 {
    const read0 = genReadMixed(readUtf);
    const readMixed_Variant = genReadMixed_Variant(read0);

    switch (tag) {
        case 0:
            return 'a';
        case 1:
            return 'b';
        case 2:
            return readMixed_Variant(din, 0);
        case 3:
            return readMixed_Variant(din, 1);
        case 4:
            return readMixed_Variant(din, 2);
        default: throw new Error('Invalid tag for MixedInst2: ' + tag);
    }
}

export function writeMixedInst2(dout: DataOut, x: MixedInst2) {
    switch (x) {
        case 'a':
            dout.u8(0);
            break;
        case 'b':
            dout.u8(1);
            break;
        default:
            switch (x.type) {
                case 'a':
                    dout.u8(2);
                    break;
                case 'b':
                    dout.u8(3);
                    break;
                case 'c':
                    dout.u8(4);
                    break;
            }
    } // switch
    writeMixedInst2_Variant(dout, x);
}

export function readMixedInst2(din: DataIn): MixedInst2 {
    const tag = din.u8();
    return readMixedInst2_Variant(din, tag);
}


// genRndObj:
const rndMixedInst2 = [
]

export function genWriteSimple_Variant<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: Simple<T>) {

        switch (x.type) {
            case 'a':
                writeU8(dout, x.x);
                writeT(dout, x.t);
                break;
            case 'b':
                writeUtf(dout, x.error);
                break;
        }
    }
}

export function genReadSimple_Variant<T>(readT: Reader<T>) {
    return function (din: DataIn, tag: U8): Simple<T> {

        switch (tag) {
            case 0:
                return {
                    type: 'a',
                    x: readU8(din),
                    t: readT(din),
                };
            case 1:
                return {
                    type: 'b',
                    error: readUtf(din),
                };
            default: throw new Error('Invalid tag for Simple: ' + tag);
        }
    }
}

export function genWriteSimple<T>(writeT: Writer<T>) {
    return function (dout: DataOut, x: Simple<T>) {
        switch (x.type) {
            case 'a':
                dout.u8(0);
                break;
            case 'b':
                dout.u8(1);
                break;
        }
        genWriteSimple_Variant(writeT)(dout, x);
    }
}

export function genReadSimple<T>(readT: Reader<T>) {
    return function (din: DataIn): Simple<T> {
        const tag = din.u8();
        return genReadSimple_Variant(readT)(din, tag);
    }
}


// genRndObj:
const rndSimple = [
    {
        type:         /* RandomAttributeTypeValGen nyi */,
        x:         /* RandomAttributeTypeValGen nyi */,
        t:         /* RandomAttributeTypeValGen nyi */,
    },
    {
        type:         /* RandomAttributeTypeValGen nyi */,
        error:         /* RandomAttributeTypeValGen nyi */,
    },
]

export function writeSimple2_Variant(dout: DataOut, x: Simple2) {

    switch (x.type) {
        case 'a2':
            writeU8(dout, x.simple2);
            break;
        case 'b2':
            writeU32(dout, x.simple2);
            break;
    }
}

export function readSimple2_Variant(din: DataIn, tag: U8): Simple2 {

    switch (tag) {
        case 0:
            return {
                type: 'a2',
                simple2: readU8(din),
            };
        case 1:
            return {
                type: 'b2',
                simple2: readU32(din),
            };
        default: throw new Error('Invalid tag for Simple2: ' + tag);
    }
}

export function writeSimple2(dout: DataOut, x: Simple2) {
    switch (x.type) {
        case 'a2':
            dout.u8(0);
            break;
        case 'b2':
            dout.u8(1);
            break;
    }
    writeSimple2_Variant(dout, x);
}

export function readSimple2(din: DataIn): Simple2 {
    const tag = din.u8();
    return readSimple2_Variant(din, tag);
}


// genRndObj:
const rndSimple2 = [
    {
        type:         /* RandomAttributeTypeValGen nyi */,
        simple2:         /* RandomAttributeTypeValGen nyi */,
    },
    {
        type:         /* RandomAttributeTypeValGen nyi */,
        simple2:         /* RandomAttributeTypeValGen nyi */,
    },
]

export function writeMySynonym_Variant(dout: DataOut, x: MySynonym) {
    const write0 = genWriteSimple(writeU8);
    const writeSimple_Variant = genWriteSimple_Variant(write0);

    switch (x.type) {
        case 'a':
            writeSimple_Variant(dout, x);
            break;
        case 'b':
            writeSimple_Variant(dout, x);
            break;
        case 'a2':
            writeSimple2_Variant(dout, x);
            break;
        case 'b2':
            writeSimple2_Variant(dout, x);
            break;
    }
}

export function readMySynonym_Variant(din: DataIn, tag: U8): MySynonym {
    const read0 = genReadSimple(readU8);
    const readSimple_Variant = genReadSimple_Variant(read0);

    switch (tag) {
        case 0:
            return readSimple_Variant(din, 0);
        case 1:
            return readSimple_Variant(din, 1);
        case 2:
            return readSimple2_Variant(din, 0);
        case 3:
            return readSimple2_Variant(din, 1);
        default: throw new Error('Invalid tag for MySynonym: ' + tag);
    }
}

export function writeMySynonym(dout: DataOut, x: MySynonym) {
    switch (x.type) {
        case 'a':
            dout.u8(0);
            break;
        case 'b':
            dout.u8(1);
            break;
        case 'a2':
            dout.u8(2);
            break;
        case 'b2':
            dout.u8(3);
            break;
    }
    writeMySynonym_Variant(dout, x);
}

export function readMySynonym(din: DataIn): MySynonym {
    const tag = din.u8();
    return readMySynonym_Variant(din, tag);
}


// genRndObj:
const rndMySynonym = [
]

export function writeMySimpleSynonym_Variant(dout: DataOut, x: MySimpleSynonym) {
    const writeSimple_Variant = genWriteSimple_Variant(writeU8);

    switch (x.type) {
        case 'a':
            writeSimple_Variant(dout, x);
            break;
        case 'b':
            writeSimple_Variant(dout, x);
            break;
        case 'a2':
            writeSimple2_Variant(dout, x);
            break;
        case 'b2':
            writeSimple2_Variant(dout, x);
            break;
    }
}

export function readMySimpleSynonym_Variant(din: DataIn, tag: U8): MySimpleSynonym {
    const readSimple_Variant = genReadSimple_Variant(readU8);

    switch (tag) {
        case 0:
            return readSimple_Variant(din, 0);
        case 1:
            return readSimple_Variant(din, 1);
        case 2:
            return readSimple2_Variant(din, 0);
        case 3:
            return readSimple2_Variant(din, 1);
        default: throw new Error('Invalid tag for MySimpleSynonym: ' + tag);
    }
}

export function writeMySimpleSynonym(dout: DataOut, x: MySimpleSynonym) {
    switch (x.type) {
        case 'a':
            dout.u8(0);
            break;
        case 'b':
            dout.u8(1);
            break;
        case 'a2':
            dout.u8(2);
            break;
        case 'b2':
            dout.u8(3);
            break;
    }
    writeMySimpleSynonym_Variant(dout, x);
}

export function readMySimpleSynonym(din: DataIn): MySimpleSynonym {
    const tag = din.u8();
    return readMySimpleSynonym_Variant(din, tag);
}


// genRndObj:
const rndMySimpleSynonym = [
]

export function writeOneLiteral_Variant(dout: DataOut, x: OneLiteral) {

    // Literal konstant, hier nix zu schreiben
}

export function readOneLiteral_Variant(din: DataIn): OneLiteral {

    return 'oneLiteral';
}

export function writeOneLiteral(dout: DataOut, x: OneLiteral) {
    writeOneLiteral_Variant(dout, x);
}

export function readOneLiteral(din: DataIn): OneLiteral {
    return readOneLiteral_Variant(din);
}


// genRndObj:
const rndOneLiteral = [
]

export function writeOneSynonym_Variant(dout: DataOut, x: OneSynonym) {

    switch (x.type) {
        case 'a':
            writeMySynonym_Variant(dout, x);
            break;
        case 'b':
            writeMySynonym_Variant(dout, x);
            break;
        case 'a2':
            writeMySynonym_Variant(dout, x);
            break;
        case 'b2':
            writeMySynonym_Variant(dout, x);
            break;
    }
}

export function readOneSynonym_Variant(din: DataIn, tag: U8): OneSynonym {

    switch (tag) {
        case 0:
            return readMySynonym_Variant(din, 0);
        case 1:
            return readMySynonym_Variant(din, 1);
        case 2:
            return readMySynonym_Variant(din, 2);
        case 3:
            return readMySynonym_Variant(din, 3);
        default: throw new Error('Invalid tag for OneSynonym: ' + tag);
    }
}

export function writeOneSynonym(dout: DataOut, x: OneSynonym) {
    switch (x.type) {
        case 'a':
            dout.u8(0);
            break;
        case 'b':
            dout.u8(1);
            break;
        case 'a2':
            dout.u8(2);
            break;
        case 'b2':
            dout.u8(3);
            break;
    }
    writeOneSynonym_Variant(dout, x);
}

export function readOneSynonym(din: DataIn): OneSynonym {
    const tag = din.u8();
    return readOneSynonym_Variant(din, tag);
}


// genRndObj:
const rndOneSynonym = [
]

type A = ((DataIn) | (DataOut))[]