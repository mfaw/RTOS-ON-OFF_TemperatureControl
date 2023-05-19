#ifndef _macros_
#define _macros_

#define SET_BIT(REG,PIN) ((REG) |= (1<<PIN))
#define CLEAR_BIT(REG,PIN) ((REG) &= ~(1<<PIN))
#define TOGGLE_BIT(REG,PIN) ((REG) ^= (1<<PIN))
#define GET_BIT(REG, PIN) (((REG) & (1<<PIN))>> PIN)
#define BIT_IS_SET(REG,BIT) (REG & (1<<BIT))
#define BIT_IS_CLEAR(REG,BIT) (!(REG & (1<<BIT)))

#endif