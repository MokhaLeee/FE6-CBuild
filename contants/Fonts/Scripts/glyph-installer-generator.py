#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import os, sys, argparse, traceback, re
from datetime import date
import shutil
from enum import Enum

class GlyphType(Enum):
    GlyS = 0 # System
    GlyT = 1 # Talk

FontType = {
    GlyphType.GlyS: 'item',
    GlyphType.GlyT: 'text',
}

FontTable = {
    GlyphType.GlyS: "GlyphType_GlyS",
    GlyphType.GlyT: "GlyphType_GlyT",
}

def show_exception_and_exit(exc_type, exc_value, tb):
    traceback.print_exception(exc_type, exc_value, tb)
    sys.exit(-1)

def append_glyph(glyphs, fpath, type, out_dir):
    with open(fpath, 'r', encoding='utf-8') as f:
        for line in f:
            match = re.search(fr'^(.)\t{FontType[type]}\t([0-9]+)\tFont{FontType[type].capitalize()}(.+).png$', line)

            if match:
                character = match.group(1)

                src = "{0}/Font{1}{2}.png".format(os.path.dirname(fpath), FontType[type].capitalize(), match.group(3))
                dst = "{0}/Font{1}_{2}_font.png".format(out_dir, FontType[type].capitalize(), "{:04X}".format(ord(character)))

                if ord(character) > 0x10000:
                    print("{0}(U_{1}) is not supported for unicode group_1 (U_0000 ~ U10000)".format(character, "{:04X}".format(ord(character))))
                    continue

                if not os.path.exists(src):
                    print("File `{0}` doesn't exist.".format(src))
                    continue

                if character not in glyphs:
                    glyphs[character] = {
                        "width" : int(match.group(2), 10),
                        "fpah"  : dst,
                    }

                    shutil.copyfile(src, dst)

def make_installer(glyphs, type, fpath):
    type_str = FontType[type].capitalize()

    with open(fpath, 'w', encoding='utf-8') as f:
        g = {}

        f.write("// Auto generated by glyph-installer-generator on {}\n\n".format(date.today()))

        for character, glyph in glyphs.items():
            unicod = ord(character)
            unicod_lo = unicod & 0xFF
            unicod_hi = (unicod >> 0x8) & 0xFF

            f.write("Font{0}_{1}: /* {2} */\n".format(type_str, "{:04X}".format(ord(character)), character))
            if unicod_lo in g:
                f.write("\tPOIN Font{0}_{1}\n".format(type_str, "{:04X}".format(ord(g[unicod_lo]))))
            else:
                f.write("\tWORD 0\n")

            f.write("\tBYTE {0} {1} 0 0\n".format(hex(unicod_hi), hex(glyph["width"])))
            f.write("\t#incbin \"Glyph{0}/{1}.img.bin\"\n".format(type_str, os.path.splitext(os.path.basename(glyph["fpah"]))[0]))
            f.write("\n")

            g[unicod_lo] = character

        f.write("ALIGN 4\n")
        f.write("{}:\n".format(FontTable[type]))
        f.write("ORG CURRENTOFFSET + 0x400\n")
        f.write("\n")

        f.write("PUSH\n")
        for lo, character in g.items():
            f.write("ORG {0} + 4 * {1}\n".format(FontTable[type], hex(lo)))
            f.write("\tPOIN Font{0}_{1}\n\n".format(type_str, "{:04X}".format(ord(character))))

        f.write("POP\n")

def main(args):
    sys.excepthook = show_exception_and_exit

    argParser = argparse.ArgumentParser(description = "")
    argParser.add_argument("-i", "--input", help = "input list file")
    arguments = argParser.parse_args()

    fpath_in = os.path.abspath(arguments.input)
    fdir_in  = os.path.dirname(fpath_in)

    fpath_out = "{0}/GlyphInstaller.event".format(fdir_in)

    if not os.path.exists(fpath_in):
        sys.exit("File `{0}` doesn't exist.".format(fpath_in))

    fpath_list = []

    with open(fpath_in, 'r') as f:
        for line in f.readlines():
            line = line.strip()

            if (len(line) == 0):
                continue

            if line[0] == '/' and line[1] == '/':
                continue

            if line[0] == '#':
                continue

            fpath_list.append(os.path.abspath(line))

    # Generate system fonts
    glyphs = {}
    glyphs_dir = "{0}/build/Glyph{1}".format(fdir_in, FontType[GlyphType.GlyS].capitalize())
    if not os.path.exists(glyphs_dir):
        os.makedirs(glyphs_dir)

    for fpath in fpath_list:
        append_glyph(glyphs, fpath, GlyphType.GlyS, glyphs_dir)

    fpath_installer = "{0}/build/Glyph{1}Installer.event".format(fdir_in, FontType[GlyphType.GlyS].capitalize())
    make_installer(glyphs, GlyphType.GlyS, fpath_installer)

    # Generate text fonts
    glyphs = {}
    glyphs_dir = "{0}/build/Glyph{1}".format(fdir_in, FontType[GlyphType.GlyT].capitalize())

    if not os.path.exists(glyphs_dir):
        os.makedirs(glyphs_dir)

    for fpath in fpath_list:
        append_glyph(glyphs, fpath, GlyphType.GlyT, glyphs_dir)

    fpath_installer = "{0}/build/Glyph{1}Installer.event".format(fdir_in, FontType[GlyphType.GlyT].capitalize())
    make_installer(glyphs, GlyphType.GlyT, fpath_installer)

    with open(fpath_out, 'w') as f:
        f.write("// Auto generated by glyph-installer-generator on {}\n\n".format(date.today()))
        f.write("ALIGN 4\n")
        f.write("#include \"build/Glyph{}Installer.event\"\n".format(FontType[GlyphType.GlyS].capitalize()))
        f.write("#include \"build/Glyph{}Installer.event\"\n".format(FontType[GlyphType.GlyT].capitalize()))

if __name__ == '__main__':
    main(sys.argv[1:])
