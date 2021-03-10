//
// Created by bytedance on 10/03/2021.
//
#include "include/CBundle.h"
#include <sstream>
#include <cstdlib>

using namespace btch;
class CBundleValue {
public:
    /*
     bundle_bool = 1,		// 值：bool
     bundle_int,			// 值：int
     bundle_float,			// 值：float
     bundle_double,		// 值：double
     bundle_number,		// 值：数字类型,存储为Double
     bundle_string,		// 值：std::string
     bundle_bundle,		// 值：CBundle
     bundle_int_array,		// 值：std::vetor<int>
     bundle_float_array,	// 值：std::vetor<float>
     bundle_float_number_array,     // 值：std::vetor<float>
     bundle_double_number_array,    // 值：std::vetor<double>
     bundle_string_array,	// 值：std::vetor<std::string>
     bundle_bundle_array,	// 值：std::vetor<CBundle>
     */

    // BundleValue的创建函数
    static CBundleValue *CreatBool(bool bValue);

    static CBundleValue *CreatInt(int64_t iValue);

    static CBundleValue *CreateIntOrigin(int iValue);

    static CBundleValue *Creatulong(unsigned long iValue);

    static CBundleValue *CreatFloat(float fValue);

    static CBundleValue *CreatDouble(double dValue);

    static CBundleValue *CreatString(const std::string &strValue);

    static CBundleValue *CreatBundle(const CBundle &bundValue);

    static CBundleValue *CreatIntArray(const std::vector<int> &arrValue);

    static CBundleValue *CreatCharArray(const std::vector<char> &arrValue);

    static CBundleValue *CreatFloatArray(const std::vector<float> &arrValue);

    static CBundleValue *CreatDoubleArray(const std::vector<double> &arrValue);

    static CBundleValue *CreatStringArray(const std::vector<std::string> &arrValue);

    static CBundleValue *CreatBundleArray(const std::vector<CBundle> &arrValue);

    // 拷贝一份BundleValue
    static CBundleValue *Clone(CBundleValue *pValue);

    // 释放Bundle
    static void Release(CBundleValue *pObj);

    // 获取对应的值
    bool GetBool() const;

    int64_t GetInt() const;

    int GetIntOrigin() const;

    unsigned long GetULong() const;

    float GetFloat() const;

    double GetDouble() const;

    std::string *GetString() const;

    CBundle *GetBundle() const;

    std::vector<float> *GetFloatArray() const;

    std::vector<double> *GetDoubleArray() const;

    std::vector<int> *GetIntArray() const;

    std::vector<char> *GetCharArray() const;

    std::vector<std::string> *GetStringArray() const;

    std::vector<CBundle> *GetBundleArray() const;

    template <class T> bool getDeepArray(std::vector<T> &outValue) const;

public:
    int m_eType;

protected:
    void *m_pValue;

protected:
    CBundleValue();

    // 析构函数
    virtual ~CBundleValue();

    template <class Type> static CBundleValue *CreatValueTempl(const Type &valueTmp);

    template <class Type> static void ReleaseValueTempl(Type *pValue);
};

template <class Type> CBundleValue *CBundleValue::CreatValueTempl(const Type &valueTmp) {
    CBundleValue *pObj = (CBundleValue *)malloc(sizeof(CBundleValue));

    if (!pObj)
        return NULL;

    ::new (pObj) CBundleValue;

    void *pBuf = malloc(sizeof(Type));
    Type *pValue = new (pBuf) Type();

    if (!pValue) {
        free(pObj);
        pObj = NULL;
        return NULL;
    }

    *pValue = valueTmp;
    pObj->m_pValue = pValue;
    return pObj;
}

template <class Type> void CBundleValue::ReleaseValueTempl(Type *pValue) {
    if (!pValue)
        return;

    pValue->~Type();
    free(pValue);
    pValue = NULL;
}

// BundleValue的创建函数
CBundleValue *CBundleValue::CreatBool(bool bValue) {
    CBundleValue *pObj = CreatValueTempl(bValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_bool;
    return pObj;
}

CBundleValue *CBundleValue::CreatInt(int64_t iValue) {
    CBundleValue *pObj = CreatValueTempl(iValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_integer;
    return pObj;
}

CBundleValue *CBundleValue::CreateIntOrigin(int iValue) {
    CBundleValue *pObj = CreatValueTempl(iValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_int;
    return pObj;
}

CBundleValue *CBundleValue::CreatFloat(float fValue) {
    return CreatDouble(double(fValue));
}

CBundleValue *CBundleValue::CreatDouble(double dValue) {
    CBundleValue *pObj = CreatValueTempl(dValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_float;
    return pObj;
}

CBundleValue *CBundleValue::CreatString(const std::string &strValue) {
    CBundleValue *pObj = CreatValueTempl(strValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_string;
    return pObj;
}

CBundleValue *CBundleValue::CreatBundle(const CBundle &bundValue) {
    CBundleValue *pObj = CreatValueTempl(bundValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_bundle;
    return pObj;
}

CBundleValue *CBundleValue::CreatIntArray(const std::vector<int> &arrValue) {
    CBundleValue *pObj = CreatValueTempl(arrValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_int_number_array;
    return pObj;
}

CBundleValue *CBundleValue::CreatCharArray(const std::vector<char> &arrValue) {
    CBundleValue *pObj = CreatValueTempl(arrValue);
    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_char_number_array;
    return pObj;
}

CBundleValue *CBundleValue::CreatFloatArray(const std::vector<float> &arrValue) {
    CBundleValue *pObj = CreatValueTempl(arrValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_float_number_array;
    return pObj;
}

CBundleValue *CBundleValue::CreatDoubleArray(const std::vector<double> &arrValue) {
    CBundleValue *pObj = CreatValueTempl(arrValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_double_number_array;
    return pObj;
}

CBundleValue *CBundleValue::CreatStringArray(const std::vector<std::string> &arrValue) {
    CBundleValue *pObj = CreatValueTempl(arrValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_string_array;
    return pObj;
}

CBundleValue *CBundleValue::CreatBundleArray(const std::vector<CBundle> &arrValue) {
    CBundleValue *pObj = CreatValueTempl(arrValue);

    if (!pObj)
        return NULL;

    pObj->m_eType = bundle_bundle_array;
    return pObj;
}

// 拷贝一份BundleValue
CBundleValue *CBundleValue::Clone(CBundleValue *pValue) {
    if (pValue == NULL) {
        return NULL;
    }

    switch (pValue->m_eType) {
        case bundle_bool: {
            return CBundleValue::CreatBool(pValue->GetBool());
        } break;

        case bundle_float: {
            return CBundleValue::CreatDouble(pValue->GetDouble());
        } break;
        case bundle_integer: {
            return CBundleValue::CreatInt(pValue->GetInt());
        } break;

        case bundle_int: {
            return CBundleValue::CreateIntOrigin(pValue->GetIntOrigin());
        }

        case bundle_string: {
            std::string *pValueTemp = pValue->GetString();
            if (pValueTemp) {
                return CBundleValue::CreatString(*pValueTemp);
            }
        } break;

        case bundle_bundle: {
            CBundle *pValueTemp = pValue->GetBundle();
            if (pValueTemp) {
                return CBundleValue::CreatBundle(*pValueTemp);
            }
        } break;

        case bundle_float_number_array: {
            std::vector<float> *pValueTemp = pValue->GetFloatArray();
            if (pValueTemp) {
                return CBundleValue::CreatFloatArray(*pValueTemp);
            }
        } break;

        case bundle_double_number_array: {
            std::vector<double> *pValueTemp = pValue->GetDoubleArray();
            if (pValueTemp) {
                return CBundleValue::CreatDoubleArray(*pValueTemp);
            }
        } break;

        case bundle_int_number_array: {
            std::vector<int> *pValueTemp = pValue->GetIntArray();
            if (pValueTemp) {
                return CBundleValue::CreatIntArray(*pValueTemp);
            }
        } break;

        case bundle_char_number_array: {
            std::vector<char> *pValueTemp = pValue->GetCharArray();
            if (pValueTemp) {
                return CBundleValue::CreatCharArray(*pValueTemp);
            }
        } break;

        case bundle_string_array: {
            std::vector<std::string> *pValueTemp = pValue->GetStringArray();
            if (pValueTemp) {
                return CBundleValue::CreatStringArray(*pValueTemp);
            }
        } break;

        case bundle_bundle_array: {
            std::vector<CBundle> *pValueTemp = pValue->GetBundleArray();
            if (pValueTemp) {
                return CBundleValue::CreatBundleArray(*pValueTemp);
            }
        } break;

        default: break;
    }

    return NULL;
}

// 释放Bundle
void CBundleValue::Release(CBundleValue *pObj) {
    if (!pObj)
        return;

    pObj->~CBundleValue();
    free(pObj);
    pObj = NULL;
}

// 获取对应的值
bool CBundleValue::GetBool() const {
    return *((bool *)m_pValue);
}

int64_t CBundleValue::GetInt() const {
    if (!m_pValue)
        return 0;
    else
        return *((int64_t *)m_pValue);
}

int CBundleValue::GetIntOrigin() const {
    if (!m_pValue)
        return 0;
    else
        return *((int *)m_pValue);
}

unsigned long CBundleValue::GetULong() const {
    if (!m_pValue)
        return 0;
    else
        return *((unsigned long *)m_pValue);
}

float CBundleValue::GetFloat() const {
    return (float)GetDouble();
}

double CBundleValue::GetDouble() const {
    if (!m_pValue)
        return 0.0;
    else
        return *((double *)m_pValue);
}

std::string *CBundleValue::GetString() const {
    return (std::string *)m_pValue;
}

CBundle *CBundleValue::GetBundle() const {
    return (CBundle *)m_pValue;
}

std::vector<float> *CBundleValue::GetFloatArray() const {
    return (std::vector<float> *)m_pValue;
}

std::vector<double> *CBundleValue::GetDoubleArray() const {
    return (std::vector<double> *)m_pValue;
}

std::vector<int> *CBundleValue::GetIntArray() const {
    return (std::vector<int> *)m_pValue;
}

std::vector<char> *CBundleValue::GetCharArray() const {
    return (std::vector<char> *)m_pValue;
}

std::vector<std::string> *CBundleValue::GetStringArray() const {
    return (std::vector<std::string> *)m_pValue;
}

std::vector<CBundle> *CBundleValue::GetBundleArray() const {
    return (std::vector<CBundle> *)m_pValue;
}

template <class T> bool CBundleValue::getDeepArray(std::vector<T> &outValue) const {
    if (m_pValue == nullptr)
        return false;
    std::vector<T> *pVector = (std::vector<T> *)m_pValue;
    for (auto value : *pVector) {
        outValue.push_back(value);
    }
    return true;
}

CBundleValue::~CBundleValue() {
    if (!m_pValue)
        return;

    switch (m_eType) {
        case bundle_bool: {
            bool *pValue = (bool *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }
        case bundle_float: {
            double *pValue = (double *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }
        case bundle_integer: {
            int64_t *pValue = (int64_t *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_int: {
            int *pValue = (int *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_string: {
            std::string *pValue = (std::string *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_bundle: {
            CBundle *pValue = (CBundle *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_int_number_array: {
            std::vector<int> *pValue = (std::vector<int> *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_float_number_array: {
            std::vector<float> *pValue = (std::vector<float> *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_double_number_array: {
            std::vector<double> *pValue = (std::vector<double> *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_string_array: {
            std::vector<std::string> *pValue = (std::vector<std::string> *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        case bundle_bundle_array: {
            std::vector<CBundle> *pValue = (std::vector<CBundle> *)m_pValue;
            CBundleValue::ReleaseValueTempl(pValue);
            break;
        }

        default: break;
    }

    m_pValue = NULL;
}

CBundleValue::CBundleValue() {
    m_eType = 0;
    m_pValue = NULL;
}

CBundle::CBundle() {}

CBundle::~CBundle() {
    clear();
}

CBundle::CBundle(CBundle &r) {
    if (this == &r)
        return;

    clear();

    std::unordered_map<std::string, void *>::iterator it;
    for (it = r.m_mapBundle.begin(); it != r.m_mapBundle.end();) {
        if (it->second) {
            CBundleValue *pBundleValue = (CBundleValue *)it->second;
            m_mapBundle[it->first] = CBundleValue::Clone(pBundleValue);
        }

        it++;
    }
}

CBundle::CBundle(const CBundle &r) {
    if (this == &r)
        return;

    clear();

    for (auto &kv : r.m_mapBundle) {
        if (kv.second) {
            CBundleValue *pBundleValue = (CBundleValue *)kv.second;
            m_mapBundle[kv.first] = CBundleValue::Clone(pBundleValue);
        }
    }
}

CBundle &CBundle::operator=(const CBundle &r) {
    if (this == &r)
        return *this;

    clear();

    for (auto &kv : r.m_mapBundle) {
        if (kv.second) {
            CBundleValue *pBundleValue = (CBundleValue *)kv.second;
            m_mapBundle[kv.first] = CBundleValue::Clone(pBundleValue);
        }
    }

    return *this;
}

CBundle &CBundle::operator+(const btch::CBundle &r) {
    if (this == &r)
        return *this;

    for (auto &kv : r.m_mapBundle) {
        if (kv.second) {
            CBundleValue *pBundleValue = (CBundleValue *)kv.second;
            m_mapBundle[kv.first] = CBundleValue::Clone(pBundleValue);
        }
    }
    return *this;
}

/**
 *	清空所有的内容
 *	@notes N/A
 */
void CBundle::clear() {
    std::unordered_map<std::string, void *>::iterator it;
    for (it = m_mapBundle.begin(); it != m_mapBundle.end();) {
        if (it->second) {
            CBundleValue *pBundleValue = (CBundleValue *)it->second;
            CBundleValue::Release(pBundleValue);
        }

        it++;
    }

    m_mapBundle.clear();
}

/**
 *	移除指定Key
 *	@notes N/A
 */
void CBundle::remove(const std::string &strKey) {
    CBundleValue *pValue = (CBundleValue *)m_mapBundle[strKey];

    if (pValue) {
        CBundleValue::Release(pValue);
    }

    m_mapBundle.erase(strKey);
}

/**
 *	检测指定的Key是否存在
 *	@param      strKey	[in]      指定Key
 *	@return     存在指定key返回true，否则返回false
 *	@notes  N/A
 */
bool CBundle::containsKey(const std::string &strKey) const {
    return m_mapBundle.find(strKey) != m_mapBundle.end();
}

/**
 *	获取所有Key
 *	@param arrKey	[out]	所有的Key集合
 *	@notes  N/A
 */
void CBundle::getKeys(std::vector<std::string> &arrKey) const {
    for (auto &kv : m_mapBundle) {
        arrKey.push_back(kv.first);
    }
}

/**
 *	根据指定Key获取值的类型
 *	@param strKey       [in]            指定Key
 *	@return 值的类型，如bundle_bool、bundle_bundle_array等
 *	@notes  N/A
 */
int CBundle::getType(std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->m_eType;
    } else
        return bundle_error;
}

/**
 *	根据指定Key获取bool类型的值
 *	@param  strKey      [in]	指定Key
 *	@return bool类型的值
 *	@notes  N/A
 */
bool CBundle::getBool(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetBool();
    } else
        return false;
}

bool CBundle::getBool(const std::string &strKey, bool &value) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        value = pValue->GetBool();
        return true;
    } else
        return false;
}

/**
 *	根据指定Key获取int64类型的值
 *	@param  strKey      [in]	指定Key
 *	@return int类型的值
 *	@notes  N/A
 */
int64_t CBundle::getInt(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetInt();
    } else
        return 0;
}

/**
 *	根据指定Key获取int64类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  value       [out]   值
 *	@return 找到key返回true，否则返回false
 *	@notes  N/A
 */
bool CBundle::getInt(const std::string &strKey, int64_t &value) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        value = pValue->GetInt();
        return true;
    } else {
        return false;
    }
}

/**
 *	根据指定Key获取int类型的值
 *	@param  strKey      [in]	指定Key
 *	@return int类型的值
 *	@notes  N/A
 */
int CBundle::getIntOrigin(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetIntOrigin();
    } else
        return 0;
}

/**
 *	根据指定Key获取int类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  value       [out]   值
 *	@return 找到key返回true，否则返回false
 *	@notes  N/A
 */
bool CBundle::getIntOrigin(const std::string &strKey, int &value) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        value = pValue->GetIntOrigin();
        return true;
    } else {
        return false;
    }
}

/**
 *	根据指定Key获取float类型的值
 *	@param  strKey      [in]	指定Key
 *	@return float类型的值
 *	@notes  N/A
 */
float CBundle::getFloat(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetFloat();
    } else
        return 0.0;
}

/**
 *	根据指定Key获取float类型的值
 *	@param  strKey      [in]	指定Key
 *	@param value 查询到之后写入变量
 *	@return 是否存在
 *	@notes  N/A
 */
bool CBundle::getFloat(const std::string &strKey, float &value) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        value = pValue->GetFloat();
        return true;
    } else
        return false;
}

/**
 *	根据指定Key获取double类型的值
 *	@param  strKey      [in]	指定Key
 *	@return double类型的值
 *	@notes  N/A
 */
double CBundle::getDouble(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetDouble();
    } else
        return 0.0;
}

/**
 *	根据指定Key获取double类型的值
 *	@param  strKey      [in]	指定Key
 *	@return 是否存在
 *	@notes  N/A
 */
bool CBundle::getDouble(const std::string &strKey, double &value) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        value = pValue->GetDouble();
        return true;
    } else
        return false;
}

/**
 *	根据指定Key获取std::string类型的值
 *	@param  strKey      [in]	指定Key
 *	@return string类型的指针
 *	@notes  N/A
 */
std::string *CBundle::getString(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetString();
    } else
        return NULL;
}

/**
 *	根据指定Key获取CBundle类型的值
 *	@param  strKey      [in]	指定Key
 *	@return CBundle类型的指针
 *	@notes  N/A
 */
CBundle *CBundle::getBundle(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetBundle();
    } else
        return NULL;
}

/**
 *	根据指定Key获取double[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@return double数组的指针
 *	@notes  N/A
 */
std::vector<float> *CBundle::getFloatArray(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetFloatArray();
    } else
        return NULL;
}

/**
 *	根据指定Key获取double[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@return double数组的指针
 *	@notes  N/A
 */
std::vector<double> *CBundle::getDoubleArray(std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetDoubleArray();
    } else
        return NULL;
}

/**
 *	根据指定Key获取std::string[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@return std::string数组的指针
 *	@notes  N/A
 */
std::vector<std::string> *CBundle::getStringArray(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetStringArray();
    } else
        return NULL;
}

/**
 *	根据指定Key获C[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@return CBundle数组的指针
 *	@notes  N/A
 */
std::vector<CBundle> *CBundle::getBundleArray(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->GetBundleArray();
    } else
        return NULL;
}

template <typename T> bool CBundle::getDeepArray(const std::string &strKey, std::vector<T> &outValue) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);
    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->getDeepArray(outValue);
    }
    return false;
}

bool CBundle::getDeepIntArray(const std::string &strKey, std::vector<int> &outValue) const {
    return getDeepArray<int>(strKey, outValue);
}

bool CBundle::getDeepCharArray(const std::string &strKey, std::vector<char> &outValue) const {
    return getDeepArray<char>(strKey, outValue);
}

bool CBundle::getDeepFloatArray(const std::string &strKey, std::vector<float> &outValue) const {
    return getDeepArray(strKey, outValue);
}

bool CBundle::getDeepDoubleArray(const std::string &strKey, std::vector<double> &outValue) const {
    return getDeepArray(strKey, outValue);
}

bool CBundle::getDeepStringArray(const std::string &strKey, std::vector<std::string> &outValue) const {
    return getDeepArray(strKey, outValue);
}

/*******根据指定Key设定对应值********************/

/**
 *	根据指定Key获取bool类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  bValue      [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setBool(const std::string &strKey, bool bValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatBool(bValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key设置int64类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  iValue      [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setInt(const std::string &strKey, int64_t iValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatInt(iValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key设置int类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  iValue      [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setIntOrigin(const std::string &strKey, int iValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreateIntOrigin(iValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

int64_t CBundle::getLong(const std::string &strKey) const {
    return getInt(strKey);
}

bool CBundle::getLong(const std::string &strKey, int64_t &value) const {
    return getInt(strKey, value);
}

void CBundle::setLong(const std::string &strKey, int64_t iValue) {
    setInt(strKey, iValue);
}

/**
 *	根据指定Key获取float类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  fValue      [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setFloat(const std::string &strKey, float fValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatFloat(fValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取double类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  dValue      [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setDouble(const std::string &strKey, double dValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatDouble(dValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取std::string类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  strValue    [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setString(const std::string &strKey, const std::string &strValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatString(strValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取VBundle类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  bundleValue [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setBundle(const std::string &strKey, const CBundle &bundleValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatBundle(bundleValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key设置int[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  arrValue    [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setIntArray(const std::string &strKey, const std::vector<int> &arrValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatIntArray(arrValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

void CBundle::setCharArray(const std::string &strKey, const std::vector<char> &arrValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatCharArray(arrValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取double[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  arrValue    [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setFloatArray(const std::string &strKey, const std::vector<float> &arrValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatFloatArray(arrValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取double[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  arrValue    [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setDoubleArray(const std::string &strKey, const std::vector<double> &arrValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatDoubleArray(arrValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取std::string[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  arrValue    [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setStringArray(const std::string &strKey, const std::vector<std::string> &arrValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatStringArray(arrValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取CBundle[]类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  arrValue    [in]	指定Value
 *	@notes  N/A
 */
void CBundle::setBundleArray(const std::string &strKey, const std::vector<CBundle> &arrValue) {
    remove(strKey);
    CBundleValue *pValue = CBundleValue::CreatBundleArray(arrValue);
    if (pValue) {
        m_mapBundle[strKey] = pValue;
    }
}

/**
 *	根据指定Key获取unsigle long类型的值
 *	@param  strKey      [in]	指定Key
 *	@return 指针的值
 *	@notes  用于指针传递的情况
 */
void *CBundle::getHandle(const std::string &strKey) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        int64_t nValue = pValue->GetInt();
        return (void *)nValue;
    } else
        return NULL;
}

bool CBundle::getHandle(const std::string &strKey, void **handle) const {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        int64_t nValue = pValue->GetInt();

        *handle = (void *)nValue;
        return true;
    } else
        return false;
}

int CBundle::getBundleType(const std::string &strKey) {
    std::unordered_map<std::string, void *>::const_iterator it = m_mapBundle.find(strKey);

    if (it != m_mapBundle.end()) {
        CBundleValue *pValue = (CBundleValue *)it->second;
        return pValue->m_eType;
    } else
        return bundle_error;
}

/**
 *	根据指定Key获取unsigle long类型的值
 *	@param  strKey      [in]	指定Key
 *	@param  iValue      [in]	指定Value
 *	@notes  用于指针传递的情况
 */
void CBundle::setHandle(const std::string &strKey, const void *iValue) {
    remove(strKey);
    int64_t dValue = (int64_t)iValue;
    m_mapBundle[strKey] = CBundleValue::CreatInt(dValue);
}

