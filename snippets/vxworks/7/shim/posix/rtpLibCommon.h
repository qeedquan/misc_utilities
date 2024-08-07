#ifndef _RTP_LIB_COMMON_H_
#define _RTP_LIB_COMMON_H_

typedef int RTP_ID;

#define VX_RTP_NAME_LENGTH 255

#define RTP_ID_ERROR -1

#define S_rtpLib_INVALID_FILE (M_rtpLib | 1)
#define S_rtpLib_INVALID_OPTION (M_rtpLib | 2)
#define S_rtpLib_ACCESS_DENIED (M_rtpLib | 3)
#define S_rtpLib_INVALID_RTP_ID (M_rtpLib | 4)
#define S_rtpLib_NO_SYMBOL_TABLE (M_rtpLib | 5)
#define S_rtpLib_INVALID_SEGMENT_START_ADDRESS (M_rtpLib | 6)
#define S_rtpLib_INVALID_SYMBOL_REGISTR_POLICY (M_rtpLib | 7)
#define S_rtpLib_INSTANTIATE_FAILED (M_rtpLib | 8)
#define S_rtpLib_INVALID_TASK_OPTION (M_rtpLib | 9)
#define S_rtpLib_RTP_NAME_LENGTH_EXCEEDED (M_rtpLib | 10)

#endif
