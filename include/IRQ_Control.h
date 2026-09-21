/*
 * IRQ_Control.h
 *
 *  Created on: Sep 21, 2026
 *      Author: Nishant Bhange
 */

#ifndef IRQ_CONTROL_H_
#define IRQ_CONTROL_H_
#include<stdint.h>
  uint32_t Get_PRIMASK(void);

  void Disable_IRQ(void);

  void Enable_IRQ(void);


#endif /* IRQ_CONTROL_H_ */
