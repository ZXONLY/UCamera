package com.zxx.camera.presenter;

import android.media.MediaCodec;

import java.net.URLEncoder;
import java.nio.ByteBuffer;

public class CircularEncoderBuffer {
    private static final String TAG = CircularEncoderBuffer.class.getSimpleName();
    private static final boolean EXTRA_DEBUG = true;
    private static final boolean VERBOSE = false;

    private ByteBuffer mDataBufferWrapper;
    private byte[] mDataBuffer;

    private int[] mPacketFlags;
    private long[] mPacketPtsUses;
    private int[] mPacketStart;
    private int[] mPacketLength;

    private int mMetaHead;
    private int mMetaTail;

    public CircularEncoderBuffer(int bitRate, int frameRate, int desiredSpanSec) {
        int dataBufferSize = bitRate * desiredSpanSec / 8;
        mDataBuffer = new byte[dataBufferSize];
        mDataBufferWrapper = ByteBuffer.wrap(mDataBuffer);

        int metaBufferCount = frameRate * desiredSpanSec * 2;
        mPacketFlags = new int[metaBufferCount];
        mPacketPtsUses = new long[metaBufferCount];
        mPacketStart = new int[metaBufferCount];
        mPacketLength = new int[metaBufferCount];
    }

    public long computeTimeSpanUsec() {
        final int metaLen = mPacketStart.length;

        if (mMetaHead == mMetaTail) {
            //empty list
            return 0;
        }

        int beforeHead = (mMetaHead + metaLen - 1) % metaLen;
        return mPacketPtsUses[beforeHead] - mPacketPtsUses[mMetaTail];
    }

    /**
     * Adds a new Encode data packet to the buffer
     */
    public void add(ByteBuffer buf, int flags, long ptsUsec) {
        int size = buf.limit() - buf.position();

        while (!canAdd(size)){
            removeTail();
        }

        final int dataLen = mDataBuffer.length;
        final int metaLen = mPacketStart.length;
        int packetStart = getHeadStart();

        mPacketFlags[mMetaHead] = flags;
        mPacketPtsUses[mMetaHead] = ptsUsec;
        mPacketStart[mMetaHead] = packetStart;
        mPacketLength[mMetaHead] = size;

        //Copy the data in. Take care if it gets split in half.
        if(packetStart + size < dataLen){
            //one chunk
            buf.get(mDataBuffer, packetStart, size);
        } else {
            //two chunk
            int firstSize = dataLen - packetStart;
            buf.get(mDataBuffer, packetStart, firstSize);
            buf.get(mDataBuffer,0,size - firstSize);
        }

        mMetaHead = (mMetaHead + 1) % metaLen;

//        if(EXTRA_DEBUG){
//
//        }

    }

    public int getFirstIndex(){
        final int metaLen = mPacketStart.length;

        int index = mMetaTail;
        while (index != mMetaHead){
            if((mPacketFlags[index] & MediaCodec.BUFFER_FLAG_SYNC_FRAME) != 0){
                break;
            }
            index = (index + 1) % metaLen;
        }
        if(index == mMetaHead){
            index = -1;
        }
        return index;
    }

    public int getNextIndex(int index){
        final int metaLen = mPacketStart.length;
        int next = (index +1 )% metaLen;
        if(next == mMetaHead){
            return -1;
        }
        return next;
    }

    public ByteBuffer getChunk(int index, MediaCodec.BufferInfo info){
        final int dataLen = mDataBuffer.length;
        int packetStart = mPacketStart[index];
        int length = mPacketLength[index];

        info.flags = mPacketFlags[index];
        info.offset = packetStart;
        info.presentationTimeUs = mPacketPtsUses[index];
        info.size = length;

        if(packetStart + length <= dataLen){
            //one Chunk; return full buffer to avoid copying data
            return mDataBufferWrapper;
        } else {
            //two chunks
            ByteBuffer tempBuf = ByteBuffer.allocate(length);
            int firstSize = dataLen - packetStart;
            tempBuf.put(mDataBuffer, mPacketStart[index],firstSize);
            tempBuf.put(mDataBuffer,0,length - firstSize);
            info.offset = 0;
            return tempBuf;
        }

    }

    private int getHeadStart(){
        if(mMetaHead == mMetaTail){
            return 0;
        }

        final int dataLen = mDataBuffer.length;
        final int metaLen = mPacketStart.length;

        int beforeHead = (mMetaHead + metaLen -1) % metaLen;
        return (mPacketStart[beforeHead] + mPacketLength[beforeHead] + 1) % dataLen;
    }

    private boolean canAdd(int size) {
        final int dataLen = mDataBuffer.length;
        final int metaLen = mPacketStart.length;

        if (size > dataLen) {
            throw new RuntimeException("Enormous packet: " + size + " vs. buffer" + dataLen);
        }
        if (mMetaHead == mMetaTail) {
            return true;
        }

        //Make Sure we can advance head without stepping on the tail.
        int nextHead = (mMetaHead + 1) % metaLen;
        if (nextHead == mMetaTail) {
            return false;
        }

        //Need the byte offset of the start of the "tail" packet, and the byte offset where "head" will store its data
        int headStart = getHeadStart();
        int tailStart = mPacketStart[mMetaTail];
        int freeSpace = (tailStart + dataLen - headStart) % dataLen;
        if(size > freeSpace){
            return false;
        }
        return true;
    }

    private void removeTail(){
        if(mMetaHead == mMetaTail){
            throw new RuntimeException("Can't removeTail() in empty buffer");
        }
        final int metaLen = mPacketStart.length;
        mMetaTail = (mMetaTail + 1 ) % metaLen;
    }
}
