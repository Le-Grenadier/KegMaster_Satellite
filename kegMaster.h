 #ifndef KEG_MASTER_H
#define	KEG_MASTER_H

#include <xc.h> 

#include "lib/KegMaster_Satellite.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

void KegMaster_procMsg(KegMaster_SatelliteMsgType* msg);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

