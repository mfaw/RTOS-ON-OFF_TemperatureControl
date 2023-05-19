#ifndef Macro_H_
#define Macro_H_

#define SET_BIT(REG,PIN) ((REG) |= (1<<PIN))

#define CLEAR_BIT(REG,PIN) ((REG) &= ~(1<<PIN))

#define TOGGLE_BIT(REG,PIN) ((REG) ^= (1<<PIN))

#define GET_BIT(REG, PIN) (((REG) & (1<<PIN))>> PIN)

/* Check if bit is set */
#define BIT_IS_SET(REG,BIT) (REG & (1<<BIT))

/* Check if bit is CLEAR */
#define BIT_IS_CLEAR(REG,BIT) (!(REG & (1<<BIT)))


#define HIGH 1u
#define LOW  0u

#ifndef FALSE
#define FALSE       (0u)
#endif
#ifndef TRUE
#define TRUE        (1u)
#endif

#define STD_HIGH 1u
#define STD_LOW  0u

#define INPUT  0u
#define OUTPUT 1u

#endif