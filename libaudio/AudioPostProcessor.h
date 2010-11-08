/*
** Copyright 2010, The Android Open-Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef ANDROID_AUDIO_POST_PROCESSOR_H
#define ANDROID_AUDIO_POST_PROCESSOR_H
#ifdef USE_PROPRIETARY_AUDIO_EXTENSIONS

extern "C" {
#include "cto_audio_mm.h"
}
#include "mot_acoustics.h"

namespace android {

class AudioPostProcessor
{
public:
                        AudioPostProcessor();
                        ~AudioPostProcessor();
            void        setPlayAudioRate(int rate);
            void        setAudioDev(struct cpcap_audio_stream *outDev,
                                    struct cpcap_audio_stream *inDev,
                                    bool is_bt, bool is_bt_ec, bool is_spdif);
            void        doMmProcessing(void * buffer, int numSamples);
            int         getEcnsRate(void);

            void        enableEcns(bool value);
            int         writeDownlinkEcns(int fd, void * buffer,
                                          bool stereo, int bytes, Mutex * fdLockp);
            int         applyUplinkEcns(void * buffer, int bytes, int rate);
            bool        isEcnsEnabled(void) { return mEcnsEnabled; };

private:
            void        configMmAudio(void);
            uint32_t    convOutDevToCTO(uint32_t outDev);
            uint32_t    convRateToCto(uint32_t rate);

            void        initEcns(int rate, int bytes);
            void        stopEcns(void);
            void        ecnsLogToFile(int bytes);
            int         read_dock_prop(char const *path);

        // CTO Multimedia Audio Processing storage buffers
            int16_t     mPcmLoggingBuf[((CTO_AUDIO_MM_DATALOGGING_BUFFER_BLOCK_BYTESIZE)/2)];
            uint32_t    mNoiseEst[((CTO_AUDIO_MM_NOISE_EST_BLOCK_BYTESIZE)/4)];
            uint16_t    mRuntimeParam[((CTO_AUDIO_MM_RUNTIME_PARAM_BYTESIZE)/2)];
            uint16_t    mStaticMem[((CTO_AUDIO_MM_STATICMEM_BLOCK_BYTESIZE)/2)];
            uint16_t    mScratchMem[((CTO_AUDIO_MM_SCRATCHMEM_BLOCK_BYTESIZE)/2)];
            CTO_AUDIO_MM_ENV_VAR mAudioMmEnvVar;
            Mutex       mMmLock;

        // EC/NS configuration etc.
            Mutex       mEcnsBufLock;
            Condition   mEcnsBufCond;  // Signal to unblock write thread
            bool        mEcnsEnabled; // Enabled by libaudio
            bool        mEcnsRunning; // ECNS module init done by read thread
            int         mEcnsRate;
            void *      mEcnsScratchBuf;  // holding cell for downlink speech "consumed".
            int         mEcnsScratchBufSize;
            void *      mEcnsOutBuf;      // buffer from downlink "write()"
            int         mEcnsOutBufSize;
            int         mEcnsOutBufReadOffset;
            int         mEcnsOutFd;       // fd pointing to output driver
            Mutex *     mEcnsOutFdLockp;
            CTO_AUDIO_USECASES_CTRL mEcnsMode;
            FILE *      mLogFp[15];
            int16_t *   mEcnsDlBuf;
            int         mEcnsDlBufSize;
            bool        mEcnsOutStereo;

        // EC/NS Module memory
            T_MOT_MEM_BLOCKS mMemBlocks;
            T_MOT_CTRL  mEcnsCtrl;
            uint16_t    mStaticMemory_1[API_MOT_STATIC_MEM_WORD16_SIZE];
            uint16_t    mMotDatalog[API_MOT_DATALOGGING_MEM_WORD16_SIZE];
            uint16_t    mParamTable[AUDIO_PROFILE_PARAMETER_BLOCK_WORD16_SIZE*CTO_AUDIO_USECASE_TOTAL_NUMBER];
};

} // namespace android

#endif // USE_PROPRIETARY_AUDIO_EXTENSIONS
#endif // ANDROID_AUDIO_POST_PROCESSOR_H
