#define MAGIC 'x'

#define BUF_CNT _IOR(MAGIC,1,int*)
#define BUF_CLR _IO(MAGIC,2)
#define BUF_DISP _IO(MAGIC,3)
