#include "FlData.h"

FlRead reader;
FILE* fo=0;
///------------------------------------------------------------------------
void SaveSeg(FlSeg *s){
  //fprintf(fo,"     %2d % 5ld %3d %8.4f %8.4f %8.4f %6.1f %6.1f %6.1f\n",s->plate,s->id,s->trk_id,s->p,s->tx,s->ty,s->x,s->y,s->z);
  fprintf(fo,"side=%1d plt=%2d id=%3d dz=%8.4f kink=%6.3f mrad \n",s->side,s->plate,s->trk_id,s->dz,s->kink*1e3);
}
///------------------------------------------------------------------------
void SaveTrk(int flg, FlTrk *t){
  fprintf(fo,"   %1d %3d % 5d %8.4f %8.4f %8.4f %6.1f %6.1f %6.1f %3d\n",flg,t->id,t->pdg,t->p,t->tx,t->ty,t->x,t->y,t->z,t->N());
  for(int ns=0;ns<t->N();++ns){
    SaveSeg(t->GetSegment(ns));
  }
}
///------------------------------------------------------------------------
void SaveVtx(FlVtx *v){
  fprintf(fo,"%3d %3d %1d %3d %6.1f %6.1f %6.1f\n",v->id,v->flag,(v->trk_in!=0),v->N(),v->x,v->y,v->z);
  if(v->trk_in) SaveTrk(0,v->trk_in);
  for(int nt=0;nt<v->N();++nt){
    SaveTrk(1,v->GetTrack(nt));
  }
}
///------------------------------------------------------------------------


void SaveAll(){
  std::map<int,FlVtx*>::const_iterator it;
  FlVtx* v;
  FlTrk* t;
  FlSeg* s;
  fprintf(fo,"%7d %4.5f\n",reader.fEvent,reader.fE0);
  for(it=FlVtx::Map.begin();it!=FlVtx::Map.end();++it){
     v=it->second;
     if(v->flag!=0 && v->N()==0)continue;
     SaveVtx(v);
   
        
     }
}
int main(int argc, char** argv){
  FlRead::FlVerbose=0;  
  reader.fDoSaveSeg[FlSeg::kBTK]=1;
  reader.fDoSaveSeg[FlSeg::kMTK]=1;
  reader.fDoSaveSeg[FlSeg::kPB]=1;
  reader.fDoSaveTrx=1;
  reader.fDoSaveVtx=1;
  
  printf("Verbose=%d\n",FlRead::FlVerbose);
  fo=fopen(argv[argc-1],"w");
  if(reader.Open(argv[argc-2])==0){
    printf("ERROR opening file %s\n",argv[argc-2]);
    return 1;
  }
  while(reader.ReadEvent()){
   if(reader.fEvent%10000==0)
    printf("Evt #%d\n",reader.fEvent);
//    if(reader.fEvent>1e6)break;
//     printf("----\n");
//     reader.PrintStat();
    FlSeg::PrintAll();
    SaveAll();
    reader.ClearAll();
    fflush(stdout);
    fflush(fo);
  }
  fclose(fo);
  reader.Close();
  return 0;
}
