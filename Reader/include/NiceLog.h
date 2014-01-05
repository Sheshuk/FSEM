
/// nice macros for Log output
#define _DumpD(x) printf("%s = %d\n",#x,x)
#define _DumpX(x) printf("%s = 0x%x\n",#x,x)
#define _DumpF(x) printf("%s = %f\n",#x,x)
#define _DumpS(x) printf("%s = \"%s\"\n",#x,x)

#define __sepLine(n) std::string(n,'.').data()
#define _LogCmd(vlev, cmd) if(vlev<=_NiceVlev){printf("\033[1;32m |%s <%s>\033[0m:\n",__sepLine(vlev),__PRETTY_FUNCTION__); cmd;}
#define _Log(vlev, ...)  if(vlev<=_NiceVlev){printf("\033[1;32m |%s <%s>\033[0m\t",__sepLine(vlev),__PRETTY_FUNCTION__);printf(__VA_ARGS__);}
#define _Err(vlev, ...)  if(vlev<=_NiceVlev){fprintf(stderr,"\033[1;31m<%s>\033[0m\t",__PRETTY_FUNCTION__);fprintf(stderr,__VA_ARGS__);}
#define _ErrF(vlev,...)  if(vlev<=_NiceVlev){fprintf(stderr,"\033[0;31m line %d in \"%s\"\033[1;31m<%s>\033[0m\t",__LINE__,__FILE__,__PRETTY_FUNCTION__);fprintf(stderr,__VA_ARGS__);}

