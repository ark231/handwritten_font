#include"read_metadata.hpp"
//#include<exiv2/exiv2.hpp>
//#include<Magick++.h>
//#include"cpp_exiftool/inc/ExifTool.h"
#include"ExifTool.h"
#include<stdexcept>
#include<spdlog/spdlog.h>
namespace handfont{
	namespace keyword_unit{
		std::string INCH = "inches";
		std::string METER = "meters";
	}
	image_metadata::image_metadata(stdfsys::path filepath){
		/*
		Magick::Image exif_image;
		try{
			//auto exif_image = Exiv2::ImageFactory::open(filepath.path().native());
			exif_image.read(filepath.path().native());
		}
		catch(std::exception e){
			spdlog::error("something happend while opening input file: {}\n{}",filepath.path().native(),e.what());
			continue;
		}
		*/
		//exif_image->readMetadata();
		//auto exif_data = exif_image->exifData();
		/*
		auto exif_data = exif_image.profile("exif");
		auto exif_resolution = exif_image.attribute("exif:XResolution");
		spdlog::info(exif_data.data());
		spdlog::info(exif_resolution);
		if(exif_data.empty()){
			spdlog::error("couldn't load metadata of image file "+filepath.path().native());
			continue;
		}
		*/

		this->unit = "";
		this->raw_resolution_x = 0;
		this->raw_resolution_y = 0;
		auto et = ExifTool();
		auto info = et.ImageInfo(filepath.native().c_str());
		if (info) {
			for (auto i=info; i; i=i->next) {
				//std::cout << i->name << " = " << i->value << std::endl;
				auto name_tmp = std::string(i->name);
				spdlog::debug("{} = {}",name_tmp,i->value);
				if(name_tmp == "PixelsPerUnitX" || name_tmp == "XResolution"){
					this->raw_resolution_x = std::stod(i->value);
				}else if(name_tmp == "PixelsPerUnitY" || name_tmp == "YResolution"){
					this->raw_resolution_y = std::stod(i->value);
				}else if(name_tmp == "PixelUnits" || name_tmp == "ResolutionUnit"){
					this->unit = std::string(i->value);
				}
			}
			delete info;
		} else if (et.LastComplete() <= 0) {
			throw std::runtime_error("Error executing exiftool!");
		}
		spdlog::debug("value which was set:");
		spdlog::debug(raw_resolution_x);
		spdlog::debug(raw_resolution_y);
		spdlog::debug(unit);
	}
	dpmm image_metadata::get_dpmm(){
		dpmm result=0;
		if(unit == keyword_unit::INCH){
			result = dpi_to_dpmm(raw_resolution_x);
		}else if(unit == keyword_unit::METER){
			result = raw_resolution_x/1000.0;
		}else{
			throw std::runtime_error("unknown unit type: "+unit);
		}
		return result;
	}
}
