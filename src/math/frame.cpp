#include<math/frame.h>
#include<record/record.h>



ls::Frame::Frame(Normal n)
{
	Z = Vec3(n);

	constructFrame(Z, &X, &Y);
}
