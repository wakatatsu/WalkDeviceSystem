#include "addFuncXL320.hpp"

void AddFuncXL320::moveWheel(int id, int value) {
  //write XL_GOAL_SPEED_L to address, so same setJointSpeed of function
  setJointSpeed(id, value);
}
