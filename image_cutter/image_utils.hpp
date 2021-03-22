#ifndef HANDFONT_IMAGE_UTILS
#define HANDFONT_IMAGE_UTILS
namespace handfont{
	template <class K,class V>
	struct key_value{
		K key;
		V value;

		key_value(K key,V value){
			this->key = key;
			this->value = value;
		}
		bool operator<(const key_value& other) const{
			return (this->key < other.value);
		}
	};

	using dpmm=double;
	using dpi=double;
	double constexpr inch_per_mm= 1/25.4;
	dpmm dpi_to_dpmm(dpi input_dpi);
}
#endif
