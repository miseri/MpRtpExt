#include "MpRtp.h"
#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_RTP_SIZE 12
#define MPRTP_HEADER_SIZE 24

void initContext(MPRTPHeaderContext* pContext)
{
  pContext->SchedulerType = ROUND_ROBIN;
  pContext->LastFlow = 0;
  pContext->Flows[0].FlowId = 0;
  pContext->Flows[1].FlowId = 1;
  pContext->Flows[0].FSSN = rand() % USHRT_MAX;
  pContext->Flows[1].FSSN = rand() % USHRT_MAX;
}

void schedulePacket(MPRTPHeaderContext* pContext, int* iFlowId, int* iFSSN)
{
  switch (pContext->SchedulerType)
  {
  case ROUND_ROBIN:
  default:
  {
    *iFlowId = (pContext->LastFlow == 0) ? 1 : 0;
    *iFSSN = pContext->Flows[*iFlowId].FSSN++;
    pContext->LastFlow = *iFlowId;
  }
  }
}

int insertMPRTPHeader(MPRTPHeaderContext* pContext,
  const unsigned char* pRtpPacket, unsigned uiRtpSize, unsigned char* pMpRtpPacket, unsigned uiMpRtpSize)
{
  int iFlowId = 0;
  int iFSSN = 0;
  // check pContext
  if (!pContext) return INVALID_CONTEXT;
  if (!pRtpPacket || uiRtpSize < MIN_RTP_SIZE) return INVALID_INPUT;
  if (!pMpRtpPacket || (uiMpRtpSize < MPRTP_HEADER_SIZE)) return INSUFFICIENT_MEMORY;

  memcpy(pMpRtpPacket, pRtpPacket, MIN_RTP_SIZE);
  memcpy(pMpRtpPacket + MPRTP_HEADER_SIZE, pRtpPacket + MIN_RTP_SIZE, uiRtpSize - MIN_RTP_SIZE);
  // set extension header bit
  pMpRtpPacket[0] = pMpRtpPacket[0] | 0x10;
  // rfc5285 header extension
  pMpRtpPacket[12] = 0xbe;
  pMpRtpPacket[13] = 0xde;
  pMpRtpPacket[14] = 0;
  pMpRtpPacket[15] = 0x02;
  // MPRTP 9.1.1
  // ID + LEN
  pMpRtpPacket[16] = 0x04;
  // MPID: subflow RTP header + LEN
  pMpRtpPacket[17] = 0x04;

  schedulePacket(pContext, &iFlowId, &iFSSN);

  fprintf(stderr, "Scheduling packet Flow: %d FSSN: %d\r\n", iFlowId, iFSSN);

  // subflow header
  // subflow id
  pMpRtpPacket[18] = 0x00;
  pMpRtpPacket[19] = iFlowId & 0xFF;
  // FSSN
  pMpRtpPacket[20] = (iFSSN >> 8) & 0xFF;
  pMpRtpPacket[21] = iFSSN & 0xFF;
  // padding
  pMpRtpPacket[22] = 0;
  pMpRtpPacket[23] = 0;

  return 0;
}
