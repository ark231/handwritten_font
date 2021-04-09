#!/usr/bin/env python3
import defcon
from fontTools.svgLib.path import SVGPath
from fontTools import fontBuilder,ufoLib
import ufo2ft
import argparse
import pathlib
import re
import toml
from logging import getLogger,StreamHandler,DEBUG,Formatter

def main():
    parser=argparse.ArgumentParser(description="make font from svg files",prog="font_maker")
    #parser.add_argument("width",type=int,help="width of output picture")
    #parser.add_argument("height",type=int,help="height of output picture")
    parser.add_argument("-p","--project_file",metavar="PFILE",type=str,help="project file path",required = True)
    parser.add_argument("-b","--debug",action="store_true",help="debug output")
    args=parser.parse_args()
    project_filedata = toml.load(open(args.project_file))
    project_path = pathlib.Path(args.project_file).resolve().parent
    logger = getLogger("font_maker")
    handler = StreamHandler()
    handler.setFormatter(Formatter("[%(asctime)s] [%(levelname)s] %(message)s"))
    if args.debug:
        logger.setLevel(DEBUG)
    logger.addHandler(handler)
    logger.propagate = False
    for dirpath in (project_path/".cache").glob("*/*"):
        result = defcon.Font()
        result.info.familyName = project_filedata["fontname"]
        UPE = 1024
        descender = round(-UPE*(1/3.0))
        ascender = UPE+descender
        xHeight = round(ascender/2)
        capHeight = ascender
        result.info.unitsPerEm = UPE
        result.info.descender = descender
        result.info.ascender = ascender
        result.info.xHeight = xHeight
        result.info.capHeight = capHeight
        cur_path = dirpath.relative_to(project_path)
        if(re.search("small",str(cur_path))):
            result.info.styleName = "small"
            if(re.search("mono",str(cur_path))):
                result.info.styleName += "-mono"
            elif(re.search("proportional",str(cur_path))):
                result.info.styleName += "-proportional"
            else:
                continue
        elif(re.search("large",str(cur_path))):
            result.info.styleName = "large"
            if(re.search("mono",str(cur_path))):
                result.info.styleName += "-mono"
            elif(re.search("proportional",str(cur_path))):
                result.info.styleName += "-proportional"
            else:
                continue
        else:
            continue
        for filename in dirpath.glob("*.svg"):
            code_point = re.search(r"[0-9a-f]+",str(filename.name))
            if(not code_point):
                continue
            code_point = code_point.group(0)
            #print(str(filename.name))
            #print(str(code_point))
            #print(int(str(code_point),16))
            #print(chr(int(str(code_point),16)))
            logger.debug('{code}: "{char}"'.format(code = str(code_point),char = chr(int(str(code_point),16))))
            glyph = result.newGlyph("U+"+str(code_point))
            glyph.unicode = int(str(code_point),16)
            glyph.width = round(UPE/2)
            pen = glyph.getPen()
            svg_data = SVGPath(str(filename),(1,0,0,1,0,descender))
            svg_data.draw(pen)
        logger.info("start compiling otf")
        result_otf = ufo2ft.compileOTF(result)
        logger.info("start compiling ttf")
        result_ttf = ufo2ft.compileTTF(result)
        logger.info("finish compiling")
        file_name = "{filename}-{stylename}".format(
            filename = project_filedata["fontname"],
            stylename = result.info.styleName
            )
        logger.info("start saving otf")
        result_otf.save(str(project_path/"output"/(file_name+".otf")))
        logger.info("start saving ttf")
        result_ttf.save(str(project_path/"output"/(file_name+".ttf")))



if __name__ == "__main__":
    main()

