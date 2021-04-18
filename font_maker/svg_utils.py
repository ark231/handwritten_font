import pathlib
import svgelements
class svg_handler:
    def __init__(self,filepath):
        self.svg_data = svgelements.SVG.parse(filepath)
    def calc_ex_rate(self,dst_width):
        return dst_width/self.svg_data.width
        pass
    def calc_Bbox(self):
        pathes=[]
        for element in self.svg_data.elements():
            if isinstance(element,svgelements.Path):
                pathes.append(element)
        return svgelements.SVG.union_bbox(pathes)
    def calc_width(self):
        pass
    def calc_height(self):
        pass
