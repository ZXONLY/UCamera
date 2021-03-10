//
// Created by bytedance on 10/03/2021.
//
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace btch {

    enum {
        bundle_error = 0,
        bundle_bool = 1,
        bundle_float,
        bundle_int,       // int
        bundle_integer,   // int64
        bundle_string,
        bundle_bundle,
        bundle_int_number_array,
        bundle_char_number_array,
        bundle_float_number_array,
        bundle_double_number_array,
        bundle_string_array,
        bundle_bundle_array,
    };

    class CBundle {

    public:
        CBundle();

        ~CBundle();

        CBundle(CBundle &r);

        CBundle(const CBundle &r);

        CBundle &operator=(const CBundle &r);

        CBundle &operator+(const CBundle &r);

        /**
         *  清空数据实体
         *	@note N/A
         */
        void clear();

        /**
         *  删除指定键值
         *  @param strKey  要删除的键值
         *  @notes N/A
         */
        void remove(const std::string &strKey);

        bool containsKey(const std::string &strKey) const;

        void getKeys(std::vector<std::string> &arrKey) const;

        int getType(std::string &strKey) const;

        bool getBool(const std::string &strKey) const;

        bool getBool(const std::string &strKey, bool &value) const;

        void setBool(const std::string &strKey, bool bValue);

        int64_t getInt(const std::string &strKey) const;

        bool getInt(const std::string &strKey, int64_t &value) const;

        void setInt(const std::string &strKey, int64_t iValue);

        int getIntOrigin(const std::string &strKey) const;

        bool getIntOrigin(const std::string &strKey, int &value) const;

        void setIntOrigin(const std::string &strKey, int iValue);

        int64_t getLong(const std::string &strKey) const;

        bool getLong(const std::string &strKey, int64_t &value) const;

        void setLong(const std::string &strKey, int64_t iValue);

        float getFloat(const std::string &strKey) const;

        bool getFloat(const std::string &strKey, float &value) const;

        void setFloat(const std::string &strKey, float fValue);

        double getDouble(const std::string &strKey) const;

        bool getDouble(const std::string &strKey, double &value) const;

        void setDouble(const std::string &strKey, double dValue);

        std::string *getString(const std::string &strKey) const;

        void setString(const std::string &strKey, const std::string &strValue);

        CBundle *getBundle(const std::string &strKey) const;

        void setBundle(const std::string &strKey, const CBundle &bundleValue);

        bool getDeepIntArray(const std::string &strKey, std::vector<int> &outValue) const;

        bool getDeepFloatArray(const std::string &strKey, std::vector<float> &outValue) const;

        bool getDeepDoubleArray(const std::string &strKey, std::vector<double> &outValue) const;

        bool getDeepStringArray(const std::string &strKey, std::vector<std::string> &outValue) const;

        bool getDeepCharArray(const std::string &strKey, std::vector<char> &outValue) const;

        void setIntArray(const std::string &strKey, const std::vector<int> &arrValue);

        void setCharArray(const std::string &strKey, const std::vector<char> &arrValue);

        std::vector<float> *getFloatArray(const std::string &strKey) const;

        void setFloatArray(const std::string &strKey, const std::vector<float> &arrValue);

        std::vector<double> *getDoubleArray(std::string &strKey) const;

        void setDoubleArray(const std::string &strKey, const std::vector<double> &arrValue);

        std::vector<std::string> *getStringArray(const std::string &strKey) const;

        void setStringArray(const std::string &strKey, const std::vector<std::string> &arrValue);

        std::vector<CBundle> *getBundleArray(const std::string &strKey) const;

        void setBundleArray(const std::string &strKey, const std::vector<CBundle> &arrValue);

        void *getHandle(const std::string &strKey) const;

        bool getHandle(const std::string &strKey, void **handle) const;

        void setHandle(const std::string &strKey, const void *iValue);

        int getBundleType(const std::string &strKey);

    private:
        template <typename T> bool getDeepArray(const std::string &strKey, std::vector<T> &outValue) const;

    protected:
        std::unordered_map<std::string, void *> m_mapBundle;
    };

}   // namespace btch
