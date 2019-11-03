#ifndef ADDFUNCXL320_HPP_INCLUDE
#define ADDFUNCXL320_HPP_INCLUDE

#include "XL320.h"

class AddFuncXL320 : public XL320{
public:
	//add function
  //write XL_GOAL_SPEED_L to address, so same setJointSpeed of function
	void moveWheel(int id, int value);
};

#endif
