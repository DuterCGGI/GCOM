#include "TEST.h"

void TEST::testNCtools()
{
	double mea_pos = 10;
	double rev_spe = 34;
	double helixAgl = 45;
	double toolR = 6;
	double cutEdgelen = 24;
	int frame = 6;

	NCtools::toolcircle(mea_pos, rev_spe, frame, "toolcircle.txt");
	NCtools::toolcutlen(helixAgl, toolR, cutEdgelen, frame, "toolcutlen.txt");
	NCtools::toolEdgecircle(mea_pos, toolR, frame, "toolEdgecircle.txt");
}