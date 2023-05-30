# data2asm.py By Stefan Wessels
# See help - makes Orca/M assembly files from RGB files
import argparse, sys

#-----------------------------------------------------------------------------
class ParseRemap(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, dict())
        for value in values:
            key, value = value.split('=')
            getattr(namespace, self.dest)[key] = value

#-----------------------------------------------------------------------------
def makemap(palette, remap):
    newMap = dict()
    for key, value in (remap.items()):
        palentry = palette.get(value)
        value = palentry if palentry else value
        newMap[int(key,16)]=int(value,16)
    return newMap

#-----------------------------------------------------------------------------
def remap(bindata, palette, remap):
    convData = bytearray()
    for i in (bindata):
        mapped = remap.get(i)
        if not mapped:
            mapped = i
        convData.append(mapped)
    return convData

#-----------------------------------------------------------------------------
def skip(bindata, skip):
    convData = bytearray()
    count = skip
    for i in (bindata):
        count -= 1
        if count == 0:
            count = skip
            continue
        convData.append(i)
    return convData

#-----------------------------------------------------------------------------
def makeindex(bindata, palette, pixelWidth):
    convData = bytearray()
    collect = pixelWidth
    pixelValue = ""
    for i in (bindata):
        pixelValue += "{:02X}".format(i)
        collect -= 1
        if 0 == collect:
            index = palette.get(pixelValue)
            if not index:
                raise NameError("ERROR: Pixel value {0} is not specified in the --palette file\n".format(pixelValue))
            convData.append(int(index,16))
            pixelValue=""
            collect = pixelWidth
    return convData

#-----------------------------------------------------------------------------
def shift(bindata, width):
    convData = bytearray()
    index = 0
    dataLen = len(bindata)
    while index + width-1 < dataLen:
        convData.append(bindata[index + width-1])
        for i in range(0, width-1):
            convData.append(bindata[index + i])
        index += width
    return convData

#-----------------------------------------------------------------------------
def convert(bindata, mask):
    convData = bytearray()
    myByte = 0
    ab = 0
    for i in(bindata):
        myByte = (myByte << 4) | i
        if myByte > 255:
            raise NameError("ERROR: Input stream has palette entries > 15.  Are -s and -d used correctly?")
        ab = 1 - ab
        if ab == 0:
            if mask:
                lhb = 0xF0 if myByte & 0xf0 == 0 else 0
                rhb = 0x0F if myByte & 0x0F == 0 else 0
                myByte = lhb | rhb
            convData.append(myByte)
            myByte = 0
    return convData

#-----------------------------------------------------------------------------
def save(bindata, fout, name, width):
    width = int(width / 2)
    dataLen = len(bindata)
    index = 0
    writeLength = 0
    fout.write(";-----------------------------------------------------------------------------\n{0} entry\n".format(name))
    while index < dataLen:
        fout.write("        dc h'")
        writeLength = dataLen - index if dataLen - index < width else width
        for i in range(writeLength):
            fout.write("{:02X}".format(bindata[index+i]))
        fout.write("'\n")
        index += writeLength
    fout.write("\n")
    if writeLength != width:
        raise NameError('ERROR: The input data was not an even multiple of {0} pixels\n'.format(width*2))

#-----------------------------------------------------------------------------
def main():
    filedata = bytearray()
    binarydata = bytearray()
    paldict = dict()
    remapDict = dict()

    parser = argparse.ArgumentParser(description='Process RAW image data into an ORCA/M ready text file ')
    parser.add_argument('-i', '--input', metavar='data_file', required=True,
                        help='The name of the file containing the RAW image palette indices')
    parser.add_argument('-o', '--output', metavar='asm_file',
                        help='Output asm file name.  If not specified output goes to stdout')
    parser.add_argument('-p', '--palette', metavar='palette_file',
                        help='The name of a file containing key=value pairs that map palette colours to palette indices, as used by the game (for example 1=red).')
    parser.add_argument('-n', '--name', required=True,
                        help='The base name of the object in the file as it will appear in the asm file (_data or _mask and _flush or _shift will be added to the name)')
    parser.add_argument('-w', '--width', type=int, required=True,
                        help='The pixel width of the object in the data file')
    parser.add_argument('-s', '--skip', type=int,
                        help="Skip every n'th input byte (normally 1 to turn 16-bit values into 8-bit values)")
    parser.add_argument('-b', '--bytesperpixel', metavar="BPP",type=int,
                        help="The width of the pixel data. Skip runs before bytes are turned into palettes and indices so alphas can be ignored that way")
    parser.add_argument('-r', '--remap', nargs='*', action=ParseRemap,
                        help='Intefer pairs in format a=b that remap pixel palette indices a in input data to b in output data.  If a palette is used then b can be a descriptive palette name (for example 0=black)')
    parser.add_argument('-d', '--indexed',action='store_true',
                        help="The input file is already in palette index format.")
    args = parser.parse_args()

    try:
        fin = open(args.input, 'rb')
        while True:
            bin = fin.read(-1)  
            if not bin:
                break
            filedata += bin
        fin.close()

        if args.output:
            fout = open(args.output, 'a')
        else:
            fout = sys.stdout

        if args.palette:
            palfile = open(args.palette, 'r')
            pallines = palfile.readlines()
            palfile.close()
            for line in (pallines):
                key, value = line.split('=')
                paldict[key.strip()] = value.strip()

        if args.remap:
            remapDict = makemap(paldict, args.remap)

        if args.skip:
            filedata = skip(filedata, args.skip)

        if not args.indexed:
            if not args.palette:
                raise NameError("ERROR: -p (palette) file must be specified if the input is not indexed\n")
            if not args.bytesperpixel:
                raise NameError("ERROR: -b (bytes per pixel) must be specified if the input is not indexed\n")
            filedata = makeindex(filedata, paldict, args.bytesperpixel)

        if args.remap:
            filedata = remap(filedata, paldict, remapDict)

        for i in range(4):
            if 0 == i:
                savedata = convert(filedata, False)
                save(savedata, fout, '{0}_flush_data'.format(args.name), args.width)
            if 1 == i:
                savedata = convert(filedata, True)
                save(savedata, fout, '{0}_flush_mask'.format(args.name), args.width)
            if 2 == i:
                binarydata = shift(filedata, args.width)
                savedata = convert(binarydata, False)
                save(savedata, fout, '{0}_shift_data'.format(args.name), args.width)
            if 3 == i:
                savedata = convert(binarydata, True)
                save(savedata, fout, '{0}_shift_mask'.format(args.name), args.width)
        
        if args.output:
            fout.close()

    except (OSError, IOError, NameError) as e:
        sys.stderr.write(str(e))

if __name__ == "__main__":
    main()
