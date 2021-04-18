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

import svg_utils
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

        metadatas = toml.load(dirpath/".metadata.toml")
        UPE = 1024
        if metadatas["general"]["has_lower_latin"]:
            descender = round(-UPE*(1/4.0))
        else:
            descender = round(-UPE*(1/3.0))
        ascender = UPE+descender#desenderは必ず負！！
        xHeight = round(ascender/2)
        capHeight = ascender
        if metadatas["general"]["is_Fixed_Base"]:
            descender*=(3/2.0)
            ascender*=(3/2.0)
            xHeight*=(3/2.0)
            capHeight*=(3/2.0)
        result.info.unitsPerEm = UPE
        result.info.descender = descender
        result.info.ascender = ascender
        result.info.xHeight = xHeight
        result.info.capHeight = capHeight
        for filename in dirpath.glob("*.svg"):
            code_point = re.search(r"[0-9a-f]+",str(filename.name))
            if not code_point:
                continue
            input_svg = svg_utils.svg_handler(filename)
            code_point = code_point.group(0)
            code_point = str(code_point)
            char_width = metadatas["chars"]["U"+code_point]["width"]
            logger.debug('{code}: "{char}"'.format(code = code_point,char = chr(int(code_point,16))))
            glyph = result.newGlyph("U+"+code_point)
            glyph.unicode = int(code_point,16)
            if char_width == "HALF":
                glyph.width = round(UPE/2)
            elif char_width == "FULL":
                glyph.width = UPE
            pen = glyph.getPen()
            guide_type = metadatas["chars"]["U"+code_point]["guide_type"]
            ex_rate = input_svg.calc_ex_rate(glyph.width)
            if guide_type == "cross":
                svg_data = SVGPath(str(filename),(ex_rate,0,0,ex_rate,0,0))
            else:
                svg_data = SVGPath(str(filename),(ex_rate,0,0,ex_rate,0,descender))
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

def getBBox(src):
    pass

if __name__ == "__main__":
    main()

