#ifndef HANDFONT_IMAGE_UTILS
#define HANDFONT_IMAGE_UTILS
#include"qr_codes.hpp"
#include"points.hpp"
#include"sides.hpp" //uses template
#include"corner.hpp"
#include"write_area.hpp"
#include<opencv2/core.hpp>
namespace handfont{
	template <class K,class V>
	struct key_value{
		K key;
		V value;

		key_value(K key,V value){
			this->key = key;
			this->value = value;
		}
	};
	template <class K,class V>
	bool operator<(const key_value<K,V>& one,const key_value<K,V>& other){
		return (one.key < other.key);
	}

	using dpmm=double;
	using dpi=double;
	double constexpr inch_per_mm= 1/25.4;
	dpmm dpi_to_dpmm(dpi input_dpi);

}
#endif
