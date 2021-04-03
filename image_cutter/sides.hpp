#ifndef HANDFONT_IMAGE_SIDES
#define HANDFONT_IMAGE_SIDES
namespace handfont{
	template <class P>
	struct side{
		P start;
		P end;
		side(P,P);
		double length();
	};
	template <class P>
	P same_points(side<P>,side<P>);
	template <class P>
	bool operator <(side<P>&,side<P>&);
}
#include"sides.cpp"
#endif
