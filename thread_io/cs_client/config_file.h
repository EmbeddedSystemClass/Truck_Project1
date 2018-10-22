#ifndef CONFIG_FILE
#warning "CONFIG_FILE not defined"
extern illist_t ill;
extern ollist_t oll;
//extern rt_llist_t roll;
int ilLoadConfig(char *filename, illist_t *ill, size_t size,char *errmsg);
int olLoadConfig(char *filename, ollist_t *oll, size_t size,char *errmsg);
int ilWriteConfig(char *filename, illist_t *ill, size_t size,char *errmsg);
int olWriteConfig(char *filename, ollist_t *oll, size_t size,char *errmsg);
//int rlLoadConfig(char *filename, rt_llist_t *oll, size_t size,char *errmsg);
//int rlWriteConfig(char *filename,  rt_llist_t *oll, size_t size,char *errmsg);
#else
#warning "CONFIG_FILE defined"
#endif
int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
int iWriteConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
int oWriteConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
int iWriteConfigXML(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
int oWriteConfigXML(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
//int rtLoadConfig(char *filename, RI_DATA *curr_o_array,size_t size,char *errmsg);
//int rtWriteConfig(char *filename, RI_DATA *curr_o_array,size_t size,char *errmsg);
int GetFileFormat(char *filename);
int getFileCreationTime(char *path,char *str);
//int WriteOdometer(char *filename, int *odo, char *errmsg);
//int LoadOdometer(char *filename, int *odo, char *errmsg);
