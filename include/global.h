/*global
*/

#ifndef __GLOBAL_H__
#define	__GLOBAL_H__

#define	IDENTITY_TASK_PRI				100
#define	IDENTITY_TASK_STACKSIZE		64*1024
#define	IDENTITY_TASK_TIMEOUT_MS		100

#define	SPLITE_TASK_PRI				100
#define	SPLITE_TASK_STACKSIZE		64*1024
#define	SPLITE_TASK_TIMEOUT_MS		100

#define	TRAIN_TASK_PRI				100
#define	TRAIN_TASK_STACKSIZE		128*1024
#define	TRAIN_TASK_TIMEOUT_MS		100

#include <dp.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

BOOL UsrInit();
void UsrExit();
#ifdef __cplusplus
}
#endif

#endif // !__GLOBAL_H__

