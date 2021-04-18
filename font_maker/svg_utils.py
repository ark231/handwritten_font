import pathlib
import svgelements
class svg_handler:
    def __init__(self,filepath):
        self._svg_data = svgelements.SVG.parse(filepath)
        self._calc_Bbox
    def calc_ex_rate(self,dst_height): #プロポーショナルでも変わらないはずだから
        return dst_height/self._svg_data.height
        pass
    def _calc_Bbox(self):
        pathes=[]
        for element in self._svg_data.elements():
            if isinstance(element,svgelements.Path):
                pathes.append(element)
        self._Bbox = svgelements.SVG.union_bbox(pathes)
    def get_width(self):
        return self._Bbox[3]
    def get_height(self):
        return self._Bbox[2]
