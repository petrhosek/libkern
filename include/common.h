#ifndef COMMON_H_
#define COMMON_H_ 1

/* logical/comparison operators */
#define ne  !=
#define eq  ==
#define gt  >
#define lt  <
#define ge  >=
#define le  <=
#define and &&
#define or  ||
#define not !

/* bitwise operators */
#define bnot ~
#define band &
#define bor  |
#define bxor ^

/* arithmetic operators */
#define mod   %

/* control constructs */
#define repeat    do
#define until(p)  while(not (p))
#define loop      while(1)
#define unless(p) if(not (p))
#define nop

/* functional constructs */
#define lambda(return_type, function_body) \
    ({ \
        return_type __fn__ function_body \
        __fn__; \
    })

#endif /* !COMMON_H_ */
