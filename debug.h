#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG
	#define DEBUG_INIT() debug_init()
	#define DEBUG_C(_c) debug_char(_c)
	#define DEBUG_S(_s) debug_str(_s)
	#define DEBUG_B(_b) debug_byte(_b)
#else
	#define DEBUG_INIT()
	#define DEBUG_C(_c) 
	#define DEBUG_S(_s) 
	#define DEBUG_B(_b) 
#endif


void debug_init(void);
void debug_char(char);
void debug_str(char*);
void debug_byte(char);

#endif /*DEBUG_H_*/
