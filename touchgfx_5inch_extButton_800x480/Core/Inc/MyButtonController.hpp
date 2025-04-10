/*
 * MyButtonController.hpp
 *
 *  Created on: Jan 21, 2024
 *      Author: MehdiBMofidi
 */

#ifndef SRC_MYBUTTONCONTROLLER_HPP_
#define SRC_MYBUTTONCONTROLLER_HPP_

#include <platform/driver/button/ButtonController.hpp>
class MyButtonController: public touchgfx::ButtonController
{
	virtual void init();
	virtual bool sample(uint8_t& key);
private:
	uint8_t previousState0;
	uint8_t previousState1;
};




#endif /* SRC_MYBUTTONCONTROLLER_HPP_ */
