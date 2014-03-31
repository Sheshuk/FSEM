#ifndef NICE_LOG_H
#define NICE_LOG_H
#include <string>

/// nice macros for Log output
//to use, you need to write 
// #define vlev MyVLevVariable
//print values:
#define _PrintVD(x) printf("%s = %d\n",#x,x); //decimal
#define _PrintVF(x) printf("%s = %f\n",#x,x); //float
#define _PrintVB(x) printf("%s = %s\n",#x,x?"TRUE":"FALSE"); //
#define _PrintVS(x) printf("%s = \"%s\"\n",#x,x);
#define _PrintV(x,fmt) printf("%s = fmt\n",#x,x);
#define __sepLine(n) std::string(n,'-').data()

#define _PrintVBS(x) printf("%s = %s\n",#x,x?"\033[1;32mTRUE\033[0m":"\033[1;31mFALSE\033[0m");

#ifndef NCOLOR //no color terminal flag
	#define _Grn(...)  {printf("\033[0;32m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _GRN(...)  {printf("\033[1;32m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _Red(...)  {printf("\033[0;31m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _RED(...)  {printf("\033[1;31m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _Blu(...)  {printf("\033[0;34m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _BLU(...)  {printf("\033[1;34m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _Yel(...)  {printf("\033[0;33m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _YEL(...)  {printf("\033[1;33m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _Cya(...)  {printf("\033[0;36m");printf(__VA_ARGS__); printf("\033[0m");}
	#define _CYA(...)  {printf("\033[1;36m");printf(__VA_ARGS__); printf("\033[0m");}
#else
    #define _Grn(...)  printf(__VA_ARGS__);
	#define _GRN(...)  printf(__VA_ARGS__);
	#define _Red(...)  printf(__VA_ARGS__);
	#define _RED(...)  printf(__VA_ARGS__);
	#define _Blu(...)  printf(__VA_ARGS__);
	#define _BLU(...)  printf(__VA_ARGS__);
	#define _Yel(...)  printf(__VA_ARGS__);
	#define _YEL(...)  printf(__VA_ARGS__);
	#define _Cya(...)  printf(__VA_ARGS__);
	#define _CYA(...)  printf(__VA_ARGS__);
#endif
#ifndef NDEBUG //no debugging info flag
	#define _LogThis(vlev)      if(vlev<=_NiceVlev){printf("|%s",__sepLine(vlev));_CYA("%s\n",__PRETTY_FUNCTION__);}
	#define _LogCmd(vlev, cmd) 	if(vlev<=_NiceVlev){printf("|%s",__sepLine(vlev));_CYA("%s\n",__PRETTY_FUNCTION__);cmd;}
	#define _Log(vlev, ...)    	if(vlev<=_NiceVlev){printf("|%s",__sepLine(vlev));_CYA("%s\t",__PRETTY_FUNCTION__);printf(__VA_ARGS__);}
	#define _Info(vlev, ...) 	if(vlev<=_NiceVlev){printf("|%s",__sepLine(vlev));_YEL("(info):\t");_Yel(__VA_ARGS__);}
	#define _Err(vlev, ...) 	if(vlev<=_NiceVlev){fprintf(stderr,"\033[1;31m<%s>\033[0m\t",__PRETTY_FUNCTION__);fprintf(stderr,__VA_ARGS__);}
	#define _ErrF(vlev,...) 	if(vlev<=_NiceVlev){fprintf(stderr,"line %d in \"%s\"\033[1;31m<%s>\033[0m\t",__LINE__,__FILE__,__FUNCTION__);fprintf(stderr,__VA_ARGS__);}
	#define _ErrCmd(vlev, cmd)  if(vlev<=_NiceVlev){fprintf(stderr,"\033[1;31m<%s>\033[0m\t",__PRETTY_FUNCTION__);cmd;}
#else 
	#define _LogThis(vlev)     ;
	#define _LogCmd(vlev, cmd) ;
	#define _Log(vlev, ...)    ;
	#define _Info(vlev, ...)   ;
	#define _Err(vlev, ...)    ;
	#define _ErrF(vlev,...)    ;
	#define _ErrCmd(vlev, cmd) ;
#endif

#endif // NICE_LOG_H
