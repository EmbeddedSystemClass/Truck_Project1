#ifndef CONFIG_FILE
#warning "CONFIG_FILE not defined"
extern illist_t ill;
extern ollist_t oll;
int ilLoadConfig(char *filename, illist_t *ill, size_t size,char *errmsg);
int olLoadConfig(char *filename, ollist_t *oll, size_t size,char *errmsg);
int ilWriteConfig(char *filename, illist_t *ill, size_t size,char *errmsg);
int olWriteConfig(char *filename, ollist_t *oll, size_t size,char *errmsg);
#else
#warning "CONFIG_FILE defined"
#endif
int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
int iWriteConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
int oWriteConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
//int iWriteConfig2(char *filename, I_DATA *curr_io_array,size_t size,int offset,char *errmsg);
//int oWriteConfig2(char *filename, O_DATA *curr_io_array,size_t size,int offset,char *errmsg);
