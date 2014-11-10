#ifndef MPRTP_HEADER_EXT
#define MPRTP_HEADER_EXT

#define ROUND_ROBIN 0
#define INVALID_CONTEXT -1
#define INVALID_INPUT -2
#define INSUFFICIENT_MEMORY -3

/**
 * @brief Stores state for one flow
 */
typedef struct FlowContext
{
  // flow-specific sequence number
  int FSSN;
  // flow id
  int FlowId;
}FlowContext;
/**
 * @brief Stores state for MPRTP scheduling. For now only two flows are supported.
 * Only round robin scheduling is implemented for now.
 */
typedef struct MPRTPHeaderContext
{
  // 0 = round-robin
  int SchedulerType;
  // state of flows
  FlowContext Flows[2];
  // used to keep track of where last packet was sent
  int LastFlow;
}MPRTPHeaderContext;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialises the MPRTP context (random SNs for the two flows)
 * @param[out] pContext MPRTP context
 */
extern void initContext(MPRTPHeaderContext* pContext);

/**
 * @brief inserts an MPRTP header into the RTP packet. Scheduling occurs based on the passed in context.
 * Currently only round-robin is supported by the context. The caller is responsible for memory allocation.
 *
 * @param[in] pContext MPRTP context
 * @Param[in] pRtpPacket pointer to RTP packet
 * @Param[in] uiRtpSize size of RTP packet
 * @Param[out] pMpRtpPacket pointer to MPRTP packet
 * @Param[in, out] uiMpRtpSize size of MPRTP packet
 * @return returns 0 if successful,
 */
extern int insertMPRTPHeader(MPRTPHeaderContext* pContext, const unsigned char* pRtpPacket, unsigned uiRtpSize, unsigned char* pMpRtpPacket, unsigned uiMpRtpSize);

#ifdef __cplusplus
}
#endif

#endif
