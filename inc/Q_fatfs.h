#ifndef Q_FATFS_H
#define Q_FATFS_H

#include "stm32f10x.h"
#include "attributes.h"
#include "ff.h"
#include "diskio.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct {
  uint32_t TotalSize;
  uint32_t FreeSize;
} Q_FATFS_Size_t;



typedef struct {
  uint32_t FoldersCount;
  uint32_t FilesCount;
} Q_FATFS_Search_t;


/** 
 * @brief  Default truncate buffer size in bytes
 * @note   If you have in plan to truncate from beginning large files a lot of times,
 *         then you should think about increasing this value as far as possible.
 *         With larger buffer size you will get faster response with truncating.
 */
#ifndef FATFS_TRUNCATE_BUFFER_SIZE
#define FATFS_TRUNCATE_BUFFER_SIZE	256
#endif

/* Memory allocation function */
#ifndef LIB_ALLOC_FUNC
#define LIB_ALLOC_FUNC    malloc
#endif

/* Memory free function */
#ifndef LIB_FREE_FUNC
#define LIB_FREE_FUNC     free
#endif


FRESULT Q_FATFS_GetDriveSize(char *str, Q_FATFS_Size_t *sizeStruct);



FRESULT Q_FATFS_TruncateBeginning(FIL *fil, uint32_t index);


FRESULT Q_FATFS_Search(char *Folder, char *tmp_buffer, uint16_t tmp_buffer_size, Q_FATFS_Search_t *FindStructure);

uint8_t Q_FATFS_SearchCallback(char *path, uint8_t is_file, Q_FATFS_Search_t *FindStructure);

uint8_t Q_FATFS_CheckCardDetectPin(void);
#endif
