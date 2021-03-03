package com.zxx.unique.utils;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;

public class ConcurrentList<E> {

    private List<E> mList = new ArrayList<>();
    private boolean mListChanged = false;
    private List<E> mImmutableLis = new ArrayList<>();

    public synchronized boolean add(@NonNull E e) {
        mListChanged = true;
        return mList.add(e);
    }

    public synchronized boolean remove(E e) {
        mListChanged = true;
        return mList.remove(e);
    }

    public synchronized void clear() {
        mListChanged = true;
        mList.clear();
    }

    public synchronized boolean isEmpty() {
        return mList.isEmpty();
    }

    /**
     * 直接使用原始list去遍历，可能存在某些回调耗时的情况，
     * 为了提高并发吞吐量，此处做一次拷贝。
     * @return
     */
    public synchronized List<E> getImmutableList() {
        if (mListChanged) {
            mImmutableLis = new ArrayList<>(mList.size());
            for (E e: mList) {
                mImmutableLis.add(e);
            }
            mListChanged = false;
        }
        return mImmutableLis;
    }
}
