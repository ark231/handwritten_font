#!/usr/bin/env python3
import defcon
import fontTools
import argparse
import pathlib
import re
import toml

def main():
    parser=argparse.ArgumentParser(description="make font from svg files",prog="font_maker")
    #parser.add_argument("width",type=int,help="width of output picture")
    #parser.add_argument("height",type=int,help="height of output picture")
    parser.add_argument("-p","--project_file",metavar="PFILE",type=str,help="project file path",required = True)
    args=parser.parse_args()
    project_filedata = toml.load(open(args.project_file))
    project_path = pathlib.Path(args.project_file).resolve().parent
    #print(project_path)
    for dirpath in (project_path/".cache").glob("*/*"):
        result = defcon.Font()
        result.info.familyName = project_filedata["fontname"]
        result.info.unitsPerEm = 1024
        cur_path = dirpath.relative_to(project_path)
        if(re.match("small",str(cur_path))):
            result.info.styleName = "small"
            if(re.match("mono"),str(cur_path)):
                result.info.styleName += "-mono"
            elif(re.match("proportional"),str(cur_path)):
                result.info.styleName += "-proportional"
            else:
                continue
        elif(re.match("large"),str(cur_path)):
            result.info.styleName = "large"
            if(re.match("mono"),str(cur_path)):
                result.info.styleName += "-mono"
            elif(re.match("proportional"),str(cur_path)):
                result.info.styleName += "-proportional"
            else:
                continue
        else:
            continue



if __name__ == "__main__":
    main()

