#pragma once

#include <map>
#include <memory>

namespace mc {
    template<typename T>
    class general_factory {
        std::map<string, std::function<std::unique_ptr<T>()>> m_fac_members;
    public:

        unique_ptr<T> get_instance(string name)
        {
            if (auto iter = m_fac_members.find(name); iter != m_fac_members.end())
                return iter->second();
            else
                throw std::invalid_argument{ "invalid type" };
        }

        template<typename T, typename ...Args>
        void register_new_type(string name, Args&& ...args)
        {
            m_fac_members.insert({ name,[...args = std::forward<Args>(args)]() {  return std::make_unique<T>(args...); } });
        }
    };
}
