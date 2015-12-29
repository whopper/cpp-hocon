#pragma once

#include <string>
#include <memory>

namespace hocon {

    class substitution_expression : public std::enable_shared_from_this<substitution_expression> {
    public:
        substitution_expression(std::string path, bool optional);

        std::string path() const;
        bool optional() const;

        std::shared_ptr<substitution_expression> change_path(std::string new_path);

        std::string to_string() const;

        template <typename T>
        bool equals(std::shared_ptr<T> other) const {
            if (auto otherExp = std::dynamic_pointer_cast<substitution_expression>(other)) {
                return(otherExp->path() == path() && otherExp->optional() == optional());
            } else {
                return false;
            }
        }

    private:
        const std::string _path;
        const bool _optional;
    };

}  // namespace hocon

