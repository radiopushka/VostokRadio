int fast_half(int in){
  if(in<0){
    in=-in;
    in=in>>1;
    return -in;
  }
  return in>>1;
}
