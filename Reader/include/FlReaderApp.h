#ifndef FLREADERAPP
#define FLREADERAPP value
class FlRead;

class FlReaderApp
{
  public:
    FlReaderApp(){}
    ~FlReaderApp(){};
    //processing functions
    void Run();
    void Start();
    void Stop();
    void Exit(){exit(0);}

    bool IsRunning(){return fRunning;}
  protected:
    static long ReadLong(char* str);
    virtual void Init();
    virtual void Finish();
    virtual void ProcessEvent()=0;
    //printing
    virtual void HelpMe();
  public:
    virtual void ReadArgs(int argc, char** argv);

  protected:
    FlRead fReader; //Fluka reader object
    unsigned long fNstart=1; //starting event
    unsigned long fNtotal=1; //events to process
    unsigned long fMerge=1;  //events to be merged together
    unsigned fVerbose =0;
    char* fInFile =0;
    char* fOutFile =0;
    bool fRunning=false;
};
#endif